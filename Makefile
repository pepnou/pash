CC ?= gcc
CFLAG ?= -g -Wall -w -v
LDFLAG ?= 

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXECUTABLE = $(SRC:.c=)

all : $(EXECUTABLE)

#COMMENT SIMPLIFIER CA POUR PAS LE RECOPIER TOUT LE TEMPS ?
#<fonction>:<fonction>.o
#	mkdir -p build
#	$(CC) -o ./build/$@ $^ $(LDFLAG)

pash:pash.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

date:date.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

prompt:prompt.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

beep:beep.o
	mkdir -p build
	$(CC) -o ./build/$@ $< $(LDFLAG)

ping:ping.o
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