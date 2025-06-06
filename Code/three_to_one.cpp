#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

void split(const fs::path& input_path, const fs::path& output_dir) {
    std::ifstream in_file(input_path, std::ios::binary);
    if (!in_file) {
        std::cerr << "Failed to open input file: " << input_path << std::endl;
        return;
    }

    // Prepare output file names
    std::string stem = input_path.stem().string(); // filename without extension
    fs::path out1 = output_dir / (stem + "_ch1.raw");
    fs::path out2 = output_dir / (stem + "_ch2.raw");
    fs::path out3 = output_dir / (stem + "_ch3.raw");

    std::ofstream out_file1(out1, std::ios::binary);
    std::ofstream out_file2(out2, std::ios::binary);
    std::ofstream out_file3(out3, std::ios::binary);
    if (!out_file1 || !out_file2 || !out_file3) {
        std::cerr << "Failed to open output files for: " << input_path << std::endl;
        return;
    }

    in_file.seekg(0, std::ios::end);
    std::streamsize filesize = in_file.tellg();
    in_file.seekg(0, std::ios::beg);

    size_t sample_count = filesize / sizeof(int16_t);
    std::vector<int16_t> data(sample_count);

    in_file.read(reinterpret_cast<char*>(data.data()), filesize);

    for (size_t i = 0; i < data.size(); ++i) {
        switch (i % 3) {
            case 0: out_file1.write(reinterpret_cast<char*>(&data[i]), sizeof(int16_t)); break;
            case 1: out_file2.write(reinterpret_cast<char*>(&data[i]), sizeof(int16_t)); break;
            case 2: out_file3.write(reinterpret_cast<char*>(&data[i]), sizeof(int16_t)); break;
        }
    }

    std::cout << "Split: " << input_path.filename() << " -> " 
              << out1.filename() << ", " 
              << out2.filename() << ", " 
              << out3.filename() << "\n";
}

int main() {
    fs::path input_dir = "../Data/3_axis_raw";
    fs::path output_dir = "../Data/3_axis_raw_split";

    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
        std::cerr << "Input directory does not exist: " << input_dir << std::endl;
        return 1;
    }

    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".raw") {
            split(entry.path(), output_dir);
        }
    }

    return 0;
}
