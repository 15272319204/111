import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from src.map_utils import (
    validate_address,
    validate_coordinates,
    format_address,
    extract_search_keyword,
    generate_place_url,
    calculate_distance,
    parse_bounds,
    get_zoom_level
)

class TestMapUtils(unittest.TestCase):
    
    def setUp(self):
        print(f"\n【测试用例: {self._testMethodName}】")
        print("-" * 50)
    
    def tearDown(self):
        print("✓ 测试通过")
        print("-" * 50)
    
    def test_validate_address_normal_case(self):
        print("🔍 测试目的: 验证正常地址的有效性")
        print()
        test_cases = ["北京市朝阳区", "上海市浦东新区陆家嘴", "广东省深圳市南山区科技园"]
        
        for i, address in enumerate(test_cases, 1):
            print(f"  {i}. 验证地址: '{address}'")
            result = validate_address(address)
            self.assertTrue(result)
            print(f"     ✓ 通过")
    
    def test_validate_address_empty(self):
        print("🔍 测试目的: 验证空地址的处理")
        print()
        test_cases = ["", "   "]
        
        for i, address in enumerate(test_cases, 1):
            print(f"  {i}. 验证: {repr(address)}")
            result = validate_address(address)
            self.assertFalse(result)
            print(f"     ✓ 正确返回False")
    
    def test_validate_address_invalid_type(self):
        print("🔍 测试目的: 验证无效类型输入的处理")
        print()
        test_cases = [None, 123, [], {}]
        descriptions = ["None", "整数", "列表", "字典"]
        
        for i, (value, desc) in enumerate(zip(test_cases, descriptions), 1):
            print(f"  {i}. 验证{desc}: {repr(value)}")
            result = validate_address(value)
            self.assertFalse(result)
            print(f"     ✓ 正确返回False")
    
    def test_validate_address_too_long(self):
        print("🔍 测试目的: 验证超长地址的处理")
        print()
        print("  1. 生成501个字符的超长地址...")
        long_address = "a" * 501
        print(f"     地址长度: {len(long_address)}")
        print("  2. 验证超长地址...")
        result = validate_address(long_address)
        self.assertFalse(result)
        print("     ✓ 正确返回False")
    
    def test_validate_coordinates_valid(self):
        print("🔍 测试目的: 验证有效坐标")
        print()
        test_cases = [
            (39.9042, 116.4074, "北京坐标"),
            (0, 0, "原点"),
            (90, 180, "最大边界"),
            (-90, -180, "最小边界")
        ]
        
        for i, (lat, lng, desc) in enumerate(test_cases, 1):
            print(f"  {i}. 验证{desc} ({lat}, {lng})")
            result = validate_coordinates(lat, lng)
            self.assertTrue(result)
            print(f"     ✓ 通过")
    
    def test_validate_coordinates_invalid(self):
        print("🔍 测试目的: 验证无效坐标范围")
        print()
        test_cases = [
            (91, 100, "纬度超过90"),
            (0, 181, "经度超过180"),
            (-91, 0, "纬度低于-90"),
            (0, -181, "经度低于-180")
        ]
        
        for i, (lat, lng, desc) in enumerate(test_cases, 1):
            print(f"  {i}. 验证{desc} ({lat}, {lng})")
            result = validate_coordinates(lat, lng)
            self.assertFalse(result)
            print(f"     ✓ 正确返回False")
    
    def test_validate_coordinates_invalid_type(self):
        print("🔍 测试目的: 验证坐标无效类型")
        print()
        test_cases = [
            ("abc", 100, "纬度为字符串"),
            (100, "xyz", "经度为字符串"),
            (None, 100, "纬度为None")
        ]
        
        for i, (lat, lng, desc) in enumerate(test_cases, 1):
            print(f"  {i}. 验证{desc} ({repr(lat)}, {repr(lng)})")
            result = validate_coordinates(lat, lng)
            self.assertFalse(result)
            print(f"     ✓ 正确返回False")
    
    def test_format_address_complete(self):
        print("🔍 测试目的: 测试完整地址格式化")
        print()
        print("  1. 准备完整地址数据:")
        address = {
            'province': '北京市',
            'city': '北京市',
            'district': '朝阳区',
            'detail': '望京SOHO'
        }
        print(f"     {address}")
        print("  2. 执行格式化...")
        result = format_address(address)
        print(f"  3. 格式化结果: '{result}'")
        self.assertEqual(result, "北京市北京市朝阳区望京SOHO")
        print("     ✓ 格式化正确")
    
    def test_format_address_partial(self):
        print("🔍 测试目的: 测试部分地址格式化")
        print()
        print("  1. 准备部分地址数据:")
        address = {
            'city': '上海市',
            'detail': '外滩'
        }
        print(f"     {address}")
        print("  2. 执行格式化...")
        result = format_address(address)
        print(f"  3. 格式化结果: '{result}'")
        self.assertEqual(result, "上海市外滩")
        print("     ✓ 格式化正确")
    
    def test_format_address_empty(self):
        print("🔍 测试目的: 测试空地址格式化")
        print()
        print("  1. 格式化空字典...")
        result = format_address({})
        self.assertEqual(result, "")
        print("     ✓ 正确返回空字符串")
        print("  2. 格式化None...")
        result = format_address(None)
        self.assertEqual(result, "")
        print("     ✓ 正确返回空字符串")
    
    def test_extract_search_keyword(self):
        print("🔍 测试目的: 测试URL关键词提取")
        print()
        test_cases = [
            ("https://ditu.amap.com/?q=北京", "北京"),
            ("https://ditu.amap.com/search?query=上海", "上海"),
            ("https://ditu.amap.com/", None)
        ]
        
        for i, (url, expected) in enumerate(test_cases, 1):
            print(f"  {i}. 提取URL: {url}")
            result = extract_search_keyword(url)
            print(f"     提取结果: {repr(result)}")
            self.assertEqual(result, expected)
            print(f"     ✓ 提取正确")
    
    def test_generate_place_url_valid(self):
        print("🔍 测试目的: 测试生成有效POI URL")
        print()
        placeid = "B0FFFAB2J7J"
        print(f"  1. 使用placeid: '{placeid}'")
        result = generate_place_url(placeid)
        expected = "https://ditu.amap.com/detail/detail.htm?placeid=B0FFFAB2J7J"
        print(f"  2. 生成结果: '{result}'")
        self.assertEqual(result, expected)
        print("     ✓ URL生成正确")
    
    def test_generate_place_url_invalid(self):
        print("🔍 测试目的: 测试无效输入生成URL")
        print()
        test_cases = [None, "", "invalid@place"]
        descriptions = ["None", "空字符串", "含特殊字符"]
        
        for i, (value, desc) in enumerate(zip(test_cases, descriptions), 1):
            print(f"  {i}. 测试{desc}: {repr(value)}")
            result = generate_place_url(value)
            self.assertIsNone(result)
            print(f"     ✓ 正确返回None")
    
    def test_calculate_distance(self):
        print("🔍 测试目的: 测试距离计算")
        print()
        print("  1. 计算北京到上海的距离:")
        print("     北京: (39.9042, 116.4074)")
        print("     上海: (31.2304, 121.4737)")
        distance = calculate_distance(39.9042, 116.4074, 31.2304, 121.4737)
        print(f"  2. 计算结果: {distance:.2f} km")
        self.assertIsNotNone(distance)
        self.assertTrue(1000 < distance < 1500)
        print("     ✓ 距离计算正确")
    
    def test_calculate_distance_invalid(self):
        print("🔍 测试目的: 测试无效输入距离计算")
        print()
        print("  1. 测试纬度为字符串...")
        result = calculate_distance("abc", 100, 200, 300)
        self.assertIsNone(result)
        print("     ✓ 正确返回None")
        print("  2. 测试纬度为None...")
        result = calculate_distance(None, 100, 200, 300)
        self.assertIsNone(result)
        print("     ✓ 正确返回None")
    
    def test_parse_bounds_valid(self):
        print("🔍 测试目的: 测试解析有效边界字符串")
        print()
        bounds_str = "39.9,116.4,40.0,116.5"
        print(f"  1. 解析边界字符串: '{bounds_str}'")
        bounds = parse_bounds(bounds_str)
        print(f"  2. 解析结果: {bounds}")
        self.assertEqual(bounds['min_lat'], 39.9)
        self.assertEqual(bounds['min_lng'], 116.4)
        self.assertEqual(bounds['max_lat'], 40.0)
        self.assertEqual(bounds['max_lng'], 116.5)
        print("     ✓ 解析正确")
    
    def test_parse_bounds_invalid(self):
        print("🔍 测试目的: 测试解析无效边界字符串")
        print()
        test_cases = ["invalid", "1,2,3"]
        descriptions = ["无效字符串", "不完整字符串"]
        
        for i, (value, desc) in enumerate(zip(test_cases, descriptions), 1):
            print(f"  {i}. 解析{desc}: '{value}'")
            result = parse_bounds(value)
            self.assertIsNone(result)
            print(f"     ✓ 正确返回None")
    
    def test_get_zoom_level(self):
        print("🔍 测试目的: 测试获取缩放级别")
        print()
        test_cases = [
            (10000000, 3),
            (5000, 13),
            (500, 16),
            (20000000, 3)
        ]
        
        for i, (distance, expected) in enumerate(test_cases, 1):
            print(f"  {i}. 距离 {distance:,} 米")
            result = get_zoom_level(distance)
            print(f"     结果: {result} (预期: {expected})")
            self.assertEqual(result, expected)
            print(f"     ✓ 正确")

if __name__ == '__main__':
    unittest.main(verbosity=2)