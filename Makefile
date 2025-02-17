ENABLE_OPT_CACHE ?= 1
ENABLE_OMP ?= 1
ENABLE_INPLACE ?= 1

ifeq ($(ENABLE_OMP), 1)
    EXTRA_FLAGS += -DENABLE_OMP -fopenmp
endif

ifeq ($(ENABLE_OPT_CACHE), 1)
    EXTRA_FLAGS += -DENABLE_OPT_CACHE
endif

ifeq ($(ENABLE_INPLACE),1)
    EXTRA_FLAGS += -DENABLE_INPLACE
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
	cd ./src && make EXTRA_FLAGS="$(EXTRA_FLAGS)"
	$(FUNC) ./main.cpp -o ./main.exe ./src/*.obj ./src/Textures/*.obj $(FLAGS)

clean:
	cd ./src && make clean
	rm -f ./*.exe
	rm -f ./*.obj
