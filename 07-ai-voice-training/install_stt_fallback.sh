#!/bin/bash
# ============================================
# STT 备用方案安装器
# 当科大讯飞 appid 过期时，安装 Vosk 离线识别
#
# 在虚拟机中运行: bash install_stt_fallback.sh
# ============================================

set -e

echo "========================================="
echo "  STT 备用方案安装"
echo "========================================="
echo ""
echo "选择安装方案:"
echo ""
echo "  A) Google Web Speech — 免费，需外网，中文识别好"
echo "     安装: pip install SpeechRecognition"
echo "     无需额外配置"
echo ""
echo "  B) Vosk 离线识别 — 无需外网，模型 ~42MB"
echo "     安装: pip install vosk"
echo "     需下载中文模型"
echo ""
echo "  C) 两者都安装"
echo ""

read -p "请选择 [A/B/C] (默认 A): " choice
choice=${choice:-A}

# ===== Python 环境检查 =====
echo ""
echo "[检查] Python 环境..."
if command -v python3 &>/dev/null; then
    echo "  Python: $(python3 --version)"
else
    echo "  [错误] 未找到 python3"
    exit 1
fi

# ===== 方案 A: Google Speech Recognition =====
install_google() {
    echo ""
    echo "========================================="
    echo "  安装 Google Web Speech"
    echo "========================================="

    echo "[安装] SpeechRecognition..."
    pip install SpeechRecognition

    # 测试
    echo ""
    echo "[测试] 验证安装..."
    python3 -c "
import speech_recognition as sr
print(f'SpeechRecognition {sr.__version__} 安装成功')
print('支持中文 (zh-CN)')
" 2>&1

    echo ""
    echo "[完成] Google STT 已就绪"
}

# ===== 方案 B: Vosk =====
install_vosk() {
    echo ""
    echo "========================================="
    echo "  安装 Vosk 离线识别"
    echo "========================================="

    echo "[安装] vosk + soundfile..."
    pip install vosk soundfile

    # 下载中文小模型
    MODEL_DIR="/tmp/vosk-model-small-cn-0.22"
    MODEL_ZIP="/tmp/vosk-model-small-cn-0.22.zip"

    if [ -d "$MODEL_DIR" ]; then
        echo "[跳过] 模型已存在: $MODEL_DIR"
    else
        echo "[下载] 中文小模型 (~42MB)..."
        MODEL_URL="https://alphacephei.com/vosk/models/vosk-model-small-cn-0.22.zip"

        if command -v wget &>/dev/null; then
            wget -O "$MODEL_ZIP" "$MODEL_URL" 2>&1 || {
                echo "[警告] wget 下载失败，请手动下载:"
                echo "  $MODEL_URL"
                echo "  解压到: $MODEL_DIR"
            }
        elif command -v curl &>/dev/null; then
            curl -L -o "$MODEL_ZIP" "$MODEL_URL" 2>&1 || {
                echo "[警告] curl 下载失败"
            }
        else
            echo "[错误] 未找到 wget 或 curl"
        fi

        if [ -f "$MODEL_ZIP" ]; then
            echo "[解压] 模型文件..."
            unzip -o "$MODEL_ZIP" -d /tmp/
            rm -f "$MODEL_ZIP"
            echo "[完成] 模型已安装: $MODEL_DIR"
        fi
    fi

    # 测试
    echo ""
    echo "[测试] 验证安装..."
    python3 -c "
import vosk
print(f'Vosk 安装成功')
print(f'模型路径: $MODEL_DIR')
import os
if os.path.isdir('$MODEL_DIR'):
    print('模型目录存在')
else:
    print('[警告] 模型目录不存在，请手动下载')
" 2>&1

    echo ""
    echo "[完成] Vosk 已就绪"
}

# ===== 执行安装 =====
case "$choice" in
    [Aa]) install_google ;;
    [Bb]) install_vosk ;;
    [Cc])
        install_google
        install_vosk
        ;;
    *)
        echo "无效选择: $choice"
        exit 1
        ;;
esac

# ===== 更新 voice_server.py 配置 =====
echo ""
echo "========================================="
echo "  配置 voice_server.py"
echo "========================================="

SERVER_PY="$(cd "$(dirname "$0")" && pwd)/voice_server.py"

if [ -f "$SERVER_PY" ]; then
    case "$choice" in
        [Aa])
            echo "Google STT 已启用 (USE_GOOGLE_STT = True)"
            echo "讯飞 SDK 失败后会自动使用 Google"
            ;;
        [Bb])
            echo "Vosk 已启用"
            echo "如需优先使用 Vosk，编辑 voice_server.py:"
            echo "  USE_VOSK = True"
            ;;
        [Cc])
            echo "两个方案都已安装"
            echo "Google 和 Vosk 都已可用"
            ;;
    esac
fi

echo ""
echo "========================================="
echo "  安装完成"
echo "========================================="
echo ""
echo "voice_server.py 会自动按优先级使用 STT 引擎:"
echo "  1. 科大讯飞 SDK (如 appid 有效)"
echo "  2. Google Web Speech (免费，需外网)"
echo "  3. Vosk 离线识别 (无外网也工作)"
echo ""
echo "启动服务器:"
echo "  source ~/openai_env38/bin/activate"
echo "  python voice_server.py"
