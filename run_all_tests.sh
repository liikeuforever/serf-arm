#!/bin/bash

# 运行所有单元测试的脚本
echo "=== 开始运行所有单元测试 ==="

# 测试文件列表
tests=(
    "buff_test"
    "chimp_test"
    "deflate_test"
    "elf_star_test"
    "elf_test"
    "fpc_test"
    "gorilla_test"
    "lz4_test"
    "lz77_test"
    "machete_test"
    "serf_test"
    "sim_piece_test"
)

# 切换到构建目录
cd build/test

passed=0
failed=0
total=${#tests[@]}

echo "找到 $total 个测试程序"
echo

# 运行每个测试
for test in "${tests[@]}"; do
    echo "=== 运行 $test ==="
    if ./"$test"; then
        echo "✅ $test: PASSED"
        ((passed++))
    else
        echo "❌ $test: FAILED"
        ((failed++))
    fi
    echo
done

# 输出总结
echo "=== 测试结果总结 ==="
echo "总测试数: $total"
echo "通过: $passed"
echo "失败: $failed"
echo "通过率: $(( passed * 100 / total ))%"

if [ $failed -eq 0 ]; then
    echo "🎉 所有测试都通过了！"
    exit 0
else
    echo "⚠️  有 $failed 个测试失败"
    exit 1
fi 