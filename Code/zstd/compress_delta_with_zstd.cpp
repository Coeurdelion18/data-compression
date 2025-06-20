#include <zstd.h>
#include <vector>
#include <iostream>
#include <string>
#include "Delta_preprocessing.h"

bool compress_delta_with_zstd(const std::vector<int16_t>& input, std::vector<uint8_t>& compressed_output) {
    if (input.empty()) {
        std::cerr << "Input data is empty.\n";
        return false;
    }

    std::vector<int16_t> delta_data = delta_encode(input);
    const uint8_t* byte_data = reinterpret_cast<const uint8_t*>(delta_data.data());
    size_t byte_size = delta_data.size() * sizeof(int16_t);

    size_t max_compressed_size = ZSTD_compressBound(byte_size);
    compressed_output.resize(max_compressed_size);  // Resize output buffer

    size_t compressed_size = ZSTD_compress(compressed_output.data(), max_compressed_size,
                                           byte_data, byte_size, 3);
    if (ZSTD_isError(compressed_size)) {
        std::cerr << "Compression failed: " << ZSTD_getErrorName(compressed_size) << "\n";
        return false;
    }

    compressed_output.resize(compressed_size);  // Trim to actual size
    return true;
}
