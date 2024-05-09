#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "alp/include/alp.hpp"

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/dataSet";

/**
 * @brief Scan all data set files in DATA_SET_DIR.
 * @return A vector contains all data set file path.
 */
std::vector<std::string> scanDataSet() {
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
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> readBlock(std::ifstream &fileInputStreamRef) {
    std::vector<double> returnData;
    int readDoubleCount = 0;
    double buffer;
    while (!fileInputStreamRef.eof() && readDoubleCount < BLOCK_SIZE) {
        fileInputStreamRef >> buffer;
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
std::vector<float> readBlock32(std::ifstream &fileInputStreamRef) {
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

TEST(TestALP, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'\n", dataSet.c_str());
        }

        std::vector<double> originalData;
        while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            alp::AlpCompressor compressor;
            uint8_t compress_output_buffer[(BLOCK_SIZE * sizeof(double)) + 8096];
            compressor.compress(originalData.data(), originalData.size(), compress_output_buffer);
            alp::AlpDecompressor decompressor;
            size_t decompress_buffer_size = alp::AlpApiUtils::align_value<size_t, alp::config::VECTOR_SIZE>(BLOCK_SIZE);
            double decompress_output_buffer[decompress_buffer_size];
            decompressor.decompress(compress_output_buffer, BLOCK_SIZE, decompress_output_buffer);
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                EXPECT_FLOAT_EQ(originalData[i], decompress_output_buffer[i]);
            }
        }

        dataSetInputStream.close();
    }
}