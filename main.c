
#include <stdio.h>
#include <time.h>
#include <curl/curl.h>
#include "hash.h"
#include "urle.h"

int main(int argc, char *argv[]) {
	
	if (argc <= 1) {
		
		printf("please specifiy a valid torrent file\n");
		return 1;
	}
	
	struct torrent info;
	char e_hash[64];	
	char e_peer_id[64];	
	char request[512];
	long uploaded = 0;
	int kb_sec = 1800;
	//FILE *file;
		
	load_torrent_info(argv[1], &info);
	urle(e_hash, info.info_hash);
	urle(e_peer_id, (unsigned char *) info.peer_id);

	sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=0&downloaded=%lu&"
			"left=0&event=started&numwant=1&compact=1", info.url, e_hash, e_peer_id, info.size);	

	//use cuel easy interface
	CURL *curl_handle = curl_easy_init();
	CURLcode res;	

	//valid handle
	if (curl_handle != NULL) {
		
		struct timespec start;
		struct timespec current;
		int i = 0;
		
		clock_gettime(CLOCK_MONOTONIC, &start);
		curl_easy_setopt(curl_handle, CURLOPT_URL, request);
	
		//preform the request
		res = curl_easy_perform(curl_handle);

		while (1) {
		
			clock_gettime(CLOCK_MONOTONIC, &current);

			int diff = current.tv_sec - start.tv_sec;
			
			//1 second passes
			if (diff >= kb_sec) {
				
				clock_gettime(CLOCK_MONOTONIC, &start);
				uploaded += (1024 * 30) * kb_sec;
				i++;
				
				sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413"
						"&uploaded=%ld&downloaded=%lu&"
						"left=0&event=&numwant=1&compact=1" 
						, info.url, e_hash, e_peer_id, uploaded, info.size);	

				//preform the request
				curl_easy_setopt(curl_handle, CURLOPT_URL, request);
				res = curl_easy_perform(curl_handle);
				printf("\n%i: %ld\n", i,  uploaded);	
			}
		}
	}
	
	//file = fopen("test.txt", "wb");	
	//curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file);
	//fclose(file);
	//free memory used by the curl easy interface
	curl_easy_cleanup(curl_handle);

	return 0;
}
