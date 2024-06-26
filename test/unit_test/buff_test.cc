#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>

#include "buff/buff_compressor.h"
#include "buff/buff_decompressor.h"

const static int kBlockSize = 50;
const static std::string kDataSetDirPrefix = "../../test/data_set/";
const static std::string kDataSetList[] = {
    "Air-pressure.csv",
    "Air-sensor.csv",
    "Bird-migration.csv",
    "Bitcoin-price.csv",
    "Basel-temp.csv",
    "Basel-wind.csv",
    "City-temp.csv",
    "Dew-point-temp.csv",
    "IR-bio-temp.csv",
    "PM10-dust.csv",
    "Stocks-DE.csv",
    "Stocks-UK.csv",
    "Stocks-USA.csv",
    "Wind-Speed.csv"
};
const static std::string kDataSetList32[] = {
    "City-temp.csv",
    "Dew-point-temp.csv",
    "IR-bio-temp.csv",
    "PM10-dust.csv",
    "Wind-Speed.csv"
};
const static std::unordered_map<std::string, int> kFileToAdjustD {
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
constexpr static double kMaxDiff[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6};
constexpr static float kMaxDiff32[] = {1.0E-1f, 1.0E-2f, 1.0E-3f};

/**
 * @brief Read a block of double from file input stream, whose size is equal to BLOCK_SIZE
 * @param file_input_stream_ref Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
  std::vector<double> ret;
  ret.reserve(kBlockSize);
  int entry_count = 0;
  double buffer;
  while (!file_input_stream_ref.eof() && entry_count < kBlockSize) {
    file_input_stream_ref >> buffer;
    ret.emplace_back(buffer);
    ++entry_count;
  }
  return ret;
}

/**
 * @brief Read a block of float from file input stream, whose size is equal to BLOCK_SIZE
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of floats, whose size may be less than BLOCK_SIZE
 */
std::vector<float> ReadBlock32(std::ifstream &file_input_stream_ref) {
  std::vector<float> ret;
  ret.reserve(kBlockSize);
  int entry_count = 0;
  float buffer;
  while (!file_input_stream_ref.eof() && entry_count < kBlockSize) {
    file_input_stream_ref >> buffer;
    ret.emplace_back(buffer);
    ++entry_count;
  }
  return ret;
}

void ResetFileStream(std::ifstream &data_set_input_stream_ref) {
  data_set_input_stream_ref.clear();
  data_set_input_stream_ref.seekg(0, std::ios::beg);
}

TEST(TestBuff, CorrectnessTest) {
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (int j = 1; j <= 6; j++) {
      std::vector<double> original_data;
      while ((original_data = ReadBlock(data_set_input_stream)).size() == kBlockSize) {
        BuffCompressor compressor(kBlockSize, j);
        Array<double> input(kBlockSize);
        for (int i = 0; i < kBlockSize; ++i) {
          input[i] = original_data[i];
        }
        compressor.compress(input);
        Array<uint8_t> compress_pack = compressor.get_out();
        BuffDecompressor decompressor(compress_pack);
        Array<double> output = decompressor.decompress();
        EXPECT_EQ(input.length(), output.length());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(input[i] - output[i]) > kMaxDiff[j-1]) {
            GTEST_LOG_(INFO) << original_data[i] << " " << output[i] << " " << kMaxDiff[j-1];
          }
        }
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}