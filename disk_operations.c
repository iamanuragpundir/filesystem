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

#define BLK_SIZE 512
#define FILE_NAME_SIZE 256

void Vdls(char *  total_files)
{	
	display(total_files) ;
}

void VdCpto(char * file_path, char * filename)
{
	file_path = strcat(file_path,filename) ;

	int file_fd = open(file_path, O_RDONLY) ;

	off_t fileLength = lseek(file_fd, 0, SEEK_END) ;
	lseek(file_fd, 0, SEEK_SET);
	
	//First block of the file will represent its filename, filesize
	char header[BLK_SIZE] ;
	int i = 0 , j = 0 ;
	//putting filename in the first 256 bytes of the block
	while (filename[i] != '\0'){
		header[i] = filename[i] ;
		i++ ;
	}

	while(i < FILE_NAME_SIZE)
		header[i++] = 0 ;

	off_t fl = fileLength ;
	int digits = 0 ;
	while (fl > 0){
		digits++ ;
		fl /= 10 ;
	}

	j = FILE_NAME_SIZE + digits - 1;
	i = j ;
	fl = fileLength ;
	//putting filelength's digits as characters from the 256th byte.
	while (fl > 0 ){
		header[i--] = 48 + fl%10 ;
		fl  =  fl/10 ;
	}

	j++ ;
	while (j < BLK_SIZE)
		header[j++] = '\0' ;

	int block_num = free_block ;
	//inserting file header into linked list created in RAM for fast searching
	insert(filename, fileLength, block_num) ; 
	
	//turn on the (block_num)th byte as 'y' in the reserved disk space, representing a file starts from this block.
	char turn_on[BLK_SIZE];
	bzero(turn_on, BLK_SIZE) ;
	readDiskBlock(fd_VD, block_num/BLK_SIZE, turn_on) ;
	turn_on[block_num%BLK_SIZE]  = 'y' ;
	writeDiskBlock(fd_VD, block_num/BLK_SIZE, turn_on);

	writeDiskBlock(fd_VD, block_num, header) ;
	block_num ++ ;

	// now copying the file data onto the disk (poor man's)
	while(fileLength > 0){
		
		char buffer[BLK_SIZE];
        read(file_fd, buffer, BLK_SIZE) ;
        writeDiskBlock(fd_VD, block_num, buffer) ;
        block_num++ ; 
        fileLength -= BLK_SIZE ;
	}
	
	free_block = block_num ;
	
	close(file_fd) ;
}

void VdCpfrom(char * copy_to_path, char * filename)
{	
	
	int block_num = if_present(filename) ;

	if (block_num == -1) // file not present
		return ;

	char read_buf[BLK_SIZE] ;
	readDiskBlock(fd_VD, block_num, read_buf) ;
	block_num ++ ;

	char extracted_filename[FILE_NAME_SIZE] ;
	getFilename(read_buf, extracted_filename) ;
	off_t extracted_filelength = filesize(read_buf) ;
	
	off_t fl = extracted_filelength ;
	strcat(copy_to_path, filename) ;	
	int new_copy = open(copy_to_path, O_WRONLY | O_CREAT, 00700) , ret;

	while (fl > 0 ) {
		if (fl>BLK_SIZE)
            ret = BLK_SIZE ;
        else
            ret = fl ;

        readDiskBlock(fd_VD, block_num, read_buf) ;
        write(new_copy, read_buf, ret) ;
        
        block_num++ ;
        fl -= ret ;
	}

	close(new_copy) ;
}
