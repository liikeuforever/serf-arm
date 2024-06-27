#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "baselines/sz_adt/sz/inc/sz.h"

const static int BLOCK_SIZE = 150;
const static std::string DATA_SET_DIR = "../../test/data_set";

constexpr static double MAX_DIFF[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

/**
 * @brief Scan all data set files in DATA_SET_DIR.
 * @return A vector contains all data set file path.
 */
std::vector<std::string> ScanDataSet() {
  namespace fs = std::filesystem;
  std::vector<std::string> dataSetList;
  for (const auto &entry : fs::recursive_directory_iterator(DATA_SET_DIR)) {
    if (fs::is_regular_file(entry)) {
      std::string fileNameOfEntry = entry.path().filename();
      if (fileNameOfEntry.substr(fileNameOfEntry.find('.') + 1, 3) == "csv") {
        dataSetList.push_back(entry.path().string());
      }
    }
  }
  return dataSetList;
}

/**
 * @brief Read a block of double from file input stream, whose size is equal to BLOCK_SIZE
 * @param file_input_stream_ref Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
  std::vector<double> returnData;
  int readDoubleCount = 0;
  double buffer;
  while (!file_input_stream_ref.eof() && readDoubleCount < BLOCK_SIZE) {
    file_input_stream_ref >> buffer;
    returnData.emplace_back(buffer);
    ++readDoubleCount;
  }
  return returnData;
}

/**
 * @brief Read a block of float from file input stream, whose size is equal to BLOCK_SIZE
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<float> ReadBlock32(std::ifstream &fileInputStreamRef) {
  std::vector<float> returnData;
  int readDoubleCount = 0;
  float buffer;
  while (!fileInputStreamRef.eof() && readDoubleCount < BLOCK_SIZE) {
    fileInputStreamRef >> buffer;
    returnData.emplace_back(buffer);
    ++readDoubleCount;
  }
  return returnData;
}

TEST(TestSZ_ADT, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
    for (const auto &max_diff : MAX_DIFF) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      if (confparams_cpr == nullptr) confparams_cpr = (sz_params*) malloc(sizeof(sz_params));
      if (exe_params == nullptr) exe_params = (sz_exedata*) malloc(sizeof(sz_exedata));
      setDefaulParams(exe_params, confparams_cpr);
      confparams_cpr->errorBoundMode = SZ_ABS;
      confparams_cpr->absErrBoundDouble = max_diff * 0.999;
      confparams_cpr->ifAdtFse = 1;

      auto *compression_output = new unsigned char [BLOCK_SIZE * 10];
      auto *decompression_output = new double [BLOCK_SIZE];

      std::vector<double> originalData;
      while ((originalData = ReadBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
        sz_params comp_params = *confparams_cpr;
        size_t compression_out_size = SZ_compress_args(SZ_DOUBLE, originalData.data(), originalData.size(),
                                                       compression_output, &comp_params);
        size_t decompression_out_size = SZ_decompress(SZ_DOUBLE, compression_output, compression_out_size, BLOCK_SIZE,
                                       (unsigned char* ) decompression_output);
        for (int i = 0; i < BLOCK_SIZE; ++i) {
          if (std::abs(originalData[i] - decompression_output[i]) > max_diff) {
            GTEST_LOG_(INFO) << originalData[i] << " " << decompression_output[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(originalData[i] - decompression_output[i]) <= max_diff);
        }
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

      dataSetInputStream.close();
    }
  }
}