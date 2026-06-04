CXX := g++
CXXFLAGS := -std=c++20 -Isrc

BUILD_DIR := build
TARGET := $(BUILD_DIR)/diy-encryptor

SRCS := src/main.cpp src/hash/sha256.cpp src/prf/hmac.cpp src/kdf/pbkdf2.cpp src/math/gf256.cpp \
		src/math/word.cpp src/math/matrix.cpp
OBJS := $(BUILD_DIR)/main.o $(BUILD_DIR)/hash/sha256.o $(BUILD_DIR)/prf/hmac.o \
		$(BUILD_DIR)/kdf/pbkdf2.o $(BUILD_DIR)/math/gf256.o $(BUILD_DIR)/math/word.o \
		$(BUILD_DIR)/math/matrix.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/main.o: src/main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/hash/sha256.o: src/hash/sha256.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/prf/hmac.o: src/prf/hmac.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/kdf/pbkdf2.o: src/kdf/pbkdf2.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/math/gf256.o: src/math/gf256.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/math/word.o: src/math/word.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/math/matrix.o: src/math/matrix.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
