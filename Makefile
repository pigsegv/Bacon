MAKEFLAGS += --no-print-directory -s -j $(shell nproc)

ROOT_PATH := $(strip $(patsubst %/, %, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
export ROOT_PATH

PLATFORM := linux

define get_dirs
	$(shell find $(1) -type d)
endef

define relative_foreach
	$(foreach _dir, $(1), $(shell realpath -s --relative-to="$(ROOT_PATH)" $(_dir)))
endef

CC := gcc
AS := fasm
LD := gcc

SRC := $(ROOT_PATH)/src
OBJ := $(ROOT_PATH)/obj
BIN := $(ROOT_PATH)/bin
INCLUDE := $(ROOT_PATH)/include

EXTERNAL_DIR := $(ROOT_PATH)/external
EXTERNAL_LIBS_DIR := $(ROOT_PATH)/external-libs

CFLAGS := -I$(SRC) -I$(INCLUDE) -I$(ROOT_PATH)
ASMFLAGS :=
LDFLAGS := -Wl,-rpath=$(BIN) -L$(BIN)

GENERATE_ASM := 1

export PLATFORM CC LD SRC OBJ BIN INCLUDE EXTERNAL_DIR EXTERNAL_LIBS_DIR CFLAGS LDFLAGS GENERATE_ASM ASMFLAGS AS

OBJ_DIRS += $(call get_dirs, $(SRC))
OBJ_DIRS += $(call get_dirs, $(ROOT_PATH)/tools)

OBJ_DIRS := $(call relative_foreach, $(patsubst %, $(OBJ)/%, $(call relative_foreach, $(OBJ_DIRS))))

CREATE_DIR_COMMAND := ./dirs.sh

PROJECTS := tools bootloader create_disk

.PHONY: all dirs clean external run qemu $(PROJECTS)


all: $(PROJECTS)

# This rule runs first due to the include
create_dirs: dirs
-include create_dirs

# ---------------------- PROJECTS ----------------------

tools:
	@$(MAKE) -C $(ROOT_PATH)/tools

bootloader:
	@$(MAKE) -C $(ROOT_PATH)/src/bootloader BOOTSECTOR_TARGET=$(BIN)/bootsector.bin STAGE15_TARGET=$(BIN)/stage-1.5.bin

create_disk: tools bootloader
	./create_disk.sh $(BIN)/disk.iso $(BIN)/fs-image.img $(BIN)/bootsector.bin $(BIN)/stage-1.5.bin

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

qemu:
	@qemu-system-x86_64 -drive file=bin/disk.iso,format=raw -m 1G -bios ~/seabios/out/bios.bin

