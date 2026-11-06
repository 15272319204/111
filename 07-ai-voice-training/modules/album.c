#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "album.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"

#define ALBUM_COUNT    4
#define MAX_IMAGES     50
#define THUMB_COLS     3
#define THUMB_ROWS     2
#define THUMB_W        280
#define THUMB_H        200
#define THUMB_GAP_X    40
#define THUMB_GAP_Y    30
#define THUMB_START_X  50
#define THUMB_START_Y  80

typedef enum { ALBUM_SCENERY=0, ALBUM_PEOPLE=1, ALBUM_ANIMAL=2, ALBUM_FOOD=3 } AlbumType;
typedef enum { MODE_THUMBNAIL=0, MODE_FULLSCREEN=1, MODE_SLIDESHOW=2 } BrowseMode;
typedef enum { EFFECT_FADE=0, EFFECT_SLIDE=1, EFFECT_ZOOM=2, EFFECT_ROTATE=3 } TransitionEffect;

static const char *album_names[ALBUM_COUNT] = {"Scenery","People","Animal","Food"};
static const char *album_dirs[ALBUM_COUNT] = {
    "/res/album/scenery/","/res/album/people/","/res/album/animal/","/res/album/food/"
};
static TransitionEffect album_effects[ALBUM_COUNT] = {EFFECT_FADE, EFFECT_SLIDE, EFFECT_ZOOM, EFFECT_ROTATE};

static char image_list[MAX_IMAGES][256];
static int  image_count = 0, current_index = 0, current_album = 0;
static BrowseMode current_mode = MODE_THUMBNAIL;
static int slideshow_running = 0, slideshow_interval = 2;

static int scan_images(const char *dir_path)
{
    image_count = 0;
    DIR *dir = opendir(dir_path);
    if (!dir) { printf("Album dir %s not found\n", dir_path); return 0; }
    struct dirent *e;
    while ((e = readdir(dir)) && image_count < MAX_IMAGES) {
        int l = strlen(e->d_name);
        if (l > 4 && strcasecmp(e->d_name + l - 4, ".bmp") == 0) {
            snprintf(image_list[image_count], sizeof(image_list[image_count]), "%s%s", dir_path, e->d_name);
            image_count++;
        }
    }
    closedir(dir);
    return image_count;
}

/* ---- 过渡特效 ---- */
static void effect_fade(int fd, const char *o, const char *n) {
    (void)o; fb_clear(fd,0,0,0); fb_draw_bmp(fd,0,0,SCREEN_W,SCREEN_H,n); fb_flush(fd);
}
static void effect_slide(int fd, const char *o, const char *n) {
    for (int i=1;i<=10;i++) {
        int off=(SCREEN_W*i)/10;
        fb_clear(fd,0,0,0);
        if(o) fb_draw_bmp(fd,-off,0,SCREEN_W,SCREEN_H,o);
        fb_draw_bmp(fd,SCREEN_W-off,0,SCREEN_W,SCREEN_H,n);
        fb_flush(fd); usleep(30000);
    }
}
static void effect_zoom(int fd, const char *o, const char *n) {
    (void)o;
    for (int i=1;i<=8;i++) {
        int w=(SCREEN_W*i)/8, h=(SCREEN_H*i)/8;
        fb_clear(fd,0,0,0);
        fb_draw_bmp(fd,(SCREEN_W-w)/2,(SCREEN_H-h)/2,w,h,n);
        fb_flush(fd); usleep(40000);
    }
}
static void effect_rotate(int fd, const char *o, const char *n) {
    (void)o;
    for (int i=1;i<=10;i++) {
        int w=(SCREEN_W*i)/10;
        fb_clear(fd,0,0,0);
        fb_draw_bmp(fd,(SCREEN_W-w)/2,0,w,SCREEN_H,n);
        fb_flush(fd); usleep(30000);
    }
}
static void apply_transition(int fd, const char *old, const char *new, TransitionEffect e) {
    if (!old || image_count<=1) { fb_draw_bmp(fd,0,0,SCREEN_W,SCREEN_H,new); return; }
    switch(e) {
        case EFFECT_FADE: effect_fade(fd,old,new); break;
        case EFFECT_SLIDE: effect_slide(fd,old,new); break;
        case EFFECT_ZOOM: effect_zoom(fd,old,new); break;
        case EFFECT_ROTATE: effect_rotate(fd,old,new); break;
        default: fb_draw_bmp(fd,0,0,SCREEN_W,SCREEN_H,new); break;
    }
}

/* ---- 绘制辅助 ---- */
static void draw_common(int fd, Label *lbl, Button *btns, int n, Button *back)
{
    label_draw(fd, lbl);
    for (int i=0;i<n;i++) btn_draw(fd,&btns[i]);
    if (back) btn_draw(fd,back);
}

/* ---- 相册选择页 ---- */
static int album_select_page(int fb_fd, int touch_fd)
{
    Button btns[ALBUM_COUNT], btn_back;
    Label lbl_title;
    lbl_title.x=300;lbl_title.y=40;lbl_title.w=400;lbl_title.h=40;
    strcpy(lbl_title.text,"=== Album ===");
    lbl_title.color_b=255;lbl_title.color_g=255;lbl_title.color_r=255;
    for (int i=0;i<ALBUM_COUNT;i++) {
        btns[i].x=52+i*240; btns[i].y=200; btns[i].w=200; btns[i].h=150;
        btns[i].label=album_names[i];
        btns[i].color_b=80+i*40; btns[i].color_g=100+i*30; btns[i].color_r=200-i*20;
        btns[i].pressed=0;
    }
    btn_back.x=400;btn_back.y=480;btn_back.w=200;btn_back.h=60;
    btn_back.label="Back"; btn_back.color_b=100;btn_back.color_g=100;btn_back.color_r=100;btn_back.pressed=0;

    /* 先画第一帧 */
    fb_clear(fb_fd,25,25,50);
    draw_common(fb_fd,&lbl_title,btns,ALBUM_COUNT,&btn_back);
    fb_flush(fb_fd);

    while (1) {
        TouchPoint pt;
        if (touch_read(touch_fd,&pt)!=0) continue;
        if (!pt.pressed) continue;
        for (int i=0;i<ALBUM_COUNT;i++)
            if (btn_hit_test(&btns[i],pt.x,pt.y)) return i;
        if (btn_hit_test(&btn_back,pt.x,pt.y)) return -1;
        /* 重绘 */
        fb_clear(fb_fd,25,25,50);
        draw_common(fb_fd,&lbl_title,btns,ALBUM_COUNT,&btn_back);
        fb_flush(fb_fd);
    }
}

/* ---- 缩略图模式 ---- */
static int thumbnail_mode(int fb_fd, int touch_fd, TransitionEffect effect)
{
    Button btn_prev, btn_next, btn_tab[3], btn_back;
    Label lbl_info;
    int pp=THUMB_COLS*THUMB_ROWS, total=(image_count+pp-1)/pp, page=0;

    btn_prev.x=30;btn_prev.y=500;btn_prev.w=80;btn_prev.h=50;btn_prev.label="<";btn_prev.color_b=80;btn_prev.color_g=80;btn_prev.color_r=200;btn_prev.pressed=0;
    btn_next.x=250;btn_next.y=500;btn_next.w=80;btn_next.h=50;btn_next.label=">";btn_next.color_b=80;btn_next.color_g=80;btn_next.color_r=200;btn_next.pressed=0;

    const char *tabs[3]={"Thumb","Full","Slide"};
    for (int i=0;i<3;i++) {
        btn_tab[i].x=400+i*160;btn_tab[i].y=510;btn_tab[i].w=140;btn_tab[i].h=45;
        btn_tab[i].label=tabs[i];btn_tab[i].pressed=0;
        btn_tab[i].color_b=(i==0)?0:100; btn_tab[i].color_g=(i==0)?200:100; btn_tab[i].color_r=(i==0)?0:100;
    }
    btn_back.x=900;btn_back.y=510;btn_back.w=100;btn_back.h=45;btn_back.label="Back";btn_back.color_b=100;btn_back.color_g=100;btn_back.color_r=100;btn_back.pressed=0;
    lbl_info.x=370;lbl_info.y=560;lbl_info.w=280;lbl_info.h=30;lbl_info.color_b=200;lbl_info.color_g=200;lbl_info.color_r=200;

    /* 首次绘制 */
    fb_clear(fb_fd,30,30,30);
    int si=page*pp;
    for (int i=0;i<pp&&(si+i)<image_count;i++) {
        int tx=THUMB_START_X+(i%THUMB_COLS)*(THUMB_W+THUMB_GAP_X);
        int ty=THUMB_START_Y+(i/THUMB_COLS)*(THUMB_H+THUMB_GAP_Y);
        fb_draw_rect(fb_fd,tx-2,ty-2,THUMB_W+4,THUMB_H+4,150,150,150,0);
        fb_draw_bmp(fb_fd,tx,ty,THUMB_W,THUMB_H,image_list[si+i]);
        if ((si+i)==current_index) fb_draw_rect(fb_fd,tx-3,ty-3,THUMB_W+6,THUMB_H+6,0,255,0,0);
    }
    snprintf(lbl_info.text,sizeof(lbl_info.text),"Page %d/%d | %d pics",page+1,total,image_count);
    label_draw(fb_fd,&lbl_info);
    if (page>0) btn_draw(fb_fd,&btn_prev);
    if (page<total-1) btn_draw(fb_fd,&btn_next);
    for (int i=0;i<3;i++) btn_draw(fb_fd,&btn_tab[i]);
    btn_draw(fb_fd,&btn_back);
    fb_flush(fb_fd);

    while (1) {
        TouchPoint pt;
        if (touch_read(touch_fd,&pt)!=0) continue;
        if (!pt.pressed) continue;
        /* 缩略图点击 */
        for (int i=0;i<pp&&(si+i)<image_count;i++) {
            int tx=THUMB_START_X+(i%THUMB_COLS)*(THUMB_W+THUMB_GAP_X);
            int ty=THUMB_START_Y+(i/THUMB_COLS)*(THUMB_H+THUMB_GAP_Y);
            if (pt.x>=tx&&pt.x<=tx+THUMB_W&&pt.y>=ty&&pt.y<=ty+THUMB_H) {
                current_index=si+i; return MODE_FULLSCREEN;
            }
        }
        if (page>0&&btn_hit_test(&btn_prev,pt.x,pt.y)) page--;
        else if (page<total-1&&btn_hit_test(&btn_next,pt.x,pt.y)) page++;
        else if (btn_hit_test(&btn_tab[1],pt.x,pt.y)) return MODE_FULLSCREEN;
        else if (btn_hit_test(&btn_tab[2],pt.x,pt.y)) return MODE_SLIDESHOW;
        else if (btn_hit_test(&btn_back,pt.x,pt.y)) return -1;
        else continue;
        /* 重绘 */
        fb_clear(fb_fd,30,30,30);
        si=page*pp;
        for (int i=0;i<pp&&(si+i)<image_count;i++) {
            int tx=THUMB_START_X+(i%THUMB_COLS)*(THUMB_W+THUMB_GAP_X);
            int ty=THUMB_START_Y+(i/THUMB_COLS)*(THUMB_H+THUMB_GAP_Y);
            fb_draw_rect(fb_fd,tx-2,ty-2,THUMB_W+4,THUMB_H+4,150,150,150,0);
            fb_draw_bmp(fb_fd,tx,ty,THUMB_W,THUMB_H,image_list[si+i]);
            if ((si+i)==current_index) fb_draw_rect(fb_fd,tx-3,ty-3,THUMB_W+6,THUMB_H+6,0,255,0,0);
        }
        snprintf(lbl_info.text,sizeof(lbl_info.text),"Page %d/%d | %d pics",page+1,total,image_count);
        label_draw(fb_fd,&lbl_info);
        if (page>0) btn_draw(fb_fd,&btn_prev);
        if (page<total-1) btn_draw(fb_fd,&btn_next);
        for (int i=0;i<3;i++) btn_draw(fb_fd,&btn_tab[i]);
        btn_draw(fb_fd,&btn_back);
        fb_flush(fb_fd);
    }
}

/* ---- 全屏模式 ---- */
static int fullscreen_mode(int fb_fd, int touch_fd, TransitionEffect effect)
{
    Button btn_prev, btn_next, btn_tab[3], btn_back;
    Label lbl_info;

    btn_prev.x=20;btn_prev.y=260;btn_prev.w=60;btn_prev.h=80;btn_prev.label="<";btn_prev.color_b=60;btn_prev.color_g=60;btn_prev.color_r=60;btn_prev.pressed=0;
    btn_next.x=944;btn_next.y=260;btn_next.w=60;btn_next.h=80;btn_next.label=">";btn_next.color_b=60;btn_next.color_g=60;btn_next.color_r=60;btn_next.pressed=0;

    const char *tabs[3]={"Thumb","Full","Slide"};
    for (int i=0;i<3;i++) {
        btn_tab[i].x=350+i*160;btn_tab[i].y=540;btn_tab[i].w=140;btn_tab[i].h=45;
        btn_tab[i].label=tabs[i];btn_tab[i].pressed=0;
        btn_tab[i].color_b=(i==1)?0:100; btn_tab[i].color_g=(i==1)?200:100; btn_tab[i].color_r=(i==1)?0:100;
    }
    btn_back.x=850;btn_back.y=540;btn_back.w=100;btn_back.h=45;btn_back.label="Back";btn_back.color_b=100;btn_back.color_g=100;btn_back.color_r=100;btn_back.pressed=0;
    lbl_info.x=400;lbl_info.y=10;lbl_info.w=250;lbl_info.h=30;lbl_info.color_b=255;lbl_info.color_g=255;lbl_info.color_r=255;

    fb_clear(fb_fd,0,0,0);
    fb_draw_bmp(fb_fd,0,0,SCREEN_W,SCREEN_H,image_list[current_index]);
    snprintf(lbl_info.text,sizeof(lbl_info.text),"%d/%d",current_index+1,image_count);
    label_draw(fb_fd,&lbl_info);
    if (current_index>0) btn_draw(fb_fd,&btn_prev);
    if (current_index<image_count-1) btn_draw(fb_fd,&btn_next);
    for (int i=0;i<3;i++) btn_draw(fb_fd,&btn_tab[i]);
    btn_draw(fb_fd,&btn_back);
    fb_flush(fb_fd);

    while (1) {
        TouchPoint pt;
        if (touch_read(touch_fd,&pt)!=0) continue;
        if (!pt.pressed) continue;

        if (current_index>0&&btn_hit_test(&btn_prev,pt.x,pt.y)) {
            const char *old=image_list[current_index];
            current_index--; fb_clear(fb_fd,0,0,0);
            apply_transition(fb_fd,old,image_list[current_index],effect);
        }
        else if (current_index<image_count-1&&btn_hit_test(&btn_next,pt.x,pt.y)) {
            const char *old=image_list[current_index];
            current_index++; fb_clear(fb_fd,0,0,0);
            apply_transition(fb_fd,old,image_list[current_index],effect);
        }
        else if (btn_hit_test(&btn_tab[0],pt.x,pt.y)) return MODE_THUMBNAIL;
        else if (btn_hit_test(&btn_tab[2],pt.x,pt.y)) { slideshow_running=0; return MODE_SLIDESHOW; }
        else if (btn_hit_test(&btn_back,pt.x,pt.y)) return -1;
        else continue;

        /* 重绘 */
        fb_clear(fb_fd,0,0,0);
        fb_draw_bmp(fb_fd,0,0,SCREEN_W,SCREEN_H,image_list[current_index]);
        snprintf(lbl_info.text,sizeof(lbl_info.text),"%d/%d",current_index+1,image_count);
        label_draw(fb_fd,&lbl_info);
        if (current_index>0) btn_draw(fb_fd,&btn_prev);
        if (current_index<image_count-1) btn_draw(fb_fd,&btn_next);
        for (int i=0;i<3;i++) btn_draw(fb_fd,&btn_tab[i]);
        btn_draw(fb_fd,&btn_back);
        fb_flush(fb_fd);
    }
}

/* ---- 幻灯片模式 ---- */
static int slideshow_mode(int fb_fd, int touch_fd, TransitionEffect effect)
{
    if (image_count <= 0) return -1;
    if (current_index < 0) current_index = 0;
    if (current_index >= image_count) current_index = image_count - 1;

    slideshow_running = 1;
    int tick = 0;
    int was_pressed = 0; /* 防止连续触发 */

    fb_clear(fb_fd, 0, 0, 0);
    if (image_list[current_index][0])
        fb_draw_bmp(fb_fd, 0, 0, SCREEN_W, SCREEN_H, image_list[current_index]);
    fb_flush(fb_fd);

    while (1) {
        usleep(100000); /* 100ms 循环 */

        /* 自动播放 */
        if (slideshow_running) {
            tick++;
            if (tick >= slideshow_interval * 10) {
                tick = 0;
                int old = current_index;
                current_index = (current_index + 1) % image_count;
                fb_clear(fb_fd, 0, 0, 0);
                apply_transition(fb_fd, image_list[old], image_list[current_index], effect);
            }
        }

        /* 触摸检测 (非阻塞) */
        TouchPoint pt;
        int got = (touch_read_try(touch_fd, &pt) == 0);
        /* 抬起时清除标记，允许下次触发 */
        if (got && !pt.pressed) { was_pressed = 0; }
        /* 连续按下不重复处理 */
        if (got && pt.pressed && !was_pressed) {
            was_pressed = 1;
            /* 底部控制栏: 按钮 x=[10-160][170-320][330-480][490-640][650-800] */
            if (pt.y > 555) {
                if (pt.x < 170) { slideshow_running = 0; return MODE_THUMBNAIL; }
                else if (pt.x < 330) { slideshow_running = !slideshow_running; tick = 0; }
                else if (pt.x < 490) { if (slideshow_interval < 10) slideshow_interval++; }
                else if (pt.x < 650) { if (slideshow_interval > 1) slideshow_interval--; }
                else { slideshow_running = 0; return -1; }
            }
            /* 上半屏: 点左半=上一张, 点右半=下一张 */
            else if (pt.x < SCREEN_W/2 && current_index > 0) {
                int old = current_index; current_index--;
                fb_clear(fb_fd, 0, 0, 0);
                apply_transition(fb_fd, image_list[old], image_list[current_index], effect);
                tick = 0;
            }
            else if (pt.x >= SCREEN_W/2 && current_index < image_count-1) {
                int old = current_index; current_index++;
                fb_clear(fb_fd, 0, 0, 0);
                apply_transition(fb_fd, image_list[old], image_list[current_index], effect);
                tick = 0;
            }
        }

        /* 绘制底部控制栏 */
        fb_draw_rect(fb_fd, 0, 558, SCREEN_W, 42, 60, 60, 60, 1);

        Button btm[5];
        const char *bl[5] = {"Thumb","Pause","+","-","Back"};
        int bc[5][3] = {{0,150,50},{200,150,150},{400,255,200},{550,100,200},{700,100,200}};
        for (int i=0;i<5;i++) {
            btm[i].x=10+i*160; btm[i].y=560; btm[i].w=150; btm[i].h=36;
            btm[i].label=bl[i]; btm[i].pressed=0;
            btm[i].color_b=bc[i][0]; btm[i].color_g=bc[i][1]; btm[i].color_r=bc[i][2];
        }
        if (slideshow_running)
            btm[1].color_b=180, btm[1].color_g=50, btm[1].color_r=50, btm[1].label="Pause";
        else
            btm[1].color_b=0, btm[1].color_g=180, btm[1].color_r=0, btm[1].label="Play";

        for (int i=0;i<5;i++) btn_draw(fb_fd, &btm[i]);

        char buf[64];
        snprintf(buf, sizeof(buf), "%d/%d %ds", current_index+1, image_count, slideshow_interval);
        fb_draw_text(fb_fd, 830, 565, buf, 255, 255, 255, 2);
        fb_flush(fb_fd);
    }
}

/* ---- 相册主入口 ---- */
void album_screen(int fb_fd, int touch_fd)
{
    int idx = album_select_page(fb_fd, touch_fd);
    if (idx < 0) return;
    current_album = idx; current_index = 0; current_mode = MODE_THUMBNAIL; slideshow_running = 0;

    if (scan_images(album_dirs[idx]) == 0) {
        fb_clear(fb_fd,30,30,30);
        Label lbl; lbl.x=200;lbl.y=220;lbl.w=600;lbl.h=60;
        snprintf(lbl.text,sizeof(lbl.text),"No images in /res/album/%s/", album_names[idx]);
        lbl.color_b=200;lbl.color_g=200;lbl.color_r=100;
        label_draw(fb_fd,&lbl);
        Button bok; bok.x=400;bok.y=320;bok.w=200;bok.h=60;bok.label="OK";
        bok.color_b=100;bok.color_g=100;bok.color_r=100;bok.pressed=0;
        btn_draw(fb_fd,&bok);
        fb_flush(fb_fd);
        while (1) {
            TouchPoint pt;
            if (touch_read(touch_fd,&pt)!=0) continue;
            if (!pt.pressed) continue;
            if (btn_hit_test(&bok,pt.x,pt.y)) return;
            /* 重绘 */
            fb_clear(fb_fd,30,30,30);
            label_draw(fb_fd,&lbl); btn_draw(fb_fd,&bok); fb_flush(fb_fd);
        }
    }

    while (1) {
        int nm = -1;
        switch (current_mode) {
            case MODE_THUMBNAIL: nm = thumbnail_mode(fb_fd, touch_fd, album_effects[idx]); break;
            case MODE_FULLSCREEN: nm = fullscreen_mode(fb_fd, touch_fd, album_effects[idx]); break;
            case MODE_SLIDESHOW: nm = slideshow_mode(fb_fd, touch_fd, album_effects[idx]); break;
        }
        if (nm < 0) { slideshow_running = 0; return; }
        current_mode = (BrowseMode)nm;
    }
}
