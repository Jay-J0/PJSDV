CXX = g++
CXXFLAGS = -ggdb -g3

TARGET = client
SRCS = $(wildcard *.cpp)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
