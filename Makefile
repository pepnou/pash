CC ?= gcc
CFLAG ?= -g -Wall -w
LDFLAG ?= 

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXECUTABLE = $(SRC:.c=)

all : $(EXECUTABLE)
	mkdir build
	mv $(SRC:.c=) ./build

%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

%:%.o
	$(CC) $(LDFLAG) $^ -o $@

clean:
	rm -f -r build %.o