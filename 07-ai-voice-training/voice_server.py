#!/usr/bin/env python3
"""
VM 端 TCP 语音识别 + AI 服务器
接收开发板发来的录音文件 → 语音识别(STT) → DeepSeek AI → 返回结果

启动方式:
    source ~/openai_env38/bin/activate
    python voice_server.py

默认监听端口: 5000
"""

import socket
import subprocess
import sys
import os
import shutil

# ===== 配置 =====
HOST = '0.0.0.0'
PORT = 5000
SAVE_DIR = '/tmp/voice_server'
WORK_DIR = '/tmp/iat'

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DEMO14_PATH = os.path.join(BASE_DIR, 'demo14-1API.py')
IAT_SDK_DIR = os.path.join(BASE_DIR, 'Linux_iat1227_0d740f4a')
STT_TOOL = os.path.join(IAT_SDK_DIR, 'stt_tool')


def deploy_iat_files():
    """自动部署讯飞 SDK 文件到工作目录"""
    msc_src = os.path.join(IAT_SDK_DIR, 'msc')
    msc_dst = os.path.join(WORK_DIR, 'msc')
    os.makedirs(msc_dst, exist_ok=True)

    cfg_src_bin = os.path.join(IAT_SDK_DIR, 'bin', 'msc', 'msc.cfg')
    cfg_src_root = os.path.join(msc_src, 'msc.cfg')
    cfg_dst = os.path.join(msc_dst, 'msc.cfg')

    if os.path.exists(cfg_src_bin):
        shutil.copy2(cfg_src_bin, cfg_dst)
    elif os.path.exists(cfg_src_root):
        shutil.copy2(cfg_src_root, cfg_dst)

    for root, dirs, files in os.walk(msc_src):
        if 'u.data' in files:
            u_data_src = os.path.join(root, 'u.data')
            u_data_dir = os.path.basename(os.path.dirname(u_data_src))
            u_data_dst_dir = os.path.join(msc_dst, u_data_dir)
            os.makedirs(u_data_dst_dir, exist_ok=True)
            shutil.copy2(u_data_src, os.path.join(u_data_dst_dir, 'u.data'))
            break


def do_stt_iflytek(audio_path):
    """科大讯飞 SDK 语音识别"""
    if not os.path.exists(STT_TOOL):
        print(f"[讯飞] stt_tool 未编译")
        return None

    u_data_check = os.path.join(WORK_DIR, 'msc')
    if not os.path.exists(u_data_check):
        deploy_iat_files()

    try:
        sdk_lib = os.path.join(IAT_SDK_DIR, 'libs', 'x64')
        env = os.environ.copy()
        env['LD_LIBRARY_PATH'] = sdk_lib + ':' + env.get('LD_LIBRARY_PATH', '')

        print(f"[讯飞] 开始识别: {audio_path}")
        result = subprocess.run(
            [STT_TOOL, audio_path],
            capture_output=True, text=True, timeout=30,
            env=env, cwd=IAT_SDK_DIR
        )

        stderr_text = result.stderr.strip()
        stdout_text = result.stdout.strip()

        if '10407' in stderr_text or 'MSPLogin failed' in stderr_text:
            print(f"[讯飞] 认证失败 (10407)")
            return None

        if result.returncode == 0 and stdout_text:
            print(f"[讯飞] 识别结果: {stdout_text}")
            return stdout_text
        else:
            print(f"[讯飞] 识别失败 (exit={result.returncode})")
            if stderr_text:
                print(f"[讯飞] stderr: {stderr_text[:300]}")
            return None

    except subprocess.TimeoutExpired:
        print("[讯飞] 识别超时")
        return None
    except Exception as e:
        print(f"[讯飞] 异常: {e}")
        return None


def do_stt(audio_path):
    """语音识别 (STT) — 先放大音量再识别"""
    print(f"[STT] 收到音频文件: {audio_path} ({os.path.getsize(audio_path)} bytes)")

    # 音频增强：高通滤波去低频噪音 + 音量归一化
    amp_path = audio_path.replace('.wav', '_amp.wav')
    try:
        subprocess.run(
            ['sox', audio_path, amp_path,
             'highpass', '200',    # 滤除 200Hz 以下低频噪音
             'norm', '-3'],        # 归一化到 -3dB
            capture_output=True, timeout=5
        )
        if os.path.exists(amp_path) and os.path.getsize(amp_path) > 100:
            print(f"[STT] 音频增强完成")
            audio_path = amp_path
    except Exception:
        pass

    text = do_stt_iflytek(audio_path)
    if text:
        return text

    print("[STT] 使用占位文本")
    return "你好小智"


def do_ai(text):
    """调用 demo14-1API.py 获取 DeepSeek AI 回复"""
    try:
        result = subprocess.run(
            ['python3', DEMO14_PATH, text],
            capture_output=True, text=True, timeout=60
        )
        if result.returncode == 0:
            reply = result.stdout.strip()
            if reply:
                print(f"[AI] 回复 ({len(reply)} chars): {reply[:100]}...")
                return reply
            else:
                return "AI 返回了空内容"
        else:
            print(f"[AI] 错误: {result.stderr[:200]}")
            return "AI 服务暂时不可用"
    except subprocess.TimeoutExpired:
        print("[AI] 超时")
        return "AI 服务超时，请重试"
    except Exception as e:
        print(f"[AI] 异常: {e}")
        return f"AI 服务异常: {e}"


def handle_client(conn, addr):
    """处理一个客户端连接"""
    print(f"\n[连接] 来自 {addr}")

    try:
        size_data = conn.recv(16).decode('utf-8').strip()
        if not size_data:
            return
        file_size = int(size_data)
        print(f"[接收] 文件大小: {file_size} 字节")

        conn.sendall(b'OK')

        os.makedirs(SAVE_DIR, exist_ok=True)
        audio_path = os.path.join(SAVE_DIR, f'recording_{addr[1]}.wav')
        received = 0
        with open(audio_path, 'wb') as f:
            while received < file_size:
                chunk = conn.recv(min(4096, file_size - received))
                if not chunk:
                    break
                f.write(chunk)
                received += len(chunk)

        print(f"[接收] 文件已保存: {audio_path} ({received} 字节)")

        text = do_stt(audio_path)
        print(f"[STT] 最终结果: {text}")

        ai_reply = do_ai(text)

        reply_bytes = ai_reply.encode('utf-8')
        conn.sendall(f"{len(reply_bytes):08d}".encode('utf-8'))
        conn.sendall(reply_bytes)
        print(f"[发送] 已返回 {len(reply_bytes)} 字节")

    except Exception as e:
        print(f"[错误] {e}")
        try:
            conn.sendall(b'00000000ERROR')
        except:
            pass
    finally:
        conn.close()
        print(f"[连接] {addr} 已断开")


def main():
    print("=" * 50)
    print("  语音识别 + AI 服务器")
    print(f"  监听: {HOST}:{PORT}")
    print("=" * 50)
    print()

    if os.path.exists(STT_TOOL):
        print(f"[讯飞] stt_tool 已编译")
        deploy_iat_files()
    else:
        print(f"[警告] stt_tool 未编译")

    if not os.path.exists(DEMO14_PATH):
        print(f"[警告] 未找到 {DEMO14_PATH}")

    print()

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        server.bind((HOST, PORT))
        server.listen(5)
        print(f"[启动] 服务器就绪，等待开发板连接...\n")

        while True:
            conn, addr = server.accept()
            handle_client(conn, addr)

    except KeyboardInterrupt:
        print("\n[退出] 服务器关闭")
    except Exception as e:
        print(f"[错误] {e}")
    finally:
        server.close()


if __name__ == '__main__':
    main()
