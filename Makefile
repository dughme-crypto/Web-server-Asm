CXX      = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra -march=native -Iinclude
SRCS     = src/main.cpp src/server.cpp src/router.cpp src/response.cpp
TARGET   = bareHTTP

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
