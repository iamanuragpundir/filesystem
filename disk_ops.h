int Vdls(char *  total_files) ;
int VdCpto(char * file_path, char * filename) ;
int VdCpfrom(char * copy_to_path, char * filename) ;
int fd_VD ;

// first available block, if no file in the disk
int free_block;
char error_msg[256] ;