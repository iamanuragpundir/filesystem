CC = gcc

option=-c

all:	teasage.o
		$(CC) teasage.o -o all

teasage.o:	teasage.c
			$(CC) $(option) teasage.c

clean: 
		rm -rf *o all