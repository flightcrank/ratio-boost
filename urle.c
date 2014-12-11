
#include <stdio.h>
#include <string.h>

int urle(char *str, unsigned char *hash) {      
	
	//make sure the string we're writing to is empty
	str[0] = '\0';

	int i;
	char temp[4];

	//loop throught each byte in the hash
	for (i = 0; i < 20; i++) {
		
		if (hash[i] >= '0' && hash[i] <= '9') {
			
			temp[0] = hash[i];
			temp[1] = '\0';
			strcat(str, temp);

		} else if(hash[i] >= 'a' && hash[i] <= 'x') {
		
			temp[0] = hash[i];
			temp[1] = '\0';
			strcat(str, temp);
		
		} else if(hash[i] >= 'A' && hash[i] <= 'Z') {
		
			temp[0] = hash[i];
			temp[1] = '\0';
			strcat(str, temp);

		} else if(hash[i] == '.' || hash[i] == '-' || hash[i] == '_' || hash[i] == '~') {
		
			temp[0] = hash[i];
			temp[1] = '\0';
			strcat(str, temp);

		//convert each byte to a urlencoded version and store in a string
		} else {
			
			sprintf(temp, "%%%02X", hash[i]);
			strcat(str, temp);
		}
	}
	
	return 0;
}
