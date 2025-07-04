#!/bin/bash

# Serf 项目高级调试测试运行脚本
# 专门解决路径问题和文件名不匹配问题

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

print_status() { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_error() { echo -e "${RED}[ERROR]${NC} $1"; }
print_debug() { echo -e "${CYAN}[DEBUG]${NC} $1"; }

# 检测项目根目录
find_project_root() {
    local current_dir="$PWD"
    while [[ "$current_dir" != "/" ]]; do
        if [[ -f "$current_dir/CMakeLists.txt" ]] && [[ -d "$current_dir/src" ]] && [[ -d "$current_dir/test" ]]; then
            echo "$current_dir"
            return 0
        fi
        current_dir="$(dirname "$current_dir")"
    done
    return 1
}

# 分析并修复路径问题
diagnose_path_issues() {
    local project_root="$1"
    
    print_status "=== 路径问题诊断 ==="
    
    # 检查数据目录
    if [[ ! -d "$project_root/test/data_set" ]]; then
        print_error "数据目录不存在: $project_root/test/data_set"
        return 1
    fi
    
    print_success "数据目录存在: $project_root/test/data_set"
    
    # 列出实际存在的数据文件
    print_status "实际存在的数据文件:"
    ls -la "$project_root/test/data_set/"*.csv | while read -r line; do
        filename=$(basename "$(echo "$line" | awk '{print $NF}')")
        size=$(echo "$line" | awk '{print $5}')
        print_debug "  $filename ($size bytes)"
    done
    
    # 检查关键文件
    local missing_files=()
    local key_files=("Air-pressure.csv" "Basel-temp.csv" "Basel-wind.csv")
    
    for file in "${key_files[@]}"; do
        if [[ -f "$project_root/test/data_set/$file" ]]; then
            print_success "✓ $file 存在"
        else
            print_warning "✗ $file 缺失"
            missing_files+=("$file")
        fi
    done
    
    # 检查测试程序期望的文件名与实际文件名的差异
    print_status "检查测试配置与实际文件的匹配性..."
    
    # 扫描测试代码中期望的文件名
    if command -v grep >/dev/null 2>&1; then
        print_debug "扫描测试代码中期望的文件名:"
        find "$project_root/test/unit_test" -name "*.cc" -exec grep -H "Air-sensor\\|Air-pressure" {} \; 2>/dev/null | while read -r line; do
            print_debug "  $line"
        done
    fi
    
    return 0
}

# 创建测试运行环境
create_test_environment() {
    local project_root="$1"
    
    print_status "=== 创建测试运行环境 ==="
    
    # 方法1：从项目根目录运行（最可靠）
    print_status "测试方法1：从项目根目录运行"
    cd "$project_root"
    
    if [[ -f "build/test/serf_test" ]]; then
        print_status "运行 serf_test（从项目根目录）..."
        if timeout 60 ./build/test/serf_test 2>&1; then
            print_success "方法1：从项目根目录运行成功！"
            return 0
        else
            print_warning "方法1：从项目根目录运行失败"
        fi
    fi
    
    # 方法2：从build/test目录运行
    print_status "测试方法2：从build/test目录运行"
    if [[ -d "$project_root/build/test" ]]; then
        cd "$project_root/build/test"
        print_debug "当前目录: $(pwd)"
        print_debug "检查相对路径: ../../test/data_set/"
        
        if [[ -d "../../test/data_set" ]]; then
            print_success "相对路径正确"
            ls -la "../../test/data_set/Air-pressure.csv" 2>/dev/null && print_success "Air-pressure.csv 可访问" || print_warning "Air-pressure.csv 不可访问"
            
            print_status "运行 serf_test（从build/test目录）..."
            if timeout 60 ./serf_test 2>&1; then
                print_success "方法2：从build/test目录运行成功！"
                return 0
            else
                print_warning "方法2：从build/test目录运行失败"
            fi
        else
            print_error "相对路径不正确"
        fi
    fi
    
    # 方法3：创建符号链接解决路径问题
    print_status "测试方法3：创建符号链接"
    cd "$project_root/build/test"
    
    if [[ ! -d "test" ]]; then
        print_status "创建符号链接: test -> ../../test"
        ln -sf "../../test" test
    fi
    
    if [[ -d "test/data_set" ]]; then
        print_success "符号链接创建成功"
        print_status "运行 serf_test（使用符号链接）..."
        if timeout 60 ./serf_test 2>&1; then
            print_success "方法3：使用符号链接成功！"
            return 0
        else
            print_warning "方法3：使用符号链接失败"
        fi
    fi
    
    # 方法4：修改工作目录并设置环境变量
    print_status "测试方法4：使用环境变量"
    cd "$project_root"
    export SERF_DATA_DIR="$project_root/test/data_set"
    
    print_status "运行 serf_test（使用环境变量）..."
    if timeout 60 ./build/test/serf_test 2>&1; then
        print_success "方法4：使用环境变量成功！"
        return 0
    else
        print_warning "方法4：使用环境变量失败"
    fi
    
    print_error "所有方法都失败了，需要进一步调试"
    return 1
}

# 深度调试文件加载问题
debug_file_loading() {
    local project_root="$1"
    
    print_status "=== 深度调试文件加载 ==="
    
    cd "$project_root"
    
    # 使用strace/dtruss跟踪文件访问（如果可用）
    if command -v strace >/dev/null 2>&1; then
        print_status "使用 strace 跟踪文件访问..."
        timeout 10 strace -e trace=openat ./build/test/serf_test 2>&1 | grep -i "air-pressure" || true
    elif command -v dtruss >/dev/null 2>&1; then
        print_status "使用 dtruss 跟踪文件访问（macOS）..."
        timeout 10 sudo dtruss -f ./build/test/serf_test 2>&1 | grep -i "air-pressure" || true
    else
        print_warning "系统调用跟踪工具不可用"
    fi
    
    # 创建一个详细的调试版本
    print_status "生成详细调试信息..."
    
    cat > temp_debug_test.cpp << 'EOF'
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

int main() {
    std::cout << "=== 文件访问调试 ===" << std::endl;
    
    // 显示当前工作目录
    std::cout << "当前工作目录: " << std::filesystem::current_path() << std::endl;
    
    // 测试不同的路径
    std::vector<std::string> test_paths = {
        "Air-pressure.csv",
        "./Air-pressure.csv", 
        "test/data_set/Air-pressure.csv",
        "./test/data_set/Air-pressure.csv",
        "../../test/data_set/Air-pressure.csv",
        "../test/data_set/Air-pressure.csv"
    };
    
    for (const auto& path : test_paths) {
        std::ifstream file(path);
        std::cout << "路径: " << path << " -> ";
        if (file.is_open()) {
            std::cout << "✓ 成功打开" << std::endl;
            file.close();
        } else {
            std::cout << "✗ 无法打开" << std::endl;
        }
        
        // 检查文件是否存在
        if (std::filesystem::exists(path)) {
            std::cout << "  文件存在: ✓" << std::endl;
        } else {
            std::cout << "  文件存在: ✗" << std::endl;
        }
    }
    
    return 0;
}
EOF
    
    if command -v g++ >/dev/null 2>&1; then
        print_status "编译并运行调试程序..."
        g++ -std=c++17 temp_debug_test.cpp -o temp_debug_test
        
        # 从不同目录运行调试程序
        echo "--- 从项目根目录运行 ---"
        cd "$project_root"
        ./temp_debug_test
        
        echo "--- 从build目录运行 ---"
        cd "$project_root/build"
        ../temp_debug_test
        
        echo "--- 从build/test目录运行 ---"
        cd "$project_root/build/test"
        ../../temp_debug_test
        
        # 清理
        rm -f "$project_root/temp_debug_test" "$project_root/temp_debug_test.cpp"
    fi
}

# 提供解决方案
provide_solutions() {
    local project_root="$1"
    
    print_status "=== 问题解决方案 ==="
    
    echo "基于诊断结果，以下是推荐的解决方案："
    echo ""
    
    echo "1. 📁 正确的运行方式："
    echo "   cd $project_root"
    echo "   ./build/test/serf_test"
    echo ""
    
    echo "2. 🔗 创建符号链接（如果路径问题持续）："
    echo "   cd $project_root/build/test"
    echo "   ln -sf ../../test test"
    echo "   ./serf_test"
    echo ""
    
    echo "3. 🏗️ 重新编译（如果有配置问题）："
    echo "   cd $project_root"
    echo "   rm -rf build"
    echo "   mkdir build && cd build"
    echo "   cmake .."
    echo "   make -j\$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    echo ""
    
    echo "4. 📝 修改配置文件（不推荐，但可作为临时解决方案）："
    echo "   编辑 test/Perf_expr_config.hpp"
    echo "   将 kDataSetDirPrefix 改为合适的路径"
    echo ""
    
    echo "5. 🐍 测试 Python 接口："
    echo "   cd $project_root"
    echo "   PYTHONPATH=build/pywrapper python3 -c 'import pyserf; print(\"OK\")'"
    echo ""
}

# 主函数
main() {
    print_status "Serf 项目高级调试器启动..."
    
    # 1. 找到项目根目录
    PROJECT_ROOT=$(find_project_root)
    if [[ $? -ne 0 ]]; then
        print_error "无法找到项目根目录！"
        exit 1
    fi
    
    print_success "项目根目录: $PROJECT_ROOT"
    
    # 2. 诊断路径问题
    if ! diagnose_path_issues "$PROJECT_ROOT"; then
        print_error "路径诊断失败"
        exit 1
    fi
    
    # 3. 尝试创建工作的测试环境
    print_status "尝试解决测试运行问题..."
    if create_test_environment "$PROJECT_ROOT"; then
        print_success "找到可工作的测试运行方法！"
    else
        print_warning "标准方法失败，进行深度调试..."
        debug_file_loading "$PROJECT_ROOT"
    fi
    
    # 4. 提供解决方案
    provide_solutions "$PROJECT_ROOT"
    
    print_success "调试完成！请根据上述建议解决问题。"
}

main "$@" 