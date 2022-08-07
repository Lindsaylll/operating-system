OBJS	= my_mini_shell.o
SOURCE	= my_mini_shell.c
HEADER	=
OUT	= myshell
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -pthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

my_mini_shell.o: my_mini_shell.c
	$(CC) $(FLAGS) my_mini_shell.c


clean:
	rm -f $(OBJS) $(OUT)
 