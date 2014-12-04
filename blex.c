
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token {
	
	char type[32];
	int pos;
	struct token *next;
};

struct token *start = NULL;
struct token *current = NULL;

void remove_last_element() {
	
	current = start;
	struct token *prev = NULL;

	while(1) {
		
		//nothing in list at all
		if (current == NULL) {
			
			break;
		}

		//skip over all elements in the linked list
		if(current->next != NULL) {
			
			prev = current;
			current = current->next;
		
		//last element found
		} else {

			printf("type = %s @ pos = %d\n", current->type, current->pos);

			//remove last elemnt
			free(current);

			//set previous element to be the new last element
			if (prev != NULL) {
				
				current = prev;
				current->next = NULL;
				break;
			
			//prev was never set, meaning we were at the first element with no other elements linked to it
			} else {
			
				break;
			}
		}
	}
}

void add_element(char type[], int pos) {

	current = start;
	
	//go to last element
	while (1) {
		
		//special case: linked list is empty
		if (current == NULL) {
		
			//set first element in linked list
			start = malloc(sizeof(struct token));
			strcpy(start->type, type);
			start->pos = pos;
			start->next = NULL;
			current = start;
			break;
		}

		//skip over all elements in the linked list
		if(current->next != NULL) {
			
			current = current->next; 
		
		//last element found
		} else {
			
			//allocate memory for a new token structure
			current->next = malloc(sizeof(struct token));
			
			//switch to the new element
			current = current->next;
			
			//add data to that new token structure
			strcpy(current->type, type);
			current->pos = pos;
			current->next = NULL;
			break;
		}
	}
}

int main() {
	
	FILE *file;
	
	//open file
	file = fopen("Pulp - Hits.torrent", "rb");	
	
	if (file == 0) {

		printf("Error opening file\n");
		
		return 1;
	}

	char str_len[10]; 	// maximum of nine numbers
	char str_value[256];	// actual string 
	int num_index = 0;	
	int c = fgetc(file);
	
	//loop through every char in the file
	while (c != EOF){
	
		int pos = ftell(file);
		
		//end token found
		if (c == 'e') {
			
			printf("END (%d): ", pos );

			//go to last added element

			//print type (and pos)

			//remove last added elemnt
			remove_last_element();
		}

		//number token found
		if (c >= '0' && c <= '9') {
			
			str_len[num_index] = c;
			num_index++;
		}

		//end of string length number reached
		//begining of string starts at the next char
		if (c == ':') {
			
			//add null byte to string length
			str_len[num_index] = '\0';

			//reset number index for next string length value
			num_index = 0;

			int len = atoi(str_len);
			int i = 0;

			//to big to put in our max string, dont try to write to the string
			//to avoid a segfault
			if (len > 255) {
				
				//skip over this data
				fseek(file, len, SEEK_CUR);
				continue;
			}

			//continue to read (str_len) number of chars into a string
			for (i = 0; i <= len; i++) {
				
				//add null byte to string
				if (i == len) {
					
					str_value[i] = '\0';
					break;
				}

				c = fgetc(file);
				str_value[i] = c;
			}

			printf("String \"%s\" (%d) \n", str_value, pos + 1);
		}

		//integer token found
		if (c == 'i') {
			
			printf("Integer (%d)\n", pos);
			add_element("integer", pos);
			
			num_index = 0;
			c = fgetc(file);
			
			while (c != 'e') {
				
				str_len[num_index] = c;
				c = fgetc(file);
				num_index++;
			}
			
			str_len[num_index] = '\0';
			num_index = 0;

			printf("%s (%d)\n", str_len, pos + 1);
			continue;
		}

		//dictionary token found
		if (c == 'd') {
		
			printf("Dictionary (%d)\n", pos );
			add_element("dictionary", pos);
		}
		
		//list token found
		if (c == 'l') {
		
			printf("List (%d)\n", pos );
			add_element("list", pos);
		}

		c = fgetc(file);
	}

	//close file
	fclose(file);
	

	return 0;
}

