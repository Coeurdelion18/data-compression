extern "C" {
    #include "heatshrink_encoder.h"
    #include "heatshrink_decoder.h"
}

#include "base64.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cmath>

static const char* TAG = "HEATSHRINK_APP";

#define INPUT_SIZE 80004
#define SINK_CHUNK_SIZE 64
#define OUT_BUFFER_SIZE 1024

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

void compression_task(void* arg) {
    ESP_LOGI(TAG, "Starting Heatshrink compression...");

    // Replace with your actual base64-encoded int16_t binary input
    std::string encoded = "AAECAwQFBgcICQ==";

    size_t decoded_size = 0;
    uint8_t* decoded = base64_decode_binary(encoded, decoded_size);
    if (!decoded) {
        ESP_LOGE(TAG, "Base64 decode failed.");
        vTaskDelete(nullptr);
        return;
    }

    // Pad if needed
    if (decoded_size < INPUT_SIZE) {
        uint8_t* padded = (uint8_t*)realloc(decoded, INPUT_SIZE);
        if (!padded) {
            ESP_LOGE(TAG, "Padding failed.");
            free(decoded);
            vTaskDelete(nullptr);
            return;
        }
        std::memset(padded + decoded_size, 0, INPUT_SIZE - decoded_size);
        decoded = padded;
        decoded_size = INPUT_SIZE;
    }

    // Prepare encoder
    heatshrink_encoder encoder;
    heatshrink_encoder_reset(&encoder);

    uint8_t out_buffer[OUT_BUFFER_SIZE];
    std::string compressed;

    size_t cursor = 0;
    size_t sunk = 0;
    size_t polled = 0;

    while (cursor < decoded_size) {
        size_t chunk = std::min(static_cast<size_t>(SINK_CHUNK_SIZE), decoded_size - cursor);
        heatshrink_encoder_sink(&encoder, &decoded[cursor], chunk, &sunk);
        cursor += sunk;

        HSE_poll_res pres;
        do {
            pres = heatshrink_encoder_poll(&encoder, out_buffer, OUT_BUFFER_SIZE, &polled);
            compressed.append(reinterpret_cast<char*>(out_buffer), polled);
        } while (pres == HSER_POLL_MORE);
    }

    // Finish stream
    HSE_finish_res fres;
    do {
        fres = heatshrink_encoder_finish(&encoder);
        HSE_poll_res pres;
        do {
            pres = heatshrink_encoder_poll(&encoder, out_buffer, OUT_BUFFER_SIZE, &polled);
            compressed.append(reinterpret_cast<char*>(out_buffer), polled);
        } while (pres == HSER_POLL_MORE);
    } while (fres == HSER_FINISH_MORE);

    free(decoded);

    std::string encoded_output = base64_encode_binary(
        reinterpret_cast<const uint8_t*>(compressed.data()),
        compressed.size()
    );

    ESP_LOGI(TAG, "Compression successful: %d → %d bytes", (int)decoded_size, (int)compressed.size());
    ESP_LOGI(TAG, "Base64 compressed output:\n%s", encoded_output.c_str());

    vTaskDelete(nullptr);
}

extern "C" void app_main(void) {
    xTaskCreate(&compression_task, "compression_task", 16384, nullptr, 5, nullptr);
}
