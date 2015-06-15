# Build commands
MAKEDIR		= @mkdir -p $(@D)

# BUild directories
BUILD_DIR 		:= ./build
OBJ_DIR 		:= $(BUILD_DIR)/obj
BIN_DIR 		:= $(BUILD_DIR)/bin

# Targets
TARGET_BIN 		:= $(BIN_DIR)/mscTools

CSOURCES 		:= $(shell find ./src/ -name '*.c')
COBJS 			:= $(addprefix $(OBJ_DIR)/,$(CSOURCES:%.c=%.o))

# Compiler flags
CF_ALL 			:= -Wall -O0 -g3 -Iinc $(shell pkg-config --cflags glib-2.0)
LF_ALL 			:= 
LL_ALL			:= $(shell pkg-config --libs glib-2.0)

# Misc
GIT_ID = $(shell git rev-parse HEAD)

# Local commands
$(OBJ_DIR)/%/Version.o: CF_TGT = -DVERSION_STRING='"$(GIT_ID)"'

$(TARGET_BIN): $(COBJS)
	$(MAKEDIR)
	$(CC) $(LF_ALL) $^ $(LL_ALL) -o $@

$(OBJ_DIR)/%.o: %.c
	@$(MAKEDIR)
	$(CC) $(CF_ALL) $(CF_TGT) -c $< -o $@


.PHONY: clean
clean:
	rm -f $(COBJS)
	rm -f $(TARGET_BIN)
