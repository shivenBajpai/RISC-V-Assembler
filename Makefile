SHELL=/bin/bash

SRCS=$(wildcard ./src/*.c)
OBJ_NAMES=$(SRCS:.c=.o)
OBJS=$(OBJ_NAMES:src=build)

run: ./bin/main
	@cd bin && ./main

.PHONY: build
build: ./bin/main

./bin/main: $(OBJS)
	@echo "Linking..."
	@gcc -o ./bin/main $(OBJS)

./build/%.o: %.c
	@echo "Compiling $<..."
	@gcc -c $< -o $(@: src=build )

.PHONY: clean
clean:
	@echo "Removing Build files..."
	-@rm ./build/*
	-@rm ./bin/main
