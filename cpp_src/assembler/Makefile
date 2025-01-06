# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./include

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SRC_FILES = main.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)
EXEC = assembler

# Targets
all: $(EXEC) clean

$(EXEC): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $(EXEC)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES)


run: $(EXEC) clean
	./$(EXEC) ../../dot_product/Code.asm
