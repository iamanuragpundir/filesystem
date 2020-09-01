int writeDiskBlock(int fd, int block_num, void * buf) ;
int readDiskBlock(int fd, int block_num, void *buf) ;
void getFilename(char  * buffer, char * filename) ;
off_t filesize(char *buffer) ;