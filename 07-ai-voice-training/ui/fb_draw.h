#ifndef FB_DRAW_H
#define FB_DRAW_H

#define SCREEN_W  1024
#define SCREEN_H  600
#define BUF_SIZE  (SCREEN_W * SCREEN_H * 4)

int  fb_open(void);
void fb_close(int fd);
void fb_clear(int fd, unsigned char b, unsigned char g, unsigned char r);
void fb_draw_pixel(int fd, int x, int y, unsigned char b, unsigned char g, unsigned char r);
void fb_draw_rect(int fd, int x, int y, int w, int h, unsigned char b, unsigned char g, unsigned char r, int filled);
void fb_draw_text(int fd, int x, int y, const char *text, unsigned char b, unsigned char g, unsigned char r, int scale);
void fb_draw_bmp(int fd, int x, int y, int w, int h, const char *path);
void fb_flush(int fd);
unsigned char* fb_get_buffer(int fd);

#endif
