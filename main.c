
#include <stdio.h>
#include <time.h>
#include <curl/curl.h>
#include "urle.h"	//encode hash
#include "hash.h"	//get torrent meta info
#include "list.h"	//print linked list
#include "blex.h"	//generate a linked list

//perform connection to the torrent tracker
int tracker_connect(CURL *handle, char *request, struct responce *rdata) {
	
	if (handle == NULL) {

		return 1;
	}

	FILE *output_file = tmpfile();
	//CURLcode res;	
	
	curl_easy_setopt(handle, CURLOPT_URL, request);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, output_file);
	
	//preform the request
	curl_easy_perform(handle);
	load_responce_info(output_file, rdata);
	fclose(output_file);
	
	if (rdata->failure[0] != 0) {
		
		printf("%s\n", rdata->failure);
		
		return 1;

	} else {
		
		printf("complete = %d incomplete = %d interval = %d\n",rdata->complete, rdata->incomplete, rdata->interval);
	}

	return 0;
}

int main(int argc, char *argv[]) {
	
	if (argc <= 1) {
		
		printf("please specifiy a valid torrent file\n");
		return 1;
	}
	
	struct torrent info = {{0}, {0}, {0}, 0};
	struct responce resp = {{0}, {0}, -1, -1, -1};
	char e_hash[64];	
	char e_peer_id[64];	
	char request[512];
	long uploaded = 0;
	FILE *torrent_file;
	
	//Open torrent file for processing
	torrent_file = fopen(argv[1], "rb");	

	if (torrent_file == 0) {

		printf("Error opening file\n");
		
		return 0;
	}
	
	//Load all the required information found in the torrent file to a torrent structure
	load_torrent_info(torrent_file, &info);
	fclose(torrent_file);
	
	//convert some data to a format needed by the tracker
	urle(e_hash, info.info_hash);
	urle(e_peer_id, (unsigned char *) info.peer_id);

	//prepare the URL, and query string needed for a correct tracker responce.
	sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=0&downloaded=%lu&left=0&event=started&numwant=1&compact=1", info.url, e_hash, e_peer_id, info.size);	

	//use libcurl to connect to the tracker server and issue a request
	CURL *curl_handle = curl_easy_init();

	int tc = tracker_connect(curl_handle, request, &resp);
	
	if (tc == 1) {
		
		return 1;
	}

	//prepare to loop at regular intervals
	struct timespec start;
	struct timespec current;
	int kb_sec = 1024;
	int i = 0;
	
	clock_gettime(CLOCK_MONOTONIC, &start);
	
	while (1) {
	
		clock_gettime(CLOCK_MONOTONIC, &current);
		int diff = current.tv_sec - start.tv_sec;

		//resp->interval number of seconds has passed
		if (diff >= resp.interval) {
			
			clock_gettime(CLOCK_MONOTONIC, &start);
			uploaded += (1024 * resp.interval) * kb_sec;
			i++;
			
			//update request query
			sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=%ld&downloaded=%lu&left=0&event=&numwant=1&compact=1", info.url, e_hash, e_peer_id, uploaded, info.size);	
	
			//preform the request
			tracker_connect(curl_handle, request, &resp);
			printf("\n%i: %ld\n", i,  uploaded);	
		}
	}

	//free memory used by the curl easy interface
	curl_easy_cleanup(curl_handle);

	return 0;
}
