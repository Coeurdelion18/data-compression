#ifndef compress_decompress_h
#define compress_decompress_h
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <fstream>
#include <iostream>
extern "C" {
    #include "lz4.h"
}
bool compress(const std::vector<int16_t>& input, std::string& output_file_name);
void decompress(const std::string& input_file_name, std::vector<int16_t>& output);

#endif // compress_decompress_h