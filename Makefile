CC = gcc
INC = include
SRC = src
BUILD = build
BIN = bin
INCLUDE = -I ./$(INC)

TARGET = tiny

OBJECTS = $(BUILD)/main.o $(BUILD)/scanner.o $(BUILD)/parser.o \
			$(BUILD)/tree.o $(BUILD)/util.o

$(BIN)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN)
	@$(CC) -o $@ $^ $(INCLUDE)

$(BUILD)/%.o: $(SRC)/%.c
	@mkdir -p $(BUILD)
	@$(CC) -o $@ -c $^ $(INCLUDE)

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD) $(BIN)