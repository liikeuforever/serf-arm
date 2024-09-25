#ifndef SERF_ALL_TEST_PERF_EXPR_CONFIG_HPP_
#define SERF_ALL_TEST_PERF_EXPR_CONFIG_HPP_

#include <string>

const static std::string kExportExprTablePrefix = "../../test/";
const static std::string kExportExprTableFileName = "perf_table.csv";
const static std::string kDataSetDirPrefix = "../../test/data_set/";
const static std::string kDataSetList[] = {
    "Air-pressure.csv",
    "Basel-temp.csv",
    "Basel-wind.csv",
    "Chengdu-traj.csv",
    "City-temp.csv",
    "Dew-point-temp.csv",
    "IR-bio-temp.csv",
    "Motor-temp.csv",
    "PM10-dust.csv",
    "Smart-grid.csv",
    "Stocks-USA.csv",
    "T-drive.csv",
    "Wind-Speed.csv"
};
const static std::unordered_map<std::string, std::string> kAbbrToDataList{
    {"AP", "Air-pressure.csv"},
    {"BT", "Basel-temp.csv"},
    {"BW", "Basel-wind.csv"},
    {"CDTR", "Chengdu-traj.csv"},
    {"CT", "City-temp.csv"},
    {"DT", "Dew-point-temp.csv"},
    {"IR", "IR-bio-temp.csv"},
    {"MT", "Motor-temp.csv"},
    {"PM10", "PM10-dust.csv"},
    {"SG", "Smart-grid.csv"},
    {"SUSA", "Stocks-USA.csv"},
    {"TD", "T-drive.csv"},
    {"WS", "Wind-Speed.csv"}
};
const static std::string kMethodListOverall[] = {
    "LZ77", "Zstd", "Snappy", "SZ2", "Machete", "SimPiece", "Deflate", "LZ4", "FPC", "Gorilla", "Chimp128",
    "Elf", "SerfQt", "SerfXOR"
};
const static double kMaxDiffOverall = 1.0E-3;
const static int kBlockSizeOverall = 50;
const static std::string kMethodListRel[] = {
    "SZ2", "SerfXOR"
};
const static double kMaxDiffRel[] = {
    0.001, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5
};
const static int kBlockSizeRel = kBlockSizeOverall;
//const static std::string kMethodList[] = {
//    "SZ2", "Machete", "SimPiece", "SerfQt", "SerfXOR"
//};
const static std::string kMethodList32[] = {
    "LZ77", "Zstd", "Snappy", "SZ2", "Deflate", "LZ4", "Chimp128", "Elf", "SerfQt", "SerfXOR"
};
const static double kMaxDiff32 = kMaxDiffOverall;
const static int kBlockSize32 = kBlockSizeOverall;
const static std::string kAbbrList[] = {
    "AP", "BT", "BW", "CDTR", "CT", "DT", "IR", "MT", "PM10", "SG", "SUSA", "TD", "WS"
};
const static std::string kAbbrList32[] = {
    "BW", "CDTR", "DT", "PM10", "SG", "WS"
};
const static std::string kMethodListAblation[] = {
    "SerfXOR", "SerfXOR-Shifter", "SerfXOR-PlusOneOpt", "SerfXOR-FastSearchOpt"
};
const static double kMaxDiffAblation = kMaxDiffOverall;
const static int kBlockSizeAblation = kBlockSizeOverall;
//const static std::string kAbbrList32[] = {
//    "AP", "AS", "BM", "BT", "BW", "CT", "DT", "IR", "PM10", "SDE", "SUK", "SUSA", "WS"
//};
const static std::string kDataSetList32[] = {
    "Basel-wind.csv",
    "Chengdu-traj.csv",
    "Dew-point-temp.csv",
    "PM10-dust.csv",
    "Smart-grid.csv",
    "Wind-Speed.csv"
};
const static std::unordered_map<std::string, int> kFileNameToAdjustDigit{
    {"Air-pressure.csv", 0},
    {"Basel-temp.csv", 80},
    {"Basel-wind.csv", 126},
    {"Chengdu-traj.csv", 0},
    {"City-temp.csv", 355},
    {"Dew-point-temp.csv", 109},
    {"IR-bio-temp.csv", 39},
    {"Motor-temp.csv", 109},
    {"PM10-dust.csv", 256},
    {"Smart-grid.csv", 4},
    {"Stocks-USA.csv", 245},
    {"T-drive.csv", 0},
    {"Wind-Speed.csv", 8}
};
//constexpr static int kBlockSizeList[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
constexpr static int kBlockSizeList[] = {50};
//constexpr static double kMaxDiffList[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static double kMaxDiffList[] = {1.0E-3};
//constexpr static double kMaxDiffList[] = {0.5};
//constexpr static int kBlockSizeList[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

#endif //SERF_ALL_TEST_PERF_EXPR_CONFIG_HPP_
