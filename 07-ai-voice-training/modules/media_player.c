/**
 * 音视频播放器 — 音频用 mplayer+FIFO, 视频用 GStreamer+kmssink 硬解码
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include "media_player.h"
#include "../ui/fb_draw.h"
#include "../ui/touch.h"
#include "../ui/widgets.h"

#define MEDIA_DIR      "/res/media/"
#define MAX_MEDIA      100
#define ITEMS_PER_PAGE 7
#define ITEM_H         52
#define FIFO_PATH      "/tmp/mp.fifo"

static char media_list[MAX_MEDIA][256];
static int  media_count=0, media_scroll=0, current_playing=-1;
static int  pipe_fd=-1, is_playing=0;

static int scan_media(void){media_count=0;DIR*d=opendir(MEDIA_DIR);if(!d)return 0;struct dirent*e;while((e=readdir(d))&&media_count<MAX_MEDIA){if(e->d_name[0]!='.')snprintf(media_list[media_count++],256,"%s%s",MEDIA_DIR,e->d_name);}closedir(d);return media_count;}

static int fifo_init(void){system("rm -rf "FIFO_PATH);if(mkfifo(FIFO_PATH,0777)==-1)return-1;pipe_fd=open(FIFO_PATH,O_RDWR);return pipe_fd>=0?0:-1;}
static void mplayer_cmd(const char*s){if(pipe_fd>0)write(pipe_fd,s,strlen(s));}

static void stop_playback(void){
    if(pipe_fd>0){write(pipe_fd,"quit\n",5);usleep(100000);}
    system("killall -9 mplayer 2>/dev/null");
    system("killall -9 gst-launch-1.0 2>/dev/null");
    is_playing=0;current_playing=-1;
}

static int is_video(const char*p){const char*e=strrchr(p,'.');if(!e)return 0;return(!strcasecmp(e,".mp4")||!strcasecmp(e,".avi")||!strcasecmp(e,".mkv")||!strcasecmp(e,".mov")||!strcasecmp(e,".wmv")||!strcasecmp(e,".flv"));}

static void play_media(int *pfb_fd, int touch_fd, int index){
    if(index<0||index>=media_count)return;
    stop_playback();
    if(pipe_fd>0)close(pipe_fd);
    fifo_init();
    char cmd[1024];
    if(is_video(media_list[index])){
        snprintf(cmd,sizeof(cmd),
            "gst-launch-1.0 playbin \"uri=file://%s\" "
            "video-sink=\"videoscale ! capsfilter caps=video/x-raw,"
            "width=1024,height=600 ! kmssink\" 2>/dev/null",
            media_list[index]);
        printf("[Media] Video: %s\n",media_list[index]);
        fb_close(*pfb_fd);
        pid_t pid=fork();
        if(pid==0){execl("/bin/sh","sh","-c",cmd,NULL);_exit(0);}
        /* 父进程: 双击屏幕退出 */
        TouchPoint pt; int tap_count=0; long last_ms=0;
        while(1){
            int st; if(waitpid(pid,&st,WNOHANG)>0) break;
            if(touch_read_try(touch_fd,&pt)==0&&pt.pressed){
                struct timeval tv; gettimeofday(&tv,NULL);
                long now_ms=tv.tv_sec*1000+tv.tv_usec/1000;
                if(now_ms-last_ms<600){tap_count++;}else{tap_count=1;}
                last_ms=now_ms;
                if(tap_count>=2){kill(pid,SIGTERM);usleep(300000);break;}
            }
            usleep(80000);
        }
        *pfb_fd=fb_open();
        current_playing=-1;is_playing=0;
    }else{
        snprintf(cmd,sizeof(cmd),"mplayer -slave -quiet -input file="FIFO_PATH" -vo null -ao alsa:device=hw=0.0 \"%s\" 2>/dev/null &",media_list[index]);
        printf("[Media] Audio: %s\n",media_list[index]);
        system(cmd);
        current_playing=index;is_playing=1;
    }
}

void media_player_screen(int fb_fd, int touch_fd)
{
    media_scroll=0;scan_media();fifo_init();

    /* ===== 布局 ===== */
    Label lbl_title,lbl_status;
    lbl_title.x=350;lbl_title.y=8;lbl_title.w=300;lbl_title.h=28;
    strcpy(lbl_title.text,"Media Player");
    lbl_title.color_b=255;lbl_title.color_g=255;lbl_title.color_r=255;
    lbl_status.x=10;lbl_status.y=558;lbl_status.w=800;lbl_status.h=28;
    lbl_status.text[0]='\0';
    lbl_status.color_b=100;lbl_status.color_g=255;lbl_status.color_r=100;

    /* 底部按钮 — 两行 */
    Button b[9];
    /* 主控行 y=490 */
    b[0].x=20;  b[0].y=490;b[0].w=130;b[0].h=50; b[0].label="Prev";   b[0].color_b=80;b[0].color_g=100;b[0].color_r=220;b[0].pressed=0;
    b[1].x=165; b[1].y=490;b[1].w=130;b[1].h=50; b[1].label="Play";   b[1].color_b=0;b[1].color_g=180;b[1].color_r=0;b[1].pressed=0;
    b[2].x=310; b[2].y=490;b[2].w=130;b[2].h=50; b[2].label="Pause";  b[2].color_b=180;b[2].color_g=100;b[2].color_r=0;b[2].pressed=0;
    b[3].x=455; b[3].y=490;b[3].w=130;b[3].h=50; b[3].label="Next";   b[3].color_b=80;b[3].color_g=100;b[3].color_r=220;b[3].pressed=0;
    /* 副控行 y=548 */
    b[4].x=20;  b[4].y=548;b[4].w=90;b[4].h=38; b[4].label="<<-10s"; b[4].color_b=80;b[4].color_g=120;b[4].color_r=200;b[4].pressed=0;
    b[5].x=120; b[5].y=548;b[5].w=90;b[5].h=38; b[5].label="+10s>>"; b[5].color_b=80;b[5].color_g=120;b[5].color_r=200;b[5].pressed=0;
    b[6].x=230; b[6].y=548;b[6].w=90;b[6].h=38; b[6].label="Vol-";   b[6].color_b=120;b[6].color_g=120;b[6].color_r=180;b[6].pressed=0;
    b[7].x=330; b[7].y=548;b[7].w=90;b[7].h=38; b[7].label="Vol+";   b[7].color_b=120;b[7].color_g=120;b[7].color_r=180;b[7].pressed=0;
    b[8].x=900; b[8].y=548;b[8].w=100;b[8].h=38;b[8].label="Back";   b[8].color_b=120;b[8].color_g=120;b[8].color_r=120;b[8].pressed=0;

    /* 首次绘制 */
    fb_clear(fb_fd,20,20,45);
    label_draw(fb_fd,&lbl_title);
    /* 列表区域 */
    if(media_count==0){
        fb_draw_rect(fb_fd,150,160,700,200,40,40,60,1);
        fb_draw_text(fb_fd,200,200,"No media files found",200,200,100,3);
        fb_draw_text(fb_fd,180,260,"Put mp3/mp4 in /res/media/ via rz",180,180,100,2);
    }else{
        for(int i=0;i<ITEMS_PER_PAGE&&(media_scroll+i)<media_count;i++){
            int idx=media_scroll+i,iy=45+i*(ITEM_H+4);
            int ip=(idx==current_playing&&is_playing);
            /* 行背景 */
            fb_draw_rect(fb_fd,10,iy,800,ITEM_H,ip?20:50,ip?80:50,ip?20:55,1);
            /* 边框 */
            fb_draw_rect(fb_fd,10,iy,800,ITEM_H,90,90,90,0);
            /* 文件名 */
            const char*bn=strrchr(media_list[idx],'/');bn=bn?bn+1:media_list[idx];
            char buf[256];snprintf(buf,sizeof(buf),"%s%s",ip?"[>] ":"",bn);
            fb_draw_text(fb_fd,20,iy+12,buf,255,255,255,2);
        }
    }
    label_draw(fb_fd,&lbl_status);
    for(int i=0;i<9;i++)btn_draw(fb_fd,&b[i]);
    fb_flush(fb_fd);

    while(1){
        if(current_playing>=0&&is_playing){
            const char*bn=strrchr(media_list[current_playing],'/');bn=bn?bn+1:media_list[current_playing];
            snprintf(lbl_status.text,sizeof(lbl_status.text),"Now: %s",bn);
        }else strcpy(lbl_status.text,"Stopped");

        TouchPoint pt;
        if(touch_read(touch_fd,&pt)!=0)continue;
        if(!pt.pressed)continue;

        int redraw=0;
        /* 列表 */
        if(pt.y>45&&pt.y<45+ITEMS_PER_PAGE*(ITEM_H+4)&&pt.x>=10&&pt.x<=810){
            int ci=(pt.y-45)/(ITEM_H+4),idx=media_scroll+ci;
            if(idx>=0&&idx<media_count){play_media(&fb_fd,touch_fd,idx);redraw=1;}
        }
        else if(btn_hit_test(&b[0],pt.x,pt.y)){if(media_count>0){int p=current_playing>0?current_playing-1:media_count-1;play_media(&fb_fd,touch_fd,p);}redraw=1;}
        else if(btn_hit_test(&b[1],pt.x,pt.y)){if(current_playing>=0)play_media(&fb_fd,touch_fd,current_playing);redraw=1;}
        else if(btn_hit_test(&b[2],pt.x,pt.y)){mplayer_cmd("pause\n");is_playing=!is_playing;redraw=1;}
        else if(btn_hit_test(&b[3],pt.x,pt.y)){if(media_count>0){int n=(current_playing+1)%media_count;play_media(&fb_fd,touch_fd,n);}redraw=1;}
        else if(btn_hit_test(&b[4],pt.x,pt.y)){mplayer_cmd("seek -10\n");redraw=1;}
        else if(btn_hit_test(&b[5],pt.x,pt.y)){mplayer_cmd("seek +10\n");redraw=1;}
        else if(btn_hit_test(&b[6],pt.x,pt.y)){mplayer_cmd("volume -10 0\n");redraw=1;}
        else if(btn_hit_test(&b[7],pt.x,pt.y)){mplayer_cmd("volume +10 0\n");redraw=1;}
        else if(btn_hit_test(&b[8],pt.x,pt.y)){stop_playback();return;}

        if(redraw){
            fb_clear(fb_fd,20,20,45);
            label_draw(fb_fd,&lbl_title);
            if(media_count==0){
                fb_draw_rect(fb_fd,150,160,700,200,40,40,60,1);
                fb_draw_text(fb_fd,200,200,"No media files found",200,200,100,3);
                fb_draw_text(fb_fd,180,260,"Put mp3/mp4 in /res/media/ via rz",180,180,100,2);
            }else{
                for(int i=0;i<ITEMS_PER_PAGE&&(media_scroll+i)<media_count;i++){
                    int idx=media_scroll+i,iy=45+i*(ITEM_H+4);
                    int ip=(idx==current_playing&&is_playing);
                    fb_draw_rect(fb_fd,10,iy,800,ITEM_H,ip?20:50,ip?80:50,ip?20:55,1);
                    fb_draw_rect(fb_fd,10,iy,800,ITEM_H,90,90,90,0);
                    const char*bn=strrchr(media_list[idx],'/');bn=bn?bn+1:media_list[idx];
                    char buf[256];snprintf(buf,sizeof(buf),"%s%s",ip?"[>] ":"",bn);
                    fb_draw_text(fb_fd,20,iy+12,buf,255,255,255,2);
                }
            }
            label_draw(fb_fd,&lbl_status);
            for(int i=0;i<9;i++)btn_draw(fb_fd,&b[i]);
            fb_flush(fb_fd);
        }
    }
}
