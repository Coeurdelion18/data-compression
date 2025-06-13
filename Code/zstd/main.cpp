#include "base64.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "zstd_compress.h"

// Safe wrapper for binary base64 decode
std::vector<uint8_t> base64_decode_binary(const std::string& encoded) {
    std::string decoded_str = base64_decode(encoded, /*remove_linebreaks=*/true);
    return std::vector<uint8_t>(decoded_str.begin(), decoded_str.end());
}

// Print buffer as hex
void print_hex(const std::vector<uint8_t>& buffer) {
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<int>(buffer[i]);
        if ((i + 1) % 32 == 0) std::cout << "\n"; // Optional line break
        else std::cout << " ";
    }
    std::cout << std::dec << "\n";  // Restore decimal mode
}

void write_hex_to_file(const std::vector<uint8_t>& buffer, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    for (size_t i = 0; i < buffer.size(); ++i) {
        out << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(buffer[i]);
        if ((i + 1) % 32 == 0)
            out << '\n';
        else
            out << ' ';
    }

    out.close();
    std::cout << "Hex written to " << filename << "\n";
}

bool hex_file_to_binary(const std::string& hex_filename, const std::string& bin_filename) {
    std::ifstream in(hex_filename);
    if (!in) {
        std::cerr << "Failed to open " << hex_filename << "\n";
        return false;
    }

    std::vector<uint8_t> binary_data;
    std::string token;

    while (in >> token) {
        if (token.empty()) continue;

        // Parse hex pair into byte
        uint8_t byte = static_cast<uint8_t>(std::stoi(token, nullptr, 16));
        binary_data.push_back(byte);
    }

    in.close();

    // Write to binary .zst file
    std::ofstream out(bin_filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to write to " << bin_filename << "\n";
        return false;
    }

    out.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
    out.close();

    std::cout << "Binary written to " << bin_filename << " (" << binary_data.size() << " bytes)\n";
    return true;
}

int main() { //This main function takes base64 encoded input, decodes it, compresses the input.
    //Then, it writes the compressed input to a txt file in hexadecimal format.
    //It parses the hexadecimal back into uint8, decompresses it, encodes in base64 and prints to another file for comparison
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

    // Convert to int16_t buffer
    size_t int16_buffer_size = decoded_buffer.size() / 2;
    std::vector<int16_t> int16_buffer(int16_buffer_size);
    std::memcpy(int16_buffer.data(), decoded_buffer.data(), int16_buffer_size * sizeof(int16_t));

    // Compress to memory
    std::vector<uint8_t> compressed_output;
    if (!compress_delta_with_zstd(int16_buffer, compressed_output)) {
        std::cerr << "Compression failed\n";
        return 1;
    }

    std::cout << "Compressed size: " << compressed_output.size() << " bytes\n";
    write_hex_to_file(compressed_output, "compressed_output_hex.txt");

    // Now, we decompress to check.
    if (!hex_file_to_binary("compressed_output_hex.txt", "reconstructed_output.zst")) {
        return 1;
    }

    // Now call the decompression routine
    std::vector<int16_t> decompressed_data;
    if (!decompress_delta_with_zstd("reconstructed_output.zst", decompressed_data)) {
        std::cerr << "Decompression failed\n";
        return 1;
    }

    std::cout << "Decompression successful. Samples: " << decompressed_data.size() << "\n";

    // Convert decompressed int16_t data to uint8_t buffer
    std::vector<uint8_t> byte_buffer(decompressed_data.size() * sizeof(int16_t));
    std::memcpy(byte_buffer.data(), decompressed_data.data(), byte_buffer.size());

    // Encode to base64
    std::string base64_output = base64_encode(byte_buffer.data(), byte_buffer.size(), false);

    // Write to file
    std::ofstream out("decompressed_output_base64.txt");
    if (!out) {
        std::cerr << "Failed to open decompressed_output_base64.txt for writing.\n";
        return 1;
    }
    out << base64_output << '\n';
    out.close();

    std::cout << "Decompressed base64 output written to decompressed_output_base64.txt\n";

    return 0;
}
