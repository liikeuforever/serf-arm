#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "serf/compressor/SerfXORCompressor.h"
#include "serf/decompressor/SerfXORDecompressor.h"
#include "serf/compressor/SerfQtCompressor.h"
#include "serf/decompressor/SerfQtDecompressor.h"
#include "deflate/DeflateCompressor.h"
#include "deflate/DeflateDecompressor.h"
#include "lz4/LZ4Compressor.h"
#include "lz4/LZ4Decompressor.h"
#include "fpc/FpcCompressor.h"
#include "fpc/FpcDecompressor.h"
#include "alp/include/alp.hpp"

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
        return compressionTime;
    }

    void increaseDecompressionTime(std::chrono::microseconds &duration) {
        decompressionTime += duration;
    }

    auto getDecompressionTime() {
        return decompressionTime;
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
    const std::string method_;
    const std::string dataSet_;
    std::string maxDiff_;

public:
    static std::string double_to_string(double val) {
        std::ostringstream stringBuffer;
        stringBuffer << std::setprecision(8) << std::fixed << val;
        return stringBuffer.str();
    }

    ExprConf() = delete;

    ExprConf(std::string method, std::string dataSet, double maxDiff) : method_(std::move(method)), dataSet_(std::move(dataSet)) {
        maxDiff_ = double_to_string(maxDiff);
    }

    bool operator == (const ExprConf &otherConf) const {
        return method_ == otherConf.method_ && dataSet_ == otherConf.dataSet_ && maxDiff_ == otherConf.maxDiff_;
    }
};

struct ExprConfHash {
    std::size_t operator()(const ExprConf &conf) const {
        return std::hash<std::string>()(conf.method_ + conf.dataSet_ + conf.maxDiff_);
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
constexpr static double MAX_DIFF[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

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

TEST(Perf, SerfXOR) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }

        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;

        for (const auto &max_diff: MAX_DIFF) {
            SerfXORCompressor xor_compressor(BLOCK_SIZE, max_diff, adjustD);
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

            std::string method = "SerfXOR";
            auto perfRecord = exprTable.find(ExprConf(method, fileName, max_diff));
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
                exprTable.insert(std::make_pair(ExprConf(method, fileName, max_diff), newPerfRecord));
            }

            dataSetInputStream.clear();
            dataSetInputStream.seekg(0, std::ios::beg);
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result_serf_xor.csv");
    resultOut << "Method,DataSet,MaxDiff,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method_ << "," << exprConf.dataSet_ << "," << exprConf.maxDiff_ << ","
                  << perfRecord.getCompressionTime().count() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime().count() << std::endl;
    }

    resultOut.flush();
    resultOut.close();

    std::unordered_map<std::string, double> maxDiff2Ratio;
    std::unordered_map<std::string, std::chrono::microseconds> maxDiff2CompressionTime;
    std::unordered_map<std::string, std::chrono::microseconds> maxDiff2DecompressionTime;
    for (auto &item: exprTable) {
        std::string maxDiff = item.first.maxDiff_;
        if (maxDiff2Ratio.find(maxDiff) == maxDiff2Ratio.end()) {
            maxDiff2Ratio.insert(std::make_pair(maxDiff, 0.0f));
            maxDiff2CompressionTime.insert(std::make_pair(maxDiff, std::chrono::microseconds::zero()));
            maxDiff2DecompressionTime.insert(std::make_pair(maxDiff, std::chrono::microseconds::zero()));
        }

        maxDiff2Ratio.find(maxDiff)->second += item.second.getCompressionRatio();
        maxDiff2CompressionTime.find(maxDiff)->second += (item.second.getCompressionTime() / item.second.getBlockCount());
        maxDiff2DecompressionTime.find(maxDiff)->second += (item.second.getDecompressionTime() / item.second.getBlockCount());
    }

    for (auto &item: maxDiff2Ratio) {
        item.second /= dataSetList.size();
    }

    for (auto &item: maxDiff2CompressionTime) {
        item.second /= dataSetList.size();
    }

    for (auto &item: maxDiff2DecompressionTime) {
        item.second /= dataSetList.size();
    }

    for (const auto &item: MAX_DIFF) {
        std::string str = ExprConf::double_to_string(item);
        std::cout << str << ", " << maxDiff2Ratio.find(str)->second << ", " << maxDiff2CompressionTime.find(str)->second.count() << ", " << maxDiff2DecompressionTime.find(str)->second.count() << std::endl;
    }
}

TEST(Perf, SerfQt) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());

        for (const auto &max_diff: MAX_DIFF) {
            SerfQtCompressor qt_compressor(max_diff);
            SerfQtDecompressor qt_decompressor(max_diff);

            int blockCount = 0;
            long compressBits = 0;
            std::vector<double> originalData;
            auto total_compression_duration = std::chrono::microseconds::zero();
            auto total_decompression_duration = std::chrono::microseconds::zero();
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                ++blockCount;
                auto compression_start = std::chrono::steady_clock::now();
                for (const auto &item: originalData) {
                    qt_compressor.addValue(item);
                }
                qt_compressor.close();
                auto compression_end = std::chrono::steady_clock::now();
                compressBits += qt_compressor.getCompressedSizeInBits();
                Array<uint8_t> result = qt_compressor.getBytes();
                auto decompression_start = std::chrono::steady_clock::now();
                std::vector<double> decompressed = qt_decompressor.decompress(result);
                auto decompression_end = std::chrono::steady_clock::now();

                total_compression_duration += std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);
                total_decompression_duration += std::chrono::duration_cast<std::chrono::microseconds>(decompression_end - decompression_start);
            }

            std::string method = "SerfQt";
            auto perfRecord = exprTable.find(ExprConf(method, fileName, max_diff));
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
                exprTable.insert(std::make_pair(ExprConf(method, fileName, max_diff), newPerfRecord));
            }

            dataSetInputStream.clear();
            dataSetInputStream.seekg(0, std::ios::beg);
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result_serf_qt.csv");
    resultOut << "Method,DataSet,MaxDiff,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method_ << "," << exprConf.dataSet_ << "," << exprConf.maxDiff_ << ","
                  << perfRecord.getCompressionTime().count() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime().count() << std::endl;
    }

    resultOut.flush();
    resultOut.close();

    std::unordered_map<std::string, double> maxDiff2Ratio;
    std::unordered_map<std::string, std::chrono::microseconds> maxDiff2CompressionTime;
    std::unordered_map<std::string, std::chrono::microseconds> maxDiff2DecompressionTime;
    for (auto &item: exprTable) {
        std::string maxDiff = item.first.maxDiff_;
        if (maxDiff2Ratio.find(maxDiff) == maxDiff2Ratio.end()) {
            maxDiff2Ratio.insert(std::make_pair(maxDiff, 0.0f));
            maxDiff2CompressionTime.insert(std::make_pair(maxDiff, std::chrono::microseconds::zero()));
            maxDiff2DecompressionTime.insert(std::make_pair(maxDiff, std::chrono::microseconds::zero()));
        }

        maxDiff2Ratio.find(maxDiff)->second += item.second.getCompressionRatio();
        maxDiff2CompressionTime.find(maxDiff)->second += (item.second.getCompressionTime() / item.second.getBlockCount());
        maxDiff2DecompressionTime.find(maxDiff)->second += (item.second.getDecompressionTime() / item.second.getBlockCount());
    }

    for (auto &item: maxDiff2Ratio) {
        item.second /= dataSetList.size();
    }

    for (auto &item: maxDiff2CompressionTime) {
        item.second /= dataSetList.size();
    }

    for (auto &item: maxDiff2DecompressionTime) {
        item.second /= dataSetList.size();
    }

    for (const auto &item: MAX_DIFF) {
        std::string str = ExprConf::double_to_string(item);
        std::cout << str << ", " << maxDiff2Ratio.find(str)->second << ", " << maxDiff2CompressionTime.find(str)->second.count() << ", " << maxDiff2DecompressionTime.find(str)->second.count() << std::endl;
    }
}

TEST(Perf, Deflate) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());

        int blockCount = 0;
        long compressBits = 0;
        std::vector<double> originalData;
        auto total_compression_duration = std::chrono::microseconds::zero();
        auto total_decompression_duration = std::chrono::microseconds::zero();
        while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            DeflateCompressor deflate_compressor;
            DeflateDecompressor deflate_decompressor;
            ++blockCount;
            auto compression_start = std::chrono::steady_clock::now();
            for (const auto &item: originalData) {
                deflate_compressor.addValue(item);
            }
            deflate_compressor.close();
            auto compression_end = std::chrono::steady_clock::now();
            compressBits += deflate_compressor.getCompressedSizeInBits();
            Array<uint8_t> result = deflate_compressor.getBytes();
            auto decompression_start = std::chrono::steady_clock::now();
            std::vector<double> decompressed = deflate_decompressor.decompress(result);
            auto decompression_end = std::chrono::steady_clock::now();

            total_compression_duration += std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);
            total_decompression_duration += std::chrono::duration_cast<std::chrono::microseconds>(decompression_end - decompression_start);
        }

        std::string method = "Deflate";
        auto perfRecord = exprTable.find(ExprConf(method, fileName, 0));
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
            exprTable.insert(std::make_pair(ExprConf(method, fileName, 0), newPerfRecord));
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result_deflate.csv");
    resultOut << "Method,DataSet,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method_ << "," << exprConf.dataSet_ << ","
                  << perfRecord.getCompressionTime().count() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime().count() << std::endl;
    }

    resultOut.flush();
    resultOut.close();
}

TEST(Perf, LZ4) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());

        int blockCount = 0;
        long compressBits = 0;
        std::vector<double> originalData;
        auto total_compression_duration = std::chrono::microseconds::zero();
        auto total_decompression_duration = std::chrono::microseconds::zero();
        while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            LZ4Compressor lz_4_compressor;
            LZ4Decompressor lz_4_decompressor;
            ++blockCount;
            auto compression_start = std::chrono::steady_clock::now();
            for (const auto &item: originalData) {
                lz_4_compressor.addValue(item);
            }
            lz_4_compressor.close();
            auto compression_end = std::chrono::steady_clock::now();
            compressBits += lz_4_compressor.getCompressedSizeInBits();
            Array<char> result = lz_4_compressor.getBytes();
            auto decompression_start = std::chrono::steady_clock::now();
            std::vector<double> decompressed = lz_4_decompressor.decompress(result);
            auto decompression_end = std::chrono::steady_clock::now();

            total_compression_duration += std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);
            total_decompression_duration += std::chrono::duration_cast<std::chrono::microseconds>(decompression_end - decompression_start);
        }

        std::string method = "LZ4";
        auto perfRecord = exprTable.find(ExprConf(method, fileName, 0));
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
            exprTable.insert(std::make_pair(ExprConf(method, fileName, 0), newPerfRecord));
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result_lz4.csv");
    resultOut << "Method,DataSet,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method_ << "," << exprConf.dataSet_ << ","
                  << perfRecord.getCompressionTime().count() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime().count() << std::endl;
    }

    resultOut.flush();
    resultOut.close();
}

TEST(Perf, FPC) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());

        int blockCount = 0;
        long compressBits = 0;
        std::vector<double> originalData;
        auto total_compression_duration = std::chrono::microseconds::zero();
        auto total_decompression_duration = std::chrono::microseconds::zero();
        while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            FpcCompressor fpc_compressor(5, BLOCK_SIZE);
            FpcDecompressor fpc_decompressor(5, BLOCK_SIZE);
            ++blockCount;
            auto compression_start = std::chrono::steady_clock::now();
            for (const auto &item: originalData) {
                fpc_compressor.addValue(item);
            }
            fpc_compressor.close();
            auto compression_end = std::chrono::steady_clock::now();
            compressBits += fpc_compressor.getCompressedSizeInBits();
            std::vector<char> result = fpc_compressor.getBytes();
            fpc_decompressor.setBytes(result.data(), result.size());
            auto decompression_start = std::chrono::steady_clock::now();
            std::vector<double> decompressed = fpc_decompressor.decompress();
            auto decompression_end = std::chrono::steady_clock::now();

            total_compression_duration += std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);
            total_decompression_duration += std::chrono::duration_cast<std::chrono::microseconds>(decompression_end - decompression_start);
        }

        std::string method = "FPC";
        auto perfRecord = exprTable.find(ExprConf(method, fileName, 0));
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
            exprTable.insert(std::make_pair(ExprConf(method, fileName, 0), newPerfRecord));
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result_fpc.csv");
    resultOut << "Method,DataSet,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method_ << "," << exprConf.dataSet_ << ","
                  << perfRecord.getCompressionTime().count() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime().count() << std::endl;
    }

    resultOut.flush();
    resultOut.close();
}

TEST(Perf, ALP) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::ifstream dataSetInputStream(dataSet);
        if (!dataSetInputStream.is_open()) {
            fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
        }
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());

        int blockCount = 0;
        long compressBits = 0;
        std::vector<double> originalData;
        auto total_compression_duration = std::chrono::microseconds::zero();
        auto total_decompression_duration = std::chrono::microseconds::zero();
        while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
            uint8_t out[(tuples_count * sizeof(double)) + 8096;];
            alp::AlpCompressor compressor;
            alp::AlpDecompressor decompressor;
            ++blockCount;
            auto compression_start = std::chrono::steady_clock::now();
            compressor.compress(originalData.data(), originalData.size(), out);
            auto compression_end = std::chrono::steady_clock::now();
            compressBits += compressor.get_size() * 8;
            double tmp[BLOCK_SIZE];
            auto decompression_start = std::chrono::steady_clock::now();
            decompressor.decompress(out, BLOCK_SIZE, tmp);
            auto decompression_end = std::chrono::steady_clock::now();

            total_compression_duration += std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);
            total_decompression_duration += std::chrono::duration_cast<std::chrono::microseconds>(decompression_end - decompression_start);
        }

        std::string method = "ALP";
        auto perfRecord = exprTable.find(ExprConf(method, fileName, 0));
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
            exprTable.insert(std::make_pair(ExprConf(method, fileName, 0), newPerfRecord));
        }

        dataSetInputStream.close();
    }

    std::ofstream resultOut("result_alp.csv");
    resultOut << "Method,DataSet,CompressionTime,CompressionRatio,DecompressionTime" << std::endl;
    for (const auto &item: exprTable) {
        auto exprConf = item.first;
        auto perfRecord = item.second;

        resultOut << exprConf.method_ << "," << exprConf.dataSet_ << ","
                  << perfRecord.getCompressionTime().count() << "," << perfRecord.getCompressionRatio() << ","
                  << perfRecord.getDecompressionTime().count() << std::endl;
    }

    resultOut.flush();
    resultOut.close();
}