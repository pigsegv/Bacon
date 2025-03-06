define relative
	$(shell realpath -s --relative-to="$(ROOT_PATH)" $(1))
endef

SRC := $(call relative, $(ROOT_PATH)/tools)
OBJ := $(call relative, $(OBJ)/tools)

export SRC OBJ

.PHONY: all

TOOLS := formatter

all: $(TOOLS)

# ---------------------- PROJECTS ----------------------

formatter:
	@$(MAKE) -C $(ROOT_PATH)/tools/formatter


