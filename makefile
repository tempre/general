##
# Pixel data test
#
# @file
# @version 0.1

CC=gcc
LIBS= -lglut -lGL -lGLU -lm
CFLAGS = -Wall

# (This should be the actual list of C files)
VPATH = code
# rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC=$(wildcard *.c)
# OBJ=$(SRC:.c=.o)

debug: $(SRC)
	$(CC) -DDEBUG -o $@ $^ $(CFLAGS) $(LIBS)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm debug main

# end
