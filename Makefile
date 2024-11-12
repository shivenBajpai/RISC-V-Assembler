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

build: $(TARGET_PATH)

run: $(TARGET_PATH)
	@cd $(OUTDIR) && ./$(TARGET)

debug: $(TARGET_PATH)
	@cd $(OUTDIR) && ./$(TARGET) -d

test: $(TARGET_PATH)
	@cd tests && ./test.bash

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
	-@rm -f ./$(OBJDIR)/*.o
	-@rm -f ./$(OUTDIR)/$(TARGET)
	-@rm -f ./$(TEST_TEMPDIR)/*
