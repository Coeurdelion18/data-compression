#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

void convert_txt_to_raw(std::ifstream& input_file, std::ofstream& output_file) {
    std::string line;
    while(std::getline(input_file, line)) {
        line = trim(line);
        std::stringstream ss(line);
        std::string reading;
        while(std::getline(ss, reading, ',')) {
            int value = std::stoi(reading);
            output_file.write(reinterpret_cast<const char*>(&value), sizeof(int));
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
        std::ifstream in_file(input_file_path, std::ios::binary);
        fs::path out_dir = output_directory;
        fs::path out = out_dir / (stem + ".raw");
        std::ofstream out_file(out, std::ios::binary);
        convert_txt_to_raw(in_file, out_file);
    }
}

int main(void) {
    std::string input_dir = "./3_axis";
    std::string out_dir = "./3_axis_raw";
    convert_all_files(input_dir, out_dir);
    return 0;
}