#include "gzip_compress.h"
#include "Delta_preprocessing.h"
#include <iostream>

bool compress_delta_with_zlib_gz(const std::vector<int16_t>& input, const std::string& output_filename) {
    if (input.empty()) {
        std::cerr << "Input data is empty.\n";
        return false;
    }

    std::vector<int16_t> delta_data = delta_encode(input);

    const uint8_t* byte_data = reinterpret_cast<const uint8_t*>(delta_data.data());
    size_t byte_size = delta_data.size() * sizeof(int16_t);

    gzFile gzfile = gzopen(output_filename.c_str(), "wb");
    if (!gzfile) {
        std::cerr << "Failed to open " << output_filename << " for writing.\n";
        return false;
    }

    int written = gzwrite(gzfile, byte_data, static_cast<unsigned int>(byte_size));
    gzclose(gzfile);

    if (written == 0) {
        std::cerr << "gzwrite failed for " << output_filename << "\n";
        return false;
    }

    return true;
}
