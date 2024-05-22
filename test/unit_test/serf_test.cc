#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "serf/compressor/serf_xor_compressor.h"
#include "serf/decompressor/serf_xor_decompressor.h"
#include "serf/compressor/tensor_serf_xor_compressor.h"
#include "serf/decompressor/tensor_serf_xor_decompressor.h"
#include "serf/compressor/serf_qt_compressor.h"
#include "serf/decompressor/serf_qt_decompressor.h"
#include "serf/compressor_32/serf_xor_compressor_32.h"
#include "serf/decompressor_32/serf_xor_decompressor_32.h"
#include "serf/compressor/net_serf_xor_compressor.h"
#include "serf/decompressor/net_serf_xor_decompressor.h"
#include "serf/compressor/net_serf_qt_compressor.h"
#include "serf/decompressor/net_serf_qt_decompressor.h"
#include "serf/compressor_32/serf_qt_compressor_32.h"
#include "serf/decompressor_32/serf_qt_decompressor_32.h"

const static int kBlockSize = 1000;
const static std::string kDataSetDir = "../../test/dataSet";
const static std::unordered_map<std::string, int> kFileToAdjustD{
    {"init.csv", 0},
    {"Air-pressure.csv", 0},
    {"Air-sensor.csv", 128},
    {"Bird-migration.csv", 60},
    {"Bitcoin-price.csv", 511220},
    {"Basel-temp.csv", 77},
    {"Basel-wind.csv", 128},
    {"City-temp.csv", 355},
    {"Dew-point-temp.csv", 94},
    {"IR-bio-temp.csv", 49},
    {"PM10-dust.csv", 256},
    {"Stocks-DE.csv", 253},
    {"Stocks-UK.csv", 8047},
    {"Stocks-USA.csv", 243},
    {"Wind-Speed.csv", 2}
};
constexpr static double kMaxDiff[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static float kMaxDiff32[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

/**
 * @brief Scan all data set files in DATA_SET_DIR.
 * @return A vector contains all data set file path.
 */
std::vector<std::string> ScanDataSet() {
  namespace fs = std::filesystem;
  std::vector<std::string> data_set_list;
  for (const auto &entry : fs::recursive_directory_iterator(kDataSetDir)) {
    if (fs::is_regular_file(entry)) {
      std::string file_name_of_entry = entry.path().filename();
      if (file_name_of_entry.substr(file_name_of_entry.find('.') + 1, 3) == "csv") {
        data_set_list.push_back(entry.path().string());
      }
    }
  }
  return data_set_list;
}

/**
 * @brief Read a block of double from file input stream, whose size is equal to BLOCK_SIZE
 * @param file_input_stream_ref Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
  std::vector<double> return_data;
  int read_double_count = 0;
  double buffer;
  while (!file_input_stream_ref.eof() && read_double_count < kBlockSize) {
    file_input_stream_ref >> buffer;
    return_data.emplace_back(buffer);
    ++read_double_count;
  }
  return return_data;
}

/**
 * @brief Read a block of float from file input stream, whose size is equal to BLOCK_SIZE
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<float> ReadBlock32(std::ifstream &fileInputStreamRef) {
  std::vector<float> return_data;
  int read_double_count = 0;
  float buffer;
  while (!fileInputStreamRef.eof() && read_double_count < kBlockSize) {
    fileInputStreamRef >> buffer;
    return_data.emplace_back(buffer);
    ++read_double_count;
  }
  return return_data;
}

TEST(TestSerfXOR, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
    int adjustD = kFileToAdjustD.find(fileName)->second;
    for (const auto &max_diff : kMaxDiff) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      SerfXORCompressor xor_compressor(1000, max_diff, adjustD);
      SerfXORDecompressor xor_decompressor(adjustD);

      std::vector<double> originalData;
      while ((originalData = ReadBlock(dataSetInputStream)).size() == kBlockSize) {
        for (const auto &item : originalData) {
          xor_compressor.AddValue(item);
        }
        xor_compressor.Close();
        Array<uint8_t> result = xor_compressor.compressed_bytes();
        std::vector<double> decompressed = xor_decompressor.decompress(result);
        EXPECT_EQ(originalData.size(), decompressed.size());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
            GTEST_LOG_(INFO) << originalData[i] << " " << decompressed[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
        }
      }

      dataSetInputStream.close();
    }
  }
}

TEST(TestSerfQt, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    for (const auto &max_diff : kMaxDiff) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      std::vector<double> originalData;
      while ((originalData = ReadBlock(dataSetInputStream)).size() == kBlockSize) {
        SerfQtCompressor qt_compressor(kBlockSize, max_diff);
        for (const auto &item : originalData) {
          qt_compressor.AddValue(item);
        }
        qt_compressor.Close();
        Array<uint8_t> result = qt_compressor.GetBytes();
        SerfQtDecompressor qt_decompressor(result);
        std::vector<double> decompressed = qt_decompressor.Decompress();
        EXPECT_EQ(originalData.size(), decompressed.size());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
            GTEST_LOG_(INFO) << " " << originalData[i] << " " << decompressed[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
        }
      }

      dataSetInputStream.close();
    }
  }
}

TEST(TestNetSerfXOR, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
    int adjustD = kFileToAdjustD.find(fileName)->second;
    for (const auto &max_diff : kMaxDiff) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      NetSerfXORCompressor xor_compressor(max_diff, adjustD);
      NetSerfXORDecompressor xor_decompressor(adjustD);

      double originalData;
      while (!dataSetInputStream.eof()) {
        dataSetInputStream >> originalData;
        Array<uint8_t> result = xor_compressor.Compress(originalData);
        double decompressed = xor_decompressor.Decompress(result);
        if (std::abs(originalData - decompressed) > max_diff) {
          GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
        }
        EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
      }

      dataSetInputStream.close();
    }
  }
}

TEST(TestNetSerfQt, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    for (const auto &max_diff : kMaxDiff) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      NetSerfQtCompressor qt_compressor(max_diff);
      NetSerfQtDecompressor qt_decompressor(max_diff);

      double originalData;
      while (!dataSetInputStream.eof()) {
        dataSetInputStream >> originalData;
        Array<uint8_t> result = qt_compressor.Compress(originalData);
        double decompressed = qt_decompressor.Decompress(result);
        if (std::abs(originalData - decompressed) > max_diff) {
          GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
        }
        EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
      }

      dataSetInputStream.close();
    }
  }
}

TEST(TestSerfXOR32, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    for (const auto &max_diff : kMaxDiff32) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      SerfXORCompressor32 xor_compressor_32(kBlockSize, max_diff);
      SerfXORDecompressor32 xor_decompressor_32;

      std::vector<float> originalData;
      while ((originalData = ReadBlock32(dataSetInputStream)).size() == kBlockSize) {
        for (const auto &item : originalData) {
          xor_compressor_32.AddValue(item);
        }
        xor_compressor_32.Close();
        Array<uint8_t> result = xor_compressor_32.out_buffer();
        std::vector<float> decompressed = xor_decompressor_32.Decompress(result);
        EXPECT_EQ(originalData.size(), decompressed.size());
        if (originalData.size() != decompressed.size()) {
          GTEST_LOG_(INFO) << dataSet << " " << max_diff;
        } else {
          for (int i = 0; i < kBlockSize; ++i) {
            if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
              GTEST_LOG_(INFO) << originalData[i] << " " << decompressed[i] << " " << max_diff;
            }
            EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
          }
        }
      }

      dataSetInputStream.close();
    }
  }
}

TEST(TestSerfQt32, CorrectnessTest) {
  std::vector<std::string> dataSetList = ScanDataSet();
  for (const auto &dataSet : dataSetList) {
    for (const auto &max_diff : kMaxDiff32) {
      std::ifstream dataSetInputStream(dataSet);
      if (!dataSetInputStream.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
      }

      SerfQtCompressor32 qt_compressor_32(max_diff);
      SerfQtDecompressor32 qt_decompressor_32(max_diff);

      std::vector<float> originalData;
      while ((originalData = ReadBlock32(dataSetInputStream)).size() == kBlockSize) {
        for (const auto &item : originalData) {
          qt_compressor_32.AddValue(item);
        }
        qt_compressor_32.Close();
        Array<uint8_t> result = qt_compressor_32.GetBytes();
        std::vector<float> decompressed = qt_decompressor_32.Decompress(result);
        EXPECT_EQ(originalData.size(), decompressed.size());
        if (originalData.size() != decompressed.size()) {
          GTEST_LOG_(INFO) << dataSet << " " << max_diff;
        } else {
          for (int i = 0; i < kBlockSize; ++i) {
            if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
              GTEST_LOG_(INFO) << originalData[i] << " " << decompressed[i] << " " << max_diff;
            }
            EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
          }
        }
      }

      dataSetInputStream.close();
    }
  }
}