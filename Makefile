CC ?= gcc
CFLAG ?= -g -Wall -w -v
LDFLAG ?= 

SRC = $(wildcard *.c)
#OBJ = $(SRC:%.c=./build/%.o)
EXECUTABLE = $(SRC:%.c=./build/%)

all : build/ $(EXECUTABLE)


build/:
	mkdir -p build


#COMMENT SIMPLIFIER CA POUR PAS LE RECOPIER TOUT LE TEMPS ?
#<fonction>:<fonction>.o
#	mkdir -p build
#	$(CC) -o ./build/$@ $^ $(LDFLAG)

#./build/pash:./build/pash.o
#	$(CC) -o $@ $< $(LDFLAG)

#./build/:./build/intro.o
#	$(CC) -o $@ $< $(LDFLAG)


./build/%.o:%.c
	$(CC) -o $@ $(CFLAG) -c $<


./build/%:./build/%.o
	$(CC) -o $@ $< $(LDFLAG)
	

clean:
	rm -f *.o


bigclean:
	rm -r build

remove:
	rm -f *.o
	rm -r build
