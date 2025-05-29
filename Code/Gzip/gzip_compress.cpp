#include "gzip_compress.h"
#include "Delta_preprocessing.h"

// Compress delta-encoded int16_t data using zlib into .gz format
bool compress_delta_with_zlib_gz(const std::vector<int16_t>& input, const std::string& output_filename) {
    std::vector<int16_t> delta_data = delta_encode(input);

    // Convert to raw bytes
    const uint8_t* byte_data = reinterpret_cast<const uint8_t*>(delta_data.data());
    size_t byte_size = delta_data.size() * sizeof(int16_t);

    gzFile gzfile = gzopen(output_filename.c_str(), "wb"); // write, binary
    if (!gzfile) {
        std::cerr << "Failed to open " << output_filename << " for writing.\n";
        return false;
    }

    int written = gzwrite(gzfile, byte_data, byte_size);
    gzclose(gzfile);

    if (written == 0) {
        std::cerr << "gzwrite failed.\n";
        return false;
    }

    std::cout << "Compressed and saved to: " << output_filename << '\n';
    return true;
}