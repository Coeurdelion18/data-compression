# Compiler
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall

# Output binary name
TARGET := zstd_main

# Source files
SRCS := \
    main.cpp \
    base64.cpp \
    compress_delta_with_zstd.cpp \
    decompress_delta_with_zstd.cpp \
    Delta_preprocessing.cpp

# Headers (not strictly required but good for dependency tools)
HDRS := \
    base64.h \
    zstd_compress.h \
    Delta_preprocessing.h

# Object files
OBJS := $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Link the final binary
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lzstd

# Compile .cpp to .o
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)
