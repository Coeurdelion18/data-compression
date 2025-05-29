# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Wall -O2
CFLAGS = -Wall -O2

# Sources
HEATSHRINK_SRC = heatshrink_encoder.c heatshrink_decoder.c
HEATSHRINK_OBJ = heatshrink_encoder.o heatshrink_decoder.o

DELTA_SRC = Delta_preprocessing.cpp
DELTA_OBJ = Delta_preprocessing.o

COMPRESSOR_SRC = heatshrink_implementation.cpp
COMPRESSOR_OBJ = heatshrink_implementation.o

DECOMPRESSOR_SRC = decompressor.cpp
DECOMPRESSOR_OBJ = decompressor.o

# Targets
all: compressor decompressor

compressor: $(COMPRESSOR_OBJ) $(HEATSHRINK_OBJ) $(DELTA_OBJ)
	$(CXX) $(CXXFLAGS) -o compressor $(COMPRESSOR_OBJ) $(HEATSHRINK_OBJ) $(DELTA_OBJ)

decompressor: $(DECOMPRESSOR_OBJ) $(HEATSHRINK_OBJ)
	$(CXX) $(CXXFLAGS) -o decompressor $(DECOMPRESSOR_OBJ) $(HEATSHRINK_OBJ)

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f *.o compressor decompressor
