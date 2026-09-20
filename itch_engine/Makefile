# Compiler settings
CXX      := g++
CXXFLAGS := -std=c++20 -O3 -march=native -Wall -Wextra -Wpedantic -Werror -Iinclude

# Directories
SRC_DIR  := src
APP_DIR  := apps
OBJ_DIR  := obj
BIN_DIR  := bin

# Targets and Files
TARGET   := $(BIN_DIR)/itch_engine
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MAIN_SRC := $(APP_DIR)/main.cpp
MAIN_OBJ := $(OBJ_DIR)/main.o

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "========================================================="
	@echo " Successfully compiled low-latency ITCH engine binary!"
	@echo " Execute using: ./$(TARGET) <path_to_binary_data_file>"
	@echo "========================================================="

# Compile source files into objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile main entry point separate
$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts safely
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean completed."

.PHONY: all clean
