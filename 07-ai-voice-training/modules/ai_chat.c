#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ai_chat.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"
#include "../ui/keyboard.h"
#include "../core/voice_thread.h"

#define MAX_MESSAGES    20
#define MSG_MAX_LEN     256

typedef struct {
    char text[MSG_MAX_LEN];
    int  is_user;   /* 1=用户消息, 0=AI回复 */
} ChatMessage;

static ChatMessage chat_history[MAX_MESSAGES];
static int chat_count = 0;

static void add_message(const char *text, int is_user)
{
    if (chat_count >= MAX_MESSAGES)
    {
        /* 移除最旧消息 */
        for (int i = 0; i < MAX_MESSAGES - 1; i++)
        {
            chat_history[i] = chat_history[i + 1];
        }
        chat_count = MAX_MESSAGES - 1;
    }

    strncpy(chat_history[chat_count].text, text, MSG_MAX_LEN - 1);
    chat_history[chat_count].text[MSG_MAX_LEN - 1] = '\0';
    chat_history[chat_count].is_user = is_user;
    chat_count++;
}

/* 外部语音指令（供主循环检查） */
int g_voice_nav_command = 0;

void ai_chat_screen(int fb_fd, int touch_fd)
{
    Keypad kp;
    Keyboard kb;
    InputBox ib_input;
    Button btn_send;
    Button btn_toggle;
    Button btn_voice_trigger;
    Button btn_clear;
    Button btn_back;
    Label lbl_title;
    Label lbl_status;
    Label lbl_msg_labels[MAX_MESSAGES];

    int use_keyboard = 0;
    g_voice_nav_command = VOICE_CMD_NONE;

    /* 初始化 */
    chat_count = 0;
    add_message("Hello! I am XiaoZhi AI. Type or speak.", 0);

    /* ===== 布局: 聊天区(y=0-330) + 输入行(y=335-375) + 键盘(y=380-595) ===== */

    /* 数字键盘 + QWERTY键盘 (底部) */
    keypad_init_digital(&kp, 150, 380, 400, 210);
    keyboard_init(&kb, 2, 380, 1020, 210);
    keyboard_set_mode(&kb, KB_MODE_ABC);

    /* 输入框 (中间行) */
    memset(&ib_input, 0, sizeof(ib_input));
    ib_input.x = 120;
    ib_input.y = 338;
    ib_input.w = 600;
    ib_input.h = 35;
    ib_input.active = 1;
    ib_input.password_mode = 0;

    /* 按钮行 (y=335-375) */
    btn_toggle.x = 5;
    btn_toggle.y = 335;
    btn_toggle.w = 50;
    btn_toggle.h = 40;
    btn_toggle.label = "Ab";
    btn_toggle.color_b = 150;
    btn_toggle.color_g = 100;
    btn_toggle.color_r = 200;
    btn_toggle.pressed = 0;

    btn_voice_trigger.x = 58;
    btn_voice_trigger.y = 335;
    btn_voice_trigger.w = 55;
    btn_voice_trigger.h = 40;
    btn_voice_trigger.label = "Mic";
    btn_voice_trigger.color_b = 200;
    btn_voice_trigger.color_g = 100;
    btn_voice_trigger.color_r = 200;
    btn_voice_trigger.pressed = 0;

    btn_clear.x = 730;
    btn_clear.y = 335;
    btn_clear.w = 65;
    btn_clear.h = 40;
    btn_clear.label = "Clr";
    btn_clear.color_b = 100;
    btn_clear.color_g = 100;
    btn_clear.color_r = 100;
    btn_clear.pressed = 0;

    btn_send.x = 800;
    btn_send.y = 335;
    btn_send.w = 100;
    btn_send.h = 40;
    btn_send.label = "Send";
    btn_send.color_b = 0;
    btn_send.color_g = 180;
    btn_send.color_r = 0;
    btn_send.pressed = 0;

    btn_back.x = 910;
    btn_back.y = 335;
    btn_back.w = 100;
    btn_back.h = 40;
    btn_back.label = "Back";
    btn_back.color_b = 100;
    btn_back.color_g = 100;
    btn_back.color_r = 100;
    btn_back.pressed = 0;

    /* 标题 */
    lbl_title.x = 300;
    lbl_title.y = 2;
    lbl_title.w = 400;
    lbl_title.h = 25;
    strcpy(lbl_title.text, "=== AI Chat ===");
    lbl_title.color_b = 255;
    lbl_title.color_g = 255;
    lbl_title.color_r = 255;

    /* 状态条 (标题下方) */
    lbl_status.x = 10;
    lbl_status.y = 2;
    lbl_status.w = 250;
    lbl_status.h = 25;
    strcpy(lbl_status.text, "Waiting...");
    lbl_status.color_b = 100;
    lbl_status.color_g = 255;
    lbl_status.color_r = 100;

    char key[8];

    while (1)
    {
        fb_clear(fb_fd, 20, 20, 40);

        /* 标题栏背景 */
        fb_draw_rect(fb_fd, 0, 0, SCREEN_W, 27, 30, 30, 60, 1);
        label_draw(fb_fd, &lbl_title);
        label_draw(fb_fd, &lbl_status);

        /* 绘制对话历史（最近 8 条，y=30-328） */
        int start_msg = chat_count > 8 ? chat_count - 8 : 0;
        for (int i = start_msg; i < chat_count; i++)
        {
            int display_idx = i - start_msg;
            int msg_y = 32 + display_idx * 36;

            if (chat_history[i].is_user)
            {
                int bw = strlen(chat_history[i].text) * 14 + 40;
                if (bw > 700) bw = 700;
                if (bw < 60) bw = 60;
                fb_draw_rect(fb_fd, 20, msg_y, bw, 34, 80, 80, 80, 1);
                fb_draw_text(fb_fd, 25, msg_y + 2, chat_history[i].text, 200, 200, 200, 2);
            }
            else
            {
                int bw = strlen(chat_history[i].text) * 14 + 40;
                if (bw > 700) bw = 700;
                if (bw < 60) bw = 60;
                int bx = SCREEN_W - bw - 20;
                fb_draw_rect(fb_fd, bx, msg_y, bw, 34, 40, 40, 120, 1);
                fb_draw_text(fb_fd, bx + 5, msg_y + 2, chat_history[i].text, 200, 200, 255, 2);
            }
        }

        /* 绘制输入区域 */
        if (use_keyboard)
            keyboard_draw(fb_fd, &kb);
        else
            keypad_draw(fb_fd, &kp);
        input_draw(fb_fd, &ib_input);
        btn_draw(fb_fd, &btn_send);
        btn_draw(fb_fd, &btn_toggle);
        btn_draw(fb_fd, &btn_voice_trigger);
        btn_draw(fb_fd, &btn_clear);
        btn_draw(fb_fd, &btn_back);
        fb_flush(fb_fd);

        /* 检查语音识别结果 */
        char ai_result[1024];
        if (voice_thread_get_result(ai_result, sizeof(ai_result)))
        {
            add_message(ai_result, 0);
            fb_flush(fb_fd);
        }

        /* 检查语音导航指令 */
        int voice_cmd = voice_thread_get_command();
        if (voice_cmd != VOICE_CMD_NONE)
        {
            g_voice_nav_command = voice_cmd;
            voice_thread_clear_command();
            return; /* 返回 main_menu，由其负责跳转 */
        }

        /* 触摸处理 */
        TouchPoint pt;
        if (touch_read(touch_fd, &pt) == 0 && pt.pressed)
        {
            /* 点击输入框 */
            if (btn_hit_test((Button*)&ib_input, pt.x, pt.y))
            {
                ib_input.active = 1;
            }
            /* 切换按钮 */
            else if (btn_hit_test(&btn_toggle, pt.x, pt.y))
            {
                use_keyboard = !use_keyboard;
                btn_toggle.label = use_keyboard ? "123" : "ABC";
            }
            /* 数字键盘 */
            else if (!use_keyboard && keypad_get_key(&kp, pt.x, pt.y, key))
            {
                if (strcmp(key, "Del") == 0)
                    input_delete(&ib_input);
                else if (strcmp(key, "OK") == 0)
                    goto send_message;
                else
                    input_append(&ib_input, key[0]);
            }
            /* QWERTY键盘 */
            else if (use_keyboard && keyboard_get_key(&kb, pt.x, pt.y, key))
            {
                if (strcmp(key, "123") == 0) keyboard_set_mode(&kb, KB_MODE_NUM);
                else if (strcmp(key, "ABC") == 0) keyboard_set_mode(&kb, KB_MODE_ABC);
                else if (strcmp(key, "Shf") == 0) {
                    kb.shift = !kb.shift;
                    keyboard_set_mode(&kb, kb.shift ? KB_MODE_ABC_S : KB_MODE_ABC);
                }
                else if (strcmp(key, "SYM") == 0) keyboard_set_mode(&kb, KB_MODE_SYM);
                else if (strcmp(key, "PIN") == 0) {
                    keyboard_set_mode(&kb, KB_MODE_PINYIN);
                    kb.pinyin_len = 0; kb.pinyin_buf[0] = '\0';
                    kb.show_candidates = 0;
                }
                else if (strcmp(key, "Del") == 0) input_delete(&ib_input);
                else if (strcmp(key, "OK") == 0) goto send_message;
                else if (strcmp(key, "Space") == 0) {
                    if (kb.mode == KB_MODE_PINYIN && kb.show_candidates && kb.candidate_count > 0)
                        input_append_str(&ib_input, kb.candidates[0]);
                    else input_append(&ib_input, ' ');
                    kb.pinyin_buf[0] = '\0'; kb.pinyin_len = 0;
                    kb.show_candidates = 0; kb.candidate_count = 0;
                }
                else if (kb.mode == KB_MODE_PINYIN && strlen(key) == 1 && key[0] >= '1' && key[0] <= '9') {
                    int idx = key[0] - '1';
                    if (kb.show_candidates && idx < kb.candidate_count)
                        input_append_str(&ib_input, kb.candidates[idx]);
                    kb.pinyin_buf[0] = '\0'; kb.pinyin_len = 0;
                    kb.show_candidates = 0; kb.candidate_count = 0;
                }
                else if (kb.mode == KB_MODE_PINYIN && strlen(key) == 1 && key[0] >= 'a' && key[0] <= 'z') {
                    keyboard_handle_pinyin(&kb, key, NULL, 0);
                }
                else input_append(&ib_input, key[0]);
            }
            /* 发送按钮 */
            else if (btn_hit_test(&btn_send, pt.x, pt.y)) goto send_message;
            /* 语音触发 */
            else if (btn_hit_test(&btn_voice_trigger, pt.x, pt.y))
            {
                strcpy(lbl_status.text, "Status: Say 'XiaoZhi'...");
            }
            /* 清空 */
            else if (btn_hit_test(&btn_clear, pt.x, pt.y))
            {
                chat_count = 0;
                add_message("Chat cleared.", 0);
            }
            /* 返回 */
            else if (btn_hit_test(&btn_back, pt.x, pt.y)) return;
        }

        continue;

send_message:
        if (strlen(ib_input.text) > 0)
        {
            /* 添加用户消息 */
            add_message(ib_input.text, 1);
            printf("[AI对话] 用户: %s\n", ib_input.text);

            /* 模拟 AI 回复（在实际系统中，此处应调用 DeepSeek API） */
            /* 语音线程在后台处理实际的 API 调用 */
            char reply[256];
            if (strstr(ib_input.text, "天气"))
                strcpy(reply, "Nice weather today!");
            else if (strstr(ib_input.text, "你好") || strstr(ib_input.text, "嗨"))
                strcpy(reply, "Hello! How can I help?");
            else if (strstr(ib_input.text, "笑话"))
                strcpy(reply, "程序员最讨厌的数字是什么？1024，因为1024=1K，而1K=1024... 好吧这个笑话不太好笑 😅");
            else if (strstr(ib_input.text, "Album"))
                strcpy(reply, "好的，正在为您打开相册...");
            else if (strstr(ib_input.text, "Draw"))
                strcpy(reply, "好的，正在为您打开写字板...");
            else
                snprintf(reply, sizeof(reply), "收到: %s。我是小智 AI，语音识别结果会由后台线程处理。", ib_input.text);

            add_message(reply, 0);
            printf("[AI对话] 小智: %s\n", reply);

            /* 检查是否需要导航 */
            if (strstr(ib_input.text, "Album")) {
                g_voice_nav_command = VOICE_CMD_ALBUM; return;
            }
            else if (strstr(ib_input.text, "Draw")) {
                g_voice_nav_command = VOICE_CMD_WRITEPAD; return;
            }
            else if (strstr(ib_input.text, "Back") || strstr(ib_input.text, "Home")) {
                g_voice_nav_command = VOICE_CMD_HOME; return;
            }

            input_clear(&ib_input);
            strcpy(lbl_status.text, "Status: 待命中...");
            lbl_status.color_b = 100;
            lbl_status.color_g = 255;
            lbl_status.color_r = 100;
        }
    }
}
