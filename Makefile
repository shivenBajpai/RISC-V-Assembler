SHELL=/bin/bash
CC = gcc
CCFLAGS = -g
CLFLAGS = 

SRCDIR=src
OBJDIR=build
OUTDIR=bin
TEST_TEMPDIR=tests/outputs
TARGET=riscv_asm

# DO NOT EDIT BELOW

SRCS=$(wildcard ./$(SRCDIR)/*.c)
OBJ_NAMES=$(patsubst %.c,%.o,$(SRCS))
OBJS=$(patsubst ./$(SRCDIR)%,./$(OBJDIR)%,$(OBJ_NAMES))
TARGET_PATH=./$(OUTDIR)/$(TARGET)

run: $(TARGET_PATH)
	@cd bin && ./$(TARGET)

debug: $(TARGET_PATH)
	@cd bin && ./$(TARGET) -d

test: $(TARGET_PATH)
	@cd tests && ./test.bash

.PHONY: build
build: $(TARGET_PATH)

$(TARGET_PATH): $(OBJS)
	@echo "Linking..."
	@$(CC) $(CLFLAGS) -o $(TARGET_PATH) $(OBJS)
	@echo "Binary generated in /bin"

./build/%.o: ./$(SRCDIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CCFLAGS) -c $< -o $@

.PHONY: report
report:
	@cd report && ./run.sh

.PHONY: clean
clean:
	@echo "Removing Build and Test files..."
	-@rm ./$(OBJDIR)/*.o
	-@rm ./$(OUTDIR)/$(TARGET)
	-@rm ./$(TEST_TEMPDIR)/*
