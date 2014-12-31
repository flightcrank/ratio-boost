
#include <stdio.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include "urle.h"	//encode hash
#include "hash.h"	//get torrent meta info
#include "blex.h"	//generate a linked list

void destroy (GtkWidget*, gpointer);
void open_file(GtkFileChooser *fc, gpointer data);
void send_request(GtkButton *button, gpointer data);
int tracker_connect(CURL *handle, char *request, struct responce *rdata);

struct torrent info = {{0}, {0}, {0}, 0};
struct responce resp = {{0}, {0}, -1, -1, -1};
char e_hash[64];	
char e_peer_id[64];	
char request[512];
long uploaded = 0;
FILE *torrent_file;

GtkBuilder *builder;
	
int main (int argc, char *argv[]) {
	
	gtk_init(&argc, &argv);
	  
	/* Construct a GtkBuilder instance and load our UI description */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ratio-boost.glade", NULL);
	
	/*connect singnals to callback functions */
	gtk_builder_connect_signals (builder, NULL);
	
	gtk_main();
	
	return 0;
}

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
		
		printf("seeders = %d leeches = %d update interval = %d\n",rdata->complete, rdata->incomplete, rdata->interval);
	}

	return 0;
}

void send_request(GtkButton *button, gpointer user_data) {

	GObject *label = gtk_builder_get_object(builder, "label6");
	
	//use libcurl to connect to the tracker server and issue a request
	CURL *curl_handle = curl_easy_init();

	//prepare the URL, and query string needed for a correct tracker responce.
	sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=0&downloaded=%lu&left=0&event=started&numwant=1&compact=1", info.url, e_hash, e_peer_id, info.size);	

	int tc = tracker_connect(curl_handle, request, &resp);
	
	//responce failure found exit program
	if (tc == 1) {
		
		puts("responce failure");
	
	} else {
	
		GString *output = g_string_new("");
		g_string_printf(output, "<b>Update Interval: </b> %d\n<b>Seeders: </b> %d\n<b>Leeches: </b> %d\n<b>Uploaded: </b>%ld", resp.interval, resp.complete, resp.incomplete, uploaded);
		gtk_label_set_markup(GTK_LABEL(label), output->str);
	}

	//free memory used by the curl easy interface
	curl_easy_cleanup(curl_handle);
}

void open_file(GtkFileChooser *fc, gpointer data) {
	
	GObject *label = gtk_builder_get_object(builder, "label2");
	GObject *message = gtk_builder_get_object(builder, "messagedialog1");

	char *str = gtk_file_chooser_get_filename(fc);

	//Open torrent file for processing
	torrent_file = fopen(str, "rb");	

	if (torrent_file == 0) {

		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "Error opening file");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		printf("Error opening file\n");
	}
	
	//Load all the required information found in the torrent file to a torrent structure
	int ti = load_torrent_info(torrent_file, &info);

	if (ti != 0) {
		
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "Invalid torrent file");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		printf("invalid torrent file or bencoded responce.\n");
	}

	//close file
	fclose(torrent_file);

	//convert some data to a format needed by the tracker
	urle(e_hash, info.info_hash);
	urle(e_peer_id, (unsigned char *) info.peer_id);
	
	GString *output = g_string_new("");
	g_string_printf(output, "<b>Info Hash: </b> %s\n<b>Peer_Id: </b>%s\n<b>Size: </b>%ld", e_hash, e_peer_id, info.size);
	gtk_label_set_markup(GTK_LABEL(label), output->str);
}

/* Stop the GTK+ main loop function when the window is destroyed. */
void destroy(GtkWidget *window, gpointer data) {
	
	gtk_main_quit();
}

