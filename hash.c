
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include "blex.h"
#include "hash.h"
#include "list.h"
#include "urle.h"

int load_torrent_info(FILE *file_name, struct torrent *tdata) {
	
	//create linked list of all torrent data
	struct element *list = generate_list(file_name);
	
	int v = validate(list);

	if (v == 1) {
		
		printf("validation of linked list failed");
		return 1;
	}

	get_info_hash(file_name, list, tdata->info_hash);
	get_peer_id(tdata->peer_id);
	get_url(tdata->url, list);
	get_size(&tdata->size, list);
	
	//linked list no longer needed, free the memory it uses
	remove_all_elements(list);

	return 0;
}

void load_responce_info(FILE *file, struct responce *rdata) {
	
	struct element *list = generate_list(file);
	struct element *temp = list;

	while (1) {
	
		if ((temp->type == 'S') && (strcmp(temp->value, "failure reason") == 0)) {
			
			temp = temp->next;
			
			//actual failure reasons string
			if ((temp->type == 'S')) {
			
				strcpy(rdata->failure, temp->value);
			}
		}
		
		if ((temp->type == 'S') && (strcmp(temp->value, "interval") == 0)) {
			
			temp = temp->next;

			if ((temp->type == 'I')) {
				
				 rdata->interval = atoi(temp->value);
			}
		}
		
		if ((temp->type == 'S') && (strcmp(temp->value, "complete") == 0)) {
		
			temp = temp->next;

			if ((temp->type == 'I')) {
				
				 rdata->complete = atoi(temp->value);
			}
		}
		
		if ((temp->type == 'S') && (strcmp(temp->value, "incomplete") == 0)) {
		
			temp = temp->next;

			if ((temp->type == 'I')) {
				
				 rdata->incomplete = atoi(temp->value);
			}
		}

		//last item on list
		if (temp->next == NULL) {
			
			break;

		//get next item in list
		} else {
		
			temp = temp->next;
		}
	}
}


void get_size(unsigned long *len, void *list) {

	*len = 0;

	struct element *temp = list;

	while (1) {
	
		if ((temp->type == 'S') && (strcmp(temp->value, "length") == 0)) {
			
			temp = temp->next;

			if ((temp->type == 'I')) {
				
				*len += atol(temp->value);
			}
		}
		
		//last item on list
		if (temp->next == NULL) {
			
			break;

		//get next item in list
		} else {
		
			temp = temp->next;
		}
	}
}

void get_url(char *url, void *list)  {

	struct element *temp = list;

	while (1) {
	
		//look for an element of type 'S' (string) with a value of announce
		if ((temp->type == 'S') && (strcmp(temp->value, "announce") == 0)) {
			
			//the next element has the value we need skip over to it
			temp = temp->next;
			
			//element should be of type 'S' with the value containing the URL
			if (temp->type == 'S') {
				
				strcpy(url, temp->value);
				break;
			}
		}
		
		//last item on list
		if (temp->next == NULL) {
			
			break;

		//get next item in list
		} else {
		
			temp = temp->next;
		}
	}
}

void get_peer_id(unsigned char *peer_id) {
	
	peer_id[0] = '-';
	peer_id[1] = 'T';
	peer_id[2] = 'R';
	peer_id[3] = '2';
	peer_id[4] = '8';
	peer_id[5] = '2';
	peer_id[6] = '0';
	peer_id[7] = '-';
	peer_id[8] =  rand() % 255;
	peer_id[9] = rand() % 255;
	peer_id[10] = rand() % 255;
	peer_id[11] = rand() % 255;
	peer_id[12] = rand() % 255;
	peer_id[13] = rand() % 255;
	peer_id[14] = rand() % 255;
	peer_id[15] = rand() % 255;
	peer_id[16] = rand() % 255;
	peer_id[17] = rand() % 255;
	peer_id[18] = rand() % 255;
	peer_id[19] = rand() % 255;
}

int get_info_hash(FILE *file, void *list, unsigned char *hash) {
	
	int start = 0;
	int end = 0;

	struct element *temp = list;

	while (1) {
		
		if ((temp->type == 'S') && (strcmp(temp->value, "info") == 0)) {
			
			temp = temp->next;

			if (temp->type == 'D') {

				start = temp->pos;
			}
		}
		
		//start has been found, now look for end
		if (start != 0) {
			
			//process reaches 'End' token that also has a value the same as start integer
			if ((temp->type == 'E') && (atoi(temp->value) == start)) {
				
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
	
	//make were at the beginning of the file
	rewind(file);

	unsigned char c = fgetc(file);
	int info_size = end - start;

	unsigned char *info_value = (unsigned char *) calloc(info_size + 1, sizeof(unsigned char));
	
	//set file start position
	fseek(file, start, SEEK_SET);
	
	int i = 0;

	//loop through every char in the file
	while (c != EOF){
		
		int pos = ftell(file);
		info_value[i] = c;

		//reached end position
		if (pos == end) {
			
			//info_value[i + 1] = '\0'; //fails with malloc must be accessing 1 element out size of allocated memory
			break;
		}
		
		c = fgetc(file);
		i++;
	}
	
	//set up the type of hashing function we want to use on the info_value array's data
	gsize size = g_checksum_type_get_length (G_CHECKSUM_SHA1);
	GChecksum *chksum = g_checksum_new (G_CHECKSUM_SHA1);
	g_checksum_update(chksum, info_value, info_size + 1);	//execute hashing function on the data
	g_checksum_get_digest(chksum, (guint8 *) hash, &size);	//store the hash as 8bit integers in a char array (hash)
	g_checksum_free(chksum);								//free mem used
	free(info_value); 										//free the calloc'd memory used by info_value used to generate the sha1 hash

	return 0; 
}
