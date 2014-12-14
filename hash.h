
struct torrent {
	
	unsigned char info_hash[20];
	char peer_id[20];
	char url[128];
	unsigned long size;
};

void load_torrent_info(char *file_name, struct torrent *tdata);

void get_url(char *url, void *list);

void get_peer_id(char *peer_id);

void get_size(unsigned long *len, void *list);

int get_info_hash(char *filename, void *list, unsigned char *hash);

