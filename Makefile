CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -MMD -MP
TEST_CXXFLAGS := $(CXXFLAGS)

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

CXXFLAGS += -I$(SRC_DIR)
TEST_CXXFLAGS += -I$(SRC_DIR) -I$(TEST_DIR)

TARGET := $(BUILD_DIR)/diy-encryptor
TEST_TARGET := $(BUILD_DIR)/test

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp")

OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))
LIB_OBJS := $(filter-out $(BUILD_DIR)/$(SRC_DIR)/main.o, $(OBJS))
TEST_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

all: $(TARGET)
test: $(TEST_TARGET)
	./$(TEST_TARGET)

-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	# -MT $@ tells compiler to use exact path of object file when assigning header dependencies
	$(CXX) $(CXXFLAGS) -MT $@ -c $< -o $@

$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS)
	$(CXX) $(TEST_CXXFLAGS) $(LIB_OBJS) $(TEST_OBJS) -o $(TEST_TARGET)

$(BUILD_DIR)/$(TEST_DIR)/%.o : $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_CXXFLAGS) -MT $@ -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
