#include <string.h>
#include "fb_draw.h"
#include "widgets.h"

/* ================================================================
 *  Button
 * ================================================================ */

void btn_draw(int fd, Button *b)
{
    unsigned char bg_b, bg_g, bg_r;
    unsigned char border_b, border_g, border_r;
    unsigned char text_b, text_g, text_r;
    int text_scale = 2;

    if (b->pressed) {
        /* darker version of the button's own color */
        bg_b     = b->color_b * 2 / 3;
        bg_g     = b->color_g * 2 / 3;
        bg_r     = b->color_r * 2 / 3;
        border_b = b->color_b / 2;
        border_g = b->color_g / 2;
        border_r = b->color_r / 2;
        text_b   = 255; text_g = 255; text_r = 255;
    } else {
        /* use the button's own colors */
        bg_b     = b->color_b;
        bg_g     = b->color_g;
        bg_r     = b->color_r;
        border_b = b->color_b / 2;
        border_g = b->color_g / 2;
        border_r = b->color_r / 2;
        /* white text on dark buttons, black text on light buttons */
        int brightness = (int)b->color_r + (int)b->color_g + (int)b->color_b;
        if (brightness < 300) {
            text_b = 255; text_g = 255; text_r = 255;
        } else {
            text_b = 0; text_g = 0; text_r = 0;
        }
    }

    /* filled background */
    fb_draw_rect(fd, b->x, b->y, b->w, b->h, bg_b, bg_g, bg_r, 1);

    /* 3 px border */
    fb_draw_rect(fd, b->x,     b->y,     b->w,     b->h,     border_b, border_g, border_r, 0);
    fb_draw_rect(fd, b->x + 1, b->y + 1, b->w - 2, b->h - 2, border_b, border_g, border_r, 0);
    fb_draw_rect(fd, b->x + 2, b->y + 2, b->w - 4, b->h - 4, border_b, border_g, border_r, 0);

    /* label centred, scale=2 for visibility */
    if (b->label && b->label[0]) {
        int char_w = 8 * text_scale;
        int char_h = 16 * text_scale;
        int text_w = (int)strlen(b->label) * char_w;
        int cx     = b->x + (b->w - text_w) / 2;
        int cy     = b->y + (b->h - char_h) / 2;
        if (cx < b->x) cx = b->x;
        if (cy < b->y) cy = b->y;
        fb_draw_text(fd, cx, cy, b->label, text_b, text_g, text_r, text_scale);
    }
}

int btn_hit_test(Button *b, int tx, int ty)
{
    if (!b) return 0;
    return (tx >= b->x && tx < b->x + b->w &&
            ty >= b->y && ty < b->y + b->h) ? 1 : 0;
}

/* ================================================================
 *  Keypad  (4 rows x 3 cols)
 * ================================================================ */

void keypad_init_digital(Keypad *kp, int x, int y, int w, int h)
{
    kp->x = x;
    kp->y = y;
    kp->w = w;
    kp->h = h;
    kp->key_w = w / KP_COLS;
    kp->key_h = h / KP_ROWS;

    /* Row 0 */
    strcpy(kp->keys[0][0], "1");
    strcpy(kp->keys[0][1], "2");
    strcpy(kp->keys[0][2], "3");

    /* Row 1 */
    strcpy(kp->keys[1][0], "4");
    strcpy(kp->keys[1][1], "5");
    strcpy(kp->keys[1][2], "6");

    /* Row 2 */
    strcpy(kp->keys[2][0], "7");
    strcpy(kp->keys[2][1], "8");
    strcpy(kp->keys[2][2], "9");

    /* Row 3 */
    strcpy(kp->keys[3][0], "Del");
    strcpy(kp->keys[3][1], "0");
    strcpy(kp->keys[3][2], "OK");
}

void keypad_draw(int fd, Keypad *kp)
{
    int row, col;

    /* background */
    fb_draw_rect(fd, kp->x, kp->y, kp->w, kp->h, 60, 60, 60, 1);

    for (row = 0; row < KP_ROWS; row++) {
        for (col = 0; col < KP_COLS; col++) {
            int cx = kp->x + col * kp->key_w;
            int cy = kp->y + row * kp->key_h;
            int margin = 2;

            /* key background (light grey) */
            fb_draw_rect(fd,
                         cx + margin, cy + margin,
                         kp->key_w - margin * 2,
                         kp->key_h - margin * 2,
                         220, 220, 220, 1);

            /* key border */
            fb_draw_rect(fd,
                         cx + margin, cy + margin,
                         kp->key_w - margin * 2,
                         kp->key_h - margin * 2,
                         150, 150, 150, 0);

            /* label centred in the key cell, scale=2 */
            {
                int char_w = 8 * 2;
                int char_h = 16 * 2;
                int text_w = (int)strlen(kp->keys[row][col]) * char_w;
                int tx     = cx + (kp->key_w - text_w) / 2;
                int ty     = cy + (kp->key_h - char_h) / 2;
                if (tx < cx + margin) tx = cx + margin;
                if (ty < cy + margin) ty = cy + margin;
                fb_draw_text(fd, tx, ty, kp->keys[row][col], 0, 0, 0, 2);
            }
        }
    }
}

int keypad_get_key(Keypad *kp, int tx, int ty, char *key_out)
{
    int row, col;

    for (row = 0; row < KP_ROWS; row++) {
        for (col = 0; col < KP_COLS; col++) {
            int cx = kp->x + col * kp->key_w;
            int cy = kp->y + row * kp->key_h;

            if (tx >= cx && tx < cx + kp->key_w &&
                ty >= cy && ty < cy + kp->key_h) {
                if (key_out) {
                    strcpy(key_out, kp->keys[row][col]);
                }
                return 1;
            }
        }
    }
    return 0;
}

/* ================================================================
 *  InputBox
 * ================================================================ */

void input_draw(int fd, InputBox *ib)
{
    unsigned char border_b, border_g, border_r;
    char display[128];
    int  i, disp_len;

    /* border colour: blue when active, grey otherwise */
    if (ib->active) {
        border_b = 255; border_g =   0; border_r =   0;   /* blue  */
    } else {
        border_b = 150; border_g = 150; border_r = 150;   /* grey  */
    }

    /* white background */
    fb_draw_rect(fd, ib->x, ib->y, ib->w, ib->h, 255, 255, 255, 1);

    /* 2 px border */
    fb_draw_rect(fd, ib->x,     ib->y,     ib->w,     ib->h,     border_b, border_g, border_r, 0);
    fb_draw_rect(fd, ib->x + 1, ib->y + 1, ib->w - 2, ib->h - 2, border_b, border_g, border_r, 0);

    /* build display string (masked if password_mode) */
    disp_len = 0;
    if (ib->password_mode) {
        for (i = 0; i < ib->len; i++) {
            display[disp_len++] = '*';
        }
    } else {
        for (i = 0; i < ib->len; i++) {
            display[disp_len++] = ib->text[i];
        }
    }

    /* blinking cursor ('|') when active */
    if (ib->active) {
        display[disp_len++] = '|';
    }
    display[disp_len] = '\0';

    /* draw the text (4 px padding from left edge), scale=2 */
    if (disp_len > 0) {
        int char_h = 16 * 2;
        int text_y = ib->y + (ib->h - char_h) / 2;
        if (text_y < ib->y) text_y = ib->y;
        fb_draw_text(fd, ib->x + 6, text_y, display, 0, 0, 0, 2);
    }
}

void input_append(InputBox *ib, char c)
{
    if (ib->len < 254) {
        ib->text[ib->len++] = c;
        ib->text[ib->len]   = '\0';
    }
}

void input_append_str(InputBox *ib, const char *s)
{
    if (s == NULL) return;
    int slen = strlen(s);
    for (int i = 0; i < slen && ib->len < 254; i++) {
        ib->text[ib->len++] = s[i];
    }
    ib->text[ib->len] = '\0';
}

void input_delete(InputBox *ib)
{
    if (ib->len > 0) {
        ib->len--;
        ib->text[ib->len] = '\0';
    }
}

void input_clear(InputBox *ib)
{
    ib->len      = 0;
    ib->text[0]  = '\0';
}

/* ================================================================
 *  Label
 * ================================================================ */

void label_draw(int fd, Label *l)
{
    int  max_chars, len, i;
    char truncated[257];

    /* each char is roughly 8*2=16 px wide at scale 2 */
    max_chars = l->w / 16;
    len       = (int)strlen(l->text);

    if (max_chars <= 0) return;

    if (len > max_chars) {
        for (i = 0; i < max_chars && i < 256; i++) {
            truncated[i] = l->text[i];
        }
        truncated[max_chars] = '\0';
    } else {
        for (i = 0; i < len && i < 256; i++) {
            truncated[i] = l->text[i];
        }
        truncated[len] = '\0';
    }

    fb_draw_text(fd, l->x, l->y, truncated,
                 (unsigned char)l->color_b,
                 (unsigned char)l->color_g,
                 (unsigned char)l->color_r, 2);
}
