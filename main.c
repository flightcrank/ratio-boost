
#include <stdio.h>
#include "hash.h"
#include "urle.h"

int main(int argc, char *argv[]) {
	
	unsigned char hash[20];	//SHA1 hash
	char e_hash[64];	//URL encoded SHA1 hash
	
	if (argc <= 1) {
		
		printf("please specifiy a valid torrent file\n");
		return 1;
	}

	get_info_hash(argv[1], hash);
	urle(e_hash, hash);

	int i;

	for (i = 0; i < 20; i++) {

		printf("%02X ", hash[i]);
	}

	printf("\n%s\n", e_hash);

	return 0;
}
