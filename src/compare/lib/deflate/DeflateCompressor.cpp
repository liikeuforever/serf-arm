#include <iostream>
#include "lz4frame.h"
using namespace std;
#include <stdio.h>

#include <string.h>
void compressData(const char *inputData, size_t inputSize, char *outputData, size_t &compressedSize)
{
    // 初始化 lz4frame 压缩流
    LZ4F_cctx *cctx;
    LZ4F_errorCode_t err = LZ4F_createCompressionContext(&cctx, LZ4F_VERSION);
    if (LZ4F_isError(err))
    {
        std::cerr << "Failed to create compression context: " << LZ4F_getErrorName(err) << std::endl;
        return;
    }

    // 设置压缩参数
    LZ4F_preferences_t prefs;
    memset(&prefs, 0, sizeof(prefs)); // 将 prefs 初始化为零
    prefs.compressionLevel = 1;       // 最大压缩级别

    void *dstBuffer;
    // 执行压缩
    err = LZ4F_compressBegin(cctx, dstBuffer, 0, &prefs);
    if (LZ4F_isError(err))
    {
        std::cerr << "Failed to begin compression: " << LZ4F_getErrorName(err) << std::endl;
        LZ4F_freeCompressionContext(cctx);
        return;
    }

    // 压缩输入数据
    err = LZ4F_compressUpdate(cctx, outputData, compressedSize, inputData, inputSize, nullptr);
    if (LZ4F_isError(err))
    {
        std::cerr << "Failed to compress data: " << LZ4F_getErrorName(err) << std::endl;
        LZ4F_freeCompressionContext(cctx);
        return;
    }

    // 结束压缩
    err = LZ4F_compressEnd(cctx, outputData + compressedSize, &compressedSize, nullptr);
    if (LZ4F_isError(err))
    {
        std::cerr << "Failed to end compression: " << LZ4F_getErrorName(err) << std::endl;
        LZ4F_freeCompressionContext(cctx);
        return;
    }

    // 释放压缩流的上下文
    LZ4F_freeCompressionContext(cctx);
}

void decompressData(const char *inputData, size_t compressedSize, char *outputData, size_t &decompressedSize)
{
    // 初始化 lz4frame 解压缩流
    LZ4F_dctx *dctx;
    LZ4F_errorCode_t err = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
    if (LZ4F_isError(err))
    {
        std::cerr << "Failed to create decompression context: " << LZ4F_getErrorName(err) << std::endl;
        return;
    }

    // 执行解压缩
    err = LZ4F_decompress(dctx, outputData, &decompressedSize, inputData, &compressedSize, nullptr);
    if (LZ4F_isError(err))
    {
        std::cerr << "Failed to decompress data: " << LZ4F_getErrorName(err) << std::endl;
        LZ4F_freeDecompressionContext(dctx);
        return;
    }

    // 释放解压缩流的上下文
    LZ4F_freeDecompressionContext(dctx);
}

int main()
{
    // 原始数据
    const char *originalData = "Hello, lz4frame!";

    // 原始数据大小
    size_t originalSize = strlen(originalData);

    // 压缩后的数据缓冲区
    const size_t compressedBufferSize = LZ4F_compressFrameBound(originalSize, nullptr);
    char compressedBuffer[compressedBufferSize];

    // 解压后的数据缓冲区
    char decompressedBuffer[originalSize];

    // 压缩数据
    size_t compressedSize = 0;
    compressData(originalData, originalSize, compressedBuffer, compressedSize);

    // 解压数据
    size_t decompressedSize = 0;
    decompressData(compressedBuffer, compressedSize, decompressedBuffer, decompressedSize);

    // 打印解压后的数据
    std::cout << "Decompressed data: " << decompressedBuffer << std::endl;

    return 0;
}
