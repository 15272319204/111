from setuptools import setup, find_packages

setup(
    name="amap_test",
    version="1.0.0",
    packages=find_packages(),
    install_requires=[
        'selenium==4.18.1',
        'pytest==7.4.4',
        'requests==2.31.0',
        'locust==2.15.1',
        'webdriver-manager==4.0.1',
        'pytest-html==4.1.1'
    ],
    entry_points={
        'console_scripts': [
            'run-tests=run_tests:main',
        ],
    },
)