# Makefile for Mula Wee Game - Original Version
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
LIBS = -lncurses

# Source files
ORIGINAL_SRC = mainGame.cpp

# Executables
ORIGINAL_TARGET = mulavee_original

# Default target
all: $(ORIGINAL_TARGET)

# Original game
$(ORIGINAL_TARGET): $(ORIGINAL_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# Clean build artifacts
clean:
	rm -f *.o $(ORIGINAL_TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(ORIGINAL_TARGET)

# Run the original game
run: $(ORIGINAL_TARGET)
	./$(ORIGINAL_TARGET)

# Build and run optimized version (v2)
v2:
	cd v2 && $(MAKE)

run-v2:
	cd v2 && $(MAKE) run

# Check for memory leaks (requires valgrind)
memcheck: $(ORIGINAL_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(ORIGINAL_TARGET)

# Format code (requires clang-format)
format:
	clang-format -i *.cpp *.hpp

.PHONY: all clean debug run v2 run-v2 memcheck format
