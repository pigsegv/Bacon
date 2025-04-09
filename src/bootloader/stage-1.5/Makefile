OBJ := $(OBJ)/stage-1.5

CFLAGS  += -Wall -Wextra -static -ggdb3 -std=gnu23 -O3 -m32 -nostdlib -ffreestanding -fno-pic -fno-pie -ffunction-sections -masm=intel -fno-stack-protector -mno-red-zone -Wdeprecated -Wshadow

ifeq ($(CC), clang)
	CFLAGS += -target i386
endif

LDFLAGS +=
ASMFLAGS := 

SRCS := $(shell find -type f -name "*.c" -printf "%P\n")
OBJS := $(patsubst %.c, $(OBJ)/%.o, $(SRCS))
DEPS := $(patsubst %.c, $(OBJ)/%.d, $(SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst %.c, $(OBJ)/%.s, $(SRCS))
endif

.PHONY: all

BINARIES := $(OBJ)/entry.bin $(OBJ)/protected.bin

ENTRY_BIN_SRCS := entry.s a20.s protected.s memory-map.s

all: $(STAGE15_TARGET) $(ASMS)

$(STAGE15_TARGET): $(BINARIES)
	@cat $^ > $@
	@echo built $@

$(OBJ)/entry.bin: $(ENTRY_BIN_SRCS)
	@$(AS) $(ASMFLAGS) $< $@ > /dev/null
	@echo built $@

$(OBJ)/protected.bin: $(OBJS) $(OBJ)/entry.bin
	@$(ROOT_PATH)/scripts/link-bootloader.sh \
		$(ROOT_PATH)/linker-scripts/bootloader-1.5.ld \
		$(OBJ)/entry.bin \
		$@ \
		"$(OBJS)"
	@echo built $@

-include $(DEPS)

$(OBJ)/%.o: %.c
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@
	@echo built $@

$(OBJ)/%.s: %.c
	@$(CC) $(CFLAGS) -S $< -o $@ -masm=intel > /dev/null
	@echo Generated $@

