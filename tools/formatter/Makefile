PROJ_SRC := $(SRC)/formatter
PROJ_OBJ := $(OBJ)/formatter
PROJ_BIN := $(BIN)
PROJ_INCLUDE := $(ROOT_PATH)/$(INCLUDE)

CFLAGS  += -Wall -Wextra -ggdb3 -std=gnu23 -fPIC -O0
LDFLAGS +=

TARGETS := $(PROJ_BIN)/format-mbr $(PROJ_BIN)/format-gpt

SRCS := $(shell find $(PROJ_SRC) -type  f -name "*.c")
OBJS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.o, $(SRCS))
DEPS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.d, $(SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst $(PROJ_SRC)/%.c, $(PROJ_OBJ)/%.s, $(SRCS))
endif

INCLUDES := $(shell find $(PROJ_INCLUDE) -type f -name "*.h")
INCLUDES += $(shell find $(PROJ_SRC) -type f -name "*.h")
INCLUDES += $(shell find $(ROOT_PATH)/$(SRC)/util -type f -name "*.h")

all: $(TARGET)

$(TARGET): $(OBJS) $(ASMS)
	@echo
	@echo building $(TARGET)
	@$(LD) $(CFLAGS) -shared $(OBJS) -o $@ $(LDFLAGS)
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

