#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "register.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"
#include "../ui/keyboard.h"
#include "../core/user_list.h"

void register_screen(int fb_fd, int touch_fd)
{
    Keypad kp;
    Keyboard kb;
    InputBox ib_nickname;
    InputBox ib_account;
    InputBox ib_password;
    Button btn_confirm;
    Button btn_back;
    Button btn_toggle;
    Label lbl_title;
    Label lbl_nickname;
    Label lbl_account;
    Label lbl_password;
    Label lbl_msg;

    int active_input = 0;  /* 0=昵称, 1=账号, 2=密码 */
    int use_keyboard = 0;
    char key[8];

    /* ===== 布局: 标题+行1+行2+行3+按钮行+键盘 ===== */

    keypad_init_digital(&kp, 150, 200, 400, 370);
    keyboard_init(&kb, 2, 200, 1020, 220);
    keyboard_set_mode(&kb, KB_MODE_ABC);

    /* 切换按钮 */
    btn_toggle.x = 5;
    btn_toggle.y = 165;
    btn_toggle.w = 55;
    btn_toggle.h = 32;
    btn_toggle.label = "ABC";
    btn_toggle.color_b = 150;
    btn_toggle.color_g = 100;
    btn_toggle.color_r = 200;
    btn_toggle.pressed = 0;

    /* 标题 */
    lbl_title.x = 350;
    lbl_title.y = 2;
    lbl_title.w = 300;
    lbl_title.h = 25;
    strcpy(lbl_title.text, "=== Register ===");
    lbl_title.color_b = 255; lbl_title.color_g = 255; lbl_title.color_r = 255;

    lbl_msg.x = 650;
    lbl_msg.y = 5;
    lbl_msg.w = 350;
    lbl_msg.h = 22;
    lbl_msg.text[0] = '\0';
    lbl_msg.color_b = 100; lbl_msg.color_g = 255; lbl_msg.color_r = 100;

    /* 行1: 昵称 y=30 */
    lbl_nickname.x = 10; lbl_nickname.y = 33; lbl_nickname.w = 70; lbl_nickname.h = 26;
    strcpy(lbl_nickname.text, "Name:");
    lbl_nickname.color_b = 255; lbl_nickname.color_g = 255; lbl_nickname.color_r = 255;
    memset(&ib_nickname, 0, sizeof(ib_nickname));
    ib_nickname.x = 85; ib_nickname.y = 30; ib_nickname.w = 550; ib_nickname.h = 35;
    ib_nickname.active = 1; ib_nickname.password_mode = 0;

    /* 行2: 账号 y=75 */
    lbl_account.x = 10; lbl_account.y = 78; lbl_account.w = 70; lbl_account.h = 26;
    strcpy(lbl_account.text, "Acct:");
    lbl_account.color_b = 255; lbl_account.color_g = 255; lbl_account.color_r = 255;
    memset(&ib_account, 0, sizeof(ib_account));
    ib_account.x = 85; ib_account.y = 75; ib_account.w = 550; ib_account.h = 35;
    ib_account.active = 0; ib_account.password_mode = 0;

    /* 行3: 密码 y=120 */
    lbl_password.x = 10; lbl_password.y = 123; lbl_password.w = 70; lbl_password.h = 26;
    strcpy(lbl_password.text, "Pwd:");
    lbl_password.color_b = 255; lbl_password.color_g = 255; lbl_password.color_r = 255;
    memset(&ib_password, 0, sizeof(ib_password));
    ib_password.x = 85; ib_password.y = 120; ib_password.w = 550; ib_password.h = 35;
    ib_password.active = 0; ib_password.password_mode = 1;

    /* 按钮行 y=165 */
    btn_confirm.x = 700;
    btn_confirm.y = 162;
    btn_confirm.w = 150;
    btn_confirm.h = 35;
    btn_confirm.label = "Register";
    btn_confirm.color_b = 0; btn_confirm.color_g = 180; btn_confirm.color_r = 0;
    btn_confirm.pressed = 0;

    btn_back.x = 860;
    btn_back.y = 162;
    btn_back.w = 100;
    btn_back.h = 35;
    btn_back.label = "Back";
    btn_back.color_b = 100; btn_back.color_g = 100; btn_back.color_r = 100;
    btn_back.pressed = 0;

    /* 首次绘制 */
    fb_clear(fb_fd, 30, 30, 60);
    label_draw(fb_fd, &lbl_title);
    label_draw(fb_fd, &lbl_nickname);
    label_draw(fb_fd, &lbl_account);
    label_draw(fb_fd, &lbl_password);
    label_draw(fb_fd, &lbl_msg);
    if (use_keyboard) keyboard_draw(fb_fd, &kb);
    else keypad_draw(fb_fd, &kp);
    input_draw(fb_fd, &ib_nickname);
    input_draw(fb_fd, &ib_account);
    input_draw(fb_fd, &ib_password);
    btn_draw(fb_fd, &btn_confirm);
    btn_draw(fb_fd, &btn_back);
    btn_draw(fb_fd, &btn_toggle);
    fb_flush(fb_fd);

    while (1)
    {
        TouchPoint pt;
        if (touch_read(touch_fd, &pt) != 0) continue;
        if (!pt.pressed) continue;

        /* 点击输入框 */
        if (btn_hit_test((Button*)&ib_nickname, pt.x, pt.y))
        { active_input = 0; ib_nickname.active = 1; ib_account.active = 0; ib_password.active = 0; }
        else if (btn_hit_test((Button*)&ib_account, pt.x, pt.y))
        { active_input = 1; ib_nickname.active = 0; ib_account.active = 1; ib_password.active = 0; }
        else if (btn_hit_test((Button*)&ib_password, pt.x, pt.y))
        { active_input = 2; ib_nickname.active = 0; ib_account.active = 0; ib_password.active = 1; }
        /* 切换键盘 */
        else if (btn_hit_test(&btn_toggle, pt.x, pt.y))
        {
            use_keyboard = !use_keyboard;
            btn_toggle.label = use_keyboard ? "123" : "ABC";
        }
        /* 数字键盘 */
        else if (!use_keyboard && keypad_get_key(&kp, pt.x, pt.y, key))
        {
            if (strcmp(key, "Del") == 0)
            {
                if (active_input == 0)
                    input_delete(&ib_nickname);
                else if (active_input == 1)
                    input_delete(&ib_account);
                else
                    input_delete(&ib_password);
                if (active_input == 1)
                    lbl_msg.text[0] = '\0';
            }
            else if (strcmp(key, "OK") == 0)
            {
                goto do_register;
            }
            else
            {
                if (active_input == 0)
                    input_append(&ib_nickname, key[0]);
                else if (active_input == 1)
                {
                    input_append(&ib_account, key[0]);
                    if (strlen(ib_account.text) > 0 &&
                        find_user(ib_account.text) != NULL)
                    {
                        strcpy(lbl_msg.text, "Account exists!");
                        lbl_msg.color_b = 100;
                        lbl_msg.color_g = 100;
                        lbl_msg.color_r = 255;
                    }
                    else
                    {
                        lbl_msg.text[0] = '\0';
                    }
                }
                else
                    input_append(&ib_password, key[0]);
            }
        }
        /* QWERTY键盘 */
        else if (use_keyboard && keyboard_get_key(&kb, pt.x, pt.y, key))
        {
            InputBox *cur = (active_input == 0) ? &ib_nickname :
                            (active_input == 1) ? &ib_account : &ib_password;
            if (strcmp(key, "123") == 0) keyboard_set_mode(&kb, KB_MODE_NUM);
            else if (strcmp(key, "ABC") == 0) keyboard_set_mode(&kb, KB_MODE_ABC);
            else if (strcmp(key, "Shf") == 0) {
                kb.shift = !kb.shift;
                keyboard_set_mode(&kb, kb.shift ? KB_MODE_ABC_S : KB_MODE_ABC);
            }
            else if (strcmp(key, "SYM") == 0) keyboard_set_mode(&kb, KB_MODE_SYM);
            else if (strcmp(key, "PIN") == 0) {
                keyboard_set_mode(&kb, KB_MODE_PINYIN);
                kb.pinyin_len = 0; kb.pinyin_buf[0] = '\0'; kb.show_candidates = 0;
            }
            else if (strcmp(key, "Del") == 0) input_delete(cur);
            else if (strcmp(key, "OK") == 0) goto do_register;
            else if (strcmp(key, "Space") == 0) {
                if (kb.mode == KB_MODE_PINYIN && kb.show_candidates && kb.candidate_count > 0)
                    input_append_str(cur, kb.candidates[0]);
                else input_append(cur, ' ');
                kb.pinyin_buf[0] = '\0'; kb.pinyin_len = 0;
                kb.show_candidates = 0; kb.candidate_count = 0;
            }
            else if (kb.mode == KB_MODE_PINYIN && strlen(key) == 1 && key[0] >= '1' && key[0] <= '9') {
                int idx = key[0] - '1';
                if (kb.show_candidates && idx < kb.candidate_count)
                    input_append_str(cur, kb.candidates[idx]);
                kb.pinyin_buf[0] = '\0'; kb.pinyin_len = 0;
                kb.show_candidates = 0; kb.candidate_count = 0;
                /* 检查账号是否已存在 */
                if (active_input == 1 && strlen(ib_account.text) > 0 && find_user(ib_account.text) != NULL) {
                    strcpy(lbl_msg.text, "Account exists!");
                    lbl_msg.color_b = 100; lbl_msg.color_g = 100; lbl_msg.color_r = 255;
                }
            }
            else if (kb.mode == KB_MODE_PINYIN && strlen(key) == 1 && key[0] >= 'a' && key[0] <= 'z') {
                keyboard_handle_pinyin(&kb, key, NULL, 0);
            }
            else input_append(cur, key[0]);
        }
        /* 点击确认注册 */
        else if (btn_hit_test(&btn_confirm, pt.x, pt.y))
        {
            goto do_register;
        }
        /* 点击返回 */
        else if (btn_hit_test(&btn_back, pt.x, pt.y))
        {
            return;
        }

        /* 每次触摸后重绘 */
        fb_clear(fb_fd, 30, 30, 60);
        label_draw(fb_fd, &lbl_title);
        label_draw(fb_fd, &lbl_nickname);
        label_draw(fb_fd, &lbl_account);
        label_draw(fb_fd, &lbl_password);
        label_draw(fb_fd, &lbl_msg);
        if (use_keyboard) keyboard_draw(fb_fd, &kb);
        else keypad_draw(fb_fd, &kp);
        input_draw(fb_fd, &ib_nickname);
        input_draw(fb_fd, &ib_account);
        input_draw(fb_fd, &ib_password);
        btn_draw(fb_fd, &btn_confirm);
        btn_draw(fb_fd, &btn_back);
        btn_draw(fb_fd, &btn_toggle);
        fb_flush(fb_fd);
        continue;

do_register:
        /* 验证输入 */
        if (strlen(ib_nickname.text) == 0)
        {
            strcpy(lbl_msg.text, "Enter nickname!");
            lbl_msg.color_b = 100;
            lbl_msg.color_g = 100;
            lbl_msg.color_r = 255;
            usleep(100000);
            continue;
        }

        if (strlen(ib_account.text) == 0)
        {
            strcpy(lbl_msg.text, "Enter account!");
            lbl_msg.color_b = 100;
            lbl_msg.color_g = 100;
            lbl_msg.color_r = 255;
            usleep(100000);
            continue;
        }

        if (strlen(ib_password.text) == 0)
        {
            strcpy(lbl_msg.text, "Enter password!");
            lbl_msg.color_b = 100;
            lbl_msg.color_g = 100;
            lbl_msg.color_r = 255;
            usleep(100000);
            continue;
        }

        /* 查重 */
        if (find_user(ib_account.text) != NULL)
        {
            strcpy(lbl_msg.text, "Account exists!");
            lbl_msg.color_b = 0;
            lbl_msg.color_g = 0;
            lbl_msg.color_r = 0;
            fb_clear(fb_fd, 0, 0, 0);  /* 黑色 = 注册失败 */
            printf(">>> LCD 显示：全屏黑色（注册失败 - 账号已存在）\n");
            usleep(1000000);
            continue;
        }

        /* 注册成功 */
        add_user(ib_nickname.text, ib_account.text, ib_password.text);
        printf("注册成功！昵称: %s, 账号: %s\n", ib_nickname.text, ib_account.text);

        fb_clear(fb_fd, 255, 255, 255);  /* 白色 = 注册成功 */
        printf(">>> LCD 显示：全屏白色（注册成功）\n");
        usleep(1000000);

        return;  /* 返回登录界面 */
    }
}
