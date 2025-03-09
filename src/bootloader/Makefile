OBJ := $(OBJ)/src/bootloader

CFLAGS  += -Wall -Wextra -ggdb3 -std=gnu23 -fPIC -O0
LDFLAGS +=
ASMFLAGS := 


DEPS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.d, $(SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.s, $(SRCS))
endif

all: $(BOOTSECTOR_TARGET)

$(BOOTSECTOR_TARGET): bootsector.s
	@echo
	@echo building $(TARGET)
	$(AS) $(ASMFLAGS) $^ $@
	@echo built $(TARGET)

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

