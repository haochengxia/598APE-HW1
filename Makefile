CONFIG ?= config0.mk # default config, no optimization

# load config
include config/$(CONFIG)

message:
	@echo "CONFIG: $(CONFIG)"
	@echo "ENABLE_OMP: $(ENABLE_OMP)"
	@echo "ENABLE_BVH: $(ENABLE_BVH)"
	@echo "ENABLE_SIMD: $(ENABLE_SIMD)"
	@echo "ENABLE_MPOOL: $(ENABLE_MPOOL)"
	@echo "ENABLE_PRAY: $(ENABLE_PRAY)"
	@echo "EXTRA_FLAGS: $(EXTRA_FLAGS)"

FUNC := g++
copt := -c 
OBJ_DIR := ./bin/
FLAGS := -O3 -lm -g -Werror $(EXTRA_FLAGS)

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix $(OBJ_DIR),$(notdir $(CPP_FILES:.cpp=.obj)))

TEXTURE_CPP_FILES := $(wildcard src/Textures/*.cpp)
TEXTURE_OBJ_FILES := $(addprefix $(OBJ_DIR)Textures/,$(notdir $(TEXTURE_CPP_FILES:.cpp=.obj)))

all:
	cd ./src && make
	$(FUNC) ./main.cpp -o ./main.exe ./src/*.obj ./src/Textures/*.obj $(FLAGS)

clean:
	cd ./src && make clean
	rm -f ./*.exe
	rm -f ./*.obj
