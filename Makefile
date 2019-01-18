CC ?= gcc
CFLAG ?= -g -Wall -w
LDFLAG ?= 

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXECUTABLE = $(SRC:.c=)

all : $(EXECUTABLE)

#COMMENT SIMPLIFIER CA POUR LA LE RECOPIER TOUT LE TEMPS ?
#<fonction>:<fonction>.o
#	mkdir -p build
#	$(CC) -o ./build/$@ $^ $(LDFLAG)

pash:pash.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

date:date.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

intro:intro.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

%.o:%.c
	$(CC) -o $@ $(CFLAG) -c $<

clean:
	rm -f *.o

bigclean:
	rm -r build