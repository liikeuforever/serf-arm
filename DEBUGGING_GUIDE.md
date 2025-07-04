# Serf 时间序列压缩库 - 调试和编译指南

## 📋 项目概述

Serf是一个用于时间序列数据压缩的高性能库，实现了多种压缩算法：
- **核心算法**：SerfXOR、SerfQt（支持64位和32位浮点数）
- **网络版本**：NetSerfXOR、NetSerfQt  
- **基线算法**：包含15+种对比算法（LZ77、Zstd、Snappy、SZ2、Gorilla、Chimp128等）
- **多语言支持**：C++核心库 + Python包装器

## 🛠️ 系统要求

**必需依赖：**
- CMake >= 3.20
- C++ 编译器（支持C++17标准）
- Git（用于获取GoogleTest）

**可选依赖：**
- Python 3.7+ 和 pybind11（Python接口）
- OpenMP（并行计算优化）

**平台支持：**
- ✅ macOS（已测试）
- ✅ Linux/*nix系统  
- ✅ Windows 32/64位

## 📁 项目结构

```
Serf/
├── src/                          # 核心算法实现
│   ├── compressor/              # 64位压缩器
│   ├── compressor_32/           # 32位压缩器  
│   ├── decompressor/            # 64位解压器
│   ├── decompressor_32/         # 32位解压器
│   └── utils/                   # 工具函数
├── test/                        # 测试和性能评估
│   ├── baselines/               # 基线算法实现
│   ├── data_set/               # 测试数据集
│   └── unit_test/              # 单元测试
├── pywrapper/                   # Python接口
│   ├── example/                # Python使用示例
│   └── wrapper.cpp             # pybind11包装器
└── reproduction_on_modelardb/   # ModelarDB复现实验
```

## 🚀 编译指南

### 步骤1：环境准备

```bash
# 克隆项目
git clone <repository-url>
cd Serf

# 检查系统环境
cmake --version  # 确保 >= 3.20
g++ --version    # 或 clang++ --version
python3 --version  # 可选，用于Python接口
```

### 步骤2：配置和构建

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译（使用并行编译加速）
make -j$(nproc)  # Linux/macOS
# 或者在macOS上：make -j$(sysctl -n hw.ncpu)
```

### 步骤3：处理编译问题

#### 问题1：Sprintz模块编译错误
```bash
# 症状：sprintz_xff_rle.cpp 语法错误
# 解决：临时禁用sprintz模块

# 编辑 test/baselines/sprintz/CMakeLists.txt
# 注释掉所有内容，添加：
# Temporarily disabled due to compilation errors

# 编辑 test/CMakeLists.txt  
# 注释掉：add_subdirectory(baselines/sprintz)
# 从target_link_libraries中移除sprintz
```

#### 问题2：GoogleTest下载失败
```bash
# 症状：无法下载GoogleTest
# 解决：手动下载或使用系统包管理器

# 方法1：使用系统包管理器
sudo apt install libgtest-dev  # Ubuntu/Debian
brew install googletest        # macOS

# 方法2：手动下载
cd build/_deps
git clone https://github.com/google/googletest.git googletest-src
```

## 🧪 测试指南

### 运行核心测试

**重要：测试程序对运行目录有严格要求**

```bash
# 从项目根目录运行（推荐方法）
cd Serf
./build/test/serf_test

# 或者从正确的深度目录运行
cd build/test  # 进入build/test目录
./serf_test    # 路径配置 ../../test/data_set/ 会正确指向数据文件
```

### 测试路径问题解决

如果遇到"Failed to open the file [Air-pressure.csv]"错误：

1. **检查数据文件是否存在**：
```bash
ls -la test/data_set/
# 应该看到：Air-pressure.csv, Basel-temp.csv 等文件
```

2. **验证路径配置**：
测试程序使用`test/Perf_expr_config.hpp`中的路径配置：
```cpp
const static std::string kDataSetDirPrefix = "../../test/data_set/";
```

3. **从正确目录运行**：
```bash
# 方法1：项目根目录
cd /path/to/Serf
./build/test/serf_test

# 方法2：build/test目录  
cd /path/to/Serf/build/test
./serf_test

# 方法3：创建符合路径要求的运行环境
mkdir -p /path/to/Serf/build/temp
cd /path/to/Serf/build/temp
../test/serf_test
```

### 单独测试各模块

```bash
cd build/test

# 测试核心Serf算法
./serf_test

# 测试基线算法
./deflate_test
./lz4_test  
./gorilla_test
./chimp_test

# 测试32位浮点数支持
./serf_32_test
```

## 🐍 Python接口测试

### 构建Python扩展

Python扩展会自动随主项目构建：
```bash
cd build
ls pywrapper/
# 应该看到：pyserf.cpython-xxx-darwin.so (macOS) 或 pyserf.cpython-xxx-linux-gnu.so (Linux)
```

### 测试Python接口

```bash
# 从pywrapper/example目录运行
cd pywrapper/example
PYTHONPATH=../../build/pywrapper python3 main.py

# 或者创建测试脚本
cat > test_pyserf.py << 'EOF'
import sys
sys.path.insert(0, '../../build/pywrapper')

from pyserf import PySerfXORCompressor, PySerfXORDecompressor

# 创建压缩器
compressor = PySerfXORCompressor(1000, 0.001, 0)
decompressor = PySerfXORDecompressor(0)

# 压缩测试数据
test_data = [1.1, 2.2, 3.3, 4.4, 5.5]
for value in test_data:
    compressor.add_value(value)
compressor.close()

# 解压缩
compressed = compressor.get()
decompressed = decompressor.decompress(compressed)

print(f"原始: {test_data}")
print(f"解压: {decompressed}")
print("测试成功!" if abs(len(test_data) - len(decompressed)) == 0 else "测试失败!")
EOF

python3 test_pyserf.py
```

## 🔧 常见问题排查

### 编译问题

1. **CMake版本过低**
```bash
# 错误：CMake 3.20 or higher is required
# 解决：升级CMake
brew upgrade cmake  # macOS
sudo apt update && sudo apt upgrade cmake  # Ubuntu
```

2. **C++编译器不支持C++17**
```bash
# 检查编译器版本
g++ --version
clang++ --version

# 升级编译器或使用较新版本
export CXX=g++-9  # 指定特定版本
```

3. **缺少pybind11**
```bash
# 症状：Could NOT find pybind11
# 解决：安装pybind11
pip install pybind11
# 或者禁用Python支持：cmake -DPYTHON_BINDINGS=OFF ..
```

### 运行时问题

1. **数据文件路径错误**
   - 确保从正确目录运行测试
   - 检查`test/Perf_expr_config.hpp`中的路径配置
   - 验证数据文件确实存在

2. **测试数据文件名不匹配**
```bash
# 症状：某些测试寻找不存在的文件（如Air-sensor.csv）
# 解决：检查测试配置或添加缺失的数据文件
grep -r "Air-sensor" test/unit_test/
```

3. **Python模块导入失败**
```bash
# 检查Python扩展是否正确构建
ls -la build/pywrapper/pyserf*

# 检查Python路径
export PYTHONPATH="$PWD/build/pywrapper:$PYTHONPATH"
```

## 📊 性能评估

### 运行完整基准测试

```bash
cd build/test
./PerformanceProgram

# 结果会保存在以下位置：
# - 压缩比：_compression_ratio_table.csv
# - 压缩时间：_compression_time_table.csv  
# - 解压时间：_decompression_time_table.csv
```

### 自定义测试

编辑`test/Perf_expr_config.hpp`以自定义：
- 测试数据集
- 算法组合
- 错误容忍度
- 块大小参数

## 🎯 项目验证检查清单

- [ ] CMake配置成功（无错误信息）
- [ ] 主要模块编译成功（serf, 基线算法）
- [ ] 核心测试通过（./serf_test中的SerfXOR和SerfQt测试）
- [ ] Python扩展构建成功（pyserf.so文件存在）
- [ ] Python接口测试通过（可导入和基本功能测试）
- [ ] 性能测试可运行（./PerformanceProgram）

## 🔍 高级调试技巧

### 1. 使用详细输出调试编译
```bash
make VERBOSE=1  # 显示详细编译命令
```

### 2. 单独编译特定目标
```bash
make serf_test          # 只编译serf测试
make pyserf            # 只编译Python扩展
make PerformanceProgram # 只编译性能测试
```

### 3. 调试Python接口
```python
import sys
print(sys.path)  # 检查Python路径

# 使用详细导入错误信息
try:
    import pyserf
except ImportError as e:
    print(f"Import error: {e}")
    import traceback
    traceback.print_exc()
```

### 4. 内存和性能分析
```bash
# 使用valgrind检查内存泄漏（Linux）
valgrind --leak-check=full ./test/serf_test

# 使用time命令测量性能
time ./test/serf_test

# macOS上使用instruments
instruments -t "Time Profiler" ./test/serf_test
```

---

## 📞 获得帮助

如果遇到其他问题：
1. 检查编译器和CMake版本是否满足要求
2. 确认所有依赖都已正确安装
3. 从干净的构建目录重新开始：`rm -rf build && mkdir build && cd build && cmake .. && make`
4. 查看具体的错误信息，通常包含解决线索

这个指南涵盖了Serf项目的主要编译和调试场景。遵循这些步骤应该能够成功构建和测试项目。 