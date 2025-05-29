#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include "Delta_preprocessing.h"

extern "C" {
    #include "heatshrink_encoder.h"
    #include "heatshrink_decoder.h"
    #include "heatshrink_common.h"
    #include "heatshrink_config.h"
}

constexpr size_t INPUT_SIZE = 256;
constexpr size_t OUT_BUFFER_SIZE = 1024;

//In the main function, we will open the input file and copy it to an int16_t vector.
//This will then be passed to our compress() function.

//We will compress int16_t data.

std::vector<uint8_t> compress(std::vector<int16_t>& input) {
    heatshrink_encoder encoder;
    heatshrink_encoder_reset(&encoder);

    std::vector<uint8_t> input_bytes(input.size() * sizeof(int16_t));
    std::memcpy(input_bytes.data(), input.data(), input_bytes.size());
    //input_bytes now contains the raw bytes

    std::vector<uint8_t> compressed;
    size_t sunk = 0, polled = 0, cursor = 0;
    uint8_t out_buffer[OUT_BUFFER_SIZE]; //This is a temporary buffer to hold the output

    constexpr size_t SINK_CHUNK_SIZE = 64;

    while (cursor < input_bytes.size()) {
        size_t chunk = std::min(SINK_CHUNK_SIZE, input_bytes.size() - cursor);
        heatshrink_encoder_sink(&encoder, &input_bytes[cursor], chunk, &sunk);
        cursor += sunk;

        HSE_poll_res pres;
        do {
            pres = heatshrink_encoder_poll(&encoder, out_buffer, OUT_BUFFER_SIZE, &polled);
            compressed.insert(compressed.end(), out_buffer, out_buffer + polled);
        } while (pres == HSER_POLL_MORE);
    }


    // Finish the stream
    HSE_finish_res fres;
    do {
        fres = heatshrink_encoder_finish(&encoder);
        HSE_poll_res pres;
        do {
            pres = heatshrink_encoder_poll(&encoder, out_buffer, OUT_BUFFER_SIZE, &polled);
            compressed.insert(compressed.end(), out_buffer, out_buffer + polled);
        } while (pres == HSER_POLL_MORE);
    } while (fres == HSER_FINISH_MORE);

    return compressed;
}

void compress_all_files_in_directory(const std::string& directory_path, const std::string& output_file_path) {
    namespace fs = std::filesystem;

    size_t total_original = 0;
    size_t total_compressed = 0;
    size_t file_count = 0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::setw(35) << std::left << "File"
              << " | Original (bytes) | Compressed (bytes) | Compression Ratio\n";
    std::cout << std::string(80, '-') << '\n';

    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (!entry.is_regular_file()) continue;

        const std::string& input_file_path = entry.path().string();

        std::ifstream in_file {input_file_path, std::ios::binary};
        in_file.seekg(0, std::ios::end);
        std::streamsize file_size = in_file.tellg();
        in_file.seekg(0, std::ios::beg);

        if (file_size % sizeof(int16_t) != 0) {
            std::cout << std::setw(35) << std::left << entry.path().filename().string()
                      << " | Invalid file size (not multiple of int16_t)\n";
            continue;
        }

        size_t sample_count = file_size / sizeof(int16_t);
        std::vector<int16_t> data(sample_count);
        in_file.read(reinterpret_cast<char*>(data.data()), file_size);
        in_file.close();

        //Encode the data using delta encoding
        std::vector<int16_t> result = delta_encode(data);
        //Pass the data to be compressed.
        std::vector<uint8_t> output = compress(result);

        std::ofstream out_file(output_file_path, std::ios::binary);
        if (!out_file) {
            std::cerr << "Failed to open " << output_file_path << " for writing.\n";
            continue;
        }

        out_file.write(reinterpret_cast<const char*>(output.data()), output.size());
        out_file.close();

        double ratio = static_cast<double>(output.size()) / file_size;

        std::cout << std::setw(35) << std::left << entry.path().filename().string()
                  << " | " << std::setw(16) << file_size
                  << " | " << std::setw(18) << output.size()
                  << " | " << ratio << '\n';

        total_original += file_size;
        total_compressed += output.size();
        file_count++;
    }

    std::cout << std::string(80, '-') << '\n';
    if (file_count > 0) {
        double mean_ratio = total_original / static_cast<double>(total_compressed);
        std::cout << "Processed " << file_count << " file(s). Mean compression ratio: "
                  << mean_ratio << '\n';
    } else {
        std::cout << "No valid files were processed.\n";
    }
}


/*
int main() {
    std::string input_file_path {"../../Data/TX-01052_5716_1677760203.raw"}; //Enter path to the input file.
    std::ifstream in_file {input_file_path, std::ios::binary};
    in_file.seekg(0, std::ios::end);
    std::streamsize file_size = in_file.tellg();
    in_file.seekg(0, std::ios::beg);
    if (file_size % sizeof(int16_t) != 0) {
        std::cout << "File size is not a multiple of int16_t\n";
        std::exit(1);
    }
    size_t sample_count = file_size / sizeof(int16_t);
    std::vector <int16_t> data(sample_count);
    in_file.read(reinterpret_cast<char*>(data.data()), file_size);
    in_file.close();
    std::vector<uint8_t> output = compress(data);
    std::string output_file_path = "./compressed_output.hsz";
    std::ofstream out_file(output_file_path, std::ios::binary);

    if (!out_file) {
        std::cerr << "Failed to open output file for writing.\n";
        return 1;
    }

    out_file.write(reinterpret_cast<const char*>(output.data()), output.size());
    out_file.close();

    std::cout << "Compression complete. Compressed file saved to: " << output_file_path << '\n';
    return 0;
}
    */

int main() {
    std::string input_directory = "../../Data";
    std::string output_file_path = "./compressed_output.hsz";
    compress_all_files_in_directory(input_directory, output_file_path);
    return 0;
}
