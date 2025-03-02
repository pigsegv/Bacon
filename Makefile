MAKEFLAGS += --no-print-directory -s -j $(shell nproc)

ROOT_PATH := $(strip $(patsubst %/, %, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
export ROOT_PATH

PLATFORM := linux

CC := gcc
AS := fasm
LD := gcc

SRC := src
OBJ := obj
BIN := bin
INCLUDE := include

EXTERNAL_DIR := $(ROOT_PATH)/external
EXTERNAL_LIBS_DIR := $(ROOT_PATH)/external-libs

CFLAGS := -I$(ROOT_PATH)/$(SRC) -isystem $(EXTERNAL_DIR) -I$(ROOT_PATH)/$(INCLUDE)
LDFLAGS := -Wl,-rpath=$(ROOT_PATH)/$(BIN) -L$(ROOT_PATH)/$(BIN) -lm

GENERATE_ASM := 1

export PLATFORM CC LD SRC OBJ BIN INCLUDE EXTERNAL_DIR EXTERNAL_LIBS_DIR CFLAGS LDFLAGS GENERATE_ASM

OBJ_DIRS := $(patsubst $(SRC)/%, $(OBJ)/%, $(shell find $(SRC)/ -mindepth 1 -type d))
CREATE_DIR_COMMAND := ./dirs.sh

PROJECTS := 

.PHONY: all dirs clean external run

all: dirs $(PROJECTS)

# ---------------------- PROJECTS ----------------------



# ---------------------- UTILITY ----------------------

external:
	@mkdir -p $(EXTERNAL_LIBS_DIR)
	@$(MAKE) -C $(EXTERNAL_DIR)

dirs: 
	@mkdir -p $(BIN) 
	@mkdir -p $(OBJ)
	@$(CREATE_DIR_COMMAND) $(OBJ_DIRS)

clean:
	-@rm -rf $(OBJ)
	-@rm -rf $(BIN)
