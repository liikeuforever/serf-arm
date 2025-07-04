#!/usr/bin/env python3
"""Serf Python接口快速测试脚本"""

import sys
import os

# 添加pyserf模块路径（根据实际情况调整）
sys.path.insert(0, '../../build/pywrapper')

def test_serf_xor():
    """测试SerfXOR压缩算法"""
    try:
        from pyserf import PySerfXORCompressor, PySerfXORDecompressor
        
        # 创建压缩器和解压器
        max_diff = 0.001
        compressor = PySerfXORCompressor(1000, max_diff, 0)
        decompressor = PySerfXORDecompressor(0)
        
        # 测试数据
        test_data = [1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9]
        print(f"原始数据: {test_data}")
        
        # 压缩
        for value in test_data:
            compressor.add_value(value)
        compressor.close()
        compressed = compressor.get()
        
        # 解压缩
        decompressed = decompressor.decompress(compressed)
        print(f"解压数据: {decompressed}")
        
        # 验证精度
        errors = [abs(orig - decomp) for orig, decomp in zip(test_data, decompressed)]
        max_error = max(errors)
        print(f"最大误差: {max_error}")
        print(f"误差界限: {max_diff}")
        print(f"测试结果: {'✅ 通过' if max_error <= max_diff else '❌ 失败'}")
        
        return max_error <= max_diff
        
    except ImportError as e:
        print(f"❌ 无法导入pyserf: {e}")
        return False
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        return False

def test_serf_qt():
    """测试SerfQt压缩算法"""
    try:
        from pyserf import PySerfQtCompressor, PySerfQtDecompressor
        
        max_diff = 0.001
        compressor = PySerfQtCompressor(10, max_diff)  # 块大小10
        decompressor = PySerfQtDecompressor()
        
        test_data = [1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.1]
        print(f"\n测试SerfQt算法")
        print(f"原始数据: {test_data}")
        
        # 压缩
        for value in test_data:
            compressor.add_value(value)
        compressor.close()
        compressed = compressor.get()
        
        # 解压缩
        decompressed = decompressor.decompress(compressed)
        print(f"解压数据: {decompressed}")
        
        # 验证精度
        errors = [abs(orig - decomp) for orig, decomp in zip(test_data, decompressed)]
        max_error = max(errors)
        print(f"最大误差: {max_error}")
        print(f"测试结果: {'✅ 通过' if max_error <= max_diff else '❌ 失败'}")
        
        return max_error <= max_diff
        
    except Exception as e:
        print(f"❌ SerfQt测试失败: {e}")
        return False

if __name__ == "__main__":
    print("🧪 Serf Python接口测试")
    print("=" * 50)
    
    # 检查模块文件是否存在
    module_path = "../../build/pywrapper"
    if not os.path.exists(module_path):
        print(f"❌ 模块路径不存在: {module_path}")
        print("请先编译项目: cmake --build build")
        sys.exit(1)
    
    # 运行测试
    test1 = test_serf_xor()
    test2 = test_serf_qt()
    
    print("\n📊 测试总结")
    print("=" * 50)
    print(f"SerfXOR: {'✅ 通过' if test1 else '❌ 失败'}")
    print(f"SerfQt:  {'✅ 通过' if test2 else '❌ 失败'}")
    
    if test1 and test2:
        print("\n🎉 所有测试通过！Serf Python接口工作正常。")
        sys.exit(0)
    else:
        print("\n⚠️ 部分测试失败，请检查安装。")
        sys.exit(1) 