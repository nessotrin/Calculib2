AUTO_CPPS = $(shell find ./src/ -type f -name "*.cpp")

AUTO_FOLDER = $(shell find ./src/ -type d)

AUTO_INCLUDE = $(shell find ./include/ -type d | sed 's/.\/include\//-I include\//')

CXX = g++
CXX_FLAGS = -g -std=c++11 -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow
BUILD_FLAGS = $(AUTO_INCLUDE) -D CALCULIB

# Final binary
BIN = Calculib2.a
# Put all auto generated stuff to this build dir.
BUILD_DIR = ./build
OUTPUT_DIR = ./lib


# List of all .cpp source files.
CPPS =  $(AUTO_CPPS)

# All .o files go to build dir.
OBJ = $(CPPS:%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)


# Default target named after the binary.
$(BIN) : $(OUTPUT_DIR)/$(BIN)

# Actual target of the binary - depends on all .o files.
$(OUTPUT_DIR)/$(BIN) : $(OBJ)
	@# Create build directories - same structure as sources.
	@mkdir -p $(@D)
	@# Just link all the object files.
	ar rcs $@ $^ -o 

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	@# The -MMD flags additionaly creates a .d file with
	@# the same name as the .o file.
	$(CXX) -c $< -o $@ $(CXX_FLAGS) -MMD $(BUILD_FLAGS)

.PHONY : clean
clean :
	# This should remove all generated files.
	-@rm $(BUILD_DIR)/$(BIN) $(OBJ) $(DEP)