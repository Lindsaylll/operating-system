OBJS	= make_numbers.o sort.o
SOURCE	= make_numbers.c sort.c
HEADER	=
OUT	= make_numbers sort
CC	 = clang
FLAGS	 = -g -c -Wall

all: make_numbers sort

make_numbers: make_numbers.c
	gcc -o make_numbers make_numbers.c

sort: sort.c
	gcc -o sort sort.c

clean:
	rm -f $(OBJS) $(OUT)
