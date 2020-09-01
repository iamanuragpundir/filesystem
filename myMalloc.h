void *mymalloc(int size) ;
void myfree(void *memory_block) ;
void initialize_mem() ;
void insert(char * filename, off_t size, int block_num) ;
void display(char * total_files) ;
int if_present(char * filename) ;