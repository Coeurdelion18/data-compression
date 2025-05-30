#include <zstd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "Delta_preprocessing.h"

bool compress_delta_with_zstd(const std::vector<int16_t>& input, const std::string& output_filename) {
    if (input.empty()) {
        std::cerr << "Input data is empty.\n";
        return false;
    }

    std::vector<int16_t> delta_data = delta_encode(input);
    const uint8_t* byte_data = reinterpret_cast<const uint8_t*>(delta_data.data());
    size_t byte_size = delta_data.size() * sizeof(int16_t);

    size_t max_compressed_size = ZSTD_compressBound(byte_size);
    std::vector<uint8_t> compressed_data(max_compressed_size);

    size_t compressed_size = ZSTD_compress(compressed_data.data(), max_compressed_size,
                                           byte_data, byte_size, 3); // level 3 compression
    if (ZSTD_isError(compressed_size)) {
        std::cerr << "Compression failed: " << ZSTD_getErrorName(compressed_size) << "\n";
        return false;
    }

    std::ofstream out(output_filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open " << output_filename << " for writing.\n";
        return false;
    }

    out.write(reinterpret_cast<const char*>(compressed_data.data()), compressed_size);
    return true;
}
