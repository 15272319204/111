#ifndef KEYBOARD_H
#define KEYBOARD_H

/* 键盘模式 */
#define KB_MODE_ABC    0   /* 小写字母 */
#define KB_MODE_ABC_S  1   /* 大写字母 */
#define KB_MODE_NUM    2   /* 数字123 */
#define KB_MODE_SYM    3   /* 符号 */
#define KB_MODE_PINYIN 4   /* 拼音输入法 */

#define KB_ROWS 4
#define KB_COLS 10

typedef struct {
    int x, y, w, h;
    char keys[KB_ROWS][KB_COLS][8];
    int key_w, key_h;
    int mode;           /* 当前模式 */
    int shift;          /* 是否按下Shift */
    char pinyin_buf[64]; /* 拼音缓冲区 */
    int pinyin_len;
    char candidates[10][8]; /* 候选字列表 */
    int candidate_count;
    int show_candidates;  /* 是否显示候选字 */
} Keyboard;

void  keyboard_init(Keyboard *kb, int x, int y, int w, int h);
void  keyboard_set_mode(Keyboard *kb, int mode);
void  keyboard_draw(int fd, Keyboard *kb);
int   keyboard_get_key(Keyboard *kb, int tx, int ty, char *key_out);
void  keyboard_handle_pinyin(Keyboard *kb, char *key_out, char *result, int maxlen);

#endif
