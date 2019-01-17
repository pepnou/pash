CC ?= gcc
CFLAG ?= -g
LDFLAG ?= 

HDRS = $(wildcard *.h)
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXECUTABLE = pash

all : $(EXECUTABLE)

$(EXECUTABLE):$(OBJ)
	$(CC) $(LDFLAG) $^ -o $@

#$(EXECUTABLE).o:$(HDRS)

%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm *.o

bigclean:
	rm *.o $(EXECUTABLE)