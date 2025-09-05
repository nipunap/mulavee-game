# Makefile for Mula Wee Game - Optimized Version 2.0
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
LIBS = -lncurses

# Source files
OPTIMIZED_SRC = optimized_game.cpp main_optimized.cpp

# Object files
OPTIMIZED_OBJ = $(OPTIMIZED_SRC:.cpp=.o)

# Executables
OPTIMIZED_TARGET = mulavee_optimized

# Default target
all: $(OPTIMIZED_TARGET)

# Optimized game
$(OPTIMIZED_TARGET): $(OPTIMIZED_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Object file compilation
%.o: %.cpp optimized_game.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f *.o $(OPTIMIZED_TARGET)

# Install (copy to /usr/local/bin)
install: $(OPTIMIZED_TARGET)
	cp $(OPTIMIZED_TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(OPTIMIZED_TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(OPTIMIZED_TARGET)

# Run the optimized game
run: $(OPTIMIZED_TARGET)
	./$(OPTIMIZED_TARGET)

# Check for memory leaks (requires valgrind)
memcheck: $(OPTIMIZED_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(OPTIMIZED_TARGET)

# Format code (requires clang-format)
format:
	clang-format -i *.cpp *.hpp

.PHONY: all clean install uninstall debug run memcheck format
