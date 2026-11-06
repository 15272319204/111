# 微信小程序（借阅 / 商城类应用）

原生微信小程序前端 + Node.js 简易后端，包含首页、详情、收藏、历史、优惠券、个人中心、管理页等页面与自定义 TabBar 组件。

- **技术栈**：微信小程序原生（WXML / WXSS / JS）、Node.js、Express 风格后端
- **类型**：小程序课程设计
- **目录**：`pages`、`components`、`utils`、`backend`（server.js 与 package.json）

## 本地运行

用微信开发者工具导入本目录（可使用测试号），按需修改 `project.config.json` 中的 AppID。后端：`cd backend && npm install && node server.js`。

## 说明

`node_modules/` 未打包，首次运行需 npm install。


---

## 原项目自带说明

# 校园影院影讯查询小程序

## 项目简介

本项目为校园影院影讯查询小程序，用于展示校园影院影片排期、影片简介、放映时间。线下海报信息更新慢，小程序可实时展示影讯，搭配数据库存储影片数据，支持用户查看、收藏影片。

## 技术栈

- **前端**: 微信小程序 (WXML/WXSS/JS)
- **后端**: Node.js + Express
- **数据存储**: 内存存储（无数据库）

## 功能模块

### 首页
- 影片海报列表
- 正在放映影片推荐（横向滚动）
- 全部影片列表（卡片展示）

### 影片详情页
- 影片介绍（导演、主演、时长、上映时间、评分）
- 放映场次展示
- 收藏功能

### 我的收藏页
- 已收藏影片列表
- 取消收藏功能

## 项目结构

```
├── pages/
│   ├── index/         # 首页
│   ├── detail/        # 影片详情页
│   └── favorites/     # 我的收藏页
├── backend/           # Node.js后端
│   ├── server.js      # 服务器入口
│   └── package.json   # 依赖配置
├── app.js
├── app.json
├── app.wxss
└── README.md
```

## 快速开始

### 前端开发

1. 打开微信开发者工具
2. 导入项目文件夹
3. 配置项目信息

### 后端开发

1. 进入 `backend` 目录
2. 安装依赖: `npm install`
3. 启动服务器: `node server.js`

### 数据库配置（可选）

1. 创建数据库: `campus_cinema`
2. 执行 `init.sql` 初始化数据
3. 修改 `server.js` 中的数据库连接配置

## API接口

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/movies` | GET | 获取所有影片列表 |
| `/api/movies/:id` | GET | 获取单个影片详情 |
| `/api/favorites` | GET | 获取收藏列表 |
| `/api/favorites/check` | GET | 检查影片是否已收藏 |
| `/api/favorites/add` | POST | 添加收藏 |
| `/api/favorites/remove` | POST | 取消收藏 |

## 注意事项

1. 后端默认运行在 `http://localhost:3000`
2. 小程序需要配置服务器域名白名单
3. 数据库连接信息需要根据实际情况修改

## 开发者信息

- 项目名称: 校园影院影讯查询小程序
- 开发工具: 微信开发者工具、Navicat
- 开发语言: JavaScript (Node.js)、WXML/WXSS