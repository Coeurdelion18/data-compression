idf_component_register(
    SRCS 
        # Common files
        "src/base64.cpp"
        "src/common/debug.c"
        "src/common/entropy_common.c"
        "src/common/error_private.c"
        "src/common/fse_decompress.c"
        "src/common/pool.c"
        "src/common/threading.c"
        "src/common/xxhash.c"
        "src/common/zstd_common.c"
        
        # Compress files
        "src/compress/zstd_preSplit.c"
        "src/compress/fse_compress.c"
        "src/compress/hist.c"
        "src/compress/huf_compress.c"
        "src/compress/zstd_compress.c"
        "src/compress/zstd_compress_literals.c"
        "src/compress/zstd_compress_sequences.c"
        "src/compress/zstd_compress_superblock.c"
        "src/compress/zstd_double_fast.c"
        "src/compress/zstd_fast.c"
        "src/compress/zstd_lazy.c"
        "src/compress/zstd_ldm.c"
        "src/compress/zstd_opt.c"
        "src/compress/zstdmt_compress.c"
        
        # Decompress files (you'll need these too)
        #"src/decompress/huf_decompress.c"
        #"src/decompress/zstd_ddict.c"
        #"src/decompress/zstd_decompress.c"
        #"src/decompress/zstd_decompress_block.c"
        
        # Dictionary builder files
        "src/dictBuilder/cover.c"
        "src/dictBuilder/divsufsort.c"
        "src/dictBuilder/fastcover.c"
        "src/dictBuilder/zdict.c"
        
        # Deprecated files (include if needed)
        "src/deprecated/zbuff_common.c"
        "src/deprecated/zbuff_compress.c"
        "src/deprecated/zbuff_decompress.c"
        "src/compress_delta_with_zstd.cpp"
        "src/Delta_preprocessing.cpp"
    INCLUDE_DIRS 
        "include"
        "src"
        "src/common"
        "src/compress"
        #"src/decompress"
        "src/dictBuilder"
        "src/deprecated"
        
    REQUIRES 
        # Add any ESP-IDF component dependencies here if needed
)

# Optional: Add compile definitions if needed
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    ZSTD_LEGACY_SUPPORT=0
    ZSTD_MULTITHREAD=0
)
