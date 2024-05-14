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
#include "chimp/ChimpCompressor.h"
#include "chimp/ChimpDecompressor.h"
#include "elf/elf.h"
#include "gorilla/gorilla_compressor.h"
#include "gorilla/gorilla_decompressor.h"


const static size_t kBlockSize = 1000;
const static size_t kDoubleSize = 64;
const static std::string kExportExprTablePrefix = "../../test/";
const static std::string kExportExprTableFileName = "perf_table.csv";
const static std::string kDataSetDirPrefix = "../../test/dataSet/";
const static std::string kDataSetList[] = {
        "init.csv",
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
        "Wind-Speed.csv"
};
const static std::unordered_map<std::string, int> kFileNameToAdjustDigit {
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
constexpr static double kMaxDiff[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};


static std::string double_to_string_with_precision(double val, size_t precision) {
    std::ostringstream stringBuffer;
    stringBuffer << std::fixed << std::setprecision(precision) << val;
    return stringBuffer.str();
}


class PerfRecord {
public:
    PerfRecord() = default;

    void IncreaseCompressionTime(std::chrono::microseconds &duration) {
        compression_time_ += duration;
    }

    auto compression_time() {
        return compression_time_;
    }

    auto AvgCompressionTimePerBlock() {
        return (double) compression_time_.count() / block_count_;
    }

    void IncreaseDecompressionTime(std::chrono::microseconds &duration) {
        decompression_time_ += duration;
    }

    auto decompression_time() {
        return decompression_time_;
    }

    auto AvgDecompressionTimePerBlock() {
        return (double) decompression_time_.count() / block_count_;
    }

    void AddCompressedSize(long size) {
        compressed_size_in_bits_ += size;
    }

    void set_block_count(int blockCount_) {
        block_count_ = blockCount_;
    }

    int block_count() {
        return block_count_;
    }

    double CalCompressionRatio() {
        return (double) compressed_size_in_bits_ / (double) (block_count_ * kBlockSize * kDoubleSize);
    }

private:
    std::chrono::microseconds compression_time_ = std::chrono::microseconds::zero();
    std::chrono::microseconds decompression_time_ = std::chrono::microseconds::zero();
    long compressed_size_in_bits_ = 0;
    int block_count_ = 0;
};


class ExprConf {
public:
    struct hash {
        std::size_t operator()(const ExprConf &conf) const {
            return std::hash<std::string>()(conf.method_ + conf.data_set_ + conf.max_diff_);
        }
    };

    ExprConf() = delete;

    ExprConf(std::string method, std::string data_set, double max_diff) : method_(std::move(method)),
                                                                          data_set_(std::move(data_set)),
                                                                          max_diff_(double_to_string_with_precision(
                                                                                  max_diff, 8)) {}

    bool operator == (const ExprConf &otherConf) const {
        return method_ == otherConf.method_ && data_set_ == otherConf.data_set_ && max_diff_ == otherConf.max_diff_;
    }

    std::string method() const {
        return method_;
    }

    std::string data_set() const {
        return data_set_;
    }

    std::string max_diff() const {
        return max_diff_;
    }

private:
    const std::string method_;
    const std::string data_set_;
    const std::string max_diff_;
};


std::unordered_map<ExprConf, PerfRecord, ExprConf::hash> expr_table;

void ExportExprTable() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream << "Method,DataSet,MaxDiff,CompressionRatio,CompressionTime(AvgPerBlock),DecompressionTime(AvgPerBlock)" << std::endl;
    // Write record
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << "," << record.CalCompressionRatio() << "," << record.AvgCompressionTimePerBlock() << "," << record.AvgDecompressionTimePerBlock() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}


/**
 * @brief Read a block of double from the file input stream, whose size is equal to kBlockSize
 * @param file_input_stream_ref Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
    std::vector<double> ret;
    int entry_count = 0;
    double buffer;
    while (!file_input_stream_ref.eof() && entry_count < kBlockSize) {
        file_input_stream_ref >> buffer;
        ret.emplace_back(buffer);
        ++entry_count;
    }
    return ret;
}

void ResetFileStream(std::ifstream &data_set_input_stream_ref) {
    data_set_input_stream_ref.clear();
    data_set_input_stream_ref.seekg(0, std::ios::beg);
}

PerfRecord PerfSerfXOR(std::ifstream &data_set_input_stream_ref, double max_diff, const std::string& data_set) {
    PerfRecord perf_record;
    SerfXORCompressor serf_xor_compressor(kBlockSize, max_diff, kFileNameToAdjustDigit.find(data_set)->second);
    SerfXORDecompressor serf_xor_decompressor(kFileNameToAdjustDigit.find(data_set)->second);

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            serf_xor_compressor.AddValue(value);
        }
        serf_xor_compressor.Close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(serf_xor_compressor.compressed_size_in_bits());
        Array<uint8_t> compression_output = serf_xor_compressor.compressed_bytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = serf_xor_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfSerfQt(std::ifstream &data_set_input_stream_ref, double max_diff) {
    PerfRecord perf_record;
    SerfQtCompressor serf_qt_compressor(max_diff);
    SerfQtDecompressor serf_qt_decompressor(max_diff);

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            serf_qt_compressor.addValue(value);
        }
        serf_qt_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(serf_qt_compressor.getCompressedSizeInBits());
        Array<uint8_t> compression_output = serf_qt_compressor.getBytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = serf_qt_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfDeflate(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        DeflateCompressor deflate_compressor;
        DeflateDecompressor deflate_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            deflate_compressor.addValue(value);
        }
        deflate_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(deflate_compressor.getCompressedSizeInBits());
        Array<uint8_t> compression_output = deflate_compressor.getBytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = deflate_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfLZ4(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        LZ4Compressor lz_4_compressor;
        LZ4Decompressor lz_4_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            lz_4_compressor.addValue(value);
        }
        lz_4_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(lz_4_compressor.getCompressedSizeInBits());
        Array<char> compression_output = lz_4_compressor.getBytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = lz_4_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfFPC(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        FpcCompressor fpc_compressor(5, kBlockSize);
        FpcDecompressor fpc_decompressor(5, kBlockSize);
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            fpc_compressor.addValue(value);
        }
        fpc_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(fpc_compressor.getCompressedSizeInBits());
        std::vector<char> compression_output = fpc_compressor.getBytes();
        fpc_decompressor.setBytes(compression_output.data(), compression_output.size());
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = fpc_decompressor.decompress();
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfALP(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        uint8_t compression_output_buffer[(kBlockSize * sizeof(double)) + 8096];
        auto decompressed_buffer_size = alp::AlpApiUtils::align_value<size_t, alp::config::VECTOR_SIZE>(kBlockSize);
        double decompress_output[decompressed_buffer_size];
        alp::AlpCompressor alp_compressor;
        alp::AlpDecompressor alp_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        alp_compressor.compress(original_data.data(), original_data.size(), compression_output_buffer);
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(alp_compressor.get_size() * 8);
        auto decompression_start_time = std::chrono::steady_clock::now();
        alp_decompressor.decompress(compression_output_buffer, kBlockSize, decompress_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfElf(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        uint8_t *compression_output_buffer;
        double decompression_output[kBlockSize];
        ssize_t compression_output_len_in_bytes;
        ssize_t decompression_len;
        ++block_count;

        auto compression_start_time = std::chrono::steady_clock::now();
        compression_output_len_in_bytes = elf_encode(original_data.data(), original_data.size(), &compression_output_buffer, 0);
        auto compression_end_time = std::chrono::steady_clock::now();

        perf_record.AddCompressedSize(compression_output_len_in_bytes * 8);

        auto decompression_start_time = std::chrono::steady_clock::now();
        decompression_len = elf_decode(compression_output_buffer, compression_output_len_in_bytes, decompression_output, 0);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfChimp128(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ChimpCompressor chimp_compressor(128);
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            chimp_compressor.addValue(value);
        }
        chimp_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(chimp_compressor.get_size());
        Array<uint8_t> compression_output = chimp_compressor.get_compress_pack();
        auto decompression_start_time = std::chrono::steady_clock::now();
        ChimpDecompressor chimp_decompressor(compression_output, 128);
        std::vector<double> decompression_output = chimp_decompressor.decompress();
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfGorilla(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        GorillaCompressor gorilla_compressor(kBlockSize);
        GorillaDecompressor gorilla_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            gorilla_compressor.addValue(value);
        }
        gorilla_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(gorilla_compressor.get_compress_size_in_bits());
        Array<uint8_t> compression_output = gorilla_compressor.get_compress_pack();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompression_output = gorilla_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

TEST(Perf, All) {
    for (const auto &data_set: kDataSetList) {
        std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
        if (!data_set_input_stream.is_open()) {
            std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
        }

        // Lossy
        for (const auto &max_diff: kMaxDiff) {
            expr_table.insert(std::make_pair(ExprConf("SerfXOR", data_set, max_diff), PerfSerfXOR(data_set_input_stream, max_diff, data_set)));
            ResetFileStream(data_set_input_stream);
            expr_table.insert(std::make_pair(ExprConf("SerfQt", data_set, max_diff), PerfSerfQt(data_set_input_stream, max_diff)));
            ResetFileStream(data_set_input_stream);
        }

        // Lossless
        expr_table.insert(std::make_pair(ExprConf("Elf", data_set, 0), PerfElf(data_set_input_stream)));
        ResetFileStream(data_set_input_stream);
        expr_table.insert(std::make_pair(ExprConf("Chimp", data_set, 0), PerfElf(data_set_input_stream)));
        ResetFileStream(data_set_input_stream);

        data_set_input_stream.close();
    }

    ExportExprTable();
}