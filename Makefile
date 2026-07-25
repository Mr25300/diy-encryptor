CXX := g++
CXXFLAGS := -std=c++17
TEST_CXXFLAGS := $(CXXFLAGS)

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

CXXFLAGS += -I$(SRC_DIR)
TEST_CXXFLAGS += -I$(SRC_DIR) -I$(TEST_DIR)

TARGET := $(BUILD_DIR)/diy-encryptor
TEST_TARGET := $(BUILD_DIR)/test

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
TEST_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS)) $(shell find $(TEST_DIR) -name "*.cpp")

OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TEST_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

all: $(TARGET)
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(TEST_CXXFLAGS) $(TEST_OBJS) -o $(TEST_TARGET)

$(BUILD_DIR)/$(TEST_DIR)/%.o : $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
