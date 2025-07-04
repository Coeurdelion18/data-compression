#include "base64.h"

#include <algorithm>
#include <string>
#include <cstring>
#include "esp_log.h"
#include "sdkconfig.h"

static const char* TAG = "BASE64";

static const char* base64_chars[2] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
};

static unsigned int pos_of_char(const unsigned char chr) {
    if (chr >= 'A' && chr <= 'Z') return chr - 'A';
    else if (chr >= 'a' && chr <= 'z') return chr - 'a' + 26;
    else if (chr >= '0' && chr <= '9') return chr - '0' + 52;
    else if (chr == '+' || chr == '-') return 62;
    else if (chr == '/' || chr == '_') return 63;

    ESP_LOGE(TAG, "Invalid base64 character: 0x%02X", chr);
    return 255; // Sentinel value
}

static std::string insert_linebreaks(std::string str, size_t distance) {
    if (str.empty()) return "";
    size_t pos = distance;
    while (pos < str.size()) {
        str.insert(pos, "\n");
        pos += distance + 1;
    }
    return str;
}

template <typename String, unsigned int line_length>
static std::string encode_with_line_breaks(String s) {
    return insert_linebreaks(base64_encode(s, false), line_length);
}

template <typename String>
static std::string encode_pem(String s) {
    return encode_with_line_breaks<String, 64>(s);
}

template <typename String>
static std::string encode_mime(String s) {
    return encode_with_line_breaks<String, 76>(s);
}

template <typename String>
static std::string encode(String s, bool url) {
    return base64_encode(reinterpret_cast<const unsigned char*>(s.data()), s.length(), url);
}

std::string base64_encode(unsigned char const* bytes_to_encode, size_t in_len, bool url) {
    size_t len_encoded = (in_len + 2) / 3 * 4;
    unsigned char trailing_char = url ? '.' : '=';

    const char* base64_chars_ = base64_chars[url];
    std::string ret;
    ret.reserve(len_encoded);

    unsigned int pos = 0;
    while (pos < in_len) {
        ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

        if (pos + 1 < in_len) {
            ret.push_back(base64_chars_[
                ((bytes_to_encode[pos + 0] & 0x03) << 4) +
                ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

            if (pos + 2 < in_len) {
                ret.push_back(base64_chars_[
                    ((bytes_to_encode[pos + 1] & 0x0f) << 2) +
                    ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
                ret.push_back(base64_chars_[
                    bytes_to_encode[pos + 2] & 0x3f]);
            } else {
                ret.push_back(base64_chars_[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
                ret.push_back(trailing_char);
            }
        } else {
            ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0x03) << 4]);
            ret.push_back(trailing_char);
            ret.push_back(trailing_char);
        }

        pos += 3;
    }

    return ret;
}

template <typename String>
static std::string decode(String const& encoded_string, bool remove_linebreaks) {
    if (encoded_string.empty()) return std::string();

    std::string data = encoded_string;
    if (remove_linebreaks) {
        data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
    }

    size_t length = data.length();
    size_t approx_length = length / 4 * 3;
    std::string ret;
    ret.reserve(approx_length);

    size_t pos = 0;
    while (pos < length) {
        unsigned int c0 = pos_of_char(data.at(pos + 0));
        unsigned int c1 = pos_of_char(data.at(pos + 1));
        if (c0 == 255 || c1 == 255) break;

        ret.push_back(static_cast<char>((c0 << 2) + ((c1 & 0x30) >> 4)));

        if (pos + 2 < length && data.at(pos + 2) != '=' && data.at(pos + 2) != '.') {
            unsigned int c2 = pos_of_char(data.at(pos + 2));
            if (c2 == 255) break;

            ret.push_back(static_cast<char>(((c1 & 0x0f) << 4) + ((c2 & 0x3c) >> 2)));

            if (pos + 3 < length && data.at(pos + 3) != '=' && data.at(pos + 3) != '.') {
                unsigned int c3 = pos_of_char(data.at(pos + 3));
                if (c3 == 255) break;

                ret.push_back(static_cast<char>(((c2 & 0x03) << 6) + c3));
            }
        }

        pos += 4;
    }

    return ret;
}

// Public interface

std::string base64_encode(std::string const& s, bool url) {
    return encode(s, url);
}

std::string base64_encode_pem(std::string const& s) {
    return encode_pem(s);
}

std::string base64_encode_mime(std::string const& s) {
    return encode_mime(s);
}

std::string base64_decode(std::string const& s, bool remove_linebreaks) {
    return decode(s, remove_linebreaks);
}

#if __cplusplus >= 201703L
std::string base64_encode(std::string_view s, bool url) {
    return encode(std::string(s), url);
}

std::string base64_encode_pem(std::string_view s) {
    return encode_pem(std::string(s));
}

std::string base64_encode_mime(std::string_view s) {
    return encode_mime(std::string(s));
}

std::string base64_decode(std::string_view s, bool remove_linebreaks) {
    return decode(std::string(s), remove_linebreaks);
}
#endif
