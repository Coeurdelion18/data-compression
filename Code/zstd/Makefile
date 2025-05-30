CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lzstd

SRCS = main.cpp \
       compress_delta_with_zstd.cpp \
       decompress_delta_with_zstd.cpp \
       Delta_preprocessing.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = compressor

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
