BIN        := demo
SRC_DIR    := .
VULKAN_DIR := ./vulkan
IMGUI_DIR  := ./imgui
IMGUI_IMPL := ./imgui_impl
GEOMETRY := ./geometry
BUILD_DIR  := build
LIB_DIR    := lib
INC_DIR    := include
MACRO := -DDEBUG
# MACRO := -DNDEBUG

CXX        := g++
CXXFLAGS   := -g -Wall -Wextra -Wpedantic -std=c++17
INC_PATH   := -I$(INC_DIR) -I$(INC_DIR)/vma -I$(INC_DIR)/exprtk
LIB_PATH   := -L$(LIB_DIR)
LIBS       := -lvulkan -lglfw3 -ldl -lpthread
LDFLAGS    := -Wl,-rpath,'$$ORIGIN/lib'

SRC := $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(VULKAN_DIR)/*.cpp) \
       $(wildcard $(IMGUI_DIR)/*.cpp) \
       $(wildcard $(IMGUI_IMPL)/*.cpp)\
       $(wildcard $(GEOMETRY)/*.cpp)\

OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LIB_PATH) $(LIBS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< $(MACRO) $(INC_PATH) -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN)

run: all
	./$(BIN)
