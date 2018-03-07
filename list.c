
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct element *current = NULL;

struct element * create_list() {
		
	struct element *temp =  malloc(sizeof(struct element));
	
	temp->type = '\0';
	temp->pos = -1;
	temp->next = NULL;

	return temp;
}

struct element * find_last_element(struct element *start) {
	
	current = start;
	
	while(1) {
		
		if (current->next != NULL) {
			
			current = current->next;
		
		//at last element
		} else {
			
			return current;
		}
	}
}

void print_all_elements(struct element *start) {
	
	current = start;
	
	while(1) {
		
		printf("%c(%d): %s \n",current->type, current->pos, current->value);
		
		if (current->next == NULL) {
			
			break;

		} else {
		
			current = current->next;
		}
	}
}

void remove_last_element(struct element *start) {
	
	current = start;

	struct element *prev = NULL;

	while(1) {
		
		//nothing in list at all
		if (current->type == '\0') {
			
			break;
		}

		//skip over all elements in the linked list
		if(current->next != NULL) {
			
			prev = current;
			current = current->next;
		
		//last element found
		} else {

			//remove last element
			free(current);

			//set previous element to be the new last element
			if (prev != NULL) {
				
				current = prev;
				current->next = NULL;
				break;
			
			//prev was never set, meaning we were at the first element with no other elements linked to it
			} else {
				
				//current->type = '\0';
				break;
			}
		}
	}
}

void add_element(struct element *start, char type, char value[], int pos) {

	current = start;

	
	//go to last element
	while (1) {
	
		//special case: linked list is empty
		if (current->type == '\0') {
		
			//set first element in linked list
			current->type = type;
			strcpy(current->value, value);
			current->pos = pos;
			current->next = NULL;
			start = current;
			break;
		}

		//skip over all elements in the linked list
		if(current->next != NULL) {
			
			current = current->next; 
		
		//last element found
		} else {
			
			//allocate memory for a new element structure
			current->next = malloc(sizeof(struct element));
			
			//switch to the new element
			current = current->next;
			
			//add data to that new element structure
			current->type = type;
			strcpy(current->value, value);
			current->pos = pos;
			current->next = NULL;
			break;
		}
	}
}

void remove_all_elements(struct element *start) {

	current = start;

	struct element *prev = NULL;

	while(1) {
		
		//nothing in list at all
		if (current->type == '\0') {
			
			free(current);
			break;
		}

		//skip over all elements in the linked list to remove the last element first
		if(current->next != NULL) {
			
			prev = current;
			current = current->next;
		
		//last element found
		} else {

			//remove last element
			free(current);

			if (prev != NULL) {
				
				current = prev;
				current->next = NULL;
				current = start;
				prev = NULL;
				continue;

			//prev was never set, meaning we were at the first element with no other elements linked to it
			} else {
				
				//first element already freed break out of the loop
				break;
			}
		}
	}
}

