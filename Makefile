# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Flex command
FLEX = flex

# Source files
LEX_SRC = scanner.lex
CPP_SRC = hw1.cpp

# Generated files
LEX_GEN = lex.yy.c
TARGET = hw1.out

# Default target
all: $(TARGET)

# Rule to generate the lexer file
$(LEX_GEN): $(LEX_SRC)
	$(FLEX) $(LEX_SRC)

# Rule to compile the C++ files and the generated lexer
$(TARGET): $(LEX_GEN) $(CPP_SRC) tokens.hpp
	$(CXX) $(CXXFLAGS) $(LEX_GEN) $(CPP_SRC) -o $(TARGET)

# Clean up generated files
clean:
	rm -f $(LEX_GEN) $(TARGET)

# .PHONY targets to avoid conflicts with file names
.PHONY: all clean
