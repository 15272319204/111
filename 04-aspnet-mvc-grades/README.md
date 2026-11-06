# 学生成绩管理系统（ASP.NET Core MVC）

基于 ASP.NET Core MVC 的成绩管理站点，含学生 / 课程 / 成绩的增删改查、分页、首页统计看板，以及登录注册与角色管理。

- **技术栈**：.NET 9、ASP.NET Core MVC、Entity Framework Core（Code First + Migrations）、Razor Pages、Identity、Bootstrap/jQuery
- **类型**：.NET 课程设计
- **目录**：`MvcGrades/Controllers`、`Models`、`Views`、`Pages/Account`、`Data`、`Migrations`

## 本地运行

```bash
cd MvcGrades
dotnet restore
dotnet ef database update
dotnet run
```

## 说明

数据库连接串在 `MvcGrades/appsettings.json`，请按本机环境修改。`bin/`、`obj/`、`.vs/` 等生成物未打包。
