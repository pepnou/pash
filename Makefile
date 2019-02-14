CC ?= gcc
CFLAG ?= -g -Wall -W -lm -v
LDFLAG ?= -lm

#OBJ = $(SRC:%.c=./build/%.o)

PASH_SRC = $(wildcard ./pash/*.c)
PASH_OBJ = $(PASH_SRC:./pash/%.c=./pash/build/%.o)
PROG = ./pash/build/pash

BUILT_IN_SRC = $(wildcard ./built-in/*.c)
BUILT_IN = $(BUILT_IN_SRC:./built-in/%.c=~/build-in/%)

all: pash/build/ ~/build-in/ $(PROG) $(BUILT_IN)


pash/build/:
	mkdir -p ./pash/build/


~/build-in/:
	mkdir -p ~/build-in/


$(PROG): $(PASH_OBJ)
	$(CC) $(LDFLAG) $^ -o $@


./pash/build/%.o : ./pash/%.c
	$(CC) -c $(CFLAG) $< -o $@


~/build-in/%.o:./built-in/%.c
	$(CC) -o $@ $(CFLAG) -c $<


~/build-in/%:~/build-in/%.o
	$(CC) -o $@ $< $(LDFLAG)


clean:
	rm -f *.o
	

bigclean:
	rm -r -f ./pash/build/
	rm -r -f ./built-in/build/
