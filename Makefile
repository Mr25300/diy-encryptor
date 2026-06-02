CXX := g++
CXXFLAGS := -std=c++20 -Isrc

BUILD_DIR := build
TARGET := $(BUILD_DIR)/diy-encryptor

SRCS := src/main.cpp src/hash/sha256.cpp src/kdf/hmac.cpp src/kdf/pbkdf2.cpp
OBJS := $(BUILD_DIR)/main.o $(BUILD_DIR)/hash/sha256.o $(BUILD_DIR)/kdf/hmac.o $(BUILD_DIR)/kdf/pbkdf2.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/main.o: src/main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/hash/sha256.o: src/hash/sha256.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/kdf/hmac.o: src/kdf/hmac.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/kdf/pbkdf2.o: src/kdf/pbkdf2.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
