// Test Program
#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <ctime>
#include <cassert>
#include <unordered_map>
#include <list>
#include "SerfCompressor.h"
#include "SerfDecompressor.h"
#include "compress.h"
#include "uncompr.h"

using std::cout, std::endl, std::string, std::vector, std::unordered_map, std::pair, std::make_pair;
using std::ifstream, std::ofstream;

// Test Parameter Config
const static string dataSetDir = "/home/ruyunlu/桌面/myCode/SerfNative/test/dataSet";
const static string testOutput = "/home/ruyunlu/桌面/myCode/SerfNative/test/result.csv";
const static int blockSize = 1000;
constexpr static int alphaArray[] = {1, 2, 3, 4, 5, 6, 7, 8};
constexpr static double alphaPrecisionTable[] = {10E0, 10E-1, 10E-2, 10E-3, 10E-4, 10E-5, 10E-6, 10E-7, 10E-8};

// Test Utils
vector<string> scanDataSetList()
{
    namespace fs = std::filesystem;
    vector<string> dataSetList;
    for (const auto &entry : fs::recursive_directory_iterator(dataSetDir))
    {
        if (fs::is_regular_file(entry))
        {
            string fileNameOfEntry = entry.path().filename();
            if (fileNameOfEntry.substr(fileNameOfEntry.find('.') + 1, 3) == "csv")
            {
                dataSetList.emplace_back(entry.path().string());
            }
        }
    }
    return std::move(dataSetList);
}

bool readBlock(ifstream &fileInputStreamRef, vector<double> &doubleBufferRef)
{
    doubleBufferRef.clear();
    int readDoubleCount = 0;
    string lineBuffer;
    while (!fileInputStreamRef.eof() && readDoubleCount < blockSize)
    {
        std::getline(fileInputStreamRef, lineBuffer);
        if (!lineBuffer.empty())
            doubleBufferRef.emplace_back(std::stod(lineBuffer));
        ++readDoubleCount;
    }
    if (doubleBufferRef.size() != blockSize)
    {
        doubleBufferRef.clear();
        return false;
    }
    return true;
}

template <typename T>
inline void hash_combine(std::size_t &seed, const T &val)
{
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T>
inline void hash_val(std::size_t &seed, const T &val)
{
    hash_combine(seed, val);
}
template <typename T, typename... Types>
inline void hash_val(std::size_t &seed, const T &val, const Types &...args)
{
    hash_combine(seed, val);
    hash_val(seed, args...);
}

template <typename... Types>
inline std::size_t hash_val(const Types &...args)
{
    std::size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

struct pairHash
{
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2> &p) const
    {
        return hash_val(p.first, p.second);
    }
};

vector<Bytef> transform(vector<double> floatings_list)
{
    vector<Bytef> byteData;
    for (const auto &value : floatings_list)
    {
        const Bytef *bytes = reinterpret_cast<const Bytef *>(&value);
        byteData.insert(byteData.end(), bytes, bytes + sizeof(double));
    }
    return byteData;
};
vector<double> reverseTransform(const vector<Bytef> &byteData)
{
    vector<double> floatings_list;
    size_t dataSize = byteData.size();
    size_t doubleSize = sizeof(double);

    if (dataSize % doubleSize != 0)
    {
        cout << "Error: Invalid byte data size for double conversion." << endl;
        return floatings_list;
    }

    for (size_t i = 0; i < dataSize; i += doubleSize)
    {
        double value;
        memcpy(&value, &byteData[i], doubleSize);
        floatings_list.push_back(value);
    }

    return floatings_list;
}

// Experiment Data
unordered_map<string, long> file2OriBits;
unordered_map<pair<string, int>, long, pairHash> fileAlpha2CmpBits;
unordered_map<pair<string, int>, pair<double, double>, pairHash> fileAlpha2CmpTimeAndDmpTime;

int main()
{
    vector<string> dataSetList = scanDataSetList();
    vector<double> doubleBuffer;
    ofstream result(testOutput);
    if (!result.is_open())
    {
        fprintf(stderr, "[Error] Failed to open the file '%s'", testOutput.c_str());
    }
    result << "DataSet,Param,Method,CB,OB,Ratio,CTime,DTime" << endl;

    for (const auto &item : dataSetList)
    {
        ifstream dataSetInputStream(item);
        if (!dataSetInputStream.is_open())
        {
            fprintf(stderr, "[Error] Failed to open the file '%s'", item.c_str());
        }
        int blockCnt = 0;
        while (readBlock(dataSetInputStream, doubleBuffer))
        {
            ++blockCnt;
        }
        file2OriBits.emplace(item, blockCnt * blockSize * 64L);
        dataSetInputStream.close();
    }

    doubleBuffer.clear();

    for (const auto &alpha : alphaArray)
    {
        for (const auto &dataSet : dataSetList)
        {
            ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open())
            {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            int blockCount = 0;
            clock_t totalCompressTime = 0;
            clock_t totalDecompressTime = 0;
            long totalCompressSize = 0;

            while (readBlock(dataSetInputStream, doubleBuffer))
            {
                // 压缩
                vector<Bytef> byteData = transform(doubleBuffer); // 源数据
                uLongf sourceLen = static_cast<uLongf>(byteData.size());
                uLong compressedSize = compressBound(blockSize * 64);
                vector<Bytef> compressedBuffer(compressedSize); // 压缩后数据
                clock_t compressStartTime = clock();
                int result = compress(compressedBuffer.data(), &compressedSize, byteData.data(), sourceLen);
                clock_t compressEndTime = clock();

                // 解压缩
                vector<double> decompressedData;
                vector<Bytef> uncompressedBuffer(sourceLen); // 解压缩后数据
                clock_t decompressStartTime = clock();
                result = uncompress(uncompressedBuffer.data(), &sourceLen, compressedBuffer.data(), compressedSize);
                clock_t decompressEndTime = clock();

                // 判断准确率
                decompressedData = reverseTransform(uncompressedBuffer);
                assert(uncompressedBuffer.size() / 8 == blockSize);
                for (int i = 0; i < doubleBuffer.size(); ++i)
                {
                    assert(std::abs(doubleBuffer[i] - decompressedData[i]) <= alphaPrecisionTable[alpha]);
                }

                ++blockCount;
                totalCompressTime += (compressEndTime - compressStartTime);
                totalDecompressTime += (decompressEndTime - decompressStartTime);
                totalCompressSize += compressedSize * 8;
            }
            dataSetInputStream.close();

            fileAlpha2CmpBits.emplace(make_pair(dataSet, alpha), totalCompressSize);
            fileAlpha2CmpTimeAndDmpTime.emplace(make_pair(dataSet, alpha), make_pair(static_cast<double>(totalCompressTime) / blockCount,
                                                                                     static_cast<double>(totalDecompressTime) / blockCount));
        }
    }

    for (const auto &alpha : alphaArray)
    {
        double cmpRatioOnCurAlpha = 0;
        double cmpTimeOnCurAlpha = 0;
        double dmpTimeOnCurAlpha = 0;
        for (const auto &dataSet : dataSetList)
        {
            cmpRatioOnCurAlpha += static_cast<double>(fileAlpha2CmpBits.find(make_pair(dataSet, alpha))->second) /
                                  static_cast<double>(file2OriBits.find(dataSet)->second);
            cmpTimeOnCurAlpha += fileAlpha2CmpTimeAndDmpTime.find(make_pair(dataSet, alpha))->second.first;
            dmpTimeOnCurAlpha += fileAlpha2CmpTimeAndDmpTime.find(make_pair(dataSet, alpha))->second.second;
            result << dataSet.substr(dataSet.find_last_of("/") + 1, dataSet.size()) << "," << alpha << ","
                   << "DeFflate"
                   << ","
                   << fileAlpha2CmpBits.find(make_pair(dataSet, alpha))->second << ","
                   << file2OriBits.find(dataSet)->second << ","
                   << static_cast<double>(fileAlpha2CmpBits.find(make_pair(dataSet, alpha))->second) /
                          static_cast<double>(file2OriBits.find(dataSet)->second)
                   << ","
                   << fileAlpha2CmpTimeAndDmpTime.find(make_pair(dataSet, alpha))->second.first << ","
                   << fileAlpha2CmpTimeAndDmpTime.find(make_pair(dataSet, alpha))->second.second
                   << endl;
        }
        cout << alpha << ","
             << "Deflate"
             << ","
             << cmpRatioOnCurAlpha / static_cast<double>(dataSetList.size()) << ","
             << cmpTimeOnCurAlpha / static_cast<double>(dataSetList.size()) << "," << dmpTimeOnCurAlpha / static_cast<double>(dataSetList.size()) << endl;
    }

    result.flush();
    result.close();

    return 0;
}