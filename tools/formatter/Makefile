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

all: $(BIN)/format-mbr $(BIN)/format-gpt $(ASMS)
	

$(BIN)/format-mbr: $(MBR_OBJS) 
	@echo
	@echo building $(TARGET)
	@$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo built $(TARGET)

$(BIN)/format-gpt: $(GPT_OBJS) 
	@echo
	@echo building $(TARGET)
	@$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo built $(TARGET)

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
