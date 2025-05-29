# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

# Sources and objects
SRC = main.cpp Delta_preprocessing.cpp gzip_compress.cpp
OBJ = $(SRC:.cpp=.o)

# Target binary
TARGET = delta_gzip

# Build all
all: $(TARGET)

# Link the binary
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) -lz

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)
