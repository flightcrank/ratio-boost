
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "blex.h"
#include "list.h"

int get_info_hash(char *file_name, unsigned char *hash) {
	
	struct element *list = generate_list(file_name);

	int start = 0;
	int end = 0;
	struct element *temp = list;

	while (1) {
		
		if ((temp->type = 'S') && (strcmp(temp->value, "info") == 0)) {
			
			temp = temp->next;

			if (temp->type == 'D') {

				start = temp->pos;
			}
		}
		
		//start has been found, now look for end
		if (start != 0) {
			
			//process rach 'End' token that also has a value the same as start integer
			if ((temp->type = 'E') && (atoi(temp->value) == start)) {
				
				end = temp->pos;
			}
		}
		
		//get next item in list
		if (temp->next == NULL) {
			
			break;

		} else {
		
			temp = temp->next;
		}
	}
	
	FILE *file;
	
	//open file
	file = fopen(file_name, "rb");	
	
	if (file == 0) {

		printf("Error opening file\n");
		
		return 0;
	}

	unsigned char c = fgetc(file);
	int info_size = end - start;

	unsigned char *info_value = malloc(info_size + 1);
	
	//set file start position
	fseek(file, start, SEEK_SET);
	
	int i = 0;

	//loop through every char in the file
	while (c != EOF){
		
		int pos = ftell(file);
		info_value[i] = c;

		//reached end position
		if (pos == end) {
			
			info_value[i + 1] = '\0';
			break;
		}
		
		c = fgetc(file);
		i++;
	}
	
	
	SHA1(info_value, info_size + 1, hash);
	free(info_value);

	/*
	//generate peer id
	hash[0] = '-';
	hash[1] = 'T';
	hash[2] = 'R';
	hash[3] = '2';
	hash[4] = '8';
	hash[5] = '2';
	hash[6] = '0';
	hash[7] = '-';
	hash[8] =  rand() % 256;
	hash[9] = rand() % 256;
	hash[10] = rand() % 256;
	hash[11] = rand() % 256;
	hash[12] = rand() % 256;
	hash[13] = rand() % 256;
	hash[14] = rand() % 256;
	hash[15] = rand() % 256;
	hash[16] = rand() % 256;
	hash[17] = rand() % 256;
	hash[18] = rand() % 256;
	hash[19] = rand() % 256;
	
	urle(e_hash, hash);

	printf("%s\n", e_hash);
	*/
	
	return 0; 
}
