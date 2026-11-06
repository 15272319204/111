#ifndef WIDGETS_H
#define WIDGETS_H

/* ---- Button ---- */
typedef struct {
    int x, y, w, h;
    const char *label;
    int color_b, color_g, color_r;
    int pressed;
} Button;

void btn_draw(int fd, Button *b);
int  btn_hit_test(Button *b, int tx, int ty);

/* ---- Keypad (4x3) ---- */
#define KP_ROWS 4
#define KP_COLS 3

typedef struct {
    int x, y, w, h;
    char keys[KP_ROWS][KP_COLS][8];
    int key_w, key_h;
} Keypad;

void keypad_init_digital(Keypad *kp, int x, int y, int w, int h);
void keypad_draw(int fd, Keypad *kp);
int  keypad_get_key(Keypad *kp, int tx, int ty, char *key_out);

/* ---- InputBox ---- */
typedef struct {
    int x, y, w, h;
    char text[256];
    int len;
    int active;
    int password_mode;
} InputBox;

void input_draw(int fd, InputBox *ib);
void input_append(InputBox *ib, char c);
void input_append_str(InputBox *ib, const char *s);
void input_delete(InputBox *ib);
void input_clear(InputBox *ib);

/* ---- Label ---- */
typedef struct {
    int x, y, w, h;
    char text[256];
    int color_b, color_g, color_r;
} Label;

void label_draw(int fd, Label *l);

#endif
