# 高德地图 API 自动化测试

针对高德地图 Web 服务搭建的单元 / 接口 / 功能 / 性能四层测试体系，附一键执行脚本与三份真实测试报告。

- **技术栈**：Python、pytest / unittest、requests、Selenium(Edge)、Locust、JMeter、pytest-html
- **类型**：软件测试课程设计
- **目录**：`src` 工具函数、`tests/unit|api|functional|performance` 四层用例、`reports` HTML 报告

## 本地运行

```bash
pip install -r requirements.txt
python run_tests.py
# 或分层执行：pytest tests/unit --html=reports/unit.html
locust -f tests/performance/test_amap_performance.py
```

## 说明

- `reports/` 下 `unit_test_report.html`、`api_test_report.html`、`functional_test_report.html` 为真实执行结果，下载后用浏览器打开即可查看。
- 代码中的 `YOUR_AMAP_API_KEY` 是脱敏占位符，运行前请替换为自己申请的高德开放平台 Key，或改成读取环境变量。
- 功能测试需要 Edge 浏览器与对应 WebDriver；JMeter 脚本为 `tests/performance/amap_performance.jmx`。
