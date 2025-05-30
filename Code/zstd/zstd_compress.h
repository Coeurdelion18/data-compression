#ifndef ZSTD_COMPRESS_H
#define ZSTD_COMPRESS_H
#include <vector>
#include <string>
#include <cstdint>
#include <zstd.h>
#include <fstream>
#include "Delta_preprocessing.h"

bool compress_delta_with_zstd(const std::vector<int16_t>& input, const std::string& output_filename);
bool decompress_delta_with_zstd(const std::string& input_filename, std::vector<int16_t>& output);


#endif // ZSTD_COMPRESS_H