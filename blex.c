
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token {
	
	char type[32];
	struct token *next;
};

int create_token(size_t token_size) {
	
	return 0;
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
	char str_value[255];	// actual string 
	int num_index = 0;	
	int c = fgetc(file);
	
	//loop through every char in the file
	while (c != EOF){
	
		int pos = ftell(file);
		
		//printf("%d\n", pos);

		if (pos > 226) {
			
			break;
		}

		//number token found
		if (c >= '0' && c <= '9') {
			
			str_len[num_index] = c;
			num_index++;
		}

		//end of string length number reached
		if (c == ':') {
			
			//add null byte to string length
			str_len[num_index] = '\0';

			//reset number index for next string length value
			num_index = 0;

			int len = atoi(str_len);
			int i = 0;

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

			printf("%s\n", str_value);
		}
		
		//end  token found
		if (c == 'e') {
			
			puts("End last Integer, List or Dictionary");
		}

		//integer token found
		if (c == 'i') {
			
			puts("Integer");
			
			num_index = 0;
			c = fgetc(file);
			
			while (c != 'e') {
				
				str_len[num_index] = c;
				c = fgetc(file);
				num_index++;
			}
			
			str_len[num_index] = '\0';
			num_index = 0;

			printf("%s\n", str_len);
			continue;
		}

		//dictionary token found
		if (c == 'd') {
		
			puts("Dictionary");
		}

		c = fgetc(file);
	}

	//close file
	fclose(file);

	return 0;
}

