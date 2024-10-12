# Copyright Plopeanu Teodora-Anca 312CA 2023-2024

# Compiler Setup
CC=gcc
CFLAGS=-g -Wall -Wextra -std=c99

# Define Targets
TARGETS=sfl

build: $(TARGETS)

sfl:
	$(CC) $(CFLAGS) sfl.c about_dlls.c about_sfls.c about_lls.c -o sfl -lm

clean:
	rm -f $(TARGETS)

pack:
	zip -FSr 312CA_PlopeanuTeodoraAnca_Tema1.zip README Makefile *.c *.h

.PHONY: pack clean