# 新闻资讯 Android 客户端

采用 MVVM 分层的 Kotlin 新闻客户端，Repository 统一调度 Retrofit 网络数据与 Room 本地缓存，支持新闻列表/分类/详情、收藏持久化和管理端增删改查。

- **技术栈**：Kotlin、MVVM、ViewModel、Repository、Retrofit、Room、RecyclerView
- **类型**：Android 课程设计
- **目录**：`app/src/main/java/.../data`（local / remote / repository）、`ui`（main / detail / admin / adapter）、`viewmodel`

## 本地运行

用 Android Studio 打开本目录（Gradle Wrapper），或命令行执行 `gradlew assembleDebug`。首次打开会按本机 SDK 自动生成 `local.properties`（已从仓库移除）。

## 说明

网络数据源配置见 `app/src/main/java/com/example/myapplication/data/remote/`；如接口失效，替换为可用的新闻 API 即可。编译产物 `build/` 与 APK 未打包。
