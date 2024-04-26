#include "DeflateCompressor.h"

DeflateCompressor::DeflateCompressor() {
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        throw std::runtime_error("[Deflate Error]: Failed to init.");
    }
    strm.avail_out = BLOCK_SIZE * static_cast<int>(sizeof(double));
    strm.next_out = compress_pack._data.get();
}

DeflateCompressor::~DeflateCompressor() {
    deflateEnd(&strm);
}

void DeflateCompressor::addValue(double v) {
    strm.avail_in = sizeof(double);
    strm.next_in = reinterpret_cast<unsigned char *>(&v);
    ret = deflate(&strm, Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR) {
        throw std::runtime_error("[Deflate Error]: Failed to addValue.");
    }
}

void DeflateCompressor::close() {
    ret = deflate(&strm, Z_FINISH);
    if (ret == Z_STREAM_ERROR) {
        throw std::runtime_error("[Deflate Error]: Failed to addValue.");
    }
}

Array<unsigned char> DeflateCompressor::getBytes() {
    return compress_pack;
}

long DeflateCompressor::getCompressedSizeInBits() {
    return (BLOCK_SIZE * static_cast<int>(sizeof(double)) - strm.avail_out) * 8;
}
