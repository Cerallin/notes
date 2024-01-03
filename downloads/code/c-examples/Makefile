# Find all subdirectories containing C source files
SUBDIRS := $(wildcard */)

# Define the compiler and flags
CC := gcc
CFLAGS := -g -std=gnu11 -O2 -flto \
	-Wall -Wno-unused-result
LIBS := -lm

# Targets for each subdirectory
TARGETS := $(addsuffix target, $(SUBDIRS))

# Default target is to build all subdirectories
all: $(TARGETS)

# Rule to build the executable in each subdirectory
%/target: %/*.c
	$(CC) $(CFLAGS) -o $@ -I. $^ $(LIBS)

# Clean all subdirectories
clean:
	for dir in $(SUBDIRS); do \
		rm $$dir/target; \
	done

.PHONY: all clean $(SUBDIRS)
