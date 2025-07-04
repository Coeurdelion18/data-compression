#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"

#define OUT_BUF_SIZE 256
#define IN_BUF_SIZE 256

int main(int argc, char *argv[]) {
    int decompress = 0;
    int window_sz2 = 8;
    int lookahead_sz2 = 4;
    int verbose = 0; (void)verbose;

    static struct option long_options[] = {
        {"decompress", no_argument, 0, 'd'},
        {"window", required_argument, 0, 'w'},
        {"lookahead", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "dw:l:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd': decompress = 1; break;
            case 'w': window_sz2 = atoi(optarg); break;
            case 'l': lookahead_sz2 = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s [-d] [-w window_sz2] [-l lookahead_sz2]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    uint8_t in_buf[IN_BUF_SIZE];
    uint8_t out_buf[OUT_BUF_SIZE];

    if (decompress) {
        heatshrink_decoder dec;
        heatshrink_decoder *decoder = &dec;
        heatshrink_decoder_reset(decoder);

        size_t sunk, polled;
        HSD_sink_res sres;
        HSD_poll_res pres;
        HSD_finish_res fres;
        size_t count;

        while ((count = fread(in_buf, 1, IN_BUF_SIZE, stdin)) > 0) {
            size_t offset = 0;
            while (offset < count) {
                sres = heatshrink_decoder_sink(decoder, &in_buf[offset], count - offset, &sunk);
                offset += sunk;

                do {
                    pres = heatshrink_decoder_poll(decoder, out_buf, OUT_BUF_SIZE, &polled);
                    fwrite(out_buf, 1, polled, stdout);
                } while (pres == HSDR_POLL_MORE);
            }

            fres = heatshrink_decoder_finish(decoder);
            while (fres == HSDR_FINISH_MORE) {
                pres = heatshrink_decoder_poll(decoder, out_buf, OUT_BUF_SIZE, &polled);
                fwrite(out_buf, 1, polled, stdout);
                fres = heatshrink_decoder_finish(decoder);
            }
        }
    } else {
        heatshrink_encoder enc;
        heatshrink_encoder *encoder = &enc;
        heatshrink_encoder_reset(encoder);

        size_t sunk, polled;
        HSE_sink_res sres;
        HSE_poll_res pres;
        HSE_finish_res fres;
        size_t count;

        while ((count = fread(in_buf, 1, IN_BUF_SIZE, stdin)) > 0) {
            size_t offset = 0;
            while (offset < count) {
                sres = heatshrink_encoder_sink(encoder, &in_buf[offset], count - offset, &sunk);
                offset += sunk;

                do {
                    pres = heatshrink_encoder_poll(encoder, out_buf, OUT_BUF_SIZE, &polled);
                    fwrite(out_buf, 1, polled, stdout);
                } while (pres == HSER_POLL_MORE);
            }
        }

        fres = heatshrink_encoder_finish(encoder);
        while (fres == HSER_FINISH_MORE) {
            pres = heatshrink_encoder_poll(encoder, out_buf, OUT_BUF_SIZE, &polled);
            fwrite(out_buf, 1, polled, stdout);
            fres = heatshrink_encoder_finish(encoder);
        }
    }

    return 0;
}
