import re
import urllib.parse

def validate_address(address):
    if not isinstance(address, str):
        return False
    if len(address.strip()) == 0:
        return False
    if len(address) > 500:
        return False
    return True

def validate_coordinates(lat, lng):
    try:
        lat = float(lat)
        lng = float(lng)
        if -90 <= lat <= 90 and -180 <= lng <= 180:
            return True
        return False
    except (ValueError, TypeError):
        return False

def format_address(address_parts):
    if not isinstance(address_parts, dict):
        return ""
    parts = []
    if 'province' in address_parts and address_parts['province']:
        parts.append(address_parts['province'])
    if 'city' in address_parts and address_parts['city']:
        parts.append(address_parts['city'])
    if 'district' in address_parts and address_parts['district']:
        parts.append(address_parts['district'])
    if 'detail' in address_parts and address_parts['detail']:
        parts.append(address_parts['detail'])
    return ''.join(parts)

def extract_search_keyword(url):
    parsed = urllib.parse.urlparse(url)
    query = urllib.parse.parse_qs(parsed.query)
    if 'q' in query:
        return query['q'][0]
    if 'query' in query:
        return query['query'][0]
    return None

def generate_place_url(place_id):
    if not place_id or not isinstance(place_id, str):
        return None
    if not re.match(r'^[A-Za-z0-9_-]+$', place_id):
        return None
    return f"https://ditu.amap.com/detail/detail.htm?placeid={place_id}"

def calculate_distance(lat1, lng1, lat2, lng2):
    from math import radians, sin, cos, sqrt, atan2
    try:
        lat1, lng1, lat2, lng2 = float(lat1), float(lng1), float(lat2), float(lng2)
        R = 6371.0
        lat1_rad = radians(lat1)
        lng1_rad = radians(lng1)
        lat2_rad = radians(lat2)
        lng2_rad = radians(lng2)
        dlat = lat2_rad - lat1_rad
        dlng = lng2_rad - lng1_rad
        a = sin(dlat / 2)**2 + cos(lat1_rad) * cos(lat2_rad) * sin(dlng / 2)**2
        c = 2 * atan2(sqrt(a), sqrt(1 - a))
        return R * c
    except (ValueError, TypeError):
        return None

def parse_bounds(bounds_str):
    pattern = r'^([-\d.]+),([-\d.]+),([-\d.]+),([-\d.]+)$'
    match = re.match(pattern, bounds_str)
    if match:
        return {
            'min_lat': float(match.group(1)),
            'min_lng': float(match.group(2)),
            'max_lat': float(match.group(3)),
            'max_lng': float(match.group(4))
        }
    return None

def get_zoom_level(scale):
    if scale >= 10000000:
        return 3
    elif scale >= 5000000:
        return 4
    elif scale >= 2000000:
        return 5
    elif scale >= 1000000:
        return 6
    elif scale >= 500000:
        return 7
    elif scale >= 200000:
        return 8
    elif scale >= 100000:
        return 9
    elif scale >= 50000:
        return 10
    elif scale >= 25000:
        return 11
    elif scale >= 10000:
        return 12
    elif scale >= 5000:
        return 13
    elif scale >= 2000:
        return 14
    elif scale >= 1000:
        return 15
    else:
        return 16