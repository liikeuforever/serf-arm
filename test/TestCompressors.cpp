#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <ctime>
#include <map>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include "lz4frame.h"
#include "deflate.h"
#include "compress.h"
#include "uncompr.h"
using namespace std;
#define MAX_LINE_LENGTH 256
#define MAX_BLOCK_SIZE 100
#define DIR "/home/ruyunlu/桌面/myCode/SerfNative/test/dataSet"
typedef struct
{
    FILE *file;
    int blockSize;
    int end;
} BlockReader;

BlockReader *createBlockReader(const char *fileName, int blockSize)
{
    BlockReader *reader = (BlockReader *)malloc(sizeof(BlockReader));
    if (reader == NULL)
    {
        fprintf(stderr, "Error allocating memory for BlockReader.\n");
        exit(EXIT_FAILURE);
    }
    char *file_end = new char[strlen(DIR) + strlen(fileName) + 1];
    strcpy(file_end, DIR);
    strcat(file_end, fileName);

    reader->file = fopen(file_end, "r");
    if (reader->file == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        free(reader);
        exit(EXIT_FAILURE);
    }

    reader->blockSize = blockSize;
    reader->end = 0;

    return reader;
}

void closeBlockReader(BlockReader *reader)
{
    if (reader != NULL)
    {
        fclose(reader->file);
        free(reader);
    }
}

double *nextBlock(BlockReader *reader)
{
    if (reader->end)
    {
        return NULL;
    }

    double *floatings = (double *)malloc(sizeof(double) * reader->blockSize);
    if (floatings == NULL)
    {
        fprintf(stderr, "Error allocating memory for floatings.\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char line[MAX_LINE_LENGTH];
    while (i < reader->blockSize && fgets(line, sizeof(line), reader->file) != NULL)
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }

        floatings[i++] = strtof(line, NULL);
    }

    if (i < reader->blockSize)
    {
        reader->end = 1;
    }

    if (i == 0)
    {
        free(floatings);
        return NULL;
    }

    return floatings;
}
vector<Bytef> transform(list<double> floatings_list)
{
    vector<Bytef> byteData;
    for (const auto &value : floatings_list)
    {
        const Bytef *bytes = reinterpret_cast<const Bytef *>(&value);
        byteData.insert(byteData.end(), bytes, bytes + sizeof(double));
    }
    return byteData;
}
list<double> reverseTransform(const vector<Bytef> &byteData) {
    list<double> floatings_list;
    size_t dataSize = byteData.size();
    size_t doubleSize = sizeof(double);

    if (dataSize % doubleSize != 0) {
        cerr << "Error: Invalid byte data size for double conversion." << endl;
        return floatings_list;
    }

    for (size_t i = 0; i < dataSize; i += doubleSize) {
        double value;
        memcpy(&value, &byteData[i], doubleSize);
        floatings_list.push_back(value);
    }

    return floatings_list;
}
class TestCompressors
{
private:
    static const string STORE_FILE;
    static const double TIME_PRECISION;
    static const int BLOCK_SIZE;
    static const string INIT_FILE;
    static const int ALPHA[8];
    vector<string> fileNames = {
        INIT_FILE,
        "Air-pressure.csv",
        "Air-sensor.csv",
        "Bird-migration.csv",
        "Bitcoin-price.csv",
        "Basel-temp.csv",
        "Basel-wind.csv",
        "City-temp.csv",
        "Dew-point-temp.csv",
        "IR-bio-temp.csv",
        "PM10-dust.csv",
        "Stocks-DE.csv",
        "Stocks-UK.csv",
        "Stocks-USA.csv",
        "Wind-Speed.csv"};
    unordered_map<string, long> fileNameParamToTotalBits;
    unordered_map<string, long> fileNameParamToTotalBlock;
    unordered_map<string, long> fileNameParamMethodToCompressedBits;
    unordered_map<string, double> fileNameParamMethodToCompressTime;
    unordered_map<string, double> fileNameParamMethodToDecompressTime;
    map<string, double> fileNameParamMethodToCompressedRatio;

public:
    TestCompressors()
    {
        // Initialize fileNames vector with file names
    }

    void testAllCompressor();
    void testDeflateCompressor(const string &fileName, int alpha);
    void testLz4Compressor(const string &fileName, int alpha);
    void testSerfCompressor(const string &fileName, int alpha);
    void testBuffCompressor(const string &fileName, int alpha);
    void writeResult(const map<string, double> &fileNameParamMethodToRatio,
                     const unordered_map<string, double> &fileNameParamMethodToCTime,
                     const unordered_map<string, double> &fileNameParamMethodToDTime,
                     const unordered_map<string, long> &fileNameParamToTotalBlock);

private:
    // Define other necessary member functions
};

const string TestCompressors::STORE_FILE = "src/test/resources/result/result.csv";
const double TestCompressors::TIME_PRECISION = 1000.0;
const int TestCompressors::BLOCK_SIZE = 1000;
const string TestCompressors::INIT_FILE = "init.csv";
const int TestCompressors::ALPHA[8] = {1, 2, 3, 4, 5, 6, 7, 8};
void TestCompressors::testDeflateCompressor(const string &fileName, int alpha)
{
    string fileNameParam = fileName + "," + to_string(alpha);
    fileNameParamToTotalBits.insert(make_pair(fileNameParam, 0L));
    fileNameParamToTotalBlock.insert(make_pair(fileNameParam, 0L));

    // Assuming fileNameParamToTotalBits, fileNameParamToTotalBlock, and other data structures are defined globally

    BlockReader *br = createBlockReader(fileName.c_str(), BLOCK_SIZE);
    if (br == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    double *floatings;
    while ((floatings = nextBlock(br)) != nullptr)
    {
        list<double> floatings_list(floatings, floatings + BLOCK_SIZE);
        double compressTime = 0;
        double decompressTime = 0;
        if (floatings_list.size() != BLOCK_SIZE)
        {
            break;
        }

        fileNameParamToTotalBits[fileNameParam] += BLOCK_SIZE * sizeof(double) * 8L;
        fileNameParamToTotalBlock[fileNameParam] += 1L;

        // 压缩后的数据缓冲区
        vector<Bytef> byteData = transform(floatings_list); // 源数据
        uLongf sourceLen = static_cast<uLongf>(byteData.size());
        uLong compressedSize = compressBound(BLOCK_SIZE * 64);
        vector<Bytef> compressedBuffer(compressedSize); // 压缩后数据
        int result = compress(compressedBuffer.data(), &compressedSize, byteData.data(), sourceLen);
        if (result == Z_OK)
        {
            // 压缩成功，compressedBuffer中存储了压缩后的数据
            std::cout << "Compression successful. Compressed size: " << compressedSize << " bytes." << std::endl;
        }
        else
        {
            std::cerr << "Compression failed with error code: " << result << std::endl;
        }

        // 解压缩
        vector<Bytef> uncompressedBuffer(sourceLen); // 解压缩后数据
        result = uncompress(uncompressedBuffer.data(), &sourceLen, compressedBuffer.data(), compressedSize);
        if (result == Z_OK)
        {
            // 解压缩成功，uncompressedBuffer中存储了解压缩后的数据
            std::cout << "Decompression successful. Compressed size: " << sourceLen << " bytes." << std::endl;
        }
        else
        {
            std::cerr << "Decompression failed with error code: " << result << std::endl;
        }
        // 判断准确率
        list<double> deValues=reverseTransform(uncompressedBuffer);
        
    }
};
void TestCompressors::testAllCompressor()
{
    cout << "test begin" << endl;
    for (string fileName : fileNames)
    {
        for (int alpha : ALPHA)
        {
            // testSerfCompressor(fileName, alpha);
            // testBuffCompressor(fileName, alpha);
            testDeflateCompressor(fileName, alpha); // 无损流式
            // testLz4Compressor(fileName, alpha);     // 无损流式
        }
    }
    cout << "test end" << endl;
    for (const auto &entry : fileNameParamMethodToCompressedBits)
    {
        const string fileNameParamMethod = entry.first;
        long compressedBits = entry.second;

        istringstream ss(fileNameParamMethod);
        string token;
        getline(ss, token, ',');
        string fileNameParam = token + ",";
        getline(ss, token, ',');
        fileNameParam += token;

        long fileTotalBits = fileNameParamToTotalBits[fileNameParam];
        fileNameParamMethodToCompressedRatio.insert(make_pair(fileNameParamMethod, (compressedBits * 1.0) / fileTotalBits));
    }
    cout << "Test All Compressor" << endl;
    writeResult(fileNameParamMethodToCompressedRatio, fileNameParamMethodToCompressTime,
                fileNameParamMethodToDecompressTime, fileNameParamToTotalBlock);
};

void TestCompressors::testSerfCompressor(const string &fileName, int alpha){};
void TestCompressors::testBuffCompressor(const string &fileName, int alpha){};
void TestCompressors::writeResult(const map<string, double> &fileNameParamMethodToRatio,
                                  const unordered_map<string, double> &fileNameParamMethodToCTime,
                                  const unordered_map<string, double> &fileNameParamMethodToDTime,
                                  const unordered_map<string, long> &fileNameParamToTotalBlock)
{
    try
    {
        // To calculate average metrics of each method and param
        map<string, vector<double>> paramMethodRatio;
        map<string, vector<double>> paramMethodCTime;
        map<string, vector<double>> paramMethodDTime;

        for (const auto &kv : fileNameParamMethodToRatio)
        {
            string fileNameParamMethod = kv.first;
            vector<string> arr;
            istringstream iss(fileNameParamMethod);
            string token;
            while (getline(iss, token, ','))
            {
                arr.push_back(token);
            }

            string paramMethod = arr[1] + "," + arr[2];
            if (paramMethodRatio.find(paramMethod) == paramMethodRatio.end())
            {
                paramMethodRatio[paramMethod] = vector<double>();
                paramMethodCTime[paramMethod] = vector<double>();
                paramMethodDTime[paramMethod] = vector<double>();
            }

            string fileNameParam = arr[0] + "," + arr[1];
            paramMethodRatio[paramMethod].push_back(fileNameParamMethodToRatio.at(fileNameParamMethod));
            paramMethodCTime[paramMethod].push_back(fileNameParamMethodToCTime.at(fileNameParamMethod) / fileNameParamToTotalBlock.at(fileNameParam));
            paramMethodDTime[paramMethod].push_back(fileNameParamMethodToDTime.at(fileNameParamMethod) / fileNameParamToTotalBlock.at(fileNameParam));
        }

        cout << "Param,Method,Ratio,CTime,DTime" << endl;
        for (const auto &paramMethod : paramMethodRatio)
        {
            cout << paramMethod.first << ",";
            cout << accumulate(paramMethod.second.begin(), paramMethod.second.end(), 0.0) / paramMethod.second.size() << ",";
            cout << accumulate(paramMethodCTime[paramMethod.first].begin(),
                               paramMethodCTime[paramMethod.first].end(), 0.0) /
                        paramMethodCTime[paramMethod.first].size()
                 << ",";
            cout << accumulate(paramMethodDTime[paramMethod.first].begin(),
                               paramMethodDTime[paramMethod.first].end(), 0.0) /
                        paramMethodDTime[paramMethod.first].size()
                 << endl;
        }

        ofstream writer(STORE_FILE, ios::out);
        if (!writer.is_open())
        {
            throw ios_base::failure("Failed to open file: " + STORE_FILE);
        }

        writer << "Dataset, Param, Method, Ratio, CTime, DTime" << endl;
        for (const auto &kv : fileNameParamMethodToRatio)
        {
            string fileNameParamMethod = kv.first;
            vector<string> arr;
            istringstream iss(fileNameParamMethod);
            string token;
            while (getline(iss, token, ','))
            {
                arr.push_back(token);
            }

            string fileNameParam = arr[0] + "," + arr[1];
            writer << fileNameParamMethod << ",";
            writer << fileNameParamMethodToRatio.at(fileNameParamMethod) << ",";
            writer << fileNameParamMethodToCTime.at(fileNameParamMethod) / fileNameParamToTotalBlock.at(fileNameParam) << ",";
            writer << fileNameParamMethodToDTime.at(fileNameParamMethod) / fileNameParamToTotalBlock.at(fileNameParam) << endl;
        }

        cout << "Done!" << endl;
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
    }
};

vector<vector<uint8_t>> convertDoubleArrayToByteArray(list<double> doubleArray)
{
    vector<vector<uint8_t>> byteArray;
    for (const auto &value : doubleArray)
    {
        // 将 double 转换为字节数组
        const uint8_t *bytePtr = reinterpret_cast<const uint8_t *>(&value);

        // 将字节数组存储到 vector
        byteArray.emplace_back(bytePtr, bytePtr + sizeof(double));
    }
    return byteArray;
}

int main()
{
    cout << "all begin" << endl;
    TestCompressors tester;
    tester.testAllCompressor();

    return 0;
}
