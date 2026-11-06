import subprocess
import sys
import os

REPORTS_DIR = "reports"

def ensure_reports_dir():
    if not os.path.exists(REPORTS_DIR):
        os.makedirs(REPORTS_DIR)

def fix_html_encoding(report_path):
    with open(report_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    if '<meta charset="utf-8"/>' not in content:
        content = content.replace('<meta charset=', '<meta charset="utf-8"/>')
    
    with open(report_path, 'w', encoding='utf-8') as f:
        f.write(content)

def print_test_summary(result, test_type):
    print("\n" + "=" * 60)
    print(f"【{test_type} - 测试结果汇总】")
    print("=" * 60)
    
    if result.returncode == 0:
        print("✓ 所有测试通过！")
    else:
        print("✗ 部分测试失败！")
    
    print(f"退出码: {result.returncode}")
    print("=" * 60)

def run_unit_tests():
    print("=" * 60)
    print("🚀 开始运行单元测试...")
    print("=" * 60)
    print()
    
    ensure_reports_dir()
    
    env = os.environ.copy()
    env['PYTHONUTF8'] = '1'
    env['PYTHONIOENCODING'] = 'utf-8'
    
    print("📋 测试文件: tests/unit/test_map_utils.py")
    print("🔍 测试内容: 地图工具类函数测试")
    print("----------------------------------------")
    print()
    
    result = subprocess.run([
        sys.executable, "-m", "pytest", 
        "tests/unit/test_map_utils.py",
        "-v", "-s", "--tb=short",
        f"--html={REPORTS_DIR}/unit_test_report.html",
        "--self-contained-html"
    ], capture_output=False, text=True, encoding='utf-8', env=env)
    
    print()
    print_test_summary(result, "单元测试")
    print(f"📊 测试报告: {REPORTS_DIR}/unit_test_report.html")
    print("=" * 60)
    return result.returncode

def run_functional_tests():
    print("=" * 60)
    print("🚀 开始运行功能测试...")
    print("=" * 60)
    print()
    
    ensure_reports_dir()
    
    env = os.environ.copy()
    env['PYTHONUTF8'] = '1'
    env['PYTHONIOENCODING'] = 'utf-8'
    
    print("📋 测试文件: tests/functional/test_amap_functional.py")
    print("🔍 测试内容: 高德地图网页功能测试 (Edge浏览器)")
    print("----------------------------------------")
    print()
    
    result = subprocess.run([
        sys.executable, "-m", "pytest",
        "tests/functional/test_amap_functional.py",
        "-v", "-s", "--tb=short",
        f"--html={REPORTS_DIR}/functional_test_report.html",
        "--self-contained-html"
    ], capture_output=False, text=True, encoding='utf-8', env=env)
    
    print()
    print_test_summary(result, "功能测试")
    print(f"📊 测试报告: {REPORTS_DIR}/functional_test_report.html")
    print("=" * 60)
    return result.returncode

def run_api_tests():
    print("=" * 60)
    print("🚀 开始运行接口测试...")
    print("=" * 60)
    print()
    
    ensure_reports_dir()
    
    env = os.environ.copy()
    env['PYTHONUTF8'] = '1'
    env['PYTHONIOENCODING'] = 'utf-8'
    
    print("📋 测试文件: tests/api/test_amap_api.py")
    print("🔍 测试内容: 高德地图REST API接口测试")
    print(f"🔑 API Key: {os.environ.get('AMAP_API_KEY', '已配置')}")
    print("----------------------------------------")
    print()
    
    result = subprocess.run([
        sys.executable, "-m", "pytest",
        "tests/api/test_amap_api.py",
        "-v", "-s", "--tb=short",
        f"--html={REPORTS_DIR}/api_test_report.html",
        "--self-contained-html"
    ], capture_output=False, text=True, encoding='utf-8', env=env)
    
    print()
    print_test_summary(result, "接口测试")
    print(f"📊 测试报告: {REPORTS_DIR}/api_test_report.html")
    print("=" * 60)
    return result.returncode

def show_project_structure():
    print("=" * 60)
    print("📂 项目结构")
    print("=" * 60)
    for root, dirs, files in os.walk('.'):
        level = root.replace('.', '').count(os.sep)
        indent = ' ' * 2 * level
        print(f"{indent}{os.path.basename(root)}/")
        subindent = ' ' * 2 * (level + 1)
        for file in sorted(files):
            print(f"{subindent}{file}")

def show_test_info():
    print("\n" + "=" * 60)
    print("📝 测试说明")
    print("=" * 60)
    print("【单元测试】: 测试地图工具类的核心函数")
    print("  - 地址验证、坐标验证、地址格式化")
    print("  - URL解析、距离计算、边界解析")
    print("  - 共18个测试用例")
    print()
    print("【功能测试】: 使用Edge浏览器测试高德地图网页")
    print("  - 页面标题验证、搜索功能、地图缩放")
    print("  - 图层切换、路线规划、POI信息窗口")
    print("  - 共12个测试用例")
    print()
    print("【接口测试】: 测试高德地图REST API")
    print("  - 地理编码、逆地理编码、POI搜索")
    print("  - 天气查询、路径规划、IP定位")
    print("  - 共12个测试用例")
    print("=" * 60)

if __name__ == "__main__":
    show_project_structure()
    show_test_info()
    
    print("\n" + "=" * 60)
    print("请选择要运行的测试:")
    print("1. 🧪 单元测试")
    print("2. 🌐 功能测试")
    print("3. 🔌 接口测试")
    print("4. 🚀 运行所有测试")
    print("5. ❌ 退出")
    print("=" * 60)
    
    try:
        choice = input("请输入选择 (1-5): ")
        
        if choice == "1":
            run_unit_tests()
        elif choice == "2":
            run_functional_tests()
        elif choice == "3":
            run_api_tests()
        elif choice == "4":
            print("\n🎉 开始运行所有测试...")
            print("=" * 60)
            
            unit_result = run_unit_tests()
            print("\n" + "=" * 60)
            print("等待5秒后继续功能测试...")
            print("=" * 60)
            import time
            time.sleep(5)
            
            func_result = run_functional_tests()
            print("\n" + "=" * 60)
            print("等待5秒后继续接口测试...")
            print("=" * 60)
            time.sleep(5)
            
            api_result = run_api_tests()
            
            print("\n" + "=" * 60)
            print("🏆 所有测试运行完成！")
            print("=" * 60)
            print(f"单元测试: {'通过' if unit_result == 0 else '失败'}")
            print(f"功能测试: {'通过' if func_result == 0 else '失败'}")
            print(f"接口测试: {'通过' if api_result == 0 else '失败'}")
            print("\n📊 测试报告已生成在 reports/ 目录下")
            print("=" * 60)
            
        elif choice == "5":
            print("👋 退出程序")
        else:
            print("❌ 无效的选择，请输入1-5")
    except Exception as e:
        print(f"❌ 发生错误: {e}")
        import traceback
        traceback.print_exc()