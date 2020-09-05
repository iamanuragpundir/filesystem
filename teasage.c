#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "disk_ops.h"
#include "myMalloc.h"
#include "utilities.h"
#include <math.h>

#define BLK_SIZE 512
#define FILE_NAME_SIZE 256
extern int errno;

char nullbyte[] = "\0";

int initial_disk_read()
{
	int if_error ;
	free_block = 80;

	for(int i = 0; i < 80; i++){
		char buf[BLK_SIZE] ;
		
		if_error = readDiskBlock(fd_VD, i, buf); //defined in utilities.c
		if (if_error < 0 )
			return if_error ;
		
		for (int j = 0; j < BLK_SIZE; j++)
			if (buf[j] == 'y'){
				

				char filename[FILE_NAME_SIZE], header_buf[BLK_SIZE] ;
	
				if_error = readDiskBlock(fd_VD, i * BLK_SIZE + j, header_buf);
				if ( if_error < 0)
					return if_error ;

				getFilename(header_buf, filename) ; // defined in disk_operations.c

				off_t size  = filesize(header_buf) ; // defined in disk_operations.c
	
				if (insert(filename, size,i * BLK_SIZE + j) == -1 ) // defined in linked_list.c
					return -2 ;

				//change the position of the freeblock, since we have a file here
				if  (size % BLK_SIZE == 0)
					free_block = i * BLK_SIZE + j + size/BLK_SIZE  + 1;
				else
					free_block =  i * BLK_SIZE + j + size/BLK_SIZE  + 2;

			}
	}	
}

int main()
{
	system("clear") ;

	int if_error ;
    off_t disksize = 20971520L - 1L;
    //Let's create a disk of about 20MB - poor man's harddisk
    //our company name is teasage - producers of such great disks in a jiffy
    //with this we will compete with seagate :)
    fd_VD = open("disk.teasage", O_RDWR | O_CREAT, 00700);
    
    if (fd_VD ==-1) //some error
    {
        printf("Error: cannot create the grand disk.\n Errorno: %d\n", errno);
        perror("Open error.");
        exit(1);
    }
   
    //writing  null byte to the disk
   
    if (write(fd_VD, nullbyte, 1) != 1){
        perror("write error");
        exit(1) ;
    }

    if (lseek(fd_VD, disksize, SEEK_SET) == -1)
        perror("lseek error");

    if (write(fd_VD, nullbyte, 1) != 1)
        perror("write error");
	
	// allocating fixed memory for storing file's info onto RAM
	//initialize_mem() defined in myMalloc.c
    initialize_mem() ; 

  	//fetching the preloaded file into the RAM from the disk
    if_error = initial_disk_read() ; 
    if (if_error < 0){
    	if (if_error == -1)
    		perror("error in initial reading of disk") ;
    	else
    		printf("error in initial reading of disk : %s\n", error_msg);
    }
    

	char file_path[256], filename[256] ;
	int choice ;
		
	while(1){
		printf("\n1. Veiw files on Disk\n2. Copy to Disk\n3. Copy from Disk\n4. exit\n") ;
		scanf("%d", &choice) ;

		if (choice == 1){
			char total_files[1000] ;
			bzero(total_files, 1000) ;
			
			if (Vdls(total_files)  == 1 ) // defined in disk_operations.c
				printf("%s\n\n", total_files);
			else
				printf("No file on the disk\n");
		}

		else if (choice == 2){
			bzero(filename, FILE_NAME_SIZE) ;
			bzero(file_path, FILE_NAME_SIZE) ;

			printf("Enter file path :");
			scanf("%s", file_path) ;
			printf("Enter file name :");
			scanf("%s", filename) ;

			if_error = VdCpto(file_path, filename) ; // defined in disk_operations.c
			if (if_error < 0){
				if (if_error == -1)
					perror("error in copying file onto disk") ;

				else
					printf("error in copying file onto disk : %s\n", error_msg);
			}

			else 	printf("success!\n" );
		}

		else if(choice == 3){
			bzero(filename, FILE_NAME_SIZE) ;
			bzero(file_path, FILE_NAME_SIZE) ;

			printf("Copy to path :");
			scanf("%s", file_path) ;
			printf("Enter file name :");
			scanf("%s", filename) ;			

			if_error = VdCpfrom(file_path, filename) ; // defined in disk_operations.c
			if (if_error < 0){
				if (if_error == -1)
					perror("error in copying file onto disk") ;

				else
					printf("error in copying file onto disk : %s\n", error_msg);
			}
			else	printf("success!\n");
		}
		else if(choice == 4)
			break;
	}
	
    close(fd_VD) ;
    return 1;
}


