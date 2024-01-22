// Test Program
#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <ctime>
#include <cassert>
#include <bitset>

#include "SerfCompressor.h"
#include "SerfDecompressor.h"

using std::cout, std::endl, std::string, std::vector;
using std::ifstream, std::ofstream;

// Test Parameter Config
const static string dataSetDir = "/home/czc/WorkSpace/ClionProjects/SerfNative/test/dataSet";
const static string testOutput = "/home/czc/WorkSpace/ClionProjects/SerfNative/test/result.csv";
const static int blockSize = 1000;
constexpr static int alphaArray[] = {1, 2, 3, 4, 5, 6, 7, 8};
constexpr static double alphaPrecisionTable[] = {10E0, 10E-1, 10E-2, 10E-3, 10E-4, 10E-5, 10E-6, 10E-7, 10E-8};

// Test Utils
vector<string> scanDataSetList() {
    namespace fs = std::filesystem;
    vector<string> dataSetList;
    for (const auto &entry: fs::recursive_directory_iterator(dataSetDir)) {
        if (fs::is_regular_file(entry)) {
            string fileNameOfEntry = entry.path().filename();
            if (fileNameOfEntry.substr(fileNameOfEntry.find('.') + 1, 3) == "csv") {
                dataSetList.emplace_back(entry.path().string());
            }
        }
    }
    return std::move(dataSetList);
}

bool readBlock(ifstream &fileInputStreamRef, vector<double> &doubleBufferRef) {
    doubleBufferRef.clear();
    int readDoubleCount = 0;
    string lineBuffer;
    while (std::getline(fileInputStreamRef, lineBuffer) && readDoubleCount < blockSize) {
        doubleBufferRef.emplace_back(std::stod(lineBuffer));
        ++readDoubleCount;
    }
    if (readDoubleCount != blockSize) {
        doubleBufferRef.clear();
        return false;
    }
    return true;
}


int main() {
//    SerfCompressor compressor_1(2);
//    SerfDecompressor decompressor_1;
//    compressor_1.addValue(0.11f);
//    compressor_1.addValue(0.21f);
//    compressor_1.close();
//    vector<char> compressed_data = compressor_1.getBytes();
//    decompressor_1.setBytes(compressed_data.data(), compressed_data.size());
//    decompressor_1.decompress();
//
//    exit(0);

    vector<string> dataSetList = scanDataSetList();
    vector<double> doubleBuffer;
    ofstream result(testOutput);
    if (!result.is_open()) {
        fprintf(stderr, "[Error] Failed to open the file '%s'", testOutput.c_str());
    }
    result << "Param, Method, Ratio, CTime, DTime" << endl;

    for (const auto &alpha: alphaArray) {
        vector<double> compressionRatioList;
        vector<double> compressionTimeList;
        vector<double> decompressionTimeList;

        for (const auto &dataSet: dataSetList) {
            ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            int blockCount = 0;
            clock_t totalCompressTime = 0;
            clock_t totalDecompressTime = 0;
            long totalCompressSize = 0;
            while (readBlock(dataSetInputStream, doubleBuffer)) {
                SerfCompressor compressor(alpha);
                SerfDecompressor decompressor;

                clock_t compressStartTime = clock();
                for (const auto &item: doubleBuffer) {
                    compressor.addValue(item);
                }
                compressor.close();
                clock_t compressEndTime = clock();

                vector<char> compressedData = compressor.getBytes();
                decompressor.setBytes(compressedData.data(), compressedData.size());
                vector<double> decompressedData;

                clock_t decompressStartTime = clock();
                decompressedData = decompressor.decompress();
                clock_t decompressEndTime = clock();

                assert(decompressedData.size() == blockSize);
                for (int i = 0; i < doubleBuffer.size(); ++i) {
                    assert(std::abs(doubleBuffer[i] - decompressedData[i]) <= alphaPrecisionTable[alpha]);
                }

                ++blockCount;
                totalCompressTime += (compressEndTime - compressStartTime);
                totalDecompressTime += (decompressEndTime - decompressStartTime);
                totalCompressSize += compressor.getCompressedSizeInBits();
            }

            compressionRatioList.emplace_back(
                    static_cast<double>(totalCompressSize) /
                    static_cast<double>((blockCount * blockSize * sizeof(double) * 8)));
            compressionTimeList.emplace_back(static_cast<double>(totalCompressTime) / blockCount);
            decompressionTimeList.emplace_back(static_cast<double>(totalDecompressTime) / blockCount);

            dataSetInputStream.close();
        }

        double compressionRatioOnCurAlpha = 0;
        double compressionTimeOnCurAlpha = 0;
        double decompressionTimeOnCurAlpha = 0;
        for (const auto &e: compressionRatioList) {
            compressionRatioOnCurAlpha += e;
        }
        for (const auto &e: compressionTimeList) {
            compressionTimeOnCurAlpha += e;
        }
        for (const auto &e: decompressionTimeList) {
            decompressionTimeOnCurAlpha += e;
        }
        compressionRatioOnCurAlpha /= compressionRatioList.size();
        compressionTimeOnCurAlpha /= compressionTimeList.size();
        decompressionTimeOnCurAlpha /= decompressionTimeList.size();

        result << alpha << "," << "SerfNative" << ","
               << compressionRatioOnCurAlpha << ","
               << compressionTimeOnCurAlpha << ","
               << decompressionTimeOnCurAlpha << endl;
    }

    result.flush();
    result.close();

    return 0;
}