#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "login.h"
#include "register.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"
#include "../ui/keyboard.h"
#include "../core/user_list.h"

/* 全局变量：记录当前登录用户昵称 */
char g_login_nickname[32] = {0};

int login_screen(int fb_fd, int touch_fd)
{
    Keypad kp;
    Keyboard kb;
    InputBox ib_account;
    InputBox ib_password;
    Button btn_login;
    Button btn_register;
    Button btn_toggle;
    Label lbl_title;
    Label lbl_account;
    Label lbl_password;
    Label lbl_msg;

    int active_input = 0;  /* 0=账号框, 1=密码框 */
    int use_keyboard = 0;  /* 0=数字键盘, 1=QWERTY键盘 */
    char key[8];
    int attempt = 0;
    int max_attempts = 3;

    /* ===== 布局设计 (1024x600) =====
     * y=5   标题
     * y=40  输入框行 (Account / Password)
     * y=100 按钮行 (Login / Register / Toggle)
     * y=155 键盘/Keypad 区域
     * y=430 错误消息
     */

    /* 初始化数字键盘 (在y=155以下) */
    keypad_init_digital(&kp, 150, 155, 400, 380);

    /* 初始化QWERTY键盘 (在y=155以下，全宽) */
    keyboard_init(&kb, 2, 155, 1020, 250);
    keyboard_set_mode(&kb, KB_MODE_ABC);

    /* 切换按钮 */
    btn_toggle.x = 850;
    btn_toggle.y = 100;
    btn_toggle.w = 150;
    btn_toggle.h = 45;
    btn_toggle.label = "ABC";
    btn_toggle.color_b = 150;
    btn_toggle.color_g = 100;
    btn_toggle.color_r = 200;
    btn_toggle.pressed = 0;

    /* 输入框 — 上方区域 */
    memset(&ib_account, 0, sizeof(ib_account));
    ib_account.x = 150;
    ib_account.y = 40;
    ib_account.w = 600;
    ib_account.h = 45;
    ib_account.active = 1;
    ib_account.password_mode = 0;

    memset(&ib_password, 0, sizeof(ib_password));
    ib_password.x = 150;
    ib_password.y = 95;
    ib_password.w = 600;
    ib_password.h = 45;
    ib_password.active = 0;
    ib_password.password_mode = 1;

    /* 登录按钮 */
    btn_login.x = 780;
    btn_login.y = 40;
    btn_login.w = 110;
    btn_login.h = 45;
    btn_login.label = "Login";
    btn_login.color_b = 0;
    btn_login.color_g = 180;
    btn_login.color_r = 0;
    btn_login.pressed = 0;

    /* 注册按钮 */
    btn_register.x = 900;
    btn_register.y = 40;
    btn_register.w = 110;
    btn_register.h = 45;
    btn_register.label = "Reg";
    btn_register.color_b = 180;
    btn_register.color_g = 180;
    btn_register.color_r = 0;
    btn_register.pressed = 0;

    /* 标签 */
    lbl_title.x = 300;
    lbl_title.y = 5;
    lbl_title.w = 400;
    lbl_title.h = 30;
    strcpy(lbl_title.text, "=== Login ===");
    lbl_title.color_b = 255;
    lbl_title.color_g = 255;
    lbl_title.color_r = 255;

    lbl_account.x = 10;
    lbl_account.y = 45;
    lbl_account.w = 130;
    lbl_account.h = 30;
    strcpy(lbl_account.text, "Account:");
    lbl_account.color_b = 255;
    lbl_account.color_g = 255;
    lbl_account.color_r = 255;

    lbl_password.x = 10;
    lbl_password.y = 100;
    lbl_password.w = 130;
    lbl_password.h = 30;
    strcpy(lbl_password.text, "Password:");
    lbl_password.color_b = 255;
    lbl_password.color_g = 255;
    lbl_password.color_r = 255;

    lbl_msg.x = 10;
    lbl_msg.y = 145;
    lbl_msg.w = 800;
    lbl_msg.h = 25;
    lbl_msg.text[0] = '\0';
    lbl_msg.color_b = 255;
    lbl_msg.color_g = 100;
    lbl_msg.color_r = 100;

    /* 首次绘制界面 */
    fb_clear(fb_fd, 30, 30, 80);
    label_draw(fb_fd, &lbl_title);
    label_draw(fb_fd, &lbl_account);
    label_draw(fb_fd, &lbl_password);
    label_draw(fb_fd, &lbl_msg);
    if (use_keyboard)
        keyboard_draw(fb_fd, &kb);
    else
        keypad_draw(fb_fd, &kp);
    input_draw(fb_fd, &ib_account);
    input_draw(fb_fd, &ib_password);
    btn_draw(fb_fd, &btn_login);
    btn_draw(fb_fd, &btn_register);
    btn_draw(fb_fd, &btn_toggle);
    fb_flush(fb_fd);

    while (1)
    {
        /* 阻塞等待触摸（和笔记文档写法一致） */
        TouchPoint pt;
        if (touch_read(touch_fd, &pt) != 0)
            continue;
        if (!pt.pressed)
            continue;

        /* 点击账号输入框 */
        if (btn_hit_test((Button*)&ib_account, pt.x, pt.y))
        {
            active_input = 0;
            ib_account.active = 1;
            ib_password.active = 0;
        }
        /* 点击密码输入框 */
        else if (btn_hit_test((Button*)&ib_password, pt.x, pt.y))
        {
            active_input = 1;
            ib_account.active = 0;
            ib_password.active = 1;
        }
        /* 点击切换按钮 */
        else if (btn_hit_test(&btn_toggle, pt.x, pt.y))
        {
            use_keyboard = !use_keyboard;
            if (use_keyboard) {
                btn_toggle.label = "123";
                btn_toggle.color_b = 100;
                btn_toggle.color_g = 150;
                btn_toggle.color_r = 200;
            } else {
                btn_toggle.label = "ABC";
                btn_toggle.color_b = 150;
                btn_toggle.color_g = 100;
                btn_toggle.color_r = 200;
            }
        }
        /* 点击数字键盘 */
        else if (!use_keyboard && keypad_get_key(&kp, pt.x, pt.y, key))
        {
            if (strcmp(key, "Del") == 0)
            {
                if (active_input == 0)
                    input_delete(&ib_account);
                else
                    input_delete(&ib_password);
            }
            else if (strcmp(key, "OK") == 0)
            {
                goto do_login;
            }
            else
            {
                if (active_input == 0)
                    input_append(&ib_account, key[0]);
                else
                    input_append(&ib_password, key[0]);
            }
        }
        /* 点击QWERTY键盘 */
        else if (use_keyboard && keyboard_get_key(&kb, pt.x, pt.y, key))
        {
            /* 模式切换键 */
            if (strcmp(key, "123") == 0) {
                keyboard_set_mode(&kb, KB_MODE_NUM);
            }
            else if (strcmp(key, "ABC") == 0) {
                keyboard_set_mode(&kb, KB_MODE_ABC);
            }
            else if (strcmp(key, "Shf") == 0) {
                kb.shift = !kb.shift;
                keyboard_set_mode(&kb, kb.shift ? KB_MODE_ABC_S : KB_MODE_ABC);
            }
            else if (strcmp(key, "SYM") == 0) {
                keyboard_set_mode(&kb, KB_MODE_SYM);
            }
            else if (strcmp(key, "PIN") == 0) {
                keyboard_set_mode(&kb, KB_MODE_PINYIN);
                kb.pinyin_len = 0;
                kb.pinyin_buf[0] = '\0';
                kb.show_candidates = 0;
            }
            else if (strcmp(key, "Del") == 0)
            {
                /* 如果拼音模式，先删拼音缓冲 */
                if (kb.mode == KB_MODE_PINYIN && kb.pinyin_len > 0) {
                    kb.pinyin_buf[--kb.pinyin_len] = '\0';
                } else {
                    if (active_input == 0)
                        input_delete(&ib_account);
                    else
                        input_delete(&ib_password);
                }
            }
            else if (strcmp(key, "OK") == 0)
            {
                goto do_login;
            }
            else if (strcmp(key, "Space") == 0)
            {
                /* 空格也尝试选择拼音候选 */
                if (kb.mode == KB_MODE_PINYIN && kb.show_candidates && kb.candidate_count > 0) {
                    if (active_input == 0) {
                        strncat(ib_account.text, kb.candidates[0], sizeof(ib_account.text) - strlen(ib_account.text) - 1);
                        ib_account.len = strlen(ib_account.text);
                    } else {
                        strncat(ib_password.text, kb.candidates[0], sizeof(ib_password.text) - strlen(ib_password.text) - 1);
                        ib_password.len = strlen(ib_password.text);
                    }
                    kb.pinyin_buf[0] = '\0';
                    kb.pinyin_len = 0;
                    kb.show_candidates = 0;
                    kb.candidate_count = 0;
                } else {
                    if (active_input == 0)
                        input_append(&ib_account, ' ');
                    else
                        input_append(&ib_password, ' ');
                }
            }
            else if (kb.mode == KB_MODE_PINYIN && strlen(key) == 1 && key[0] >= '1' && key[0] <= '9')
            {
                /* 数字键选择拼音候选字 */
                int idx = key[0] - '1';
                if (kb.show_candidates && idx < kb.candidate_count) {
                    if (active_input == 0) {
                        strncat(ib_account.text, kb.candidates[idx], sizeof(ib_account.text) - strlen(ib_account.text) - 1);
                        ib_account.len = strlen(ib_account.text);
                    } else {
                        strncat(ib_password.text, kb.candidates[idx], sizeof(ib_password.text) - strlen(ib_password.text) - 1);
                        ib_password.len = strlen(ib_password.text);
                    }
                    kb.pinyin_buf[0] = '\0';
                    kb.pinyin_len = 0;
                    kb.show_candidates = 0;
                    kb.candidate_count = 0;
                }
            }
            else if (kb.mode == KB_MODE_PINYIN && strlen(key) == 1)
            {
                /* 拼音模式：字母追加到拼音缓冲 */
                keyboard_handle_pinyin(&kb, key, NULL, 0);
                /* 如果拼音匹配成功，显示候选字 */
            }
            else
            {
                /* 普通字母/符号输入 */
                if (active_input == 0)
                    input_append(&ib_account, key[0]);
                else
                    input_append(&ib_password, key[0]);
            }
        }
        /* 点击登录按钮 */
        else if (btn_hit_test(&btn_login, pt.x, pt.y))
        {
            goto do_login;
        }
        /* 点击注册按钮 */
        else if (btn_hit_test(&btn_register, pt.x, pt.y))
        {
            register_screen(fb_fd, touch_fd);
            memset(&ib_account, 0, sizeof(ib_account));
            ib_account.x = 150; ib_account.y = 40; ib_account.w = 600; ib_account.h = 45;
            ib_account.active = 1;
            memset(&ib_password, 0, sizeof(ib_password));
            ib_password.x = 150; ib_password.y = 95; ib_password.w = 600; ib_password.h = 45;
            ib_password.active = 0;
            ib_password.password_mode = 1;
            lbl_msg.text[0] = '\0';
            attempt = 0;
        }

        /* 每次触摸后重绘 */
        fb_clear(fb_fd, 30, 30, 80);
        label_draw(fb_fd, &lbl_title);
        label_draw(fb_fd, &lbl_account);
        label_draw(fb_fd, &lbl_password);
        label_draw(fb_fd, &lbl_msg);
        if (use_keyboard)
            keyboard_draw(fb_fd, &kb);
        else
            keypad_draw(fb_fd, &kp);
        input_draw(fb_fd, &ib_account);
        input_draw(fb_fd, &ib_password);
        btn_draw(fb_fd, &btn_login);
        btn_draw(fb_fd, &btn_register);
        btn_draw(fb_fd, &btn_toggle);
        fb_flush(fb_fd);
        continue;

do_login:
        /* 执行登录验证 */
        if (strlen(ib_account.text) == 0)
        {
            strcpy(lbl_msg.text, "Enter account!");
            goto redraw_login;
        }

        User *user = find_user(ib_account.text);
        if (user == NULL)
        {
            strcpy(lbl_msg.text, "Account not found!");
            fb_clear(fb_fd, 0, 0, 255);
            fb_flush(fb_fd);
            usleep(800000);
            goto redraw_login;
        }

        if (strcmp(user->password, ib_password.text) == 0)
        {
            /* 登录成功 - 全屏绿色 */
            strcpy(g_login_nickname, user->nickname);
            fb_clear(fb_fd, 0, 255, 0);
            fb_flush(fb_fd);
            printf(">>> LCD 显示：全屏绿色（登录成功）\n");
            printf(">>> 欢迎 %s\n", user->nickname);
            usleep(1000000);
            return 1;
        }
        else
        {
            attempt++;
            if (attempt >= max_attempts)
            {
                strcpy(lbl_msg.text, "3 tries failed!");
                fb_clear(fb_fd, 0, 0, 255);
                fb_flush(fb_fd);
                printf(">>> LCD 显示：全屏红色（登录失败）\n");
                usleep(1000000);
                return 0;
            }
            else
            {
                sprintf(lbl_msg.text, "Wrong password! %d tries left", max_attempts - attempt);
                ib_password.text[0] = '\0';
                ib_password.len = 0;
            }
        }

redraw_login:
        fb_clear(fb_fd, 30, 30, 80);
        label_draw(fb_fd, &lbl_title);
        label_draw(fb_fd, &lbl_account);
        label_draw(fb_fd, &lbl_password);
        label_draw(fb_fd, &lbl_msg);
        keypad_draw(fb_fd, &kp);
        input_draw(fb_fd, &ib_account);
        input_draw(fb_fd, &ib_password);
        btn_draw(fb_fd, &btn_login);
        btn_draw(fb_fd, &btn_register);
        fb_flush(fb_fd);
        continue;
    }

    return 0;
}
