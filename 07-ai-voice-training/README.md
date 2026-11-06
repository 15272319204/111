# 语音交互实训系统（Linux / C）

Linux 环境下的语音交互实训项目，包含语音识别接入、AI 对话、相册 / 登录 / 主菜单等业务模块，以及基于 framebuffer 的触摸界面绘制。

- **技术栈**：C、Python、Makefile、讯飞语音 SDK（Linux iat）、framebuffer UI、Socket
- **类型**：嵌入式 / 语音实训
- **目录**：`core` 基础模块、`modules` 业务模块、`ui` 界面绘制、`ig` 图片音频资源、`Linux_iat1227_0d740f4a` SDK 接口与示例、`voice_server.py` 服务端

## 本地运行

在 Linux（或对应开发板交叉编译环境）中执行 `make`；语音能力需按 `setup_iat.sh` 安装 SDK 并配置自行申请的讯飞 AppID / Key。

## 说明

仓库仅保留源码、脚本与资源，可执行文件与第三方 SDK 二进制已移除；讯飞授权凭证需自行申请，未包含在内。
