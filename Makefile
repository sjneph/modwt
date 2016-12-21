MAIN	= include
CC	= g++
SFLAGS	= -static -ansi -Wall -pedantic -std=c++11 -O3 -I$(MAIN)
OBJDIR	= src/objects

LIB1	= $(MAIN)
SOURCE1	= src/WaveletApp.cpp
BIN	= bin

NAME1	= modwt

.cpp.o:; $(CC) -c $(SFLAGS) $<

waves:
	mkdir -p $(BIN) && $(CC) -o $(BIN)/$(NAME1) $(SFLAGS) $(SOURCE1)

clean:
	rm -f $(BIN)/$(NAME1)
