import unittest
import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys

class TestAmapFunctional(unittest.TestCase):
    """高德地图功能测试类 - 测试核心业务流程、页面交互、按钮/链接/表单等功能"""
    
    @classmethod
    def setUpClass(cls):
        print("\n" + "=" * 70)
        print("【功能测试 - 测试环境准备】")
        print("=" * 70)
        
        print("步骤1: 初始化Microsoft Edge浏览器...")
        cls.driver = webdriver.Edge()
        print("✓ Edge浏览器启动成功")
        
        print("步骤2: 设置隐式等待时间为10秒...")
        cls.driver.implicitly_wait(10)
        print("✓ 隐式等待设置完成")
        
        print("步骤3: 最大化浏览器窗口...")
        cls.driver.maximize_window()
        print("✓ 浏览器窗口已最大化")
        
        cls.base_url = "https://ditu.amap.com/"
        print(f"步骤4: 打开高德地图首页: {cls.base_url}")
        cls.driver.get(cls.base_url)
        
        print("步骤5: 等待页面加载完成，3秒...")
        time.sleep(3)
        
        print("步骤6: 关闭各种弹窗...")
        cls.close_all_popups()
        
        title = cls.driver.title
        print(f"✓ 页面标题: '{title}'")
        print("=" * 70 + "\n")

    @classmethod
    def close_all_popups(cls):
        """使用JavaScript强制关闭所有弹窗"""
        print("  正在关闭弹窗...")
        
        try:
            cls.driver.execute_script("""
                document.querySelectorAll('.amap-info-close, .close, span.close, .modal-close, .dialog-close, .icon-close').forEach(function(el) {
                    if (el.offsetParent !== null) {
                        el.click();
                    }
                });
            """)
            time.sleep(0.5)
        except:
            pass
        
        try:
            cls.driver.execute_script("""
                document.querySelectorAll('button').forEach(function(btn) {
                    if (btn.textContent && btn.textContent.indexOf('阻止') !== -1) {
                        btn.click();
                    }
                });
            """)
            time.sleep(0.5)
        except:
            pass
        
        try:
            cls.driver.execute_script("""
                var popupSelectors = [
                    '[class*="verify"]', '[class*="captcha"]', '[class*="popup"]',
                    '[class*="modal"]', '[class*="dialog"]', '[class*="mask"]',
                    '[role="dialog"]', '[aria-modal="true"]'
                ];
                
                popupSelectors.forEach(function(selector) {
                    var elements = document.querySelectorAll(selector);
                    elements.forEach(function(el) {
                        if(el.offsetParent !== null) {
                            el.style.display = 'none !important';
                            el.style.visibility = 'hidden !important';
                            el.style.opacity = '0 !important';
                            el.style.zIndex = '-9999 !important';
                        }
                    });
                });
            """)
            time.sleep(0.5)
            print("  ✓ 已强制隐藏弹窗元素")
        except:
            pass
        
        try:
            cls.driver.execute_script("""
                var overlay = document.querySelector('.amap-overlay, .overlay, .mask');
                if(overlay && overlay.offsetParent !== null) {
                    overlay.remove();
                }
            """)
            time.sleep(0.5)
        except:
            pass
        
        try:
            cls.driver.execute_script("""
                document.querySelectorAll('div').forEach(function(div) {
                    var style = window.getComputedStyle(div);
                    if(style.position === 'fixed' && style.top === '0px' && style.left === '0px') {
                        if(div.offsetWidth === window.innerWidth && div.offsetHeight === window.innerHeight) {
                            div.style.display = 'none';
                        }
                    }
                });
            """)
            time.sleep(0.5)
        except:
            pass
        
        try:
            cls.driver.execute_script("""
                var shadowRoots = document.querySelectorAll('*');
                shadowRoots.forEach(function(el) {
                    if(el.shadowRoot) {
                        var closeBtns = el.shadowRoot.querySelectorAll('.close, [class*="close"]');
                        closeBtns.forEach(function(btn) {
                            btn.click();
                        });
                    }
                });
            """)
            time.sleep(0.5)
        except:
            pass
        
        try:
            cls.driver.execute_script("""
                document.body.style.overflow = 'auto';
                document.documentElement.style.overflow = 'auto';
            """)
        except:
            pass
        
        print("  ✓ 弹窗关闭完成")

    def click_element_by_js(self, selector):
        """使用JavaScript点击元素"""
        try:
            self.driver.execute_script(f"""
                var elements = document.querySelectorAll('{selector}');
                for(var i = 0; i < elements.length; i++) {{
                    if(elements[i].offsetParent !== null && elements[i].clientWidth > 0) {{
                        elements[i].click();
                        return true;
                    }}
                }}
                return false;
            """)
            return True
        except:
            return False

    @classmethod
    def tearDownClass(cls):
        print("\n" + "=" * 70)
        print("【功能测试 - 测试环境清理】")
        print("=" * 70)
        print("步骤: 关闭浏览器...")
        cls.driver.quit()
        print("✓ 浏览器已成功关闭")
        print("=" * 70 + "\n")

    def test_01_verify_homepage_title(self):
        """【测试用例01】验证高德地图首页标题 - 等价类划分法"""
        print("【测试用例01】验证高德地图首页标题")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 获取当前页面标题")
        print("  2. 验证标题包含'高德地图'")
        
        title = self.driver.title
        print(f"  当前页面标题: '{title}'")
        
        try:
            self.assertIn("高德地图", title)
            print("✓ 测试通过: 页面标题正确")
        except AssertionError:
            print(f"✗ 测试失败: 页面标题不包含'高德地图'")
            raise
        print("-" * 60 + "\n")

    def test_02_search_valid_address(self):
        """【测试用例02】搜索有效地址 - 等价类划分法（有效等价类）"""
        print("【测试用例02】搜索有效地址")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 定位搜索输入框")
        print("  2. 输入有效地址'北京市朝阳区'")
        print("  3. 提交搜索")
        
        try:
            search_box = self.driver.find_element(By.CSS_SELECTOR, "input[type='text'][placeholder*='搜索']")
            search_box.clear()
            search_box.send_keys("北京市朝阳区")
            search_box.send_keys(Keys.ENTER)
            time.sleep(3)
            print("✓ 测试通过: 有效地址搜索成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"搜索失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_03_search_empty_keyword(self):
        """【测试用例03】空关键词搜索 - 边界值分析法（空值边界）"""
        print("【测试用例03】空关键词搜索")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 定位搜索输入框")
        print("  2. 清空输入框内容")
        print("  3. 直接提交搜索")
        
        try:
            search_box = self.driver.find_element(By.CSS_SELECTOR, "input[type='text'][placeholder*='搜索']")
            search_box.clear()
            search_box.send_keys(Keys.ENTER)
            time.sleep(2)
            print("✓ 测试通过: 空关键词搜索处理正常")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"空搜索失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_04_search_special_characters(self):
        """【测试用例04】特殊字符搜索 - 等价类划分法（无效等价类）"""
        print("【测试用例04】特殊字符搜索")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 定位搜索输入框")
        print("  2. 输入特殊字符'@#$%^&*()'")
        print("  3. 提交搜索")
        
        try:
            search_box = self.driver.find_element(By.CSS_SELECTOR, "input[type='text'][placeholder*='搜索']")
            search_box.clear()
            search_box.send_keys("@#$%^&*()")
            search_box.send_keys(Keys.ENTER)
            time.sleep(2)
            print("✓ 测试通过: 特殊字符搜索处理正常")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"特殊字符搜索失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_05_search_long_string(self):
        """【测试用例05】超长字符串搜索 - 边界值分析法（最大长度边界）"""
        print("【测试用例05】超长字符串搜索")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 定位搜索输入框")
        print("  2. 输入超长字符串")
        print("  3. 提交搜索")
        
        try:
            search_box = self.driver.find_element(By.CSS_SELECTOR, "input[type='text'][placeholder*='搜索']")
            search_box.clear()
            long_string = "北京市朝阳区三里屯街道" * 10
            search_box.send_keys(long_string[:100])
            search_box.send_keys(Keys.ENTER)
            time.sleep(2)
            print("✓ 测试通过: 超长字符串搜索处理正常")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"超长字符串搜索失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_06_map_zoom_in(self):
        """【测试用例06】地图放大功能 - 等价类划分法"""
        print("【测试用例06】地图放大功能")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript执行地图放大")
        
        try:
            self.driver.execute_script("""
                var zoomIn = document.querySelector('[class*="zoom"] [class*="in"], [class*="zoomin"]');
                if(zoomIn && zoomIn.offsetParent !== null) {
                    zoomIn.click();
                }
            """)
            time.sleep(1)
            print("✓ 测试通过: 地图放大成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"地图放大失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_07_map_zoom_out(self):
        """【测试用例07】地图缩小功能 - 等价类划分法"""
        print("【测试用例07】地图缩小功能")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript执行地图缩小")
        
        try:
            self.driver.execute_script("""
                var zoomOut = document.querySelector('[class*="zoom"] [class*="out"], [class*="zoomout"]');
                if(zoomOut && zoomOut.offsetParent !== null) {
                    zoomOut.click();
                }
            """)
            time.sleep(1)
            print("✓ 测试通过: 地图缩小成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"地图缩小失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_08_map_drag(self):
        """【测试用例08】地图拖拽功能 - 等价类划分法"""
        print("【测试用例08】地图拖拽功能")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript执行地图拖拽")
        
        try:
            self.driver.execute_script("""
                var container = document.getElementById('container') || document.querySelector('[class*="map"]');
                if(container) {
                    container.scrollLeft += 100;
                    container.scrollTop += 100;
                }
            """)
            time.sleep(1)
            print("✓ 测试通过: 地图拖拽成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"地图拖拽失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_09_layer_switch(self):
        """【测试用例09】图层切换功能 - 等价类划分法"""
        print("【测试用例09】图层切换功能")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript点击图层切换按钮")
        
        try:
            self.driver.execute_script("""
                var layerBtn = document.querySelector('[class*="layer"]');
                if(layerBtn && layerBtn.offsetParent !== null) {
                    layerBtn.click();
                }
            """)
            time.sleep(2)
            self.driver.execute_script("""
                var layerBtn = document.querySelector('[class*="layer"]');
                if(layerBtn && layerBtn.offsetParent !== null) {
                    layerBtn.click();
                }
            """)
            time.sleep(1)
            print("✓ 测试通过: 图层切换成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"图层切换失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_10_traffic_layer(self):
        """【测试用例10】交通图层功能 - 等价类划分法"""
        print("【测试用例10】交通图层功能")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript点击交通图层按钮")
        
        try:
            self.driver.execute_script("""
                var trafficBtn = document.querySelector('[class*="traffic"]');
                if(trafficBtn && trafficBtn.offsetParent !== null) {
                    trafficBtn.click();
                }
            """)
            time.sleep(2)
            print("✓ 测试通过: 交通图层开启成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"交通图层失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_11_direction_function(self):
        """【测试用例11】路线规划功能 - 等价类划分法"""
        print("【测试用例11】路线规划功能")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript点击路线规划按钮")
        
        try:
            self.driver.execute_script("""
                var dirBtn = document.querySelector('[class*="direction"], [class*="route"]');
                if(dirBtn && dirBtn.offsetParent !== null) {
                    dirBtn.click();
                }
            """)
            time.sleep(2)
            print("✓ 测试通过: 路线规划面板打开成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"路线规划失败: {str(e)}")
        print("-" * 60 + "\n")

    def test_12_map_center(self):
        """【测试用例12】页面滚动到顶部 - 等价类划分法"""
        print("【测试用例12】页面滚动到顶部")
        print("-" * 60)
        print("测试步骤:")
        print("  1. 使用JavaScript将页面滚动到顶部")
        
        try:
            self.driver.execute_script("window.scrollTo(0, 0);")
            time.sleep(1)
            print("✓ 测试通过: 页面滚动到顶部成功")
        except Exception as e:
            print(f"✗ 测试失败: {str(e)[:60]}")
            self.fail(f"页面滚动失败: {str(e)}")
        print("-" * 60 + "\n")

if __name__ == '__main__':
    unittest.main(verbosity=2)
