# demo14-1API.py
import sys
from openai import OpenAI

API_KEY = "sk-5274114ec1d64e3a809c2f0860d203ec"  # 请替换成你的 API Key
BASE_URL = "https://api.deepseek.com"

def get_answer(question):
    client = OpenAI(api_key=API_KEY, base_url=BASE_URL)
    response = client.chat.completions.create(
        model="deepseek-chat",
        messages=[{"role": "user", "content": question}]
    )
    return response.choices[0].message.content

if __name__ == "__main__":
    question = sys.argv[1] if len(sys.argv) > 1 else "Hello, who are you?"
    print(get_answer(question))


