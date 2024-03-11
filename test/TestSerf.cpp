#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "serf/compressor/SerfCompressor.h"
#include "serf/decompressor/SerfDecompressor.h"

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/dataSet";
constexpr static double MAX_DIFF[] = {1.0E-1, 0.5, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

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
        returnData.push_back(buffer);
        ++readDoubleCount;
    }
}

TEST(TestSerf, BoundingTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }
    }
}