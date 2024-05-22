#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "lzw/src/LZW.h"

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/dataSet";

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

std::vector<std::string> ReadBlock(std::ifstream &file_input_stream_ref) {
    std::vector<std::string> returnData;
    int read_string_count = 0;
    std::string buffer;
    while (!file_input_stream_ref.eof() && read_string_count < BLOCK_SIZE) {
        std::getline(file_input_stream_ref, buffer);
        if (buffer.empty()) continue;
        returnData.emplace_back(buffer);
        ++read_string_count;
    }
    return returnData;
}

TEST(TestLZW, CorrectnessTest) {
    std::vector<std::string> dataSetList = ScanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'\n", dataSet.c_str());
        }

        std::vector<std::string> originalData;
        while ((originalData = ReadBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            std::string input_string = "";
            for (const auto &item: originalData) input_string += (item + " ");
            LZW *lzw = LZW::instance();
            auto [compression_result, compression_result_code] = lzw->Compress(input_string);
            EXPECT_EQ(compression_result_code, LZWResultCode::LZW_OK);
            auto [decompression_result, decompression_result_code] = lzw->Decompress(compression_result);
            EXPECT_EQ(decompression_result_code, LZWResultCode::LZW_OK);
            EXPECT_EQ(input_string, decompression_result);
        }

        dataSetInputStream.close();
    }
}