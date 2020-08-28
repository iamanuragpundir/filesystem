#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define BLK_SIZE 512

extern int errno;

char nullbyte[] = "\0";

void storeFileOntoDisk(char * file_path) ;
void getFileFromDisk(char * file_path) ;
int readDiskBlock(int fd, int block_num, void * buf) ;
int writeDiskBlock(int fd, int block_num, void * buf) ;

int fd_VD ;
off_t fileLength, add_offset ;

int main()
{
    off_t disksize = 10857600L - 1L;
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
    
    char filepath[256] ;
    printf("Enter filepath : ") ;
    scanf("%s", filepath) ;
    storeFileOntoDisk(filepath) ;
    getFileFromDisk(filepath) ;
    close(fd_VD) ;
    return 0;
}


int writeDiskBlock(int fd, int block_num, void * buf){
    lseek(fd, block_num * BLK_SIZE, SEEK_SET) ;
    return write(fd, buf, BLK_SIZE) ;
}


void storeFileOntoDisk(char * file_path)
{
    int file_fd = open(file_path, O_RDONLY) ;
    if(file_fd == -1){
        perror("open file error:") ;
        exit(1) ;
    }

    fileLength = lseek(file_fd, 0, SEEK_END) ;
    if (fileLength%BLK_SIZE == 0)
        add_offset = 0 ;
    else
        add_offset = BLK_SIZE - fileLength%BLK_SIZE ;
    
    fileLength  += add_offset ; //padding the fileLength
    off_t fl = fileLength ;
    lseek(file_fd, 0, SEEK_SET) ; //setting pointer to begining of the file
    lseek(fd_VD, 0, SEEK_SET) ;
    int block_num = 0 ;

    while(fl > 0){
    
        char buffer[BLK_SIZE];
        read(file_fd, buffer, BLK_SIZE) ;
        writeDiskBlock(fd_VD, block_num, buffer) ;
        
        block_num++ ;
        fl -= BLK_SIZE ;
    }

    close(file_fd) ;
}

int readDiskBlock(int fd, int block_num, void *buf){
 	lseek(fd, block_num * BLK_SIZE, SEEK_SET) ;
    return read(fd, buf, BLK_SIZE) ;
}
void getFileFromDisk(char * file_path)
{   
    char new_file[256] ;
    printf("enter new_file name : ") ;
    scanf("%s", new_file) ;

    lseek(fd_VD, 0, SEEK_SET) ;
    int new_copy = open(new_file, O_WRONLY | O_CREAT, 00700) ;
    char read_buf[BLK_SIZE] ;
    int block_num = 0 , ret;
    off_t fl = fileLength- add_offset ;

    while(fl > 0){
        if (fl>BLK_SIZE)
            ret = BLK_SIZE ;
        else
            ret = fl ;

        readDiskBlock(fd_VD, block_num, read_buf) ;

        if(write(new_copy, read_buf, ret)==-1 )
            perror("GFD write error :") ;

        block_num++ ;
        fl -= ret ;
    }
    printf("Success ! Go check the newly created %s file  in the current directory\n", new_file) ;
    close(new_copy) ;
}



