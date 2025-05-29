#ifndef GZIP_H
#define GZIP_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <zlib.h>
#include <fstream>

bool compress_delta_with_zlib_gz(const std::vector<int16_t>& input, const std::string& output_filename);

#endif