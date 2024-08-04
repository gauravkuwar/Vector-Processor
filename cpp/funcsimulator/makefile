# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Iinclude -Wall -Wextra

# Source files
SRCS = src/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = funcsim

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
