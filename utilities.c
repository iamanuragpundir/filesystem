#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "disk_ops.h"

#define FILE_NAME_SIZE 256
#define BLK_SIZE 512
#define LAST_BLK 40960

int writeDiskBlock(int fd, int block_num, void * buf){

	if (block_num > LAST_BLK ){
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
	if (block_num > LAST_BLK || block_num < 0){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "Invalid block number for readDiskBlock()") ;
		return -2 ;
	}

 	if (lseek(fd, block_num * BLK_SIZE, SEEK_SET) == -1 )
 		return -1 ;
 	
    return read(fd, buf, BLK_SIZE) ;
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
