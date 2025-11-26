#!/bin/bash

# Calibmar 编译和安装脚本
# 这个脚本将帮助你从源代码编译和安装 Calibmar 工具及其 Python 绑定

set -e  # 遇到错误立即退出

echo "========================================"
echo "Calibmar 编译和安装脚本"
echo "========================================"

# 检查操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "检测到 Linux 系统"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "检测到 macOS 系统"
else
    echo "不支持的操作系统: $OSTYPE"
    exit 1
fi

# 检查必要的工具
echo "检查必要的工具..."
command -v git >/dev/null 2>&1 || { echo "需要安装 git"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "需要安装 cmake"; exit 1; }
command -v make >/dev/null 2>&1 || { echo "需要安装 make 或 ninja"; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "需要安装 python3"; exit 1; }

echo "所有必要的工具都已安装。"

# 创建构建目录
echo "创建构建目录..."
mkdir -p build
cd build

# 配置 CMake
echo "配置 CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CUDA_ARCHITECTURES=86 \
    -DCALIBMAR_PYTHON_BINDINGS=ON \
    -DCALIBMAR_TESTS_ENABLED=OFF

# 编译
echo "编译项目..."
make -j$(nproc)

# 安装 Python 包
echo "安装 Python 包..."
cd ..
python3 -m pip install .

echo "========================================"
echo "安装完成！"
echo ""
echo "使用方法："
echo "1. 导入包：import pycalibmar"
echo "2. 查看示例：python example_usage.py"
echo "3. 运行测试：python test_bindings.py"
echo ""
echo "注意："
echo "- 确保 CUDA 架构设置正确（当前设置为 86，对应 RTX 30 系列）"
echo "- 如果遇到 CUDA 相关错误，可以禁用 CUDA：-DCUDA_ENABLED=OFF"
echo "- 对于其他 GPU 架构，请修改 CMAKE_CUDA_ARCHITECTURES"
echo "========================================"