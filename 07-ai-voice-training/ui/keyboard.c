#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fb_draw.h"
#include "keyboard.h"

/* QWERTY 键盘布局 */
static const char *abc_lower[KB_ROWS][KB_COLS] = {
    {"q","w","e","r","t","y","u","i","o","p"},
    {"a","s","d","f","g","h","j","k","l","Del"},
    {"Shf","z","x","c","v","b","n","m","OK",""},
    {"123",",","Space",".","/","PIN","","","",""}
};

static const char *abc_upper[KB_ROWS][KB_COLS] = {
    {"Q","W","E","R","T","Y","U","I","O","P"},
    {"A","S","D","F","G","H","J","K","L","Del"},
    {"Shf","Z","X","C","V","B","N","M","OK",""},
    {"123",",","Space",".","/","PIN","","","",""}
};

static const char *num_keys[KB_ROWS][KB_COLS] = {
    {"1","2","3","4","5","6","7","8","9","0"},
    {"@","#","$","%","&","*","-","+","=","Del"},
    {"ABC","(",")","!","?","'","\"",";",":","OK"},
    {"",",","Space",".","/","","","","",""}
};

static const char *sym_keys[KB_ROWS][KB_COLS] = {
    {"~","`","|","\\","<",">","{","}","[","]"},
    {"^","_","-","+","=","&","*","/","%","Del"},
    {"ABC","?","!","@","#","$","(",")",";","OK"},
    {"123",",","Space",".",":","'","\"","","",""}
};

void keyboard_init(Keyboard *kb, int x, int y, int w, int h)
{
    memset(kb, 0, sizeof(Keyboard));
    kb->x = x;
    kb->y = y;
    kb->w = w;
    kb->h = h;
    kb->key_w = w / KB_COLS;
    kb->key_h = h / KB_ROWS;
    kb->mode = KB_MODE_ABC;
    kb->shift = 0;
    keyboard_set_mode(kb, KB_MODE_ABC);
}

void keyboard_set_mode(Keyboard *kb, int mode)
{
    kb->mode = mode;
    const char *(*layout)[KB_COLS];
    switch (mode) {
    case KB_MODE_ABC:
        layout = kb->shift ? abc_upper : abc_lower;
        break;
    case KB_MODE_ABC_S:
        layout = abc_upper;
        break;
    case KB_MODE_NUM:
        layout = num_keys;
        break;
    case KB_MODE_SYM:
        layout = sym_keys;
        break;
    case KB_MODE_PINYIN:
        layout = abc_lower;
        break;
    default:
        layout = abc_lower;
        break;
    }
    for (int r = 0; r < KB_ROWS; r++)
        for (int c = 0; c < KB_COLS; c++)
            if (layout[r][c])
                strncpy(kb->keys[r][c], layout[r][c], 7);
            else
                kb->keys[r][c][0] = '\0';
}

void keyboard_draw(int fd, Keyboard *kb)
{
    /* 背景 */
    fb_draw_rect(fd, kb->x, kb->y, kb->w, kb->h, 50, 50, 50, 1);

    for (int r = 0; r < KB_ROWS; r++) {
        for (int c = 0; c < KB_COLS; c++) {
            if (kb->keys[r][c][0] == '\0') continue;

            int cx = kb->x + c * kb->key_w;
            int cy = kb->y + r * kb->key_h;
            int m = 2;

            /* 特殊键用不同颜色 */
            int bg_r = 220, bg_g = 220, bg_b = 220;
            int txt_r = 0, txt_g = 0, txt_b = 0;

            if (strcmp(kb->keys[r][c], "Del") == 0) {
                bg_r = 220; bg_g = 100; bg_b = 100;
                txt_r = 255; txt_g = 255; txt_b = 255;
            } else if (strcmp(kb->keys[r][c], "OK") == 0) {
                bg_r = 100; bg_g = 200; bg_b = 100;
                txt_r = 255; txt_g = 255; txt_b = 255;
            } else if (strcmp(kb->keys[r][c], "Shf") == 0) {
                bg_r = 100; bg_g = 150; bg_b = 200;
                txt_r = 255; txt_g = 255; txt_b = 255;
            } else if (strcmp(kb->keys[r][c], "Space") == 0) {
                bg_r = 200; bg_g = 200; bg_b = 200;
            } else if (strcmp(kb->keys[r][c], "123") == 0 ||
                       strcmp(kb->keys[r][c], "ABC") == 0 ||
                       strcmp(kb->keys[r][c], "PIN") == 0 ||
                       strcmp(kb->keys[r][c], "SYM") == 0) {
                bg_r = 180; bg_g = 180; bg_b = 220;
            }

            /* 画按键 */
            fb_draw_rect(fd, cx + m, cy + m, kb->key_w - m*2, kb->key_h - m*2,
                         bg_b, bg_g, bg_r, 1);
            fb_draw_rect(fd, cx + m, cy + m, kb->key_w - m*2, kb->key_h - m*2,
                         120, 120, 120, 0);

            /* 画文字 (scale=2) */
            int char_w = 8 * 2;
            int char_h = 16 * 2;
            int text_w = (int)strlen(kb->keys[r][c]) * char_w;
            int tx = cx + (kb->key_w - text_w) / 2;
            int ty = cy + (kb->key_h - char_h) / 2;
            if (tx < cx + m) tx = cx + m;
            if (ty < cy + m) ty = cy + m;
            fb_draw_text(fd, tx, ty, kb->keys[r][c], txt_b, txt_g, txt_r, 2);
        }
    }

    /* 如果处于拼音模式，显示拼音缓冲和候选字 */
    if (kb->mode == KB_MODE_PINYIN && kb->pinyin_len > 0) {
        /* 拼音缓冲显示 */
        char py_display[80];
        snprintf(py_display, sizeof(py_display), "PY: %s", kb->pinyin_buf);
        fb_draw_rect(fd, kb->x, kb->y - 30, kb->w, 28, 80, 80, 80, 1);
        fb_draw_text(fd, kb->x + 5, kb->y - 26, py_display, 255, 255, 100, 2);
    }
}

int keyboard_get_key(Keyboard *kb, int tx, int ty, char *key_out)
{
    for (int r = 0; r < KB_ROWS; r++) {
        for (int c = 0; c < KB_COLS; c++) {
            if (kb->keys[r][c][0] == '\0') continue;
            int cx = kb->x + c * kb->key_w;
            int cy = kb->y + r * kb->key_h;
            if (tx >= cx && tx < cx + kb->key_w &&
                ty >= cy && ty < cy + kb->key_h) {
                if (key_out)
                    strcpy(key_out, kb->keys[r][c]);
                return 1;
            }
        }
    }
    return 0;
}

/* 简单英汉拼音映射表（常用字） */
typedef struct {
    const char *py;
    const char *chars;
} PyEntry;

static PyEntry py_table[] = {
    {"a",     "啊阿"},
    {"ai",    "爱矮挨"},
    {"an",    "安暗按"},
    {"ba",    "吧把爸八"},
    {"bai",   "白百拜"},
    {"ban",   "半办版班"},
    {"bao",   "包报保宝"},
    {"bei",   "被北杯背"},
    {"ben",   "本笨"},
    {"bi",    "比笔必闭"},
    {"bian",  "边变便编"},
    {"biao",  "表标"},
    {"bie",   "别"},
    {"bin",   "宾"},
    {"bing",  "并病冰"},
    {"bo",    "博波播"},
    {"bu",    "不步部补"},
    {"ca",    "擦"},
    {"cai",   "才彩采"},
    {"can",   "参残"},
    {"cao",   "草"},
    {"ce",    "册测策"},
    {"cha",   "查差插"},
    {"chai",  "拆"},
    {"chan",  "产"},
    {"chang", "长场常唱"},
    {"chao",  "超朝潮"},
    {"che",   "车"},
    {"chen",  "陈沉晨"},
    {"cheng", "成城程称"},
    {"chi",   "吃尺迟"},
    {"chong", "重冲充"},
    {"chu",   "出处初除"},
    {"chuai", "揣"},
    {"chuan", "穿传船串"},
    {"chuang","窗床创"},
    {"chui",  "吹"},
    {"chun",  "春纯"},
    {"ci",    "此次词"},
    {"cong",  "从聪"},
    {"cu",    "粗"},
    {"cui",   "催翠"},
    {"cun",   "村存"},
    {"cuo",   "错"},
    {"da",    "大打达答"},
    {"dai",   "代带待袋"},
    {"dan",   "但单蛋弹"},
    {"dang",  "当党"},
    {"dao",   "到道刀倒"},
    {"de",    "的地得德"},
    {"deng",  "等灯登"},
    {"di",    "地第底敌"},
    {"dian",  "点电店典"},
    {"diao",  "掉调"},
    {"die",   "爹跌"},
    {"ding",  "定顶丁"},
    {"diu",   "丢"},
    {"dong",  "动东冬懂"},
    {"dou",   "都豆斗"},
    {"du",    "读度独"},
    {"duan",  "段短断"},
    {"dui",   "对队"},
    {"dun",   "吨蹲"},
    {"duo",   "多夺"},
    {"e",     "额恶"},
    {"en",    "恩"},
    {"er",    "二儿而"},
    {"fa",    "发法"},
    {"fan",   "反饭翻"},
    {"fang",  "方放房"},
    {"fei",   "飞非费"},
    {"fen",   "分份粉"},
    {"feng",  "风丰封"},
    {"fo",    "佛"},
    {"fu",    "夫父服福"},
    {"ga",    "嘎"},
    {"gai",   "该改"},
    {"gan",   "干感赶"},
    {"gang",  "刚钢"},
    {"gao",   "高告搞"},
    {"ge",    "个歌各"},
    {"gei",   "给"},
    {"gen",   "跟根"},
    {"geng",  "更"},
    {"gong",  "工公共功"},
    {"gou",   "狗够购"},
    {"gu",    "古故顾"},
    {"gua",   "瓜挂"},
    {"guai",  "怪拐"},
    {"guan",  "关管官观"},
    {"guang", "光广"},
    {"gui",   "贵归鬼"},
    {"gun",   "滚"},
    {"guo",   "国过果锅"},
    {"ha",    "哈"},
    {"hai",   "还海害"},
    {"han",   "汉韩喊"},
    {"hang",  "行航"},
    {"hao",   "好号毫"},
    {"he",    "和何合河"},
    {"hei",   "黑"},
    {"hen",   "很恨"},
    {"heng",  "横"},
    {"hong",  "红洪"},
    {"hou",   "后候厚"},
    {"hu",    "虎湖互户"},
    {"hua",   "花话化画"},
    {"huai",  "坏怀"},
    {"huan",  "换欢环"},
    {"huang", "黄皇"},
    {"hui",   "回会灰"},
    {"hun",   "婚混"},
    {"huo",   "火活伙"},
    {"ji",    "几机记计"},
    {"jia",   "家加假价"},
    {"jian",  "见件间建"},
    {"jiang", "将讲江"},
    {"jiao",  "叫教交角"},
    {"jie",   "接节姐解"},
    {"jin",   "进今金近"},
    {"jing",  "经精京"},
    {"jiong", "窘"},
    {"jiu",   "九久就"},
    {"ju",    "句具居"},
    {"juan",  "卷"},
    {"jue",   "决觉"},
    {"jun",   "军均"},
    {"ka",    "卡"},
    {"kai",   "开"},
    {"kan",   "看"},
    {"kang",  "康"},
    {"kao",   "考靠"},
    {"ke",    "可科客课"},
    {"ken",   "肯"},
    {"kong",  "空控"},
    {"kou",   "口"},
    {"ku",    "苦哭"},
    {"kua",   "夸"},
    {"kuai",  "快块"},
    {"kuan",  "宽"},
    {"kuang", "况矿"},
    {"kui",   "亏"},
    {"kun",   "困"},
    {"kuo",   "扩"},
    {"la",    "拉啦"},
    {"lai",   "来"},
    {"lan",   "蓝兰"},
    {"lang",  "浪朗"},
    {"lao",   "老劳"},
    {"le",    "了乐"},
    {"lei",   "类雷"},
    {"leng",  "冷"},
    {"li",    "里力理立"},
    {"lia",   "俩"},
    {"lian",  "连脸联"},
    {"liang", "两亮量"},
    {"liao",  "了料"},
    {"lie",   "列"},
    {"lin",   "林临"},
    {"ling",  "零另领"},
    {"liu",   "六流留"},
    {"long",  "龙"},
    {"lou",   "楼"},
    {"lu",    "路录陆"},
    {"lv",    "绿旅律"},
    {"luan",  "乱"},
    {"lun",   "论轮"},
    {"luo",   "落罗"},
    {"ma",    "吗妈马"},
    {"mai",   "买卖"},
    {"man",   "慢满"},
    {"mang",  "忙"},
    {"mao",   "猫毛"},
    {"me",    "么"},
    {"mei",   "没美每妹"},
    {"men",   "门们"},
    {"meng",  "梦猛"},
    {"mi",    "米密迷"},
    {"mian",  "面棉"},
    {"miao",  "秒苗"},
    {"mie",   "灭"},
    {"min",   "民"},
    {"ming",  "名明命"},
    {"mo",    "摸末"},
    {"mou",   "某"},
    {"mu",    "目母木"},
    {"na",    "那拿哪"},
    {"nai",   "奶耐"},
    {"nan",   "男难南"},
    {"nao",   "脑闹"},
    {"ne",    "呢"},
    {"nei",   "内"},
    {"neng",  "能"},
    {"ni",    "你泥"},
    {"nian",  "年念"},
    {"niang", "娘"},
    {"niao",  "鸟"},
    {"nie",   "捏"},
    {"nin",   "您"},
    {"ning",  "宁"},
    {"niu",   "牛"},
    {"nong",  "农"},
    {"nu",    "努"},
    {"nv",    "女"},
    {"nuan",  "暖"},
    {"nue",   "虐"},
    {"nuo",   "诺"},
    {"o",     "哦"},
    {"ou",    "欧"},
    {"pa",    "怕爬"},
    {"pai",   "排拍派"},
    {"pan",   "盘判"},
    {"pang",  "旁胖"},
    {"pao",   "跑炮"},
    {"pei",   "配陪"},
    {"pen",   "喷"},
    {"peng",  "朋碰"},
    {"pi",    "批皮"},
    {"pian",  "片篇"},
    {"piao",  "票漂"},
    {"pie",   "撇"},
    {"pin",   "品"},
    {"ping",  "平评"},
    {"po",    "破"},
    {"pu",    "普"},
    {"qi",    "七其起气"},
    {"qia",   "恰"},
    {"qian",  "前钱千签"},
    {"qiang", "强枪墙"},
    {"qiao",  "桥巧"},
    {"qie",   "切且"},
    {"qin",   "亲琴"},
    {"qing",  "请清青情"},
    {"qiong","穷"},
    {"qiu",   "求球秋"},
    {"qu",    "去取区"},
    {"quan",  "全权圈"},
    {"que",   "却确缺"},
    {"qun",   "群"},
    {"ran",   "然"},
    {"rang",  "让"},
    {"rao",   "绕"},
    {"re",    "热"},
    {"ren",   "人认任"},
    {"reng",  "仍"},
    {"ri",    "日"},
    {"rong",  "容荣"},
    {"rou",   "肉"},
    {"ru",    "如入"},
    {"ruan",  "软"},
    {"rui",   "瑞"},
    {"run",   "润"},
    {"ruo",   "弱"},
    {"sa",    "撒"},
    {"sai",   "赛"},
    {"san",   "三散"},
    {"sang",  "桑"},
    {"sao",   "扫"},
    {"se",    "色"},
    {"sen",   "森"},
    {"sha",   "啥杀沙"},
    {"shai",  "晒"},
    {"shan",  "山善闪"},
    {"shang", "上伤商"},
    {"shao",  "少烧"},
    {"she",   "社设蛇"},
    {"shei",  "谁"},
    {"shen",  "什深身神"},
    {"sheng", "生声升省"},
    {"shi",   "是时十事"},
    {"shou",  "手受收"},
    {"shu",   "书数树"},
    {"shua",  "刷"},
    {"shuai", "帅摔"},
    {"shuan", "拴"},
    {"shuang","双爽"},
    {"shui",  "水睡谁"},
    {"shun",  "顺"},
    {"shuo",  "说"},
    {"si",    "四死思"},
    {"song",  "送松"},
    {"sou",   "搜"},
    {"su",    "速苏"},
    {"suan",  "算酸"},
    {"sui",   "岁虽随"},
    {"sun",   "孙"},
    {"suo",   "所"},
    {"ta",    "他她它"},
    {"tai",   "太台"},
    {"tan",   "谈弹"},
    {"tang",  "唐堂糖"},
    {"tao",   "讨套"},
    {"te",    "特"},
    {"teng",  "疼"},
    {"ti",    "提题体"},
    {"tian",  "天田甜"},
    {"tiao",  "条跳调"},
    {"tie",   "铁"},
    {"ting",  "听停庭"},
    {"tong",  "同通统"},
    {"tou",   "头偷"},
    {"tu",    "土图突"},
    {"tuan",  "团"},
    {"tui",   "推退"},
    {"tun",   "吞"},
    {"tuo",   "拖脱"},
    {"wa",    "瓦挖"},
    {"wai",   "外"},
    {"wan",   "玩完万晚"},
    {"wang",  "王往望网"},
    {"wei",   "为位未委"},
    {"wen",   "文问温"},
    {"weng",  "翁"},
    {"wo",    "我握"},
    {"wu",    "无五物"},
    {"xi",    "西细喜系"},
    {"xia",   "下夏吓"},
    {"xian",  "先现线县"},
    {"xiang", "想向相香"},
    {"xiao",  "小笑校"},
    {"xie",   "写谢些"},
    {"xin",   "新心信"},
    {"xing",  "行性星形"},
    {"xiong", "兄胸凶"},
    {"xiu",   "修秀休"},
    {"xu",    "需许续"},
    {"xuan",  "选宣"},
    {"xue",   "学血雪"},
    {"xun",   "寻训"},
    {"ya",    "呀压牙"},
    {"yan",   "眼言严研"},
    {"yang",  "样阳洋"},
    {"yao",   "要药腰"},
    {"ye",    "也夜业"},
    {"yi",    "一以已意"},
    {"yin",   "因音银引"},
    {"ying",  "应影英"},
    {"yo",    "哟"},
    {"yong",  "用永勇"},
    {"you",   "有又右友"},
    {"yu",    "于与雨语"},
    {"yuan",  "元原远院"},
    {"yue",   "月越乐"},
    {"yun",   "云运"},
    {"za",    "杂"},
    {"zai",   "在再"},
    {"zan",   "咱赞"},
    {"zang",  "脏"},
    {"zao",   "早造"},
    {"ze",    "则择责"},
    {"zei",   "贼"},
    {"zen",   "怎"},
    {"zeng",  "增"},
    {"zha",   "炸扎"},
    {"zhai",  "摘宅"},
    {"zhan",  "站战占"},
    {"zhang", "张长章"},
    {"zhao",  "找照赵"},
    {"zhe",   "者这折"},
    {"zhen",  "真阵针"},
    {"zheng", "正整政"},
    {"zhi",   "只之知制"},
    {"zhong", "中种重"},
    {"zhou",  "周州"},
    {"zhu",   "主住注"},
    {"zhua",  "抓"},
    {"zhuai", "拽"},
    {"zhuan", "转专"},
    {"zhuang","装状"},
    {"zhui",  "追"},
    {"zhun",  "准"},
    {"zhuo",  "桌"},
    {"zi",    "自子字"},
    {"zong",  "总宗"},
    {"zou",   "走"},
    {"zu",    "组足族"},
    {"zuan",  "钻"},
    {"zui",   "最嘴"},
    {"zun",   "尊"},
    {"zuo",   "做作左坐"},
    {NULL, NULL}
};

void keyboard_handle_pinyin(Keyboard *kb, char *key_out, char *result, int maxlen)
{
    /* 如果用户选择了候选字（key_out以'1'-'9'开头且是单字符） */
    if (kb->mode == KB_MODE_PINYIN && kb->show_candidates && key_out) {
        int idx = key_out[0] - '1';
        if (idx >= 0 && idx < kb->candidate_count) {
            /* 选择了候选字 */
            if (result) {
                int len = strlen(result);
                strncat(result, kb->candidates[idx], maxlen - len - 1);
            }
            /* 清除拼音缓冲 */
            kb->pinyin_buf[0] = '\0';
            kb->pinyin_len = 0;
            kb->show_candidates = 0;
            kb->candidate_count = 0;
            return; /* 已消耗此键 */
        }
    }

    /* 非拼音模式，直接返回 */
    if (kb->mode != KB_MODE_PINYIN)
        return;

    if (strcmp(key_out, "Del") == 0) {
        if (kb->pinyin_len > 0) {
            kb->pinyin_buf[--kb->pinyin_len] = '\0';
        }
        if (kb->pinyin_len == 0) {
            kb->show_candidates = 0;
            kb->candidate_count = 0;
        }
        /* 查找当前拼音对应的候选 */
        goto find_candidates;
    }

    if (strcmp(key_out, "Space") == 0) {
        /* 空格选择第一个候选字 */
        if (kb->show_candidates && kb->candidate_count > 0) {
            if (result) {
                int len = strlen(result);
                strncat(result, kb->candidates[0], maxlen - len - 1);
            }
        }
        kb->pinyin_buf[0] = '\0';
        kb->pinyin_len = 0;
        kb->show_candidates = 0;
        kb->candidate_count = 0;
        return;
    }

    /* 单字母键追加到拼音缓冲 */
    if (strlen(key_out) == 1 && key_out[0] >= 'a' && key_out[0] <= 'z') {
        if (kb->pinyin_len < 60) {
            kb->pinyin_buf[kb->pinyin_len++] = key_out[0];
            kb->pinyin_buf[kb->pinyin_len] = '\0';
        }
    }

find_candidates:
    /* 查找匹配的拼音 */
    if (kb->pinyin_len > 0) {
        for (int i = 0; py_table[i].py != NULL; i++) {
            if (strncmp(kb->pinyin_buf, py_table[i].py, kb->pinyin_len) == 0 &&
                strlen(py_table[i].py) == (size_t)kb->pinyin_len) {
                /* 完全匹配 */
                const char *chars = py_table[i].chars;
                int clen = strlen(chars);
                /* UTF-8中文每字3字节 */
                kb->candidate_count = 0;
                for (int j = 0; j < clen && kb->candidate_count < 9; ) {
                    strncpy(kb->candidates[kb->candidate_count], chars + j, 3);
                    kb->candidates[kb->candidate_count][3] = '\0';
                    kb->candidate_count++;
                    j += 3;
                }
                kb->show_candidates = 1;
                break;
            }
        }
    }

    /* 拼音模式消耗按键，不输出字母 */
    if (strlen(key_out) == 1 && key_out[0] >= 'a' && key_out[0] <= 'z') {
        key_out[0] = '\0';  /* 清空，表示已消耗 */
    }
}
