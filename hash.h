
struct torrent {
	
	unsigned char info_hash[20];
	char peer_id[20];
	char url[128];
	unsigned long size;
};

struct responce {

	char failure[256];
	char id[64];
	int interval;
	int complete;
	int incomplete;
};

int load_torrent_info(FILE *file_name, struct torrent *tdata);

void load_responce_info(FILE *file_name, struct responce *rdata);

void get_url(char *url, void *list);

void get_peer_id(char *peer_id);

void get_size(unsigned long *len, void *list);

int get_info_hash(FILE *filename, void *list, unsigned char *hash);

