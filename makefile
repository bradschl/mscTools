# Build commands
MAKEDIR		= @mkdir -p $(@D)

# BUild directories
BUILD_DIR 		:= ./build
OBJ_DIR 		:= $(BUILD_DIR)/obj
BIN_DIR 		:= $(BUILD_DIR)/bin

# Targets
TARGET_BIN 		:= $(BIN_DIR)/mscTester

CSOURCES 		:= $(shell find ./src/ -name '*.c')
COBJS 			:= $(addprefix $(OBJ_DIR)/,$(CSOURCES:%.c=%.o))

$(TARGET_BIN): $(COBJS)
	$(MAKEDIR)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	@$(MAKEDIR)
	$(CC) -Iinc -c $< -o $@


.PHONY: clean
clean:
	rm -f $(COBJS)
	rm -f $(TARGET_BIN)
