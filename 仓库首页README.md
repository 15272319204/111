# 个人项目源码集

计算机科学与技术（本科）课程设计与实训项目源码，共 7 个项目。仓库仅收录源码、配置文件、素材与说明文档，已移除 `node_modules`、`.venv`、`build`、`bin/obj`、`target` 等依赖与编译产物，并对 API Key、本机路径做了脱敏处理。

## 项目索引

| 项目 | 技术栈 | 类型 | 入口 |
| --- | --- | --- | --- |
| 高德地图 API 自动化测试 | pytest / requests / Selenium / Locust / JMeter | 软件测试 | [03-amap-api-testing](./03-amap-api-testing) |
| 个人健康管理系统 | Spring Boot / MyBatis-Plus / MySQL / Redis / JWT / Vue | Web 全栈 | [01-health-management.zip](./01-health-management.zip)（压缩包，需下载解压） |
| 新闻资讯 Android 客户端 | Kotlin / MVVM / Retrofit / Room | 移动端 | [02-android-news-app](./02-android-news-app) |
| 学生成绩管理系统 | ASP.NET Core MVC / EF Core / Identity | .NET Web | [04-aspnet-mvc-grades](./04-aspnet-mvc-grades) |
| 飞机大战 | Python / pygame | 桌面小游戏 | [05-aircraft-wars-pygame](./05-aircraft-wars-pygame) |
| 微信小程序应用 | 小程序原生 / Node.js | 小程序 | [06-wechat-miniprogram](./06-wechat-miniprogram) |
| 语音交互实训系统 | C / Python / 讯飞 SDK / framebuffer | 嵌入式实训 | [07-ai-voice-training](./07-ai-voice-training) |

## 运行结果（可直接查看）

[03-amap-api-testing/reports](./03-amap-api-testing/reports) 内有 3 份真实执行生成的 HTML 测试报告：

- 单元层 `unit_test_report.html`
- 接口层 `api_test_report.html`
- 功能层 `functional_test_report.html`

GitHub 会把 `.html` 显示为源码，请用下面任一方式查看渲染后的报告：

- 在线预览：[接口测试报告](https://htmlpreview.github.io/?https://github.com/15272319204/111/blob/main/03-amap-api-testing/reports/api_test_report.html) · [单元测试报告](https://htmlpreview.github.io/?https://github.com/15272319204/111/blob/main/03-amap-api-testing/reports/unit_test_report.html) · [功能测试报告](https://htmlpreview.github.io/?https://github.com/15272319204/111/blob/main/03-amap-api-testing/reports/functional_test_report.html)
- 或在报告页点 **Raw / Download** 下载到本地，用浏览器打开（需保留同级 `assets` 目录）。

## 本地运行

每个项目目录下的 `README.md` 都写明了技术栈、目录结构、启动命令和注意事项。通用前提：本机需装对应运行环境（JDK + Maven、Node.js、Python 3、Android Studio / .NET 9 SDK、Linux + GCC 等），依赖需要联网重新下载。

## 说明

- 项目均为课程 / 实训作品，用于学习展示；健康系统前端基于 `vue-admin-template` 改造，业务模块与权限部分为自行开发。
- 涉及第三方服务（高德地图、讯飞语音、新闻 API）的凭证均为占位或需自行申请，不包含真实密钥。
- 根目录下 3 个 `源码合集_*.zip` 是源码备份包，内容与各项目目录一致，供离线下载使用。
