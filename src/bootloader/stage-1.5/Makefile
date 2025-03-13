OBJ := $(OBJ)/stage-1.5

CFLAGS  += -Wall -Wextra -ggdb3 -std=gnu23 -fPIC -O0
LDFLAGS +=
ASMFLAGS := 


DEPS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.d, $(SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.s, $(SRCS))
endif

.PHONY: all

BINARIES := $(OBJ)/entry.bin

ENTRY_BIN_SRCS := entry.s a20.s

all: $(STAGE15_TARGET)


$(STAGE15_TARGET): $(OBJ)/entry.bin
	@echo
	@echo building $@
	@cat $(BINARIES) > $@
	@echo built $@

$(OBJ)/entry.bin: $(ENTRY_BIN_SRCS)
	@echo
	@echo building $@
	@$(AS) $(ASMFLAGS) $< $@
	@echo built $@



-include $(DEPS)

$(PROJ_OBJ)/%.o: $(PROJ_SRC)/%.c
	@echo building $@
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@
	@echo built $@
	@echo

$(PROJ_OBJ)/%.s: $(PROJ_SRC)/%.c
	@echo Generating assembly $@
	@$(CC) $(CFLAGS) -S -masm=intel $< -o $@
	@echo Generated $@
	@echo

