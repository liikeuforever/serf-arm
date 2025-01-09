#include <gtest/gtest.h>

#include "baselines/sz_adt/sz/inc/sz.h"

#include "Perf_expr_config.hpp"
#include "Perf_file_utils.hpp"
#include "Perf_expr_data_struct.hpp"

void PerfADT(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  // Default config
  if (confparams_cpr == nullptr) confparams_cpr = (sz_params*) malloc(sizeof(sz_params));
  if (exe_params == nullptr) exe_params = (sz_exedata*) malloc(sizeof(sz_exedata));
  setDefaulParams(exe_params, confparams_cpr);
  confparams_cpr->errorBoundMode = SZ_ABS;
  confparams_cpr->absErrBoundDouble = max_diff * 0.999;
  confparams_cpr->ifAdtFse = 1;

  // (De)compression output buffer
  auto *compression_output = new unsigned char [block_size * 10];
  auto *decompression_output = new double [block_size];

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    sz_params comp_params = *confparams_cpr;
    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_out_size = SZ_compress_args(SZ_DOUBLE, original_data.data(), original_data.size(),
                                                   compression_output, &comp_params);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_out_size * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    size_t decompression_out_size = SZ_decompress(SZ_DOUBLE, compression_output, compression_out_size, block_size,
                                                  (unsigned char* ) decompression_output);
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
  }

  if(confparams_cpr!= nullptr) {
    free(confparams_cpr);
    confparams_cpr = nullptr;
  }
  if(exe_params != nullptr) {
    free(exe_params);
    exe_params = nullptr;
  }
  delete[] compression_output;
  delete[] decompression_output;

  perf_record.set_block_count(block_count);
  table_to_insert.insert(std::make_pair(ExprConf("ADT-FSE", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

TEST(Perf, SZ_ADT) {
  const static int kBlockSizeList[] = {100, 200, 400};
  ExprTable expr_table_sz_adt;

  for (const auto &data_set : kDataSetList) {
    std::ifstream data_input_stream(kDataSetDirPrefix + data_set);
    if (!data_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &block_size : kBlockSizeList) {
      PerfADT(data_input_stream, kBlockSizeParamAbsMaxDiff, block_size, data_set, expr_table_sz_adt);
    }

    data_input_stream.close();
  }
}