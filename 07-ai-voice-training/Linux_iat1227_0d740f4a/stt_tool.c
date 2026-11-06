/*
 * 科大讯飞语音听写命令行工具
 * 用法: ./stt_tool <audio.wav>
 * 输出: 识别结果文字
 *
 * 基于 iat_online_record_sample 修改
 * 编译: gcc -o stt_tool stt_tool.c speech_recognizer.c linuxrec.c \
 *        -I../include -L../libs/x64 -lmsc -lpthread -ldl
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "speech_recognizer.h"

#define FRAME_LEN    640
#define BUFFER_SIZE  4096

/* 全局结果缓冲区 */
static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

void on_result(const char *result, char is_last)
{
    if (result) {
        size_t left = g_buffersize - 1 - strlen(g_result);
        size_t size = strlen(result);
        if (left < size) {
            g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
            if (g_result)
                g_buffersize += BUFFER_SIZE;
            else
                return;
        }
        strncat(g_result, result, size);
    }
}

void on_speech_begin()
{
    if (g_result) free(g_result);
    g_result = (char*)malloc(BUFFER_SIZE);
    g_buffersize = BUFFER_SIZE;
    memset(g_result, 0, g_buffersize);
}

void on_speech_end(int reason)
{
    /* silent */
}

/* 识别音频文件 */
static int recognize_file(const char* audio_file, const char* session_params)
{
    int errcode = 0;
    FILE* f_pcm = NULL;
    char* p_pcm = NULL;
    unsigned long pcm_count = 0;
    unsigned long pcm_size = 0;
    unsigned long read_size = 0;
    struct speech_rec iat;
    struct speech_rec_notifier recnotifier = {
        on_result,
        on_speech_begin,
        on_speech_end
    };

    f_pcm = fopen(audio_file, "rb");
    if (NULL == f_pcm) {
        fprintf(stderr, "Cannot open %s\n", audio_file);
        return -1;
    }

    fseek(f_pcm, 0, SEEK_END);
    pcm_size = ftell(f_pcm);
    fseek(f_pcm, 0, SEEK_SET);

    /* 跳过 WAV 文件头 (44 字节) 获取 PCM 数据 */
    if (pcm_size > 44) {
        fseek(f_pcm, 44, SEEK_SET);
        pcm_size -= 44;
    } else {
        fseek(f_pcm, 0, SEEK_SET);
    }

    p_pcm = (char *)malloc(pcm_size);
    if (NULL == p_pcm) {
        fprintf(stderr, "Out of memory!\n");
        fclose(f_pcm);
        return -1;
    }

    read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm);
    fclose(f_pcm);
    if (read_size != pcm_size) {
        fprintf(stderr, "Read error!\n");
        free(p_pcm);
        return -1;
    }

    errcode = sr_init(&iat, session_params, SR_USER, &recnotifier);
    if (errcode) {
        fprintf(stderr, "SR init failed: %d\n", errcode);
        free(p_pcm);
        return -1;
    }

    errcode = sr_start_listening(&iat);
    if (errcode) {
        fprintf(stderr, "SR start failed: %d\n", errcode);
        free(p_pcm);
        sr_uninit(&iat);
        return -1;
    }

    /* 分块发送音频数据 */
    while (1) {
        unsigned int len = 10 * FRAME_LEN; /* 200ms per chunk */
        int ret;

        if (pcm_size < 2 * len)
            len = pcm_size;
        if (len <= 0)
            break;

        ret = sr_write_audio_data(&iat, &p_pcm[pcm_count], len);
        if (0 != ret) {
            fprintf(stderr, "Write audio failed: %d\n", ret);
            break;
        }

        pcm_count += (long)len;
        pcm_size -= (long)len;
    }

    errcode = sr_stop_listening(&iat);
    sr_uninit(&iat);
    free(p_pcm);

    return errcode;
}

int main(int argc, char* argv[])
{
    const char* audio_file;
    int ret;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <audio.wav>\n", argv[0]);
        return 1;
    }
    audio_file = argv[1];

    /* 登录参数 — appid 来自你的 SDK */
    /* work_dir 可通过环境变量 IAT_WORK_DIR 覆盖 */
    static char login_params[256];
    const char *work_dir_env = getenv("IAT_WORK_DIR");
    if (!work_dir_env) work_dir_env = "/tmp/iat";
    snprintf(login_params, sizeof(login_params),
             "appid = 0d740f4a, work_dir = %s", work_dir_env);

    /* 会话参数 — 中文普通话 16kHz */
    const char* session_params =
        "sub = iat, domain = iat, language = zh_cn, "
        "accent = mandarin, sample_rate = 16000, "
        "result_type = plain, result_encoding = utf8";

    /* 1. 登录 MSP */
    ret = MSPLogin(NULL, NULL, login_params);
    if (MSP_SUCCESS != ret) {
        fprintf(stderr, "MSPLogin failed, error %d (0x%04X)\n", ret, ret);
        fprintf(stderr, "  错误说明: ");
        switch(ret) {
            case 10407: fprintf(stderr, "应用认证失败 — appid 无效或服务未激活\n"); break;
            case 10114: fprintf(stderr, "DNS 解析失败 — 无法连接讯飞云服务器\n"); break;
            case 10160: fprintf(stderr, "网络连接超时\n"); break;
            default:    fprintf(stderr, "未知错误，请查看日志 /tmp/iat/msc/msc.log\n"); break;
        }
        fprintf(stderr, "  检查: work_dir=%s\n", work_dir_env);
        fprintf(stderr, "  检查: %s/msc/msc.cfg 是否存在\n", work_dir_env);
        fprintf(stderr, "  检查: %s/msc/ 下是否有 u.data 许可证文件\n", work_dir_env);
        return 1;
    }

    /* 2. 识别 */
    ret = recognize_file(audio_file, session_params);

    /* 3. 输出结果 */
    if (g_result && strlen(g_result) > 0) {
        /* 去掉末尾的句号(. 和 UTF-8 。 E3 80 82) */
        int len = strlen(g_result);
        while (len > 0 && g_result[len-1] == '.')
            g_result[--len] = '\0';
        /* 去掉末尾中文句号 (UTF-8: E3 80 82) */
        if (len >= 3 && (unsigned char)g_result[len-3] == 0xE3
            && (unsigned char)g_result[len-2] == 0x80
            && (unsigned char)g_result[len-1] == 0x82)
            g_result[len-3] = '\0';
        printf("%s\n", g_result);
    } else {
        printf("\n");
    }

    /* 4. 清理 */
    if (g_result) free(g_result);
    MSPLogout();

    return (ret == 0) ? 0 : 1;
}
