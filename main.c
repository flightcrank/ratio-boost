
#include <stdio.h>
#include <curl/curl.h>
#include "hash.h"
#include "urle.h"

int main(int argc, char *argv[]) {
	
	if (argc <= 1) {
		
		printf("please specifiy a valid torrent file\n");
		return 1;
	}
	
	struct torrent info;
	load_torrent_info(argv[1], &info);

	char e_hash[64];	
	char e_peer_id[64];	
	char request[512];
	
	urle(e_hash, info.info_hash);
	urle(e_peer_id, (unsigned char *) info.peer_id);

	sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=0&downloaded=0&left=1&event=started&compact=1", info.url, e_hash, e_peer_id);	

	//use cuel easy interface
	CURL *curl_handle = curl_easy_init();
	CURLcode res;	

	//valid handel
	if (curl_handle != NULL) {
		
 		curl_easy_setopt(curl_handle, CURLOPT_URL, request);

		//preform the request
		res = curl_easy_perform(curl_handle);
	
		printf("\n");

		// Check for errors 
   		 if (res != CURLE_OK) {
		 	
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
	}
	
	//free memory used by the curl easy interface
	curl_easy_cleanup(curl_handle);

	return 0;
}
