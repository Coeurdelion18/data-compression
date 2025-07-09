// extern "C" {
//     #include "heatshrink_encoder.h"
//     #include "heatshrink_decoder.h"
//     #include "esp_spiffs.h"
//     #include "esp_vfs.h"
//     #include "esp_log.h"
// }

// #include "base64.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #include <cstdint>
// #include <cstring>
// #include <cstdlib>
// #include <string>
// #include <cmath>

// static const char* TAG = "HEATSHRINK_APP";

// #define INPUT_SIZE 106496
// #define SINK_CHUNK_SIZE 64
// #define OUT_BUFFER_SIZE 1024

// std::string base64_encode_binary(const uint8_t* data, size_t len) {
//     return base64_encode(reinterpret_cast<const unsigned char*>(data), len, false);
// }

// uint8_t* base64_decode_binary(const std::string& encoded, size_t& out_size) {
//     std::string decoded = base64_decode(encoded, true);
//     out_size = decoded.size();
//     if (out_size == 0) return nullptr;

//     uint8_t* buffer = (uint8_t*)malloc(out_size);
//     if (!buffer) return nullptr;
//     std::memcpy(buffer, decoded.data(), out_size);
//     return buffer;
// }

// void compression_task(void* arg) {
//     ESP_LOGI(TAG, "Mounting SPIFFS...");

//     esp_vfs_spiffs_conf_t conf = {
//         .base_path = "/spiffs",
//         .partition_label = nullptr,
//         .max_files = 5,
//         .format_if_mount_failed = true
//     };

//     esp_err_t ret = esp_vfs_spiffs_register(&conf);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "SPIFFS mount failed: %s", esp_err_to_name(ret));
//         vTaskDelete(nullptr);
//         return;
//     }

//     FILE* f = fopen("/spiffs/input.txt", "rb");
//     if (!f) {
//         ESP_LOGE(TAG, "Failed to open /spiffs/input.txt");
//         esp_vfs_spiffs_unregister(nullptr);
//         vTaskDelete(nullptr);
//         return;
//     }

//     fseek(f, 0, SEEK_END);
//     long file_size = ftell(f);
//     rewind(f);

//     char* buffer = (char*)malloc(file_size + 1);
//     fread(buffer, 1, file_size, f);
//     buffer[file_size] = '\0';
//     fclose(f);

//     std::string encoded(buffer);
//     free(buffer);

//     ESP_LOGI(TAG, "Read %ld bytes from file.", file_size);

//     size_t decoded_size = 0;
//     uint8_t* decoded = base64_decode_binary(encoded, decoded_size);
//     if (!decoded) {
//         ESP_LOGE(TAG, "Base64 decode failed.");
//         esp_vfs_spiffs_unregister(nullptr);
//         vTaskDelete(nullptr);
//         return;
//     }

//     // Pad to fixed size if needed
//    /* if (decoded_size < INPUT_SIZE) {
//         uint8_t* padded = (uint8_t*)realloc(decoded, INPUT_SIZE);
//         if (!padded) {
//             ESP_LOGE(TAG, "Memory padding failed.");
//             free(decoded);
//             esp_vfs_spiffs_unregister(nullptr);
//             vTaskDelete(nullptr);
//             return;
//         }
//         std::memset(padded + decoded_size, 0, INPUT_SIZE - decoded_size);
//         decoded = padded;
//         decoded_size = INPUT_SIZE;
//     }
//     */
//     // Heatshrink encoding
//     heatshrink_encoder encoder;
//     heatshrink_encoder_reset(&encoder);

//     uint8_t out_buffer[OUT_BUFFER_SIZE];
//     std::string compressed;

//     size_t cursor = 0;
//     size_t sunk = 0;
//     size_t polled = 0;

//     while (cursor < decoded_size) {
//         size_t chunk = std::min((size_t)SINK_CHUNK_SIZE, decoded_size - cursor);
//         heatshrink_encoder_sink(&encoder, &decoded[cursor], chunk, &sunk);
//         cursor += sunk;

//         HSE_poll_res pres;
//         do {
//             pres = heatshrink_encoder_poll(&encoder, out_buffer, OUT_BUFFER_SIZE, &polled);
//             compressed.append(reinterpret_cast<char*>(out_buffer), polled);
//         } while (pres == HSER_POLL_MORE);
//     }

//     HSE_finish_res fres;
//     do {
//         fres = heatshrink_encoder_finish(&encoder);
//         HSE_poll_res pres;
//         do {
//             pres = heatshrink_encoder_poll(&encoder, out_buffer, OUT_BUFFER_SIZE, &polled);
//             compressed.append(reinterpret_cast<char*>(out_buffer), polled);
//         } while (pres == HSER_POLL_MORE);
//     } while (fres == HSER_FINISH_MORE);

//     free(decoded);

//     std::string encoded_output = base64_encode_binary(
//         reinterpret_cast<const uint8_t*>(compressed.data()),
//         compressed.size()
//     );

//     ESP_LOGI(TAG, "Compression successful: %d → %d bytes", (int)decoded_size, (int)compressed.size());
//     ESP_LOGI(TAG, "Base64 compressed output:\n%s", encoded_output.c_str());

//     esp_vfs_spiffs_unregister(nullptr);
//     vTaskDelete(nullptr);
// }

// extern "C" void app_main(void) {
//     xTaskCreate(&compression_task, "compression_task", 16384, nullptr, 5, nullptr);
// }

extern "C" {
    #include "heatshrink_encoder.h"
    #include "esp_spiffs.h"
    #include "esp_vfs.h"
    #include "esp_log.h"
}

#include "base64.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>

static const char* TAG = "HEATSHRINK_APP";

#define READ_CHUNK_SIZE 512
#define SINK_CHUNK_SIZE 64
#define POLL_CHUNK_SIZE 128

std::string base64_encode_binary(const uint8_t* data, size_t len) {
    return base64_encode(reinterpret_cast<const unsigned char*>(data), len, false);
}

void compression_task(void* arg) {
    ESP_LOGI(TAG, "Mounting SPIFFS...");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = nullptr,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS mount failed: %s", esp_err_to_name(ret));
        vTaskDelete(nullptr);
        return;
    }
    FILE* f = fopen("/spiffs/input.raw", "rb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open /spiffs/input.raw");
        esp_vfs_spiffs_unregister(nullptr);
        vTaskDelete(nullptr);
        return;
    }

    heatshrink_encoder encoder;
    heatshrink_encoder_reset(&encoder);

    uint8_t read_buf[READ_CHUNK_SIZE];
    uint8_t out_buf[POLL_CHUNK_SIZE];
    std::string compressed;

    size_t total_read = 0;

    while (!feof(f)) {
        size_t read = fread(read_buf, 1, READ_CHUNK_SIZE, f);
        if (read == 0) break;
        total_read += read;

        size_t cursor = 0;
        while (cursor < read) {
            size_t chunk = std::min((size_t)SINK_CHUNK_SIZE, read - cursor);
            size_t sunk = 0;
            heatshrink_encoder_sink(&encoder, &read_buf[cursor], chunk, &sunk);
            cursor += sunk;

            HSE_poll_res pres;
            size_t polled;
            do {
                pres = heatshrink_encoder_poll(&encoder, out_buf, POLL_CHUNK_SIZE, &polled);
                compressed.append(reinterpret_cast<char*>(out_buf), polled);
            } while (pres == HSER_POLL_MORE);
        }
    }

    fclose(f);

    // Finish and flush remaining output
    HSE_finish_res fres;
    do {
        fres = heatshrink_encoder_finish(&encoder);
        HSE_poll_res pres;
        size_t polled;
        do {
            pres = heatshrink_encoder_poll(&encoder, out_buf, POLL_CHUNK_SIZE, &polled);
            compressed.append(reinterpret_cast<char*>(out_buf), polled);
        } while (pres == HSER_POLL_MORE);
    } while (fres == HSER_FINISH_MORE);

    std::string encoded_output = base64_encode_binary(
        reinterpret_cast<const uint8_t*>(compressed.data()), compressed.size());

    ESP_LOGI(TAG, "Compression successful: %d → %d bytes", (int)total_read, (int)compressed.size());
    ESP_LOGI(TAG, "Base64 compressed output:\n%s", encoded_output.c_str());

    esp_vfs_spiffs_unregister(nullptr);
    vTaskDelete(nullptr);
}

extern "C" void app_main(void) {
    xTaskCreate(&compression_task, "compression_task", 16384, nullptr, 5, nullptr);
}

