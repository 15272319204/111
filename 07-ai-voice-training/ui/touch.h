#ifndef TOUCH_H
#define TOUCH_H

#define TOUCH_DEVICE "/dev/input/event2"  /* Capacitance_ts */

typedef struct {
    int x;
    int y;
    int pressed;  // 1=pressed, 0=released
} TouchPoint;

int  touch_open(void);
void touch_close(int fd);
int  touch_read(int fd, TouchPoint *pt);      /* 阻塞读取 */
int  touch_read_try(int fd, TouchPoint *pt);  /* 非阻塞读取，无数据立即返回-1 */

#endif
