# Default compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# Source files
SRC = main.cpp

# Output directory and binary name
BUILD_DIR = build
TARGET = $(BUILD_DIR)/main

# Build the target (default)
$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: clean rebuild

# Rebuild target
rebuild: clean $(TARGET)
