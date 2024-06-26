#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "baselines/elf/elf.h"

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/data_set";

/**
 * @brief Scan all data set files in DATA_SET_DIR.
 * @return A vector contains all data set file path.
 */
std::vector<std::string> ScanDataSet() {
    namespace fs = std::filesystem;
    std::vector<std::string> dataSetList;
    for (const auto &entry: fs::recursive_directory_iterator(DATA_SET_DIR)) {
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

TEST(TestElf, CorrectnessTest) {
    std::vector<std::string> dataSetList = ScanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'\n", dataSet.c_str());
        }

        std::vector<double> original_data;
        while ((original_data = ReadBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            uint8_t *compression_output_buffer;
            double decompression_output[BLOCK_SIZE];
            ssize_t compression_output_len_in_bytes = elf_encode(original_data.data(), original_data.size(),
                                                             &compression_output_buffer, 0);
            elf_decode(compression_output_buffer, compression_output_len_in_bytes, decompression_output,
                     0);
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                EXPECT_TRUE(original_data[i] - decompression_output[i] == 0);
            }
        }

        dataSetInputStream.close();
    }
}