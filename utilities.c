#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "utilities.h"
#include "disk_ops.h"


int writeDiskBlock(int fd, int block_num, void * buf){

	if (block_num > TOTAL_BLOCKS ){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "Disk full.") ;
		return -2 ;
	}

	if (block_num < 0){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "Invalid block number in writeDiskBlock()") ;
		return -2 ;
	}
    if (lseek(fd, block_num * BLK_SIZE, SEEK_SET)==-1)
    	return -1 ;

    return write(fd, buf, BLK_SIZE) ;
}

int readDiskBlock(int fd, int block_num, void *buf){
	if (block_num > TOTAL_BLOCKS || block_num < 0){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "Invalid block number for readDiskBlock()") ;
		return -2 ;
	}

 	if (lseek(fd, block_num * BLK_SIZE, SEEK_SET) == -1 )
 		return -1 ;
 	
    return read(fd, buf, BLK_SIZE) ;
}

int eraseDiskBlock(int fd, int block_num){

	char buf[BLK_SIZE] ;
	readDiskBlock(fd, block_num, buf) ;

	int i = 0 ;
	while (i < BLK_SIZE){
		buf[i] = '\0' ;
		i++ ;
	}

	writeDiskBlock(fd, block_num, buf) ;
	
	return 1 ;
}

void getFilename(char  * buffer, char * filename){
	int i = 0 ;
	while (i < FILE_NAME_SIZE && buffer[i] != '\0'){
		filename[i] = buffer[i] ;
		i++ ;
	}
	filename[i] = '\0' ;
}

off_t filesize(char *buffer){
	int i = FILE_NAME_SIZE ;
	off_t ans  = 0 ;

	while(i < BLK_SIZE && buffer[i] !='\0'){
		ans = ans*10 + buffer[i] - 48 ;
		i++ ;
	}
	return ans ;
}

int ifFreeBlock(int fd, int block_num){
	lseek(fd, block_num * BLK_SIZE, SEEK_SET) ;
	char buf[BLK_SIZE] ;
	bzero(buf, BLK_SIZE) ;
	readDiskBlock(fd, block_num, buf) ;
	int i = 0 ;
	while(i < BLK_SIZE){
		if (buf[i] != '\0')
			return 0 ;

		i++ ;
	}

	return 1 ;
}

int next_free_block(int fd){
	
	while (free_block < TOTAL_BLOCKS){
		if (ifFreeBlock(fd, free_block))
			return free_block ;

		else	free_block ++ ;
	}

	return -1 ;
}

int search_file (char * filename_to_search){
	for(int i = 0; i < RESERVED_BLOCKS; i++){
		char buf[BLK_SIZE] ;
		
		readDiskBlock(fd_VD, i, buf); //defined in utilities.c
		
		
		for (int j = 0; j < BLK_SIZE; j++)
			if (buf[j] == 'y'){
				
				char filename[FILE_NAME_SIZE], header_buf[BLK_SIZE] ;
	
				readDiskBlock(fd_VD, i * BLK_SIZE + j, header_buf);
				
				getFilename(header_buf, filename) ; // defined in utilities.c
				if (strcmp(filename, filename_to_search) == 0)
					return i * BLK_SIZE + j ; //returning the block number of the header block.
				
	
		
			}
	}

	return -11 ;
}
