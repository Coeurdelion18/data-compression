#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

extern "C" {
    #include "heatshrink_encoder.h"
    #include "heatshrink_decoder.h"
    #include "heatshrink_common.h"
    #include "heatshrink_config.h"
}
constexpr size_t INPUT_SIZE = 256;
constexpr size_t OUT_BUFFER_SIZE = 1024;

std::vector<int16_t> decompress(std::vector<uint8_t>& compressed_data) {
    heatshrink_decoder decoder;  // statically allocated
    heatshrink_decoder_reset(&decoder);

    std::vector<uint8_t> decompressed_bytes;
    size_t cursor = 0;
    size_t sunk = 0, polled = 0;
    uint8_t out_buffer[OUT_BUFFER_SIZE];

    // Sink the full compressed input
    while (cursor < compressed_data.size()) {
        HSD_sink_res sres = heatshrink_decoder_sink(
            &decoder, &compressed_data[cursor],
            compressed_data.size() - cursor, &sunk);
        if (sres != HSDR_SINK_OK) {
            std::cerr << "Sink error: " << sres << '\n';
            std::exit(1);
        }
        cursor += sunk;

        // Poll after each sink
        HSD_poll_res pres;
        do {
            pres = heatshrink_decoder_poll(&decoder, out_buffer, OUT_BUFFER_SIZE, &polled);
            if (pres != HSDR_POLL_EMPTY && pres != HSDR_POLL_MORE) {
                std::cerr << "Poll error: " << pres << '\n';
                std::exit(1);
            }
            decompressed_bytes.insert(decompressed_bytes.end(), out_buffer, out_buffer + polled);
        } while (pres == HSDR_POLL_MORE);
    }

    // Signal end of input
    HSD_finish_res fres;
    do {
        fres = heatshrink_decoder_finish(&decoder);

        HSD_poll_res pres;
        do {
            pres = heatshrink_decoder_poll(&decoder, out_buffer, OUT_BUFFER_SIZE, &polled);
            if (pres != HSDR_POLL_EMPTY && pres != HSDR_POLL_MORE) {
                std::cerr << "Poll error after finish: " << pres << '\n';
                std::exit(1);
            }
            decompressed_bytes.insert(decompressed_bytes.end(), out_buffer, out_buffer + polled);
        } while (pres == HSDR_POLL_MORE);

    } while (fres == HSDR_FINISH_MORE);

    // Convert back to int16_t
    if (decompressed_bytes.size() % sizeof(int16_t) != 0) {
        std::cerr << "Decompressed data size is not a multiple of int16_t\n";
        std::exit(1);
    }

    size_t count = decompressed_bytes.size() / sizeof(int16_t);
    std::vector<int16_t> result(count);
    std::memcpy(result.data(), decompressed_bytes.data(), decompressed_bytes.size());
    return result;
}


int main() {
    std::string input_file_path = "./compressed_output.hsz";
    std::ifstream in_file(input_file_path, std::ios::binary);

    if (!in_file) {
        std::cerr << "Failed to open compressed file.\n";
        return 1;
    }

    in_file.seekg(0, std::ios::end);
    size_t file_size = in_file.tellg();
    in_file.seekg(0, std::ios::beg);

    std::vector<uint8_t> compressed_data(file_size);
    in_file.read(reinterpret_cast<char*>(compressed_data.data()), file_size);
    in_file.close();

    std::vector<int16_t> decompressed_data = decompress(compressed_data);

    std::string output_txt_path = "./decompressed_output.txt";
    std::ofstream out_file(output_txt_path);
    if (!out_file) {
        std::cerr << "Failed to open output text file.\n";
        return 1;
    }

    for (int16_t value : decompressed_data) {
        out_file << value << '\n';
    }

    out_file.close();
    std::cout << "Decompression complete. Output saved to: " << output_txt_path << '\n';
    return 0;
}
