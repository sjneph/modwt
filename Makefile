MAIN	= include
CC	= g++
SFLAGS	= -static -ansi -Wall -pedantic -O3 -I$(MAIN)
OBJDIR	= src/objects

LIB1	= $(MAIN)
SOURCE1	= src/WaveletApp.cpp
BIN	= bin

NAME1	= modwt

.cpp.o:; $(CC) -c $(SFLAGS) $<

waves: dependencies
	mkdir -p $(BIN) && $(CC) -o $(BIN)/$(NAME1) $(SFLAGS) $(SOURCE1) $(OBJDIR)/Formats.o

dependencies:
	mkdir -p $(OBJDIR)
	$(CC) -c $(SFLAGS) $(LIB1)/Formats.cpp -o $(OBJDIR)/Formats.o

clean:
	rm -f $(BIN)/$(NAME1)
	rm -f $(OBJDIR)/Formats.o
