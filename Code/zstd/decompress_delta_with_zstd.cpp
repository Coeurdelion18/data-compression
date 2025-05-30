#include <zstd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "Delta_preprocessing.h"
#include <cstring>

bool decompress_delta_with_zstd(const std::string& input_filename, std::vector<int16_t>& output) {
    std::ifstream in(input_filename, std::ios::binary | std::ios::ate);
    if (!in) {
        std::cerr << "Failed to open " << input_filename << " for reading.\n";
        return false;
    }

    size_t compressed_size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<uint8_t> compressed_data(compressed_size);
    in.read(reinterpret_cast<char*>(compressed_data.data()), compressed_size);

    unsigned long long uncompressed_size = ZSTD_getFrameContentSize(compressed_data.data(), compressed_size);
    if (uncompressed_size == ZSTD_CONTENTSIZE_ERROR || uncompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        std::cerr << "Could not determine uncompressed size.\n";
        return false;
    }

    std::vector<uint8_t> decompressed_data(uncompressed_size);
    size_t result = ZSTD_decompress(decompressed_data.data(), uncompressed_size,
                                    compressed_data.data(), compressed_size);

    if (ZSTD_isError(result)) {
        std::cerr << "Decompression failed: " << ZSTD_getErrorName(result) << "\n";
        return false;
    }

    size_t sample_count = uncompressed_size / sizeof(int16_t);
    output.resize(sample_count);
    std::memcpy(output.data(), decompressed_data.data(), uncompressed_size);

    output = delta_decode(output);
    return true;
}
