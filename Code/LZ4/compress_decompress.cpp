#include <iostream>
#include <fstream>
#include <string>
#include <vector>

extern "C" {
    #include "lz4.h"
}

bool compress(const std::vector<int16_t>& input, std::string& output_file_name) {
    // Prepare output buffer
    size_t maxCompressedSize = LZ4_compressBound(input.size() * sizeof(int16_t));
    std::vector<char> compressedBuffer(maxCompressedSize);

    // Compress the data
    int compressedSize = LZ4_compress_default(reinterpret_cast<const char*>(input.data()), compressedBuffer.data(), input.size() * sizeof(int16_t), maxCompressedSize);
    if (compressedSize < 0) {
        throw std::runtime_error("Compression failed");
        return false;
    }

    // Write the compressed data to the output file
    std::ofstream out(output_file_name, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Could not open output file: " + output_file_name);
        return false;
    }
    
    out.write(compressedBuffer.data(), compressedSize);
    out.close();
    return true;
}

void decompress(const std::string& input_file_name, std::vector<int16_t>& output) {
    std::ifstream in(input_file_name, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Could not open input file: " + input_file_name);
    }

    // Read the compressed data into a buffer
    std::vector<char> compressedBuffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    // Prepare output buffer
    size_t decompressedSize = LZ4_decompress_safe(compressedBuffer.data(), nullptr, compressedBuffer.size(), 0);
    if (decompressedSize < 0) {
        throw std::runtime_error("Decompression failed");
    }
    
    output.resize(decompressedSize / sizeof(int16_t));
    
    // Decompress the data
    int result = LZ4_decompress_safe(compressedBuffer.data(), reinterpret_cast<char*>(output.data()), compressedBuffer.size(), decompressedSize);
    if (result < 0) {
        throw std::runtime_error("Decompression failed");
    }
}
