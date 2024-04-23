#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "serf/compressor/SerfXORCompressor.h"
#include "serf/decompressor/SerfXORDecompressor.h"
#include "serf/compressor/TorchSerfXORCompressor.h"
#include "serf/decompressor/TorchSerfXORDecompressor.h"
#include "serf/compressor/SerfQtCompressor.h"
#include "serf/decompressor/SerfQtDecompressor.h"
#include "serf/compressor32/SerfXORCompressor32.h"
#include "serf/decompressor32/SerfXORDecompressor32.h"
#include "serf/compressor/NetSerfXORCompressor.h"
#include "serf/decompressor/NetSerfXORDecompressor.h"
#include "serf/compressor/NetSerfQtCompressor.h"
#include "serf/decompressor/NetSerfQtDecompressor.h"
#include "serf/compressor32/SerfQtCompressor32.h"
#include "serf/decompressor32/SerfQtDecompressor32.h"

class PerfRecord {
private:
    std::chrono::microseconds compressionTime = std::chrono::microseconds::zero();
    std::chrono::microseconds decompressionTime = std::chrono::microseconds::zero();
    long compressedSizeInBits = 0;
    int blockCount = 0;

public:
    void increaseCompressionTime(std::chrono::microseconds &duration) {
        compressionTime += duration;
    }

    auto getCompressionTime() {
        return compressionTime.count();
    }

    void increaseDecompressionTime(std::chrono::microseconds &duration) {
        decompressionTime += duration;
    }

    auto getDecompressionTime() {
        return decompressionTime.count();
    }

    void addCompressedSize(long size) {
        compressedSizeInBits += size;
    }

    void setBlockCount(int blockCount_) {
        blockCount = blockCount_;
    }

    int getBlockCount() const {
        return blockCount;
    }

    double getCompressionRatio() const {
        return (double) compressedSizeInBits / (double) (blockCount * 1000 * 64L);
    }
};

class ExprConf {
public:
    const std::string method;
    const std::string dataSet;
    const std::string maxDiff;

public:
    ExprConf() = delete;

    ExprConf(std::string method, std::string dataSet, double maxDiff) : method(method), dataSet(dataSet),
                                                                          maxDiff(std::to_string(maxDiff)) {}

    bool operator == (const ExprConf &otherConf) const {
        return method == otherConf.method && dataSet == otherConf.dataSet && maxDiff == otherConf.maxDiff;
    }
};

struct ExprConfHash {
    std::size_t operator()(const ExprConf &conf) const {
        return std::hash<std::string>()(conf.method + conf.dataSet + conf.maxDiff);
    }
};

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/dataSet";
const static std::unordered_map<std::string, int> FILE_TO_ADJUST_D{
        {"init.csv",           0},
        {"Air-pressure.csv",   0},
        {"Air-sensor.csv",     128},
        {"Bird-migration.csv", 60},
        {"Bitcoin-price.csv",  511220},
        {"Basel-temp.csv",     77},
        {"Basel-wind.csv",     128},
        {"City-temp.csv",      355},
        {"Dew-point-temp.csv", 94},
        {"IR-bio-temp.csv",    49},
        {"PM10-dust.csv",      256},
        {"Stocks-DE.csv",      253},
        {"Stocks-UK.csv",      8047},
        {"Stocks-USA.csv",     243},
        {"Wind-Speed.csv",     2}
};
constexpr static double MAX_DIFF[] = {1.0E-1, 0.5, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static float MAX_DIFF_32[] = {1.0E-1, 0.5, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

std::unordered_map<ExprConf, PerfRecord, ExprConfHash> exprTable;

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

TEST(TestSerfXOR, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfXORCompressor xor_compressor(1000, max_diff, adjustD);
            SerfXORDecompressor xor_decompressor(adjustD);

            std::vector<double> originalData;
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    xor_compressor.addValue(item);
                }
                xor_compressor.close();
                Array<uint8_t> result = xor_compressor.getBytes();
                std::vector<double> decompressed = xor_decompressor.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                for (int i = 0; i < BLOCK_SIZE; ++i) {
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
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfQtCompressor qt_compressor(max_diff);
            SerfQtDecompressor qt_decompressor(max_diff);

            std::vector<double> originalData;
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    qt_compressor.addValue(item);
                }
                qt_compressor.close();
                Array<uint8_t> result = qt_compressor.getBytes();
                std::vector<double> decompressed = qt_decompressor.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                for (int i = 0; i < BLOCK_SIZE; ++i) {
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
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            NetSerfXORCompressor xor_compressor(max_diff, adjustD);
            NetSerfXORDecompressor xor_decompressor(adjustD);

            double originalData;
            while (!dataSetInputStream.eof()) {
                dataSetInputStream >> originalData;
                Array<uint8_t> result = xor_compressor.compress(originalData);
                double decompressed = xor_decompressor.decompress(result);
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
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            NetSerfQtCompressor qt_compressor(max_diff);
            NetSerfQtDecompressor qt_decompressor(max_diff);

            double originalData;
            while (!dataSetInputStream.eof()) {
                dataSetInputStream >> originalData;
                Array<uint8_t> result = qt_compressor.compress(originalData);
                double decompressed = qt_decompressor.decompress(result);
                if (std::abs(originalData - decompressed) > max_diff) {
                    GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
                }
                EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestTorchSerfXOR, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            TorchSerfXORCompressor xor_compressor(max_diff, adjustD);
            TorchSerfXORDecompressor xor_decompressor(adjustD);

            double originalData;
            while (!dataSetInputStream.eof()) {
                dataSetInputStream >> originalData;
                Array<uint8_t> result = xor_compressor.compress(originalData);
                double decompressed = xor_decompressor.decompress(result);
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
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF_32) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfXORCompressor32 xor_compressor_32(BLOCK_SIZE, max_diff);
            SerfXORDecompressor32 xor_decompressor_32;

            std::vector<float> originalData;
            while ((originalData = readBlock32(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    xor_compressor_32.addValue(item);
                }
                xor_compressor_32.close();
                Array<uint8_t> result = xor_compressor_32.getBytes();
                std::vector<float> decompressed = xor_decompressor_32.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                if (originalData.size() != decompressed.size()) {
                    GTEST_LOG_(INFO) << dataSet << " " << max_diff;
                } else {
                    for (int i = 0; i < BLOCK_SIZE; ++i) {
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
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF_32) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfQtCompressor32 qt_compressor_32(max_diff);
            SerfQtDecompressor32 qt_decompressor_32(max_diff);

            std::vector<float> originalData;
            while ((originalData = readBlock32(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    qt_compressor_32.addValue(item);
                }
                qt_compressor_32.close();
                Array<uint8_t> result = qt_compressor_32.getBytes();
                std::vector<float> decompressed = qt_decompressor_32.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                if (originalData.size() != decompressed.size()) {
                    GTEST_LOG_(INFO) << dataSet << " " << max_diff;
                } else {
                    for (int i = 0; i < BLOCK_SIZE; ++i) {
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

TEST(TestSerf, PerformanceTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }

        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;

        for (const auto &max_diff: MAX_DIFF) {
            SerfXORCompressor xor_compressor(1000, max_diff, adjustD);
            SerfXORDecompressor xor_decompressor(adjustD);

            int blockCount = 0;
            long compressBits = 0;
            std::vector<double> originalData;
            auto total_compression_duration = std::chrono::microseconds::zero();
            auto total_decompression_duration = std::chrono::microseconds::zero();
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                ++blockCount;
                auto compression_start = std::chrono::steady_clock::now();
                for (const auto &item: originalData) {
                    xor_compressor.addValue(item);
                }
                xor_compressor.close();
                auto compression_end = std::chrono::steady_clock::now();
                compressBits += xor_compressor.getCompressedSizeInBits();
                Array<uint8_t> result = xor_compressor.getBytes();
                auto decompression_start = std::chrono::steady_clock::now();
                std::vector<double> decompressed = xor_decompressor.decompress(result);
                auto decompression_end = std::chrono::steady_clock::now();

                total_compression_duration += std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);
                total_decompression_duration += std::chrono::duration_cast<std::chrono::microseconds>(decompression_end - decompression_start);
            }

            auto perfRecord = exprTable.find(ExprConf("SerfXOR", fileName, max_diff));
            if (perfRecord != exprTable.end()) {
                perfRecord->second.setBlockCount(blockCount);
                perfRecord->second.addCompressedSize(compressBits);
                perfRecord->second.increaseCompressionTime(total_compression_duration);
                perfRecord->second.increaseDecompressionTime(total_decompression_duration);
            } else {
                auto newPerfRecord = PerfRecord();
                newPerfRecord.setBlockCount(blockCount);
                newPerfRecord.addCompressedSize(compressBits);
                newPerfRecord.increaseCompressionTime(total_compression_duration);
                newPerfRecord.increaseDecompressionTime(total_decompression_duration);
                exprTable.insert(std::make_pair(ExprConf("SerfXOR", fileName, max_diff), newPerfRecord));
            }

            dataSetInputStream.clear();
            dataSetInputStream.seekg(0, std::ios::beg);
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result.csv");
    resultOut << "Method,DataSet,MaxDiff,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method << "," << exprConf.dataSet << "," << exprConf.maxDiff << ","
                  << perfRecord.getCompressionTime() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime() << std::endl;
    }
    resultOut.flush();
    resultOut.close();

//    std::cout << "Method, DataSet, MaxDiff, CompressionTime, CompressionRatio, DecompressionTime" << std::endl;
//    for (const auto &item: exprTable) {
//        auto exprConf = item.first;
//        auto perfRecord = item.second;
//
//        std::cout << exprConf.method << ", " << exprConf.dataSet << ", " << exprConf.maxDiff << ", "
//                  << perfRecord.getCompressionTime() << ", " << perfRecord.getCompressionRatio() << ", "
//                  << perfRecord.getDecompressionTime() << std::endl;
//    }
}