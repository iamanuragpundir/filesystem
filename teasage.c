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

void initial_disk_read()
{	free_block = 80 ;
	for(int i = 0; i < 80; i++){
		char buf[BLK_SIZE] ;
		readDiskBlock(fd_VD, i, buf) ;
	
		for (int j = 0; j < BLK_SIZE; j++)
			if (buf[j] == 'y'){
				

				char filename[FILE_NAME_SIZE], header_buf[BLK_SIZE] ;
	
				readDiskBlock(fd_VD, j, header_buf) ;

				getFilename(header_buf, filename) ;

				off_t size  = filesize(header_buf) ;
	
				insert(filename, size, j) ;

				//change the position of the freeblock, since we have a file here
				printf("%d %ld\n",free_block, size );
				free_block = j + ceil(size/BLK_SIZE)  + 1;
				

				
			}
	}	
}

int main()
{
	system("clear") ;

    off_t disksize = 20971520L;
    //Let's create a disk of about 10MB - poor man's harddisk
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
	
    initialize_mem() ;
    initial_disk_read() ;
    

	char file_path[256], filename[256] ;
	int choice ;
		
	while(1){
		printf("1. Veiw files on Disk\n2. Copy to Disk\n3. Copy from Disk\n4. exit\n") ;
		scanf("%d", &choice) ;

		if (choice == 1){
			char total_files[1000] ;
			bzero(total_files, 1000) ;
			Vdls(total_files) ;
			printf("%s\n\n", total_files);
		}
		else if (choice == 2){
			bzero(filename, FILE_NAME_SIZE) ;
			bzero(file_path, FILE_NAME_SIZE) ;

			printf("Enter file path :");
			scanf("%s", file_path) ;
			printf("Enter file name :");
			scanf("%s", filename) ;

			VdCpto(file_path, filename) ;
		}

		else if(choice == 3){
			bzero(filename, FILE_NAME_SIZE) ;
			bzero(file_path, FILE_NAME_SIZE) ;

			printf("Copt to path :");
			scanf("%s", file_path) ;
			printf("Enter file name :");
			scanf("%s", filename) ;			

			VdCpfrom(file_path, filename) ;
		}
		else if(choice == 4)
			break;
	}
	
    close(fd_VD) ;
    return 0;
}


