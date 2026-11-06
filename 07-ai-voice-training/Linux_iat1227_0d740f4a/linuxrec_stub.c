/* linuxrec 空桩 — 文件模式不需要录音功能 */
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "linuxrec.h"

record_dev_id get_default_input_dev() {
    record_dev_id id; id.u.name = NULL; return id;
}
int get_input_dev_num() { return 0; }

int create_recorder(struct recorder **out_rec,
        void (*on_data_ind)(char*, unsigned long, void*), void* user_cb_para) {
    (void)on_data_ind; (void)user_cb_para;
    *out_rec = NULL; return -1;
}
void destroy_recorder(struct recorder *rec) { (void)rec; }

int open_recorder(struct recorder *rec, record_dev_id dev, WAVEFORMATEX *fmt) {
    (void)rec; (void)dev; (void)fmt; return -1;
}
void close_recorder(struct recorder *rec) { (void)rec; }

int start_record(struct recorder *rec) { (void)rec; return -1; }
int stop_record(struct recorder *rec) { (void)rec; return -1; }
int is_record_stopped(struct recorder *rec) { (void)rec; return 1; }
