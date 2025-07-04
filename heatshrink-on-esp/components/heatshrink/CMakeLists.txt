idf_component_register (
    SRCS
        "src/decompressor.cpp"
        "src/Delta_preprocessing.cpp"
        "src/heatshrink_decoder.c"
        "src/heatshrink_encoder.c"
        "src/heatshrink.c"
        "src/base64.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
)