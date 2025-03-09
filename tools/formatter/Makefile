OBJ := $(OBJ)/formatter

CFLAGS += -Wall -Wextra -ggdb3 -std=gnu23 -O0
LDFLAGS += 

MBR_SRCS := mbr.c
MBR_OBJS := $(patsubst %.c, $(OBJ)/%.o, $(MBR_SRCS))

GPT_SRCS := gpt.c
GPT_OBJS := $(patsubst %.c, $(OBJ)/%.o, $(GPT_SRCS))
DEPS := $(patsubst %.c, $(OBJ)/%.d, $(MBR_SRCS) $(GPT_SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst %.c, $(OBJ)/%.s, $(MBR_SRCS) $(GPT_SRCS))
endif

all: $(MBR_TARGET) $(GPT_TARGET) $(ASMS)

$(MBR_TARGET): $(MBR_OBJS) 
	@echo
	@echo building $@
	@$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo built $@

$(GPT_TARGET): $(GPT_OBJS) 
	@echo
	@echo building $@
	@$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo built $@

-include $(DEPS)

$(OBJ)/%.o: %.c
	@echo building $@
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@
	@echo built $@
	@echo

$(OBJ)/%.s: %.c
	@echo Generating assembly $@
	@$(CC) $(CFLAGS) -S -masm=intel $< -o $@
	@echo Generated $@
	@echo
