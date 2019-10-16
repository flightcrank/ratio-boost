
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"


int validate(struct element *list) {

	struct element *last = find_last_element(list);

	if (list->type == 'D' && last->type == 'E') {
		
		return 0;
	}

	return 1;
}

struct element * generate_list(FILE *file) {
	
	//make sure were at the beginning of the file
	rewind(file);

	struct element *temp = create_list();
	struct element *list = create_list();
	
	char str_len[24]; 	// maximum of 23 numbers
	char str_value[256];	// actual string 
	int num_index = 0;	
	int c = fgetc(file);
	
	//loop through every char in the file
	while (c != EOF){
	
		int pos = ftell(file);
		
		//end token found
		if (c == 'e') {
			
			struct element *end = find_last_element(temp);
			char num[10];
			sprintf(num, "%d", end->pos);

			add_element(list, 'E', num, pos);
			remove_last_element(temp);
		}

		//number token found
		if (c >= '0' && c <= '9') {
			
			str_len[num_index] = c;
			num_index++;
		}

		//end of string length number reached
		//beginning of string starts at the next char
		if (c == ':') {
			
			//add null byte to string length
			str_len[num_index] = '\0';

			//reset number index for next string length value
			num_index = 0;

			int len = atoi(str_len);
			int i = 0;

			//to big to put in our max string, don't try to write to the string
			//to avoid a segfault
			if (len > 255) {
				
				//skip over this data
				fseek(file, len, SEEK_CUR);
				continue;
			}

			//continue to read len number of chars into a string
			for (i = 0; i <= len; i++) {
				
				//add null byte to string
				if (i == len) {
					
					str_value[i] = '\0';
					add_element(list, 'S', str_value, pos + 1);
					break;
				}

				c = fgetc(file);
				str_value[i] = c;
			}
			
			//string has been processed and added to the linked list. consume the next char in the
			//file and start from the top of the while loop
			c = fgetc(file);
			continue;
		}

		//integer token found
		if (c == 'i') {
			
			add_element(temp, 'I', "Integer", pos);
			
			num_index = 0;
			c = fgetc(file);
			
			while (c != 'e') {
				
				//str_len[] can only hold 24 (0 - 23) values quit out of loop of this value is met 
				if (num_index == 23) {
					break;
				}

				str_len[num_index] = c;
				c = fgetc(file);
				num_index++;
			}
			
			str_len[num_index] = '\0';
			num_index = 0;

			add_element(list, 'I', str_len, pos);
			
			continue;
		}

		//dictionary token found
		if (c == 'd') {
		
			add_element(list, 'D', "Dictionary", pos);
			add_element(temp, 'D', "Dictionary", pos);
		}
		
		//list token found
		if (c == 'l') {
		
			add_element(list, 'L', "List", pos);
			add_element(temp, 'L', "List", pos);
		}

		c = fgetc(file);
	}
	

	return list;
}
