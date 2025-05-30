//The input is a raw file with 16 bit signed integers in sequence. 
//The objective in this file is to parse the data and convert it to a readable format.
//Then, we will compress the data using different algorithms.

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

void raw_to_csv(std::ifstream& in_file, std::ofstream& out_file) {
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
    for (size_t i = 0; i < data.size(); ++i) {
        out_file << data[i];
        if (i != data.size() - 1)
            out_file << ',';  // comma between values except after last
    }
}

void raw_to_char(std::ifstream& in_file, std::ofstream& out_file) {
    in_file.seekg(0, std::ios::end);
    std::streamsize file_size = in_file.tellg();
    in_file.seekg(0, std::ios::beg);
    if (file_size % sizeof(char) != 0) {
        std::cout << "File size is not a multiple of int16_t\n";
        std::exit(1);
    }
    size_t sample_count = file_size / sizeof(char);
    std::vector <char> data(sample_count);
    in_file.read(reinterpret_cast<char*>(data.data()), file_size);
    for (size_t i = 0; i < data.size(); ++i) {
        out_file << data[i];
    }
}

int main() {

    std::string input_file_path {"/home/siddharthsastri/Documents/Petasense/Data/s3_vm4/VM4P-00018-1734523888880.raw"};
 //Enter path to the input file.
    std::ifstream input_file {input_file_path, std::ios::binary};
    std::ofstream out_file{"out_file_full.csv"};

    if (!input_file) {
        std::cerr << "Error: Could not open input file: " << input_file_path << std::endl;
        return 1;
    }

    if (!out_file) {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    raw_to_csv(input_file, out_file);

    return 0;
}