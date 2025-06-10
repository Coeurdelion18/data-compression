#include "base64.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include "zstd_compress.h"

// Safe wrapper for binary base64 decode
std::vector<uint8_t> base64_decode_binary(const std::string& encoded) {
    std::string decoded_str = base64_decode(encoded, /*remove_linebreaks=*/true);
    return std::vector<uint8_t>(decoded_str.begin(), decoded_str.end());
}

int main() {
    std::ifstream file("data1.txt");
    if (!file) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    std::string encoded((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::vector<uint8_t> decoded_buffer = base64_decode_binary(encoded);

    // Pad to 80004 bytes if necessary
    if (decoded_buffer.size() < 80004)
        decoded_buffer.resize(80004, 0);

    // Convert to int16_t buffer (80004 bytes → 40002 elements)
    size_t int16_buffer_size = decoded_buffer.size() / 2;
    std::vector<int16_t> int16_buffer(int16_buffer_size);
    std::memcpy(int16_buffer.data(), decoded_buffer.data(), int16_buffer_size * sizeof(int16_t));

    // Compress
    if (!compress_delta_with_zstd(int16_buffer, "compressed_output.zst")) {
        std::cerr << "Compression failed\n";
        return 1;
    }

    std::cout << "Compression successful.\n";
    return 0;
}
