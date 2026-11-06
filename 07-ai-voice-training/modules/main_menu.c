#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main_menu.h"
#include "album.h"
#include "writepad.h"
#include "media_player.h"
#include "ai_chat.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"

/* 外部引用登录模块的全局变量 */
extern char g_login_nickname[32];

int main_menu_screen(int fb_fd, int touch_fd)
{
    Button btn_album;
    Button btn_writepad;
    Button btn_media;
    Button btn_ai;
    Button btn_exit;
    Label lbl_title;
    Label lbl_welcome;

    int btn_w = 220;
    int btn_h = 120;
    int start_x = 60;
    int start_y = 150;
    int gap = 20;

    /* 标题 */
    lbl_title.x = 200;
    lbl_title.y = 20;
    lbl_title.w = 600;
    lbl_title.h = 40;
    strcpy(lbl_title.text, "=== Main Menu ===");
    lbl_title.color_b = 255;
    lbl_title.color_g = 255;
    lbl_title.color_r = 255;

    /* 欢迎语 */
    lbl_welcome.x = 700;
    lbl_welcome.y = 20;
    lbl_welcome.w = 300;
    lbl_welcome.h = 40;
    snprintf(lbl_welcome.text, sizeof(lbl_welcome.text), "欢迎, %s", g_login_nickname);
    lbl_welcome.color_b = 100;
    lbl_welcome.color_g = 255;
    lbl_welcome.color_r = 100;

    /* 相册按钮 */
    btn_album.x = start_x;
    btn_album.y = start_y;
    btn_album.w = btn_w;
    btn_album.h = btn_h;
    btn_album.label = "Album";
    btn_album.color_b = 100;
    btn_album.color_g = 150;
    btn_album.color_r = 255;
    btn_album.pressed = 0;

    /* 写字板按钮 */
    btn_writepad.x = start_x + btn_w + gap;
    btn_writepad.y = start_y;
    btn_writepad.w = btn_w;
    btn_writepad.h = btn_h;
    btn_writepad.label = "Draw";
    btn_writepad.color_b = 100;
    btn_writepad.color_g = 255;
    btn_writepad.color_r = 150;
    btn_writepad.pressed = 0;

    /* 音视频按钮 */
    btn_media.x = start_x + (btn_w + gap) * 2;
    btn_media.y = start_y;
    btn_media.w = btn_w;
    btn_media.h = btn_h;
    btn_media.label = "Media";
    btn_media.color_b = 255;
    btn_media.color_g = 200;
    btn_media.color_r = 50;
    btn_media.pressed = 0;

    /* AI语音按钮 */
    btn_ai.x = start_x + (btn_w + gap) * 3;
    btn_ai.y = start_y;
    btn_ai.w = btn_w;
    btn_ai.h = btn_h;
    btn_ai.label = "AI Chat";
    btn_ai.color_b = 255;
    btn_ai.color_g = 100;
    btn_ai.color_r = 200;
    btn_ai.pressed = 0;

    /* 退出按钮 */
    btn_exit.x = 350;
    btn_exit.y = 400;
    btn_exit.w = 300;
    btn_exit.h = 80;
    btn_exit.label = "Exit";
    btn_exit.color_b = 50;
    btn_exit.color_g = 50;
    btn_exit.color_r = 200;
    btn_exit.pressed = 0;

    /* 首次绘制 */
    fb_clear(fb_fd, 20, 20, 50);
    label_draw(fb_fd, &lbl_title);
    label_draw(fb_fd, &lbl_welcome);
    btn_draw(fb_fd, &btn_album);
    btn_draw(fb_fd, &btn_writepad);
    btn_draw(fb_fd, &btn_media);
    btn_draw(fb_fd, &btn_ai);
    btn_draw(fb_fd, &btn_exit);
    fb_flush(fb_fd);

    while (1)
    {
        /* 阻塞等待触摸 */
        TouchPoint pt;
        if (touch_read(touch_fd, &pt) != 0)
            continue;
        if (!pt.pressed)
            continue;

        if (btn_hit_test(&btn_album, pt.x, pt.y))
        {
            album_screen(fb_fd, touch_fd);
        }
        else if (btn_hit_test(&btn_writepad, pt.x, pt.y))
        {
            writepad_screen(fb_fd, touch_fd);
        }
        else if (btn_hit_test(&btn_media, pt.x, pt.y))
        {
            media_player_screen(fb_fd, touch_fd);
        }
        else if (btn_hit_test(&btn_ai, pt.x, pt.y))
        {
            ai_chat_screen(fb_fd, touch_fd);
            /* 检查语音指令导航 */
            extern int g_voice_nav_command;
            if (g_voice_nav_command == 1) {
                album_screen(fb_fd, touch_fd);
                g_voice_nav_command = 0;
            } else if (g_voice_nav_command == 2) {
                writepad_screen(fb_fd, touch_fd);
                g_voice_nav_command = 0;
            }
        }
        else if (btn_hit_test(&btn_exit, pt.x, pt.y))
        {
            return 0;
        }

        /* 从子模块返回后重绘 */
        fb_clear(fb_fd, 20, 20, 50);
        label_draw(fb_fd, &lbl_title);
        label_draw(fb_fd, &lbl_welcome);
        btn_draw(fb_fd, &btn_album);
        btn_draw(fb_fd, &btn_writepad);
        btn_draw(fb_fd, &btn_media);
        btn_draw(fb_fd, &btn_ai);
        btn_draw(fb_fd, &btn_exit);
        fb_flush(fb_fd);
    }

    return 0;
}
