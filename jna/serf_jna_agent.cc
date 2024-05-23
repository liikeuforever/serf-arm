#include "serf/compressor/net_serf_xor_compressor.h"
#include "serf/decompressor/net_serf_xor_decompressor.h"

extern "C" {

struct ByteArrayJava {
  char *bytes;
  int length;
};

static NetSerfXORCompressor *global_compressor;
static NetSerfXORDecompressor *global_decompressor;
static char *share_mem;
static struct ByteArrayJava *share_ret;

void NativeSerfXORInit(int capacity, double maxDiff, long adjustD) {
  share_ret = new struct ByteArrayJava;
  share_mem = new char [64];
  global_compressor = new NetSerfXORCompressor(capacity, maxDiff, adjustD);
  global_decompressor = new NetSerfXORDecompressor(capacity, adjustD);
}

void NativeSerfXORDestroy() {
  delete global_compressor;
  delete global_decompressor;
  delete[] share_mem;
  delete share_ret;
}

struct ByteArrayJava *NativeSerfXORCompress(double input) {
  Array<uint8_t> result = global_compressor->Compress(input);
  share_ret->length = result.length();
  share_ret->bytes = share_mem;
  memcpy(share_mem, result.begin(), result.length());
  return share_ret;
}

double NativeSerfXORDecompress(char *data, int len) {
  Array<uint8_t> input(len);
  for (int i = 0; i < len; ++i) {
    input[i] = data[i];
  }
  return global_decompressor->Decompress(input);
}

}