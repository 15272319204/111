# Proxm 嵌入式多功能系统 Makefile
# 交叉编译器: aarch64-linux-gnu-gcc 7.5.0

TOOLCHAIN = ../gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin
CC        = $(TOOLCHAIN)/aarch64-linux-gnu-gcc
CFLAGS    = -Wall -O2 -pthread -I.
LDFLAGS   = -pthread -lm -fno-use-linker-plugin -static

# ===== 语音功能配置 =====
# 启用真实 TCP 语音:  make ENABLE_VOICE=1
# 默认使用模拟数据
ifdef ENABLE_VOICE
CFLAGS += -DENABLE_VOICE
endif

TARGET  = proxm

# 源文件列表
SRCS = main.c \
       ui/fb_draw.c \
       ui/touch.c \
       ui/widgets.c \
       ui/keyboard.c \
       modules/login.c \
       modules/register.c \
       modules/main_menu.c \
       modules/album.c \
       modules/writepad.c \
       modules/media_player.c \
       modules/ai_chat.c \
       core/user_list.c \
       core/file_io.c \
       core/voice_thread.c

OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "============================================"
	@echo "  编译完成: $(TARGET)"
	@echo "  语音: $(if $(ENABLE_VOICE),真实TCP模式,模拟模式)"
	@echo "============================================"
	@echo ""
	@echo "  === 使用说明 ==="
	@echo "  1. 上传 proxm 到开发板: cd / && rz -y"
	@echo "  2. 开发板运行: chmod +x /proxm && /proxm"
	@echo ""
	@echo "  === 真实语音模式 ==="
	@echo "  虚拟机: source ~/openai_env38/bin/activate"
	@echo "  虚拟机: python voice_server.py"
	@echo "  开发板: wifi_start.sh 'SSID' 'password'"
	@echo ""

# 编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "清理完成"

# 部署到开发板（修改 IP 地址）
deploy:
	@echo "部署 proxm 到开发板:"
	@echo "  cd / && rz -y"
	@echo "  选择 proxm 上传"
	@echo "  chmod +x /proxm && /proxm"

# 虚拟机端启动语音服务器
vm-server:
	@echo "在虚拟机中运行:"
	@echo "  source ~/openai_env38/bin/activate"
	@echo "  cd /mnt/hgfs/test12/proxm"
	@echo "  python voice_server.py"

.PHONY: all clean deploy vm-server
