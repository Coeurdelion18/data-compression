# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Wall -O2
CFLAGS = -Wall -O2

# Source files
CXX_SOURCES = main.cpp compress_decompress.cpp
C_SOURCES = lz4.c

# Object files
OBJECTS = $(CXX_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o)

# Output binary
TARGET = compress_app

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
