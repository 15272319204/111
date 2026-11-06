from locust import HttpUser, task, between, constant
import json

class AmapUser(HttpUser):
    wait_time = between(1, 3)
    
    @task(3)
    def test_geocode_api(self):
        params = {
            "address": "北京市朝阳区望京SOHO",
            "key": "YOUR_AMAP_API_KEY"
        }
        self.client.get("/v3/geocode/geo", params=params)
    
    @task(2)
    def test_regeocode_api(self):
        params = {
            "location": "116.4074,39.9042",
            "key": "YOUR_AMAP_API_KEY"
        }
        self.client.get("/v3/geocode/regeo", params=params)
    
    @task(2)
    def test_poi_search(self):
        params = {
            "keywords": "酒店",
            "city": "北京",
            "key": "YOUR_AMAP_API_KEY"
        }
        self.client.get("/v3/place/text", params=params)
    
    @task(1)
    def test_weather_api(self):
        params = {
            "city": "110000",
            "key": "YOUR_AMAP_API_KEY"
        }
        self.client.get("/v3/weather/weatherInfo", params=params)
    
    @task(1)
    def test_direction_api(self):
        params = {
            "origin": "116.4074,39.9042",
            "destination": "121.4737,31.2304",
            "key": "YOUR_AMAP_API_KEY"
        }
        self.client.get("/v3/direction/driving", params=params)
    
    def on_start(self):
        print("User started")
    
    def on_stop(self):
        print("User stopped")

if __name__ == "__main__":
    import os
    os.system("locust -f test_amap_performance.py --host=https://restapi.amap.com")