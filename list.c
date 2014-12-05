
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct element *current = NULL;

struct element * create_list() {
		
	struct element *temp =  malloc(sizeof(struct element));
	
	temp->type = '\0';
	//temp->value = "";
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
		current = current->next;
		
		if (current->next == NULL) {
			
			break;
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


			//remove last elemnt
			free(current);

			//set previous element to be the new last element
			if (prev != NULL) {
				
				current = prev;
				current->next = NULL;
				break;
			
			//prev was never set, meaning we were at the first element with no other elements linked to it
			} else {
				
				start = NULL;
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

