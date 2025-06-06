#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(", \t\n\r\f\v");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(", \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

void convert_txt_to_raw(std::ifstream& input_file, std::ofstream& output_file) {
    std::string line;
    while (std::getline(input_file, line)) {
        std::stringstream ss(line);
        std::string reading;

        while (std::getline(ss, reading, ',')) {
            reading = trim(reading);

            if (!reading.empty()) {
                try {
                    int value = std::stoi(reading);
                    int16_t val = static_cast<int16_t>(value);
                    output_file.write(reinterpret_cast<const char*>(&val), sizeof(int16_t));
                } catch (const std::exception& e) {
                    std::cerr << "Skipping invalid value: '" << reading << "' (" << e.what() << ")\n";
                }
            }
        }
    }
}


void convert_all_files(const std::string& directory_path, std::string& output_directory) {
    if (!fs::exists(output_directory)) {
        fs::create_directories(output_directory);
    }
    for(const auto& entry : fs::directory_iterator(directory_path)) {
        const std::string input_file_path = entry.path().string();
        std::string stem = entry.path().stem().string();
        std::ifstream in_file(input_file_path);
        fs::path out_dir = output_directory;
        fs::path out = out_dir / (stem + ".raw");
        std::ofstream out_file(out, std::ios::binary);
        convert_txt_to_raw(in_file, out_file);
    }
}

int main(void) {
    std::string input_dir = "../Data/3_axis";
    std::string out_dir = "../Data/3_axis_raw";
    convert_all_files(input_dir, out_dir);
    return 0;
}