#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "buff/buff_compressor.h"
#include "buff/buff_decompressor.h"

const static int kBlockSize = 1000;
const static std::string kDataSetDir = "../../test/dataSet";

const static std::unordered_map<std::string, int> kFileToMaxPrecision = {
        {"Basel-wind.csv",     8},
        {"City-temp.csv",      1},
        {"PM10-dust.csv",      3},
        {"Stocks-DE.csv",      3},
        {"Basel-temp.csv",     10},
        {"init.csv",           6},
        {"Air-pressure.csv",   5},
        {"Wind-Speed.csv",     2},
        {"Bitcoin-price.csv",  4},
        {"Stocks-UK.csv",      2},
        {"Stocks-USA.csv",     2},
        {"Dew-point-temp.csv", 2},
        {"Air-sensor.csv",     17},
        {"Bird-migration.csv", 5},
        {"IR-bio-temp.csv",    2},
};

/**
 * @brief Scan all data set files in kDataSetDir.
 * @return A vector contains all data set file path.
 */
std::vector<std::string> ScanDataSet() {
    namespace fs = std::filesystem;
    std::vector<std::string> data_set_list;
    for (const auto &entry: fs::recursive_directory_iterator(kDataSetDir)) {
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
 * @brief Read a block of double from file input stream, whose size is equal to kBlockSize
 * @param file_input_stream_ref Input steam where this function reads
 * @return A vector of doubles, whose size may be less than kBlockSize
 */
std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
    std::vector<double> return_block;
    int read_double_count = 0;
    double buffer;
    while (!file_input_stream_ref.eof() && read_double_count < kBlockSize) {
        file_input_stream_ref >> buffer;
        return_block.emplace_back(buffer);
        ++read_double_count;
    }
    return return_block;
}

TEST(TestBuff, CorrectnessTest) {
    std::vector<std::string> data_set_list = ScanDataSet();
    for (const auto &data_set: data_set_list) {
        std::ifstream data_set_input_stream(data_set);
        if (!data_set_input_stream.is_open()) {
            std::fprintf(stderr, "[Error] Failed to open the file '%s'\n", data_set.c_str());
        }

        std::string file_name = data_set.substr(data_set.find_last_of('/') + 1, data_set.size());
        std::vector<double> original_data;
        while ((original_data = ReadBlock(data_set_input_stream)).size() == kBlockSize) {
            Array<double> input(kBlockSize);
            for (int i = 0; i < kBlockSize; ++i) {
                input[i] = original_data[i];
            }
            BuffCompressor compressor(kBlockSize, kFileToMaxPrecision.find(file_name)->second);
            compressor.compress(input);
            Array<uint8_t> compress_pack = compressor.get_out();
            BuffDecompressor decompressor(compress_pack);
            Array<double> output = decompressor.decompress();
            for (int i = 0; i < kBlockSize; ++i) {
//                EXPECT_FLOAT_EQ(originalData[i], output[i]);
                if (original_data[i] - output[i] != 0) {
                    std::cout << data_set << std::endl;
                    exit(-1);
                }
            }
        }

        data_set_input_stream.close();
    }
}