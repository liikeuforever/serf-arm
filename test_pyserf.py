#!/usr/bin/env python3
import sys
import os

# 添加pyserf模块路径
sys.path.insert(0, '../../build/pywrapper')

try:
    from pyserf import PySerfXORCompressor, PySerfXORDecompressor
    print("成功导入pyserf模块!")
    
    # 简单测试压缩和解压缩
    compressor = PySerfXORCompressor(1000, 0.001, 0)
    decompressor = PySerfXORDecompressor(0)
    
    # 添加一些测试数据
    test_data = [1.1, 2.2, 3.3, 4.4, 5.5]
    for value in test_data:
        compressor.add_value(value)
    
    compressor.close()
    compressed = compressor.get()
    
    # 解压缩
    decompressed = decompressor.decompress(compressed)
    
    print(f"原始数据: {test_data}")
    print(f"解压缩后: {decompressed}")
    print("pyserf测试成功!")
    
except ImportError as e:
    print(f"无法导入pyserf模块: {e}")
except Exception as e:
    print(f"测试过程中出错: {e}") 