#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "buff/buff_compressor.h"
#include "buff/buff_decompressor.h"

const static int kBlockSize = 1000;
const static std::string kDataSetDir = "../../test/data_set";

int block_max_precision;

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
    std::string double_value_raw_string;
    size_t max_precision = 0;
    size_t cur_precision;
    while (!file_input_stream_ref.eof() && read_double_count < kBlockSize) {
        std::getline(file_input_stream_ref, double_value_raw_string);
        if (double_value_raw_string.empty()) continue;
        if (double_value_raw_string.find('.') == std::string::npos) {
            cur_precision = 1;
            max_precision = std::max(max_precision, cur_precision);
        } else {
            cur_precision = double_value_raw_string.size() - double_value_raw_string.find('.') - 1;
            max_precision = std::max(max_precision, cur_precision);
        }
        double buffer = std::stod(double_value_raw_string);
        return_block.emplace_back(buffer);
        ++read_double_count;
    }
    block_max_precision = (int) max_precision;
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
            BuffCompressor compressor(kBlockSize, block_max_precision);
            compressor.compress(input);
            compressor.close();
            Array<uint8_t> compress_pack = compressor.get_out();
            BuffDecompressor decompressor(compress_pack);
            Array<double> output = decompressor.decompress();
            EXPECT_EQ(original_data.size(), output.length());
            for (int i = 0; i < kBlockSize; ++i) {
                EXPECT_FLOAT_EQ(original_data[i], output[i]);
                if (original_data[i] - output[i] != 0) {
                    exit(-1);
                }
            }
        }

        data_set_input_stream.close();
    }
}