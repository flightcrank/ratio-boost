
struct torrent {
	
	unsigned char info_hash[20];
	char peer_id[20];
	char url[128];
};

void load_torrent_info(char *file_name, struct torrent *tdata);

void get_url(char *url, void *list);

void get_peer_id(char *peer_id);

int get_info_hash(char *filename, void *list, unsigned char *hash);

