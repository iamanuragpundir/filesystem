#include<stdio.h>
#include "myMalloc.h"

#define GIVEN_MEM 2000000

/* allocate 2 million bytes in an array statically */
unsigned char arr[GIVEN_MEM];

typedef struct memory_block{
    char allocated ;
    int size ;
    struct memory_block *next, *prev;

} block ;

block * begin = (block*) arr ;

void initialize_mem(){
    begin->allocated = 'n' ;
    begin->size = GIVEN_MEM - sizeof(block) ;
    begin->next = begin->prev =NULL ;
}

void split_block(block *large_block, int size){

    block *new_block  =(void*)((void*)(large_block) + sizeof(block) + size );
    new_block->size = (large_block->size )- sizeof(block) - size ;
    large_block->size = size ;

    new_block->prev = large_block ;
    new_block->next = large_block->next ;
    large_block->next = new_block ;
    large_block->allocated = 'y' ;
    new_block->allocated = 'n' ;

}

void * mymalloc(int size){
    block *ptr = begin ;

    while (ptr!=NULL){
        
        // if the size of encountered free block is greater than required, so here the block needs to be splitted (exact_req + remaining)
        if (ptr->allocated == 'n' && ptr->size > size){
            split_block(ptr, size) ;
            return (void *)(++ptr) ;
        }

        // if a free memory block with exact size is found , then simply return that particular block 
        else if (ptr->allocated == 'n' && ptr->size == size){
            ptr->allocated = 'y' ;
            return (void *)(++ptr) ;
        }
        
        ptr = ptr->next ;
    }

    return NULL ;
}

void myfree(void * memory_block){

    block * toFree = (block*)memory_block ;
    toFree-- ;

    unsigned char *s = (unsigned char *)(toFree + 1 );
    int i = 0 ;
    while (i < toFree->size){
        s[i] = 0 ;
        i++ ;
    }

    toFree->allocated = 'n' ;

    if(toFree->next != NULL && toFree->next->allocated == 'n') {
        toFree->size += toFree->next->size + sizeof(block) ;
        toFree->next = toFree->next->next ;
    }

    if (toFree->prev != NULL && toFree->prev->allocated == 'n')
        myfree(toFree->prev + 1) ;
}
