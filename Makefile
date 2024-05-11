# Directories
SRC_DIR=src
BIN_DIR=bin
NH_DIR=neushoorn

# Flags
#CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
CFLAGS = -std=c11 -Wall -Wextra
CFLAGS += -I./neushoorn/
CFLAGS += -DDEBUG
CFLAGS += -ggdb
LDFLAGS = -ffast-math
LDFLAGS += -lm
LDFLAGS += -lSDL2
LDFLAGS += -lGL

# Files
NH_LIB=./neushoorn/build/libneushoorn.a

# Targets
# Library
$(NH_LIB): $(NH_DIR)
	cd ./neushoorn && make clean build
# Binary
$(BIN_DIR)/main: $(SRC_DIR)/main.c $(NH_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
# Directories
$(NH_DIR):
	git clone https://github.com/alexydens/neushoorn.git
$(BIN_DIR):
	mkdir -p $@

# Phony targets
.PHONY: clean build test

# Clean
clean:
	rm -rf $(BIN_DIR) $(NH_DIR)

# Build
build: $(BIN_DIR) $(BIN_DIR)/main

# Test
test: $(BIN_DIR) $(BIN_DIR)/main
	./$(BIN_DIR)/main
