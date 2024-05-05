CC = gcc
CFLAGS = -g2 -ggdb -I./headers -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -lpthread

.SUFFIXES:
.SUFFIXES: .c .o

DEBUG = ./build/debug
RELEASE = ./build/release

OUT_DIR = $(DEBUG)
vpath %.c src
vpath %.h src
vpath %.o build/debug

ifeq ($(MODE), release)
	CFLAGS = -I./headers -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -lpthread
	OUT_DIR = $(RELEASE)
	vpath %.o build/release
endif

sort_index_objects = $(OUT_DIR)/sort_index.o $(OUT_DIR)/utils.o
sort_index_prog = $(OUT_DIR)/sort_index

generator_objects = $(OUT_DIR)/generator.o $(OUT_DIR)/utils.o
generator_prog = $(OUT_DIR)/generator

read_objects = $(OUT_DIR)/read.o $(OUT_DIR)/utils.o
read_prog = $(OUT_DIR)/read

all: $(sort_index_prog) $(generator_prog) $(read_prog)

$(sort_index_prog) : $(sort_index_objects)
	@$(CC) $(CFLAGS) $(sort_index_objects) -o $@

$(generator_prog) : $(generator_objects)
	@$(CC) $(CFLAGS) $(generator_objects) -o $@

$(read_prog) : $(read_objects)
	@$(CC) $(CFLAGS) $(read_objects) -o $@
	
$(OUT_DIR)/%.o : %.c
	@$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	@rm -rf $(DEBUG)/* $(RELEASE)/* test