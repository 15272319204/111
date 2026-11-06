/**
 * 语音识别线程 — TCP 客户端
 *
 * 工作流程（按笔记 十四/十五/十六 节）:
 *   1. arecord 录音 → /tmp/voice.wav
 *   2. TCP 连接虚拟机服务器 (IP:5000)
 *   3. 发送录音文件到虚拟机
 *   4. 虚拟机: 科大讯飞 STT → DeepSeek AI → 返回结果
 *   5. 接收 AI 回复，存入全局缓冲区
 *   6. 检查语音控制指令（打开相册/写字板/返回主页）
 *
 * 依赖:
 *   开发板需连接 WiFi: wifi_start.sh 'SSID' 'password'
 *   虚拟机需运行: python voice_server.py
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "voice_thread.h"

/* ===== 虚拟机服务器配置（根据实际环境修改） ===== */
#ifndef VM_IP
#define VM_IP       "192.168.74.189"   /* 虚拟机 IP 地址 */
#endif
#define VM_PORT     5000

/* 是否启用语音功能 */
#ifndef ENABLE_VOICE
/* 默认禁用，编译时加 -DENABLE_VOICE 启用 */
#endif

/* ===== 全局变量 ===== */
static pthread_t voice_tid;
static int voice_running = 0;
static pthread_mutex_t voice_mutex = PTHREAD_MUTEX_INITIALIZER;

/* 最新 AI 回复结果 */
static char g_ai_result[1024] = {0};

/* 语音控制指令 */
static int g_voice_command = VOICE_CMD_NONE;

/* 状态字符串 */
static char g_state[64] = "Stopped";

/* ===== 内部辅助函数 ===== */

static void set_state(const char *state)
{
    pthread_mutex_lock(&voice_mutex);
    strncpy(g_state, state, sizeof(g_state) - 1);
    g_state[sizeof(g_state) - 1] = '\0';
    pthread_mutex_unlock(&voice_mutex);
    printf("[语音线程] %s\n", state);
}

static void set_result(const char *result)
{
    pthread_mutex_lock(&voice_mutex);
    strncpy(g_ai_result, result, sizeof(g_ai_result) - 1);
    g_ai_result[sizeof(g_ai_result) - 1] = '\0';
    pthread_mutex_unlock(&voice_mutex);
}

static void set_command(int cmd)
{
    pthread_mutex_lock(&voice_mutex);
    g_voice_command = cmd;
    pthread_mutex_unlock(&voice_mutex);
}

/* 检查语音控制关键词 */
static int check_voice_command(const char *text)
{
    if (strstr(text, "相册") || strstr(text, "album") || strstr(text, "Album"))
        return VOICE_CMD_ALBUM;
    if (strstr(text, "写字板") || strstr(text, "draw") || strstr(text, "Draw"))
        return VOICE_CMD_WRITEPAD;
    if (strstr(text, "主页") || strstr(text, "返回") || strstr(text, "home") || strstr(text, "Home"))
        return VOICE_CMD_HOME;
    return VOICE_CMD_NONE;
}

/* 开发板录音 */
static int record_audio(const char *filename, int duration_sec)
{
    char cmd[256];
    /* 先设置麦克风通道确保一致 */
    system("amixer -c 0 sset 'Capture MIC Path' 'Hands Free Mic' 2>/dev/null");
    /* 笔记标准录音指令: arecord -d3 -c1 -r16000 -twav -fS16_LE example.wav */
    snprintf(cmd, sizeof(cmd),
             "arecord -d%d -c1 -r16000 -twav -fS16_LE %s 2>/dev/null",
             duration_sec, filename);
    printf("[录音] %s (%d秒)\n", filename, duration_sec);
    return system(cmd);
}

/* TCP 发送音频文件并接收 AI 回复 */
static int tcp_send_and_recv(const char *filename, char *reply, int maxlen)
{
    int sock_fd;
    struct sockaddr_in server_addr;
    int retry;

    /* 1. 创建 TCP 套接字 */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        printf("[TCP] 套接字创建失败\n");
        return -1;
    }

    /* 2. 连接虚拟机服务器 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(VM_PORT);
    server_addr.sin_addr.s_addr = inet_addr(VM_IP);

    printf("[TCP] 正在连接 %s:%d ...\n", VM_IP, VM_PORT);

    /* 重试 3 次 */
    for (retry = 0; retry < 3; retry++)
    {
        if (connect(sock_fd, (struct sockaddr *)&server_addr,
                    sizeof(server_addr)) == 0)
        {
            break;
        }
        printf("[TCP] 连接失败，重试 %d/3...\n", retry + 1);
        sleep(1);
    }
    if (retry >= 3)
    {
        printf("[TCP] 无法连接到虚拟机服务器\n");
        printf("[TCP] 请确保:\n");
        printf("      1. 开发板已连 WiFi: wifi_start.sh 'SSID' 'password'\n");
        printf("      2. 虚拟机 IP: %s\n", VM_IP);
        printf("      3. 虚拟机已启动: python voice_server.py\n");
        close(sock_fd);
        return -1;
    }

    printf("[TCP] 已连接到虚拟机\n");

    /* 3. 获取文件大小 */
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("[TCP] 无法打开录音文件 %s\n", filename);
        close(sock_fd);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("[TCP] 发送文件: %s (%ld 字节)\n", filename, file_size);

    /* 4. 发送文件大小（文本格式，16字节） */
    char size_buf[32];
    snprintf(size_buf, sizeof(size_buf), "%016ld", file_size);
    if (send(sock_fd, size_buf, 16, 0) != 16)
    {
        printf("[TCP] 发送文件大小失败\n");
        fclose(fp);
        close(sock_fd);
        return -1;
    }

    /* 5. 等待确认 */
    char ack[4];
    if (recv(sock_fd, ack, 2, 0) <= 0)
    {
        printf("[TCP] 未收到确认\n");
        fclose(fp);
        close(sock_fd);
        return -1;
    }

    /* 6. 发送文件内容 */
    char send_buf[4096];
    size_t n;
    while ((n = fread(send_buf, 1, sizeof(send_buf), fp)) > 0)
    {
        if (send(sock_fd, send_buf, n, 0) != (ssize_t)n)
        {
            printf("[TCP] 发送文件数据失败\n");
            fclose(fp);
            close(sock_fd);
            return -1;
        }
    }
    fclose(fp);
    printf("[TCP] 文件发送完成\n");

    /* 7. 接收回复长度（8字节） */
    char len_buf[16];
    memset(len_buf, 0, sizeof(len_buf));
    int received = 0;
    while (received < 8)
    {
        int n_recv = recv(sock_fd, len_buf + received, 8 - received, 0);
        if (n_recv <= 0)
        {
            printf("[TCP] 接收回复长度失败\n");
            close(sock_fd);
            return -1;
        }
        received += n_recv;
    }

    int reply_len = atoi(len_buf);
    if (reply_len <= 0 || reply_len > maxlen - 1)
    {
        printf("[TCP] 无效的回复长度: %d\n", reply_len);
        close(sock_fd);
        return -1;
    }

    /* 8. 接收回复内容 */
    char *reply_buf = (char *)malloc(reply_len + 1);
    if (reply_buf == NULL)
    {
        close(sock_fd);
        return -1;
    }

    received = 0;
    while (received < reply_len)
    {
        int n_recv = recv(sock_fd, reply_buf + received,
                          reply_len - received, 0);
        if (n_recv <= 0) break;
        received += n_recv;
    }
    reply_buf[received] = '\0';

    strncpy(reply, reply_buf, maxlen - 1);
    reply[maxlen - 1] = '\0';

    free(reply_buf);
    close(sock_fd);

    printf("[TCP] 收到 AI 回复 (%d 字节): %s\n", received, reply);
    return 0;
}

/* ===== 语音线程主循环 ===== */
static void *voice_thread_loop(void *arg)
{
    (void)arg;
    char ai_reply[1024];
    int round = 0;

    printf("\n[语音线程] 已启动（TCP 模式）\n");
    printf("[语音线程] 目标服务器: %s:%d\n", VM_IP, VM_PORT);
    set_state("Waiting...");

    while (voice_running)
    {
        round++;
        printf("\n[语音线程] ==== 第 %d 轮 ====\n", round);

        /* 阶段1: 等待触发（间隔 2 秒） */
        set_state("Waiting...");
        sleep(2);
        if (!voice_running) break;

        /* 阶段2: 录音 5 秒 */
        set_state("Recording... (5s)");
#ifdef ENABLE_VOICE
        int rec_ret = record_audio("/tmp/voice_input.wav", 5);
        if (rec_ret != 0)
        {
            set_state("Recording failed!");
            continue;
        }

        /* 阶段3: TCP 发送 + 接收 AI 回复 */
        set_state("Connecting to VM...");
        ai_reply[0] = '\0';
        int tcp_ret = tcp_send_and_recv("/tmp/voice_input.wav",
                                          ai_reply, sizeof(ai_reply));
        if (tcp_ret != 0)
        {
            set_state("TCP failed!");
            continue;
        }
#else
        /* 未启用 ENABLE_VOICE，使用模拟 */
        printf("[模拟] 录音 3 秒（未定义 ENABLE_VOICE）\n");
        set_state("(Sim) Recording...");
        sleep(1);

        /* 模拟 STT 结果 */
        const char *sim_inputs[] = {
            "你好小智",
            "打开相册",
            "今天天气怎么样",
            "打开写字板",
            "讲个笑话",
            "返回主页",
        };
        const char *sim_text = sim_inputs[(round - 1) % 6];

        set_state("(Sim) Recognizing...");
        printf("[模拟] STT 识别结果: %s\n", sim_text);

        /* 模拟 AI 回复 */
        set_state("(Sim) AI replying...");
        sleep(1);

        if (strstr(sim_text, "天气"))
            strcpy(ai_reply, "今天天气晴朗，适合出门活动！");
        else if (strstr(sim_text, "笑话"))
            strcpy(ai_reply, "为什么程序员喜欢暗色主题？因为太亮了会招 bug！");
        else if (strstr(sim_text, "你好") || strstr(sim_text, "小智"))
            strcpy(ai_reply, "你好！我是小智，有什么可以帮你的？");
        else
            snprintf(ai_reply, sizeof(ai_reply),
                     "收到: %s。如需真实 AI，请编译时加 -DENABLE_VOICE。", sim_text);

        printf("[模拟] AI 回复: %s\n", ai_reply);
#endif

        /* 阶段4: 检查语音控制指令 */
        int cmd = check_voice_command(ai_reply);
        if (cmd != VOICE_CMD_NONE)
        {
            printf("[语音线程] 检测到语音指令: %d\n", cmd);
            set_command(cmd);

            /* 同时设置回复文字 */
            switch (cmd)
            {
                case VOICE_CMD_ALBUM:
                    set_result("好的，正在为您打开相册...");
                    break;
                case VOICE_CMD_WRITEPAD:
                    set_result("好的，正在为您打开写字板...");
                    break;
                case VOICE_CMD_HOME:
                    set_result("好的，正在返回主菜单...");
                    break;
            }
        }

        /* 存储 AI 回复 */
        if (strlen(ai_reply) > 0)
        {
            set_result(ai_reply);
        }

        set_state("Waiting...");

        /* 不清除指令，由 UI 侧读取后调用 voice_thread_clear_command 清除 */
    }

    printf("[语音线程] 已退出\n");
    set_state("Stopped");
    return NULL;
}

/* ===== 公开接口 ===== */

void voice_thread_start(void)
{
    if (voice_running) return;

    voice_running = 1;
    int ret = pthread_create(&voice_tid, NULL, voice_thread_loop, NULL);
    if (ret != 0)
    {
        printf("[语音线程] 创建失败！\n");
        voice_running = 0;
    }
}

void voice_thread_stop(void)
{
    if (!voice_running) return;

    voice_running = 0;
    pthread_join(voice_tid, NULL);
}

int voice_thread_get_result(char *buf, int maxlen)
{
    pthread_mutex_lock(&voice_mutex);
    if (g_ai_result[0] != '\0')
    {
        strncpy(buf, g_ai_result, maxlen - 1);
        buf[maxlen - 1] = '\0';
        /* 读取后清空 */
        g_ai_result[0] = '\0';
        pthread_mutex_unlock(&voice_mutex);
        return 1;
    }
    pthread_mutex_unlock(&voice_mutex);
    return 0;
}

int voice_thread_get_command(void)
{
    pthread_mutex_lock(&voice_mutex);
    int cmd = g_voice_command;
    pthread_mutex_unlock(&voice_mutex);
    return cmd;
}

void voice_thread_clear_command(void)
{
    pthread_mutex_lock(&voice_mutex);
    g_voice_command = VOICE_CMD_NONE;
    pthread_mutex_unlock(&voice_mutex);
}

void voice_thread_set_state_callback(void (*cb)(const char *state))
{
    (void)cb;
    /* 简化: 不使用回调，直接在 printf 输出状态 */
}

int voice_thread_is_running(void)
{
    return voice_running;
}
