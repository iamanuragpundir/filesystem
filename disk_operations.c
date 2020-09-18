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

int block_num_padding(int block_num, char *res){

	int i = MOVE_BYTES- 1 ;
	while(i >= 0 && block_num>0){
		res[i] = 48 + block_num%10 ;
		i-- ; block_num /= 10 ;
	}

	while (i >= 0){
		res[i] = 48 ;
		i-- ;
	}

}
void setPrevMoveByte_ofCurBlock(int prev, int free_block, char *buf){

	char res[MOVE_BYTES] ;
	block_num_padding(prev, res) ;
	
	int i = MOVE_BYTES- 1 , j = BLK_SIZE - MOVE_BYTES- 1 ;
	while (i >= 0){
		buf[j] = res[i] ;
		j-- ; i-- ;
	}
}

void setNextMoveByte_ofPrevBlock(int fd_VD, int prev, int free_block){
	char buf[BLK_SIZE], res[MOVE_BYTES] ;
	readDiskBlock(fd_VD, prev, buf) ;
	
	block_num_padding(free_block, res) ;
	
	int i = MOVE_BYTES - 1 , j = BLK_SIZE-1;
	while (i >= 0){
		buf[j] = res[i] ;
		j-- ; i-- ;
	}

	writeDiskBlock(fd_VD, prev, buf) ;
}

int getPrevMoveByte(int fd, int block_num){
	char buf[BLK_SIZE] ;
	readDiskBlock(fd, block_num, buf) ;
	int prev_blk = 0 , i = BLK_SIZE - 2*MOVE_BYTES ;
	
	if (buf[i] == '\0')
		return -13 ;
	
	while (i < BLK_SIZE - MOVE_BYTES){
		prev_blk = prev_blk*10 + (buf[i] - 48) ;
		i++ ;
	}

	return prev_blk ;
}

int getNextMoveByte(int fd, int block_num){
	char buf[BLK_SIZE] ;
	readDiskBlock(fd, block_num, buf) ;

	int next_blk = 0 , i = BLK_SIZE - MOVE_BYTES ;

	if (buf[i] == '\0')
		return -13 ;

	while (i < BLK_SIZE ){
		next_blk = next_blk*10 + (buf[i] - 48) ;
		i++ ;
	}

	return next_blk ;
}

int Vdls(char *  total_files)
{	
	for(int i = 0; i < RESERVED_BLOCKS; i++){
		char buf[BLK_SIZE] ;
		
		readDiskBlock(fd_VD, i, buf); //defined in utilities.c
		
		
		for (int j = 0; j < BLK_SIZE; j++)
			if (buf[j] == 'y'){
				
				char filename[FILE_NAME_SIZE], header_buf[BLK_SIZE] ;
	
				readDiskBlock(fd_VD, i * BLK_SIZE + j, header_buf);
				
				getFilename(header_buf, filename) ; // defined in utilities.c
				strcat(total_files, filename) ;
				strcat(total_files, ", ") ;

				off_t size  = filesize(header_buf) ; // defined in utilities.c
	
		
			}
	}

	return 1 ;
}

int VdCpto(char * file_path, char * filename)
{
	/*
	if (if_present (filename) != -1){
		bzero(error_msg, 256) ;
		strcpy(error_msg, "File with same name already exists on the disk.") ;
		return -2 ;
	}
	we do not have structure in RAM now, change this reading style
	*/
	int if_error ;

	file_path = strcat(file_path,filename) ;

	int file_fd = open(file_path, O_RDONLY) ;
	if (file_fd == -1)	return -1 ;

	off_t fileLength = lseek(file_fd, 0, SEEK_END) ;
	if (fileLength == -1)	return -1 ;
	printf("file length during input %ld %s \n", fileLength, filename) ;
	lseek(file_fd, 0, SEEK_SET) ; //setting pointer to begining of the file .
	
	//First block of the file will represent its filename, filesize
	char header[BLK_SIZE] ; // change this to myMalloc style
	int i = 0 , j = 0 ;
	//putting filename in the first 256 bytes of the block
	while (filename[i] != '\0'){
		header[i] = filename[i] ;
		i++ ;
	}

	while(i < FILE_NAME_SIZE) //padding the bytes remaining out of 256 with null char
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


	free_block = next_free_block(fd_VD) ;
	printf("block used %ld\n",free_block ) ;
	if (free_block == -1)
		return -10 ;

	//turn on the (block_num)th byte as 'y' in the reserved disk space, representing a file starts from this block.

	char turn_on[BLK_SIZE]; //allocate thorugh myMalloc
	bzero(turn_on, BLK_SIZE) ; //allocate thorugh myMalloc
	
	readDiskBlock(fd_VD, free_block/BLK_SIZE, turn_on) ; //defined in utilities.c
	
	turn_on[free_block%BLK_SIZE]  = 'y' ;

	writeDiskBlock(fd_VD, free_block/BLK_SIZE, turn_on);	//defined in utilities.c
	
	writeDiskBlock(fd_VD, free_block, header) ;	
	
	

	// now copying the file data onto the disk (poor man's)
	while(fileLength > 0){
		
		char buffer[BLK_SIZE];	//allocate thorugh myMalloc or re-use 
		read(file_fd, buffer, BLK_SIZE - 2*MOVE_BYTES) ;

		int prev = free_block ;

		free_block = next_free_block(fd_VD) ;
		if (free_block == -1)
			return -10 ;

		setPrevMoveByte_ofCurBlock(prev, free_block, buffer) ;
		setNextMoveByte_ofPrevBlock(fd_VD, prev, free_block) ;
		
        //here at last few bytes, the next block in sequence needs to be mentioned
        //so read number of bytes  = BLK_SIZE - digits(No_of_blocks)
        
        writeDiskBlock(fd_VD, free_block, buffer) ;
        printf("block used %ld\n",free_block ) ;
        //bzero(buffer, BLK_SIZE) ;
        //readDiskBlock(fd_VD, prev , buffer) ;
		//printf("%s\n %ld", buffer, sizeof(buffer));
		//printf("--------------------------------------------------------------------------\n")  ;
        fileLength = fileLength -  BLK_SIZE + 2*MOVE_BYTES  ;
	}
	
	
	close(file_fd) ;

	return 1 ;
}

int VdCpfrom(char * copy_to_path, char * filename)
{	
	int cur_blk, prev_blk  = search_file (filename) ;
	printf(" block mila %d\n", prev_blk);
	if (prev_blk < 0){
		return -2 ;
	} // file not present
		
	

	char read_buf[BLK_SIZE] ;
	
	readDiskBlock(fd_VD, prev_blk, read_buf) ;

	char extracted_filename[FILE_NAME_SIZE] ;
	getFilename(read_buf, extracted_filename) ;	 //defined in utilities.c
	off_t extracted_filelength = filesize(read_buf) ;	 //defined in utilities.c
	off_t fl = extracted_filelength ;
	printf(" extracted length %ld\n", fl);
	strcat(copy_to_path, filename) ;	
	int new_copy = open(copy_to_path, O_WRONLY | O_CREAT, 00700) , ret;
	if (new_copy == -1)
		return -1 ;

	while (fl > 0 ) {
		if (fl> BLK_SIZE - 2*MOVE_BYTES)
            ret = BLK_SIZE - 2*MOVE_BYTES ;
        else
            ret = fl ;

        cur_blk = getNextMoveByte(fd_VD, prev_blk) ;
        bzero(read_buf, BLK_SIZE) ;
        readDiskBlock(fd_VD, cur_blk, read_buf) ; 
        write(new_copy, read_buf, ret) ;
       
        fl -= ret ;
        prev_blk = cur_blk ;
	}

	close(new_copy) ;
	return 1 ; 
}

int deleteFileFromDisk( char * filename)
{
	// first search, if the file exists or not
	int cur_blk, prev_blk  = search_file (filename) ;
	printf("%s\n",filename );
	if (prev_blk < 0){
		return -2 ;
	} // file not present


	//turn off the byte that coresponds to the header block of the file being deleted.
	char buf[BLK_SIZE] ; //allocate thorugh myMalloc
	bzero(buf, BLK_SIZE) ; //allocate thorugh myMalloc
	readDiskBlock(fd_VD, prev_blk/BLK_SIZE, buf) ; //defined in utilities.c
	buf[prev_blk%BLK_SIZE]  = '\0' ;
	writeDiskBlock(fd_VD, prev_blk/BLK_SIZE, buf);	//defined in utilities.c

	bzero(buf, BLK_SIZE) ;
	readDiskBlock(fd_VD, prev_blk, buf) ;
	off_t fl = filesize(buf) ;
	//if the header block lies behind the curr free_block being pointed, then point the current free_block as header block
	// as it is about to be freed
	if (prev_blk < free_block)
		free_block = prev_blk ;

	//erasing evry block that belongs to that file

	do{
		cur_blk = getNextMoveByte(fd_VD, prev_blk) ;
		eraseDiskBlock(fd_VD, prev_blk) ;
		prev_blk = cur_blk ;
	}while(prev_blk > 0) ; 

	return 1 ;
}