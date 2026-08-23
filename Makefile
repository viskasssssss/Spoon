# ==============================================================================
# project
# ==============================================================================

PROJECTNAME := Spoon

CXX := g++
CC  := gcc

BUILD_TYPE ?= release

# ==============================================================================
# directories
# ==============================================================================

BUILD_DIR := build/$(BUILD_TYPE)
OBJ_DIR   := $(BUILD_DIR)/obj
BIN       := $(BUILD_DIR)/$(PROJECTNAME).exe

THIRDPARTY := thirdparty
INCLUDE_DIR := $(THIRDPARTY)/include
LIB_DIR     := $(THIRDPARTY)/lib
ASSET_DIR   := $(THIRDPARTY)/copy_on_build

# ==============================================================================
# platform
# ==============================================================================

ifeq ($(OS),Windows_NT)

    PLATFORM_FLAGS := -DSP_PLATFORM_WINDOWS

    LDLIBS := \
        -lmingw32 \
        -lSDL2main \
        -lSDL2 \
        -lopengl32

else

    UNAME_S := $(shell uname -s)

    ifeq ($(UNAME_S),Linux)

        PLATFORM_FLAGS := -DSP_PLATFORM_LINUX

        LDLIBS := \
            -lSDL2 \
            -lGL

    else

        $(error Unsupported platform: $(UNAME_S))

    endif

endif

# ==============================================================================
# includes / libraries
# ==============================================================================

INCLUDE_DIRS := \
    -I$(INCLUDE_DIR) \
	-Ithirdparty/include/imgui/ \
	-Ithirdparty/include/SDL2/ \
    -Isrc \
    -Isrc/util

LDFLAGS := \
    -L$(LIB_DIR)

# ==============================================================================
# source files
# ==============================================================================

rwildcard = $(foreach d,$(wildcard $1/*), \
    $(call rwildcard,$d,$2) \
    $(filter $(subst *,%,$2),$d))

SRC := \
    $(call rwildcard,src,*.cpp) \
    $(call rwildcard,src,*.cc) \
    $(call rwildcard,src,*.c) \
    $(wildcard thirdparty/imgui/*.cpp) \
    $(wildcard thirdparty/glad/*.c)

SRC := $(sort $(SRC))

# ==============================================================================
# object files
# ==============================================================================

OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(filter %.cpp,$(SRC)))
OBJ += $(patsubst %.cc,$(OBJ_DIR)/%.o,$(filter %.cc,$(SRC)))
OBJ += $(patsubst %.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SRC)))

DEP := $(OBJ:.o=.d)

# ==============================================================================
# compiler flags
# ==============================================================================

COMMON_FLAGS := \
    -Wall \
    -Wextra \
    -MMD \
    -MP \
    $(PLATFORM_FLAGS) \
    $(INCLUDE_DIRS)

CXXFLAGS := $(COMMON_FLAGS)
CFLAGS   := $(COMMON_FLAGS)

ifeq ($(BUILD_TYPE),debug)

    CXXFLAGS += \
        -g \
        -O0 \
        -DSP_DEBUG

    CFLAGS += \
        -g \
        -O0 \
        -DSP_DEBUG

else ifeq ($(BUILD_TYPE),release)

    CXXFLAGS += \
        -O2 \
        -DSP_RELEASE \
        -DNDEBUG

    CFLAGS += \
        -O2 \
        -DSP_RELEASE \
        -DNDEBUG

else

    $(error BUILD_TYPE must be debug or release)

endif

# ==============================================================================
# targets
# ==============================================================================

.PHONY: all default debug release build clean rebuild assets

default: release

all: release

debug:
	@$(MAKE) BUILD_TYPE=debug build

release:
	@$(MAKE) BUILD_TYPE=release build

build: $(BIN)
	@echo "  ASSETS  $(ASSET_DIR)"
ifeq ($(OS),Windows_NT)
	@if exist "$(ASSET_DIR)" xcopy /y /d /e "$(ASSET_DIR)\*" "$(BUILD_DIR)\" >nul
else
	@cp -r "$(ASSET_DIR)/." "$(BUILD_DIR)/" 2>/dev/null || true
endif
	@echo ""
	@echo "  BUILD   $(BUILD_TYPE)"
	@echo "  OUTPUT  $(BIN)"

# ==============================================================================
# linking
# ==============================================================================

$(BIN): $(OBJ)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@echo "  LINK    $@"
	@$(CXX) $(OBJ) $(LDFLAGS) -o $@ $(LDLIBS)
	@echo "  DONE    $@"

# ==============================================================================
# cxx compilation
# ==============================================================================

$(OBJ_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(subst /,\,$(dir $@))"
	@echo "  CXX     $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cc
	@if not exist "$(dir $@)" mkdir "$(subst /,\,$(dir $@))"
	@echo "  CXX     $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ==============================================================================
# c compilation
# ==============================================================================

$(OBJ_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(subst /,\,$(dir $@))"
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
# Cleaning
# ==============================================================================

clean:
	@echo "  CLEAN   build/"
ifeq ($(OS),Windows_NT)
	@if exist "build" rmdir /s /q "build"
else
	@rm -rf build
endif

# ==============================================================================
# rebuild
# ==============================================================================

rebuild: clean build

# ==============================================================================
# dependencies
# ==============================================================================

-include $(DEP)