# Default values for optimization flags
ENABLE_OMP ?= 0
ENABLE_BVH ?= 0
ENABLE_SIMD ?= 0
ENABLE_MPOOL ?= 0
ENABLE_PRAY ?= 0

# Add flags based on enabled optimizations
ifeq ($(ENABLE_OMP), 1)
    EXTRA_FLAGS += -DENABLE_OMP -fopenmp
endif

ifeq ($(ENABLE_BVH), 1)
    EXTRA_FLAGS += -DENABLE_BVH
endif

ifeq ($(ENABLE_SIMD), 1)
    EXTRA_FLAGS += -DENABLE_SIMD -mavx2
endif

ifeq ($(ENABLE_MPOOL), 1)
    EXTRA_FLAGS += -DENABLE_MPOOL
endif


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

message:
	@echo "CONFIG: $(CONFIG)"
	@echo "ENABLE_OMP: $(ENABLE_OMP)"
	@echo "ENABLE_BVH: $(ENABLE_BVH)"
	@echo "ENABLE_SIMD: $(ENABLE_SIMD)"
	@echo "ENABLE_MPOOL: $(ENABLE_MPOOL)"
	@echo "EXTRA_FLAGS: $(EXTRA_FLAGS)"
