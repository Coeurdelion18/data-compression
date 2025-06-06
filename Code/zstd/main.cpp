#include "zstd_compress.h"  // Your renamed header with compress/decompress functions
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <filesystem>

int compress_all_files(const std::string& directory_path, const std::string& output_dir) {
    namespace fs = std::filesystem;
    fs::create_directories(output_dir);

    size_t file_count = 0;
    double total_ratio = 0.0;

    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (!entry.is_regular_file()) continue;
        const std::string input_file_path = entry.path().string();

        std::ifstream in_file(input_file_path, std::ios::binary);
        if (!in_file) {
            std::cerr << "Failed to open " << input_file_path << "\n";
            continue;
        }

        in_file.seekg(0, std::ios::end);
        std::streamsize file_size = in_file.tellg();
        in_file.seekg(0, std::ios::beg);

        if (file_size % sizeof(int16_t) != 0) {
            std::cout << std::setw(35) << std::left << entry.path().filename().string()
                      << " | Invalid file size\n";
            continue;
        }

        size_t sample_count = file_size / sizeof(int16_t);
        std::vector<int16_t> data(sample_count);
        in_file.read(reinterpret_cast<char*>(data.data()), file_size);
        in_file.close();

        std::string compressed_filename = output_dir + "/" + entry.path().filename().string() + ".zst";
        if (!compress_delta_with_zstd(data, compressed_filename)) {
            std::cout << std::setw(35) << std::left << entry.path().filename().string()
                      << " | Compression failed\n";
            continue;
        }

        std::error_code ec;
        auto compressed_size = fs::file_size(compressed_filename, ec);
        if (ec || compressed_size == 0) {
            std::cout << std::setw(35) << std::left << entry.path().filename().string()
                      << " | Could not determine compressed file size\n";
            continue;
        }

        double ratio = static_cast<double>(file_size) / compressed_size;
        std::cout << std::setw(35) << std::left << entry.path().filename().string()
                  << " | Compression ratio: " << std::fixed << std::setprecision(2) << file_size << " "<< compressed_size << " " << ratio << '\n';

        total_ratio += ratio;
        file_count++;
    }

    if (file_count > 0) {
        std::cout << "\nMean compression ratio: " << std::fixed << std::setprecision(2)
                  << (total_ratio / file_count) << '\n';
    } else {
        std::cout << "\nNo valid files processed.\n";
    }

    return 0;
}

int main() {
    const std::string input_dir = "../../Data/3_axis_raw_split";
    const std::string output_dir = "./compressed_zstd";
    return compress_all_files(input_dir, output_dir);
}
