import unittest
import requests
import json

class TestAmapAPI(unittest.TestCase):
    BASE_URL = "https://restapi.amap.com/v3"
    API_KEY = "YOUR_AMAP_API_KEY"
    
    def setUp(self):
        print(f"\n【接口测试 - {self._testMethodName}】")
        print("-" * 50)
        if not self.API_KEY or self.API_KEY == "YOUR_API_KEY":
            print("✗ 请配置有效的高德地图API Key")
            self.skipTest("请配置有效的高德地图API Key")
        print(f"✓ API Key已配置")
    
    def tearDown(self):
        print("✓ 测试完成")
        print("-" * 50)
    
    def test_geocode_api_normal(self):
        print("步骤1: 准备地理编码API请求参数...")
        url = f"{self.BASE_URL}/geocode/geo"
        params = {
            "address": "北京市朝阳区望京SOHO",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "1")
        self.assertIn("geocodes", data)
        self.assertGreater(len(data["geocodes"]), 0)
        print("✓ 地理编码API测试通过")
    
    def test_geocode_api_empty_address(self):
        print("步骤1: 准备空地址参数...")
        url = f"{self.BASE_URL}/geocode/geo"
        params = {
            "address": "",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "0")
        print("✓ 空地址地理编码测试通过")
    
    def test_geocode_api_invalid_key(self):
        print("步骤1: 准备无效API Key参数...")
        url = f"{self.BASE_URL}/geocode/geo"
        params = {
            "address": "北京市",
            "key": "INVALID_KEY"
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "0")
        print("✓ 无效API Key测试通过")
    
    def test_regeocode_api_normal(self):
        print("步骤1: 准备逆地理编码API请求参数...")
        url = f"{self.BASE_URL}/geocode/regeo"
        params = {
            "location": "116.4074,39.9042",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "1")
        self.assertIn("regeocode", data)
        print("✓ 逆地理编码API测试通过")
    
    def test_regeocode_api_invalid_coordinates(self):
        print("步骤1: 准备缺少location参数...")
        url = f"{self.BASE_URL}/geocode/regeo"
        params = {
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "0")
        print("✓ 缺少参数逆地理编码测试通过")
    
    def test_poi_search_api(self):
        print("步骤1: 准备POI搜索API请求参数...")
        url = f"{self.BASE_URL}/place/text"
        params = {
            "keywords": "酒店",
            "city": "北京",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        print(f"  返回POI数量: {len(data.get('pois', []))}")
        self.assertEqual(data["status"], "1")
        self.assertIn("pois", data)
        print("✓ POI搜索API测试通过")
    
    def test_poi_search_with_boundary(self):
        print("步骤1: 准备带边界的POI搜索参数...")
        url = f"{self.BASE_URL}/place/text"
        params = {
            "keywords": "餐厅",
            "city": "上海",
            "boundary": "rectangle:121.45,31.22,121.48,31.25",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "1")
        print("✓ 带边界POI搜索测试通过")
    
    def test_weather_api(self):
        print("步骤1: 准备天气API请求参数...")
        url = f"{self.BASE_URL}/weather/weatherInfo"
        params = {
            "city": "110000",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "1")
        self.assertIn("lives", data)
        print("✓ 天气API测试通过")
    
    def test_direction_driving_api(self):
        print("步骤1: 准备路径规划API请求参数...")
        url = f"{self.BASE_URL}/direction/driving"
        params = {
            "origin": "116.4074,39.9042",
            "destination": "121.4737,31.2304",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        self.assertEqual(data["status"], "1")
        self.assertIn("route", data)
        print("✓ 路径规划API测试通过")
    
    def test_ip_location_api(self):
        print("步骤1: 准备IP定位API请求参数...")
        url = f"{self.BASE_URL}/ip"
        params = {
            "ip": "223.5.5.5",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        print(f"  响应状态: {data.get('status')}")
        print(f"  定位城市: {data.get('city')}")
        self.assertEqual(data["status"], "1")
        self.assertIn("city", data)
        print("✓ IP定位API测试通过")
    
    def test_map_static_api(self):
        print("步骤1: 准备静态地图API请求参数...")
        url = f"{self.BASE_URL}/staticmap"
        params = {
            "location": "116.4074,39.9042",
            "zoom": "10",
            "size": "400*400",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        print(f"  请求参数: {params}")
        
        print("步骤2: 发送GET请求...")
        response = requests.get(url, params=params)
        print(f"  HTTP状态码: {response.status_code}")
        
        print("步骤3: 验证响应结果...")
        self.assertEqual(response.status_code, 200)
        content_type = response.headers.get("Content-Type", "")
        print(f"  响应内容类型: {content_type}")
        self.assertTrue(content_type.startswith("image/"))
        print("✓ 静态地图API测试通过")
    
    def test_api_response_time(self):
        print("步骤1: 准备响应时间测试参数...")
        url = f"{self.BASE_URL}/geocode/geo"
        params = {
            "address": "北京市",
            "key": self.API_KEY
        }
        print(f"  请求URL: {url}")
        
        print("步骤2: 发送GET请求并记录时间...")
        response = requests.get(url, params=params)
        response_time = response.elapsed.total_seconds()
        print(f"  HTTP状态码: {response.status_code}")
        print(f"  响应时间: {response_time:.3f} 秒")
        
        print("步骤3: 验证响应时间...")
        self.assertEqual(response.status_code, 200)
        self.assertLess(response_time, 5)
        print("✓ API响应时间测试通过")

if __name__ == '__main__':
    unittest.main(verbosity=2)