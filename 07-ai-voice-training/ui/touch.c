#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include "touch.h"

int touch_open(void)
{
    int fd = open(TOUCH_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("touch_open: open " TOUCH_DEVICE);
    }
    /* 阻塞模式，和笔记文档写法一致 */
    return fd;
}

void touch_close(int fd)
{
    if (fd >= 0)
        close(fd);
}

int touch_read(int fd, TouchPoint *pt)
{
    if (fd < 0 || !pt)
        return -1;

    struct input_event ev;
    int have_x = 0, have_y = 0;
    int got_syn = 0;

    /* read events until we hit SYN_REPORT or run out of data */
    while (1) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n < 0) {
            /* no data available (non-blocking) or real error */
            return -1;
        }
        if (n == 0) {
            /* EOF — should not happen for evdev */
            return -1;
        }
        if (n != sizeof(ev)) {
            /* partial read */
            return -1;
        }

        switch (ev.type) {
        case EV_ABS:
            if (ev.code == ABS_X) {
                pt->x = ev.value;
                have_x = 1;
            } else if (ev.code == ABS_Y) {
                pt->y = ev.value;
                have_y = 1;
            }
            /* Multi-touch protocol */
            else if (ev.code == ABS_MT_POSITION_X) {
                pt->x = ev.value;
                have_x = 1;
            } else if (ev.code == ABS_MT_POSITION_Y) {
                pt->y = ev.value;
                have_y = 1;
            } else if (ev.code == ABS_MT_TRACKING_ID) {
                /* tracking_id >= 0 = finger down, -1 = finger up */
                pt->pressed = (ev.value >= 0) ? 1 : 0;
            }
            break;

        case EV_KEY:
            if (ev.code == BTN_TOUCH) {
                pt->pressed = ev.value;  /* 1 = pressed, 0 = released */
            }
            break;

        case EV_SYN:
            got_syn = 1;
            break;

        default:
            break;
        }

        if (got_syn)
            break;
    }

    /* if we never got X or Y, use defaults (last known or 0) */
    if (!have_x) pt->x = 0;
    if (!have_y) pt->y = 0;

    return 0;
}

/* 非阻塞版本 — 用于需要持续渲染的场景（如幻灯片） */
int touch_read_try(int fd, TouchPoint *pt)
{
    if (fd < 0 || !pt) return -1;

    /* 设置非阻塞 */
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    int ret = touch_read(fd, pt);

    /* 恢复阻塞 */
    if (flags >= 0) fcntl(fd, F_SETFL, flags);

    return ret;
}
