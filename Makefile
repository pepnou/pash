CC ?= gcc
CFLAG ?= -g
LDFLAG ?= 

SRC = *.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = pash

all : $(EXECUTABLE)

$(EXECUTABLE):$(OBJ)
	$(CC) $(LDFLAG) $^ -o $@

%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm *.o