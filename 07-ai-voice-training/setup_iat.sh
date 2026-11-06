#!/bin/bash
# ============================================
# 科大讯飞 STT 环境部署脚本
# 在虚拟机中运行: bash setup_iat.sh
# ============================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
IAT_SDK="$SCRIPT_DIR/Linux_iat1227_0d740f4a"
WORK_DIR="/tmp/iat"
MSC_SRC="$IAT_SDK/msc"
MSC_DST="$WORK_DIR/msc"

echo "========================================="
echo "  科大讯飞 STT 环境部署"
echo "========================================="
echo ""
echo "SDK 目录: $IAT_SDK"
echo "工作目录: $WORK_DIR"
echo ""

# 1. 创建工作目录结构
echo "[1/4] 创建工作目录..."
mkdir -p "$MSC_DST"

# 2. 复制 msc.cfg（优先用 bin/msc/ 中有完整配置的版本）
echo "[2/4] 部署 msc.cfg..."
if [ -f "$IAT_SDK/bin/msc/msc.cfg" ]; then
    cp "$IAT_SDK/bin/msc/msc.cfg" "$MSC_DST/msc.cfg"
    echo "  -> 使用 bin/msc/msc.cfg (完整配置)"
elif [ -f "$MSC_SRC/msc.cfg" ]; then
    cp "$MSC_SRC/msc.cfg" "$MSC_DST/msc.cfg"
    echo "  -> 使用 msc/msc.cfg"
else
    # 创建默认 msc.cfg
    cat > "$MSC_DST/msc.cfg" << 'CFGEOF'
## Copyright (C) 2010-2015 iFLYTEK.
[logger]
file                             = msc.log
title                            = Mobile Speech Client
level                            = -1
output                           = 1
filter                           = -1
style                            = -1
flush                            = 0
CFGEOF
    echo "  -> 创建默认 msc.cfg"
fi

# 3. 复制许可证文件 u.data（关键！缺这个会导致 10407 错误）
echo "[3/4] 部署许可证文件..."
U_DATA_DIR=$(find "$MSC_SRC" -name "u.data" -type f 2>/dev/null | head -1)
if [ -n "$U_DATA_DIR" ]; then
    U_DATA_PARENT=$(dirname "$U_DATA_DIR")
    U_DATA_HASH=$(basename "$U_DATA_PARENT")
    mkdir -p "$MSC_DST/$U_DATA_HASH"
    cp "$U_DATA_DIR" "$MSC_DST/$U_DATA_HASH/u.data"
    echo "  -> 已部署: msc/$U_DATA_HASH/u.data"
else
    echo "  [警告] 未找到 u.data 许可证文件！"
    echo "  SDK 将无法通过云端认证（错误 10407）"
fi

# 4. 复制 libmsc.so 到系统库路径
echo "[4/4] 部署 libmsc.so..."
if [ -f "$IAT_SDK/libs/x64/libmsc.so" ]; then
    sudo cp "$IAT_SDK/libs/x64/libmsc.so" /usr/lib/ 2>/dev/null || {
        echo "  sudo 失败，尝试复制到 /tmp/"
        cp "$IAT_SDK/libs/x64/libmsc.so" /tmp/
    }
    sudo ldconfig 2>/dev/null || true
    echo "  -> libmsc.so 已部署"
else
    echo "  [警告] 未找到 libs/x64/libmsc.so"
fi

# 5. 验证部署
echo ""
echo "========================================="
echo "  验证部署结果"
echo "========================================="
echo ""

check_file() {
    if [ -f "$1" ]; then
        echo "  [OK] $1 ($(wc -c < "$1") bytes)"
    else
        echo "  [MISSING] $1"
    fi
}

check_file "$MSC_DST/msc.cfg"
U_DATA_CHECK=$(find "$MSC_DST" -name "u.data" -type f 2>/dev/null | head -1)
if [ -n "$U_DATA_CHECK" ]; then
    echo "  [OK] $U_DATA_CHECK ($(wc -c < "$U_DATA_CHECK") bytes)"
else
    echo "  [MISSING] u.data 许可证文件未找到！"
fi

# 6. 测试 STT
echo ""
echo "========================================="
echo "  测试 STT 工具"
echo "========================================="
echo ""

STT_TOOL="$IAT_SDK/stt_tool"
if [ -f "$STT_TOOL" ]; then
    # 用 SDK 自带的测试音频
    TEST_WAV="$IAT_SDK/iat_test.wav"
    if [ -f "$TEST_WAV" ]; then
        echo "测试音频: $TEST_WAV"
        echo "运行: LD_LIBRARY_PATH=$IAT_SDK/libs/x64 $STT_TOOL $TEST_WAV"
        echo ""
        export LD_LIBRARY_PATH="$IAT_SDK/libs/x64:$LD_LIBRARY_PATH"
        "$STT_TOOL" "$TEST_WAV" 2>&1 || true
    else
        echo "注意: 没有测试音频文件，跳过测试"
        echo "可以用 arecord 录制测试: arecord -d3 -c1 -r16000 -twav -fS16_LE /tmp/test.wav"
    fi
else
    echo "[警告] stt_tool 未编译，请先运行:"
    echo "  cd $IAT_SDK && make -f Makefile.stt"
fi

echo ""
echo "========================================="
echo "  部署完成"
echo "========================================="
echo ""
echo "如果 STT 仍然报 10407 错误，说明 appid 0d740f4a 可能已过期。"
echo "替代方案: 运行 install_stt_fallback.sh 安装 Vosk 离线识别。"
