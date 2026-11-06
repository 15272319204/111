#ifndef VOICE_THREAD_H
#define VOICE_THREAD_H

/* 语音控制指令码 */
#define VOICE_CMD_NONE     0
#define VOICE_CMD_ALBUM    1
#define VOICE_CMD_WRITEPAD 2
#define VOICE_CMD_HOME     3

/*
 * ===== 语音功能使用说明 =====
 *
 * 【模拟模式（默认）】
 *   编译: make
 *   语音线程使用预设的模拟数据，不连接虚拟机
 *
 * 【真实 TCP 模式】
 *   编译: make ENABLE_VOICE=1
 *   前提:
 *     1. 开发板连接 WiFi: wifi_start.sh 'SSID' 'password'
 *     2. 虚拟机运行服务器: python voice_server.py
 *     3. 修改 voice_thread.c 中的 VM_IP 为虚拟机实际 IP
 *
 * 【工作流程（按笔记 十四/十五/十六 节）】
 *   1. 开发板 arecord 录音
 *   2. TCP 发送录音文件到虚拟机
 *   3. 虚拟机: 科大讯飞 STT → demo14-1API.py → DeepSeek AI
 *   4. TCP 返回 AI 回复到开发板
 *   5. 检查语音控制指令（打开相册/写字板/返回主页）
 */

void voice_thread_start(void);
void voice_thread_stop(void);
int  voice_thread_get_result(char *buf, int maxlen);
int  voice_thread_get_command(void);
void voice_thread_clear_command(void);
void voice_thread_set_state_callback(void (*cb)(const char *state));
int  voice_thread_is_running(void);

#endif
