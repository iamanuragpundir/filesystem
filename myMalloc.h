void *mymalloc(int size) ;
void myfree(void *memory_block) ;
void initialize_mem() ;
int insert(char * filename, off_t size, int block_num) ;
int display(char * total_files) ;
int if_present(char * filename) ;