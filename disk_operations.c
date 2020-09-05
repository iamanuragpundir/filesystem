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

int Vdls(char *  total_files)
{	
	return display(total_files) ; //defined in linked_list.c
}

int VdCpto(char * file_path, char * filename)
{
	if (if_present (filename) != -1){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "File with same name already exists on the disk.") ;
		return -2 ;
	}
	int if_error ;

	file_path = strcat(file_path,filename) ;

	int file_fd = open(file_path, O_RDONLY) ;
	if (file_fd == -1)	return -1 ;

	off_t fileLength = lseek(file_fd, 0, SEEK_END) ;
	if (fileLength == -1)	return -1 ;

	if(lseek(file_fd, 0, SEEK_SET) == -1)	return -1 ;
	
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
	if (insert(filename, fileLength, block_num) == -1) // defined in linked_list.c
		return -2 ; 
	
	//turn on the (block_num)th byte as 'y' in the reserved disk space, representing a file starts from this block.
	char turn_on[BLK_SIZE];
	bzero(turn_on, BLK_SIZE) ;
	
	if_error = readDiskBlock(fd_VD, block_num/BLK_SIZE, turn_on) ; //defined in utilities.c
	if (if_error < 0)
		return -1 ;
	
	turn_on[block_num%BLK_SIZE]  = 'y' ;

	if_error = writeDiskBlock(fd_VD, block_num/BLK_SIZE, turn_on);	//defined in utilities.c
	if (if_error < 0 )
		return if_error ;

	if_error = writeDiskBlock(fd_VD, block_num, header) ;	
	if(if_error  < 0)
		return if_error ;

	block_num ++ ;

	// now copying the file data onto the disk (poor man's)
	while(fileLength > 0){
		
		char buffer[BLK_SIZE];
        
        if (read(file_fd, buffer, BLK_SIZE) == -1 )
        	return -1 ;
        
        if_error = writeDiskBlock(fd_VD, block_num, buffer) ;
        if (if_error < 0)
        	return if_error ;

        block_num++ ; 
        fileLength -= BLK_SIZE ;
	}
	
	free_block = block_num ;
	
	close(file_fd) ;

	return 1 ;
}

int VdCpfrom(char * copy_to_path, char * filename)
{	
	int if_error ;
	
	int block_num = if_present(filename) ;	//defined in linked_list.c

	if (block_num == -1){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "File not on disk.") ;
		return -2 ;
	} // file not present
		

	char read_buf[BLK_SIZE] ;
	
	if_error = readDiskBlock(fd_VD, block_num, read_buf) ;
	if (if_error  < 0)
		return if_error ;

	block_num ++ ;

	char extracted_filename[FILE_NAME_SIZE] ;
	getFilename(read_buf, extracted_filename) ;	 //defined in utilities.c
	off_t extracted_filelength = filesize(read_buf) ;	 //defined in utilities.c
	
	off_t fl = extracted_filelength ;
	strcat(copy_to_path, filename) ;	
	int new_copy = open(copy_to_path, O_WRONLY | O_CREAT, 00700) , ret;
	if (new_copy == -1)
		return -1 ;

	while (fl > 0 ) {
		if (fl>BLK_SIZE)
            ret = BLK_SIZE ;
        else
            ret = fl ;

        if_error = readDiskBlock(fd_VD, block_num, read_buf) ;
        if (if_error < 0)
        	return if_error ;

        if (write(new_copy, read_buf, ret) == -1 )
        	return -1 ;
        
        block_num++ ;
        fl -= ret ;
	}

	close(new_copy) ;
	return 1 ;
}
