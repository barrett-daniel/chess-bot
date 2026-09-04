CXX := clang++
SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests
TARGET := run
TEST_BIN := test

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
SDL_SRCS := $(SRC_DIR)/main.cpp \
            $(SRC_DIR)/delegates/renderer.cpp \
            $(SRC_DIR)/delegates/input_manager.cpp
NONSDL_SRCS := $(filter-out $(SDL_SRCS), $(SRCS))

SDL_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SDL_SRCS))
NONSDL_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(NONSDL_SRCS))
OBJS := $(SDL_OBJS) $(NONSDL_OBJS)

DEBUG_FLAGS := -O0 -g -DDEBUG
RELEASE_FLAGS := -O3

BUILD_MODE ?= release

BASE_CXXFLAGS = -std=c++20 -MMD -MP $(if $(filter debug,$(BUILD_MODE)),$(DEBUG_FLAGS),$(RELEASE_FLAGS))

SDL_CFLAGS := $(shell pkg-config --cflags sdl3 sdl3-image)
SDL_LIBS := $(shell pkg-config --libs sdl3 sdl3-image)
TEST_FLAGS := -I/opt/homebrew/include

TEST_PROJECT_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS))
TEST_MAIN := $(TEST_DIR)/test_implementation.cpp

ifdef TEST
  SELECTED_TESTS := $(TEST_DIR)/$(TEST) $(TEST_MAIN)
else
  SELECTED_TESTS := $(wildcard $(TEST_DIR)/*.cpp)
endif

all: build

build: BUILD_MODE := release
build: $(TARGET)

debug: BUILD_MODE := debug
debug: $(TARGET)

$(TARGET): $(OBJS)
	@echo "[linking] $@"
	@$(CXX) $^ -o $@ $(SDL_LIBS)

$(SDL_OBJS): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "[compiling with SDL] $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(BASE_CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

$(NONSDL_OBJS): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "[compiling] $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(BASE_CXXFLAGS) -c $< -o $@

test:
	@echo "[linking test binary] $(TEST_BIN)"
	@$(CXX) $(BASE_CXXFLAGS) $(TEST_PROJECT_SRCS) $(SELECTED_TESTS) -o $(TEST_BIN) $(TEST_FLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_BIN)

-include $(OBJS:.o=.d)

.PHONY: all clean test debug build
