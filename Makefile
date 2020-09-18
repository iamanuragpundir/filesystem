CC = gcc

option=-c

all:	teasage.o linked_list.o myMalloc.o utilities.o disk_operations.o
		$(CC) teasage.o linked_list.o myMalloc.o utilities.o disk_operations.o -o  all

teasage.o:	teasage.c disk_ops.h
			$(CC) $(option) teasage.c

myMalloc.o:	myMalloc.c myMalloc.h
			$(CC) $(option) myMalloc.c

linked_list.o: linked_list.c
				$(CC) $(option) linked_list.c

disk_operations.o :	disk_operations.c
					$(CC) $(option) disk_operations.c

utilities.o:	utilities.c utilities.h
				$(CC) $(option) utilities.c


clean: 
		rm -rf *o all
