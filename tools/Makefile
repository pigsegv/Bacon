define relative
	$(shell realpath -s --relative-to="$(ROOT_PATH)" $(1))
endef

OBJ := $(OBJ)/tools

export OBJ


TOOLS := formatter

.PHONY: all $(TOOLS)

all: $(TOOLS)

# ---------------------- PROJECTS ----------------------

formatter:
	@$(MAKE) -C $(ROOT_PATH)/tools/formatter MBR_TARGET=$(BIN)/format-mbr GPT_TARGET=$(BIN)/format-gpt
