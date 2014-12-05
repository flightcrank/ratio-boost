
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

int main() {
	
	struct element *list = create_list();
	struct element *lex_list = create_list();
	
	FILE *file;
	
	//open file
	file = fopen("Pulp - Hits.torrent", "rb");	
	
	if (file == 0) {

		printf("Error opening file\n");
		
		return 1;
	}

	char str_len[10]; 	// maximum of nine numbers
	char str_value[64];	// actual string 
	int num_index = 0;	
	int c = fgetc(file);
	
	//loop through every char in the file
	while (c != EOF){
	
		int pos = ftell(file);
		
		//end token found
		if (c == 'e') {
			
			struct element *end = find_last_element(list);
			char num[10];
			sprintf(num, "%d", end->pos);

			add_element(lex_list, 'E', num, pos);
			remove_last_element(list);
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
			if (len > 63) {
				
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

			//printf("String \"%s\" (%d) \n", str_value, pos + 1);
			add_element(lex_list, 'S', str_value, pos + 1);
		}

		//integer token found
		if (c == 'i') {
			
			add_element(list, 'I', "Integer", pos);
			
			num_index = 0;
			c = fgetc(file);
			
			while (c != 'e') {
				
				str_len[num_index] = c;
				c = fgetc(file);
				num_index++;
			}
			
			str_len[num_index] = '\0';
			num_index = 0;

			add_element(lex_list, 'I', str_len, pos);
			
			continue;
		}

		//dictionary token found
		if (c == 'd') {
		
			add_element(lex_list, 'D', "Dictionary", pos);
			add_element(list, 'D', "Dictionary", pos);
		}
		
		//list token found
		if (c == 'l') {
		
			add_element(lex_list, 'L', "List", pos);
			add_element(list, 'L', "List", pos);
		}

		c = fgetc(file);
	}

	//close file
	fclose(file);

	int start = 0;
	int end = 0;
	struct element *temp = lex_list;

	while (1) {
		
		if ((temp->type = 'S') && (strcmp(temp->value, "info") == 0)) {
			
			temp = temp->next;

			if (temp->type == 'D') {
			
				start = temp->pos;
			}
		}
		
		//start has been found
		if (start != 0) {
			
			//process rach 'End' token that also has a value the same as start integer
			if ((temp->type = 'E') && (atoi(temp->value) == start)) {
				
				end = temp->pos;
			}
		}
		
		if (temp->next == NULL) {
			
			break;

		} else {
		
			temp = temp->next;
		}
	}

	printf("start = %d, end = %d\n", start, end);

	return 0;
}

