#include <stdio.h>
#include<stdlib.h>
#include<string.h> 
#include "myMalloc.h"

typedef struct filedata {
	char * filename ;
	off_t size ;
	int block_num ;
	struct filedata *next ;
}node ;

node *head = NULL ;

void insert (char * filename, off_t size, int block_num){		
	//not bothering for the same filename for the time being
	//using mymalloc
	node *new = (node *)malloc(sizeof(node)) ; 
	new->filename  = (char *)malloc(strlen(filename)) ;
	strcpy(new->filename, filename);
	new->block_num = block_num ;
	new->size = size ;
	new->next = NULL ;
	
	if (head == NULL)
		head = new ;

	else{
		node *temp = head ;
		while (temp->next != NULL)
			temp  = temp->next ;

		temp->next = new ;	
	}
	
}

void display(char * total_files){

	if (head == NULL){
		total_files[0] = '\0' ;
		return ;
	}
	
	node *ptr = head ;

	while(ptr!= NULL){
		strcat(total_files, ptr->filename) ;
		strcat(total_files, ", ") ;
		ptr = ptr->next ;
	}

}

// if file is present, returns block number at the which the file header is present
// -1 otherwise
int if_present(char *filename){
	if (head == NULL)
		return -1 ;

	node * ptr = head ;
	while (ptr != NULL){
		if  (strcmp(ptr->filename, filename) == 0)
			return ptr->block_num ;

		ptr = ptr->next ;
	}

	return -1 ;
}