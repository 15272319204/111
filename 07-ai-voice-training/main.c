/**
 * Proxm 嵌入式多功能系统 — 主入口
 *
 * 平台: aarch64 Linux 嵌入式开发板
 * LCD: 1024x600 framebuffer (/dev/fb0)
 * 输入: 触摸屏 (evdev)
 *
 * 功能:
 *   1. 登录/注册（链表管理用户，数字键盘输入）
 *   2. 相册（4 种分类，缩略图/全屏/幻灯片模式，4 种转场特效）
 *   3. 写字板（触摸绘制，颜色/笔触选择，保存BMP）
 *   4. 音视频播放（mplayer 后端，播放列表和控制栏）
 *   5. AI 语音对话（pthread 后台语音识别 + DeepSeek API）
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ui/fb_draw.h"
#include "ui/touch.h"
#include "core/user_list.h"
#include "core/file_io.h"
#include "core/voice_thread.h"
#include "modules/login.h"
#include "modules/main_menu.h"

#define SAVE_FILE "user_data.txt"

/* 外部引用：语音导航指令（ai_chat 模块设置） */
extern int g_voice_nav_command;

int main(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║     Proxm 嵌入式多功能系统 v1.0          ║\n");
    printf("║     aarch64 Linux 平台                   ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");

    /* ===== 1. 打开设备 ===== */
    printf("[初始化] 正在打开 LCD 设备 (%s)...\n", "/dev/fb0");
    int fb_fd = fb_open();
    if (fb_fd == -1)
    {
        printf("[错误] 无法打开 LCD 设备，程序退出\n");
        return -1;
    }
    printf("[初始化] LCD 设备打开成功\n");

    printf("[初始化] 正在打开触摸设备 (%s)...\n", TOUCH_DEVICE);
    int touch_fd = touch_open();
    if (touch_fd == -1)
    {
        printf("[警告] 无法打开触摸设备，将仅使用键盘输入\n");
        /* 触摸不可用不致命，继续运行 */
    }

    /* ===== 2. 加载已有用户数据 ===== */
    printf("[初始化] 正在加载用户数据...\n");
    load_users_from_file(SAVE_FILE);

    /* ===== 3. 启动语音识别线程 ===== */
    printf("[初始化] 正在启动语音识别线程...\n");
    voice_thread_start();

    /* ===== 4. 显示启动画面 ===== */
    fb_clear(fb_fd, 0, 0, 50);  /* 深蓝背景 */
    fb_draw_rect(fb_fd, 250, 250, 500, 80, 0, 100, 200, 1);
    fb_draw_text(fb_fd, 300, 270, "Proxm 多功能系统", 255, 255, 255, 2);
    usleep(1500000);  /* 1.5 秒启动画面 */

    /* ===== 5. 登录循环 ===== */
    int logged_in = 0;
    while (!logged_in)
    {
        printf("\n===== 进入登录界面 =====\n");
        if (login_screen(fb_fd, touch_fd))
        {
            logged_in = 1;
            printf("登录成功！进入主菜单...\n");
        }
        else
        {
            printf("登录失败，重新进入登录界面...\n");
            /* 登录失败后短暂等待再重试 */
            fb_clear(fb_fd, 0, 0, 0);
            usleep(500000);
        }
    }

    /* ===== 6. 主菜单循环 ===== */
    while (1)
    {
        printf("\n===== 主菜单 =====\n");
        int ret = main_menu_screen(fb_fd, touch_fd);
        if (ret == 0)
        {
            printf("用户选择退出系统\n");
            break;  /* 退出 */
        }
    }

    /* ===== 7. 退出流程 ===== */
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║           正在退出系统...                 ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("\n");

    /* 7.1 打印所有用户信息 */
    print_all_users();

    /* 7.2 保存到文件 */
    save_users_to_file(SAVE_FILE);

    /* 7.3 回传文件到 PC */
    transfer_file_to_pc(SAVE_FILE);

    /* 7.4 释放链表内存 */
    free_all_users();
    printf("用户链表已释放\n");

    /* 7.5 停止语音线程 */
    voice_thread_stop();
    printf("语音线程已停止\n");

    /* ===== 8. 关闭设备 ===== */
    if (touch_fd != -1)
    {
        touch_close(touch_fd);
        printf("触摸设备已关闭\n");
    }

    fb_close(fb_fd);
    printf("LCD 设备已关闭\n");

    printf("\n系统已正常退出，再见！\n");
    return 0;
}
