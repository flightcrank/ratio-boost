
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "blex.h"
#include "list.h"

int main() {
	
	struct element *list = generate_list();

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
	file = fopen("Pulp - Hits.torrent", "rb");	
	
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
	
	unsigned char hash[SHA_DIGEST_LENGTH]; // == 20
	
	SHA1(info_value, info_size + 1, hash);

	for (i = 0; i < 20; i++) {

		printf("%02X ", hash[i]);
	}

	printf("\n");

	free(info_value);
	
	return 0; 
}
