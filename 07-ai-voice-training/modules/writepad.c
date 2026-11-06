#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "writepad.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"

#define CANVAS_X      0
#define CANVAS_Y      0
#define CANVAS_W      SCREEN_W
#define CANVAS_H      440

#define TOOLBAR_Y     450
#define TOOLBAR_H     150

#define PEN_THIN      2
#define PEN_MEDIUM    4
#define PEN_THICK     8

/* 画笔颜色 (BGRA) */
typedef struct {
    unsigned char b, g, r;
} PenColor;

static PenColor colors[] = {
    {0,   0,   0  },   /* 黑色 */
    {0,   0,   255 },   /* 红色 */
    {0,   0,   0  },   /* 蓝色 — 修正 */
    {0,   255, 0  },   /* 绿色 */
    {0,   255, 255 },   /* 黄色 */
    {255, 255, 255 },   /* 白色（橡皮擦） */
};
#define COLOR_COUNT 6

static int current_color = 0;   /* 默认黑色 */
static int pen_size = PEN_MEDIUM;
static int eraser_mode = 0;

/* 保存画布为 BMP 文件 */
static void save_canvas_bmp(int fb_fd, const char *filename)
{
    /* 简化实现：使用 system 调用 fbgrab 或直接拷贝 framebuffer */
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dd if=/dev/fb0 of=%s bs=4096 count=600 2>/dev/null", filename);
    int ret = system(cmd);

    if (ret == 0)
    {
        printf("画布已保存到: %s\n", filename);
    }
    else
    {
        printf("画布保存失败！请确保有写入权限\n");
    }
}

void writepad_screen(int fb_fd, int touch_fd)
{
    Button btn_colors[COLOR_COUNT];
    Button btn_sizes[3];
    Button btn_clear;
    Button btn_save;
    Button btn_eraser;
    Button btn_back;
    Label lbl_info;

    const char *color_names[COLOR_COUNT] = {"K","R","B","G","Y","W"};

    /* 颜色按钮 */
    for (int i = 0; i < COLOR_COUNT; i++)
    {
        btn_colors[i].x = 10 + i * 52;
        btn_colors[i].y = TOOLBAR_Y + 5;
        btn_colors[i].w = 48;
        btn_colors[i].h = 48;
        btn_colors[i].label = color_names[i];
        btn_colors[i].color_b = colors[i].b;
        btn_colors[i].color_g = colors[i].g;
        btn_colors[i].color_r = colors[i].r;
        btn_colors[i].pressed = 0;
    }

    /* 笔触大小按钮 */
    const char *size_names[3] = {"1","2","3"};
    int sizes[3] = {PEN_THIN, PEN_MEDIUM, PEN_THICK};
    for (int i = 0; i < 3; i++)
    {
        btn_sizes[i].x = 340 + i * 70;
        btn_sizes[i].y = TOOLBAR_Y + 5;
        btn_sizes[i].w = 60;
        btn_sizes[i].h = 48;
        btn_sizes[i].label = size_names[i];
        btn_sizes[i].color_b = 80;
        btn_sizes[i].color_g = 150;
        btn_sizes[i].color_r = 200;
        btn_sizes[i].pressed = (sizes[i] == pen_size) ? 1 : 0;
    }

    /* 功能按钮 */
    btn_clear.x = 10;
    btn_clear.y = TOOLBAR_Y + 60;
    btn_clear.w = 90;
    btn_clear.h = 50;
    btn_clear.label = "Clear";
    btn_clear.color_b = 200;
    btn_clear.color_g = 100;
    btn_clear.color_r = 50;
    btn_clear.pressed = 0;

    btn_save.x = 110;
    btn_save.y = TOOLBAR_Y + 60;
    btn_save.w = 90;
    btn_save.h = 50;
    btn_save.label = "Save";
    btn_save.color_b = 50;
    btn_save.color_g = 180;
    btn_save.color_r = 50;
    btn_save.pressed = 0;

    btn_eraser.x = 210;
    btn_eraser.y = TOOLBAR_Y + 60;
    btn_eraser.w = 100;
    btn_eraser.h = 50;
    btn_eraser.label = "Eraser";
    btn_eraser.color_b = 200;
    btn_eraser.color_g = 150;
    btn_eraser.color_r = 200;
    btn_eraser.pressed = eraser_mode;

    btn_back.x = 900;
    btn_back.y = TOOLBAR_Y + 60;
    btn_back.w = 100;
    btn_back.h = 50;
    btn_back.label = "Back";
    btn_back.color_b = 100;
    btn_back.color_g = 100;
    btn_back.color_r = 100;
    btn_back.pressed = 0;

    lbl_info.x = 400;
    lbl_info.y = TOOLBAR_Y + 70;
    lbl_info.w = 300;
    lbl_info.h = 30;
    lbl_info.color_b = 200;
    lbl_info.color_g = 200;
    lbl_info.color_r = 200;

    /* 初始化画布为白色 */
    fb_clear(fb_fd, 255, 255, 255);

    int last_x = -1, last_y = -1;
    int drawing = 0;

    while (1)
    {
        /* 绘制工具栏背景 */
        fb_draw_rect(fb_fd, 0, TOOLBAR_Y, SCREEN_W, TOOLBAR_H, 50, 50, 50, 1);

        for (int i = 0; i < COLOR_COUNT; i++)
        {
            btn_colors[i].pressed = (i == current_color && !eraser_mode) ? 1 : 0;
            btn_draw(fb_fd, &btn_colors[i]);
        }

        for (int i = 0; i < 3; i++)
        {
            btn_sizes[i].pressed = (sizes[i] == pen_size) ? 1 : 0;
            btn_draw(fb_fd, &btn_sizes[i]);
        }

        btn_eraser.pressed = eraser_mode;
        btn_draw(fb_fd, &btn_clear);
        btn_draw(fb_fd, &btn_save);
        btn_draw(fb_fd, &btn_eraser);
        btn_draw(fb_fd, &btn_back);

        snprintf(lbl_info.text, sizeof(lbl_info.text),
                 "Pen: %dpx | %s",
                 pen_size, eraser_mode ? "Eraser" : "Pen");
        label_draw(fb_fd, &lbl_info);

        /* 处理触摸 */
        TouchPoint pt;
        if (touch_read(touch_fd, &pt) == 0 && pt.pressed)
        {
            /* 检查是否在画布区域 */
            if (pt.y < CANVAS_H)
            {
                drawing = 1;
                PenColor c;
                if (eraser_mode)
                {
                    c.b = 255; c.g = 255; c.r = 255;  /* 白色橡皮擦 */
                }
                else
                {
                    c = colors[current_color];
                }

                if (last_x >= 0 && last_y >= 0)
                {
                    /* 画线连接上一个点和当前点 */
                    int dx = pt.x - last_x;
                    int dy = pt.y - last_y;
                    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
                    if (steps == 0) steps = 1;

                    for (int i = 0; i <= steps; i++)
                    {
                        int px = last_x + (dx * i) / steps;
                        int py = last_y + (dy * i) / steps;
                        /* 画笔触大小的矩形 */
                        for (int dy2 = -pen_size/2; dy2 < pen_size/2; dy2++)
                        {
                            for (int dx2 = -pen_size/2; dx2 < pen_size/2; dx2++)
                            {
                                fb_draw_pixel(fb_fd, px + dx2, py + dy2, c.b, c.g, c.r);
                            }
                        }
                    }
                }
                else
                {
                    /* 单个点 */
                    for (int dy2 = -pen_size/2; dy2 < pen_size/2; dy2++)
                    {
                        for (int dx2 = -pen_size/2; dx2 < pen_size/2; dx2++)
                        {
                            fb_draw_pixel(fb_fd, pt.x + dx2, pt.y + dy2, c.b, c.g, c.r);
                        }
                    }
                }
                last_x = pt.x;
                last_y = pt.y;
            }

            /* 工具栏区域（仅处理按下） */

            /* 颜色按钮 */
            for (int i = 0; i < COLOR_COUNT; i++)
            {
                if (btn_hit_test(&btn_colors[i], pt.x, pt.y))
                {
                    current_color = i;
                    eraser_mode = 0;
                    goto redraw_toolbar;
                }
            }

            /* 笔触大小 */
            for (int i = 0; i < 3; i++)
            {
                if (btn_hit_test(&btn_sizes[i], pt.x, pt.y))
                {
                    pen_size = sizes[i];
                    goto redraw_toolbar;
                }
            }

            /* 清屏 */
            if (btn_hit_test(&btn_clear, pt.x, pt.y))
            {
                fb_draw_rect(fb_fd, CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H, 255, 255, 255, 1);
                goto redraw_toolbar;
            }

            /* 保存 */
            if (btn_hit_test(&btn_save, pt.x, pt.y))
            {
                /* 创建保存目录 */
                mkdir("/res/writepad", 0755);

                char filename[256];
                time_t t = time(NULL);
                snprintf(filename, sizeof(filename),
                         "/res/writepad/drawing_%ld.bmp", (long)t);
                save_canvas_bmp(fb_fd, filename);

                /* 显示保存成功提示 */
                fb_draw_rect(fb_fd, 300, 200, 400, 60, 50, 200, 50, 1);
                Label lbl_save;
                lbl_save.x = 320;
                lbl_save.y = 215;
                lbl_save.w = 360;
                lbl_save.h = 30;
                snprintf(lbl_save.text, sizeof(lbl_save.text), "Saved: %s", filename);
                lbl_save.color_b = 255;
                lbl_save.color_g = 255;
                lbl_save.color_r = 255;
                label_draw(fb_fd, &lbl_save);
                usleep(1500000);
                /* 恢复画布区域 */
                fb_draw_rect(fb_fd, 300, 200, 400, 60, 255, 255, 255, 1);
                goto redraw_toolbar;
            }

            /* 橡皮擦 */
            if (btn_hit_test(&btn_eraser, pt.x, pt.y))
            {
                eraser_mode = !eraser_mode;
                goto redraw_toolbar;
            }

            /* 返回 */
            if (btn_hit_test(&btn_back, pt.x, pt.y))
            {
                return;
            }
        }
        else
        {
            last_x = -1;
            last_y = -1;
        }

        fb_flush(fb_fd);
        continue;

redraw_toolbar:
        fb_flush(fb_fd);
    }
}
