#include "base64.h"
#include "zstd.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "APP";

// --- Base64 utility ---
std::string base64_encode_binary(const uint8_t* data, size_t len) {
    return base64_encode(reinterpret_cast<const unsigned char*>(data), len, false);
}

uint8_t* base64_decode_binary(const std::string& encoded, size_t& out_size) {
    std::string decoded = base64_decode(encoded, true);
    out_size = decoded.size();
    if (out_size == 0) return nullptr;

    uint8_t* buffer = (uint8_t*)malloc(out_size);
    if (!buffer) return nullptr;
    std::memcpy(buffer, decoded.data(), out_size);
    return buffer;
}

// --- Compression task ---
void compression_task(void* arg) {
    ESP_LOGI(TAG, "Starting compression...");

    // Paste your base64-encoded input here
    std::string encoded = "AAECAwQFBgcICQ==";

    size_t decoded_size = 0;
    uint8_t* decoded = base64_decode_binary(encoded, decoded_size);
    if (!decoded) {
        ESP_LOGE(TAG, "Base64 decode failed.");
        vTaskDelete(nullptr);
        return;
    }

    // Pad if necessary
    if (decoded_size < 80004) {
        uint8_t* padded = (uint8_t*)realloc(decoded, 80004);
        if (!padded) {
            ESP_LOGE(TAG, "Padding failed.");
            free(decoded);
            vTaskDelete(nullptr);
            return;
        }
        std::memset(padded + decoded_size, 0, 80004 - decoded_size);
        decoded = padded;
        decoded_size = 80004;
    }

    // Cast to int16_t safely
    size_t sample_count = decoded_size / 2;
    int16_t* input_data = (int16_t*)malloc(sample_count * sizeof(int16_t));
    if (!input_data) {
        ESP_LOGE(TAG, "Failed to allocate input buffer.");
        free(decoded);
        vTaskDelete(nullptr);
        return;
    }
    std::memcpy(input_data, decoded, sample_count * sizeof(int16_t));
    free(decoded);

    // Prepare compressed output
    size_t input_bytes = sample_count * sizeof(int16_t);
    size_t max_comp_size = ZSTD_compressBound(input_bytes);
    uint8_t* compressed = (uint8_t*)malloc(max_comp_size);
    if (!compressed) {
        ESP_LOGE(TAG, "Failed to allocate compression buffer.");
        free(input_data);
        vTaskDelete(nullptr);
        return;
    }

    // Compress with level 1 (fastest, lowest memory)
    size_t comp_size = ZSTD_compress(compressed, max_comp_size, input_data, input_bytes, 1);
    free(input_data);

    if (ZSTD_isError(comp_size)) {
        ESP_LOGE(TAG, "Compression failed: %s", ZSTD_getErrorName(comp_size));
        free(compressed);
        vTaskDelete(nullptr);
        return;
    }

    // Encode and print result
    std::string encoded_output = base64_encode_binary(compressed, comp_size);
    free(compressed);

    ESP_LOGI(TAG, "Compression successful: %d → %d bytes", (int)input_bytes, (int)comp_size);
    ESP_LOGI(TAG, "Base64 compressed output:\n%s", encoded_output.c_str());

    // Done
    vTaskDelete(nullptr);
}

extern "C" void app_main(void) {
    xTaskCreate(&compression_task, "compression_task", 16384, nullptr, 5, nullptr);
}
