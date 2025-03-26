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

CFLAGS := -I$(SRC) -I$(INCLUDE) -I$(ROOT_PATH) -isystem $(EXTERNAL_DIR)
ASMFLAGS :=
LDFLAGS := -Wl,-rpath=$(BIN) -L$(BIN)

GENERATE_ASM := 1

export PLATFORM CC LD SRC OBJ BIN INCLUDE EXTERNAL_DIR EXTERNAL_LIBS_DIR CFLAGS LDFLAGS GENERATE_ASM ASMFLAGS AS

OBJ_DIRS += $(call get_dirs, $(SRC))
OBJ_DIRS += $(call get_dirs, $(ROOT_PATH)/tools)

OBJ_DIRS := $(call relative_foreach, $(patsubst %, $(OBJ)/%, $(call relative_foreach, $(OBJ_DIRS))))

CREATE_DIR_COMMAND := ./scripts/dirs.sh

PROJECTS := tools bootloader format_disk fs

.PHONY: all dirs clean external run qemu $(PROJECTS) test


all: $(PROJECTS)

# This rule runs first due to the include
create_dirs: dirs
-include create_dirs

# ---------------------- PROJECTS ----------------------

tools:
	@$(MAKE) -C $(ROOT_PATH)/tools

bootloader:
	@$(MAKE) -C $(SRC)/bootloader BOOTSECTOR_TARGET=$(BIN)/bootsector.bin STAGE15_TARGET=$(BIN)/stage-1.5.bin

fs: 
	@$(MAKE) -C $(SRC)/fs

format_disk: tools bootloader
	./scripts/create_disk.sh no-format $(BIN)/disk.iso $(BIN)/bootsector.bin $(BIN)/stage-1.5.bin
	@echo formatted $(BIN)/disk.iso


# ---------------------- UTILITY ----------------------

create_disk: tools
	./scripts/create_disk.sh format $(BIN)/disk.iso $(BIN)/bootsector.bin $(BIN)/stage-1.5.bin
	@echo created $(BIN)/disk.iso

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
	@qemu-system-x86_64 -drive file=bin/disk.iso,format=raw -m 1G -vga qxl -serial file:./logs/log.txt -cpu host -enable-kvm
