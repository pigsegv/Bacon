OBJ := $(OBJ)/src/fs

CFLAGS += -Wall -Wextra -ggdb3 -std=gnu23 -fPIC -O0 -flto -fsanitize=address


LDFLAGS += -flto -fsanitize=address
ASMFLAGS := 

SRCS := $(shell find -type f -name "*.c" -printf "%P\n")
OBJS := $(patsubst %.c, $(OBJ)/%.o, $(SRCS))
DEPS := $(patsubst %.c, $(OBJ)/%.d, $(SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst %.c, $(OBJ)/%.s, $(SRCS))
endif

BINARIES := $(BIN)/fs-test

.PHONY: all
all: $(BINARIES) $(ASMS)

$(BIN)/fs-test: $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo built $@

-include $(DEPS)

$(OBJ)/%.o: %.c
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@
	@echo built $@

$(OBJ)/%.s: %.c
	@$(CC) $(CFLAGS) -S $< -o $@ -masm=intel > /dev/null
	@echo Generated $@
