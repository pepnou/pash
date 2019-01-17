CC ?= gcc
CFLAG ?= -g
LDFLAG ?= 

#HDRS = $(wildcard *.h)
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXECUTABLE = $(SRC:.c=)

all : $(EXECUTABLE)
	mkdir build
	mv $(SRC:.c=) ./build

#$(EXECUTABLE):$(OBJ)
#	$(CC) $(LDFLAG) $^ -o $@

%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

%:%.o
	mkdir build
	$(CC) $(LDFLAG) $^ -o ./build/$@

#$(EXECUTABLE).o:$(HDRS)

clean:
	rm -f *.o

bigclean:
	rm -f -r build %.o