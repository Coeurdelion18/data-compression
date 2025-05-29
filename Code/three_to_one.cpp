#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

void split(std::ifstream& in_file, std::ofstream& out_file1, std::ofstream& out_file2, std::ofstream& out_file3) {
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
}

int main() {
    std::string input_file_path {"../Data/TX-01338_8266_1673371909.raw"};

    std::ifstream input_file(input_file_path, std::ios::binary);
    if (!input_file) {
        std::cerr << "Failed to open input file: " << input_file_path << std::endl;
        return 1;
    }

    std::ofstream out_file1("out_file1.raw", std::ios::binary);
    std::ofstream out_file2("out_file2.raw", std::ios::binary);
    std::ofstream out_file3("out_file3.raw", std::ios::binary);
    if (!out_file1 || !out_file2 || !out_file3) {
        std::cerr << "Failed to open one or more output files." << std::endl;
        return 1;
    }

    split(input_file, out_file1, out_file2, out_file3);
    return 0;
}
