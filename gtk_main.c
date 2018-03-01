
#include <stdio.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include <string.h>
#include "urle.h"	//encode hash
#include "hash.h"	//get torrent meta info
#include "blex.h"	//generate a linked list

void destroy (GtkWidget*, gpointer);
void open_file(GtkFileChooser *fc, gpointer data);
void send_request(GtkButton *button, gpointer data);
void tracker_connect();

struct torrent info = {{0}, {0}, {0}, 0};
struct responce resp = {{0}, {0}, -1, -1, -1};
char e_hash[64];	
char e_peer_id[64];	
char request[512];
unsigned long uploaded = 0;
unsigned long downloaded = 0;
FILE *torrent_file;
guint id = 0;

GtkBuilder *builder;
CURL *curl_handle = NULL;

int main (int argc, char *argv[]) {
	
	gtk_init(&argc, &argv);
	
	//use libcurl to connect to the tracker server and issue a request
	curl_handle = curl_easy_init();
	  
	/* Construct a GtkBuilder instance and load our UI description */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ratio-boost.glade", NULL);
	
	/*connect singnals to callback functions */
	gtk_builder_connect_signals (builder, NULL);
	
	//main program loop	
	gtk_main();
	
	//free memory used by the curl easy interface
	curl_easy_cleanup(curl_handle);
	
	return 0;
}

//perform connection to the torrent tracker
void tracker_connect() {

	GObject *upload_field = gtk_builder_get_object(builder, "upload_value");
	GObject *download_field = gtk_builder_get_object(builder, "download_value");
	GObject *output_label = gtk_builder_get_object(builder, "output_data");
	GObject *message = gtk_builder_get_object(builder, "messagedialog1");
	int upload_val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(upload_field));
	int download_val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(download_field));
	
	//prepare the URL, and query string needed for a correct tracker responce.
	sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=%ld&downloaded=%lu&left=0&event=&numwant=1&compact=1", info.url, e_hash, e_peer_id, uploaded, info.size);	
	
	FILE *output_file = tmpfile();

	if (curl_handle == NULL) {
		
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "CURL failed to initialse: NULL handle");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		printf("Curl failed to initialise: NULL handel");
	}
	
	curl_easy_setopt(curl_handle, CURLOPT_URL, request);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, output_file);

	//preform the request
	curl_easy_perform(curl_handle);
	load_responce_info(output_file, &resp);
	fclose(output_file);
	
	if (resp.failure[0] != 0) {
		
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "Tracker responce failure");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		printf("responce failure: %s\n", resp.failure);
		
	} else {
		
		GString *output = g_string_new("");
		float mb = (float)(uploaded / 1024) / 1024;
		g_string_printf(output, "<b>Update Interval: </b> %d\n<b>Seeders: </b> %d\n<b>Leeches: </b> %d\n<b>Uploaded (MB): </b>%.2f", resp.interval, resp.complete, resp.incomplete, mb);
		gtk_label_set_markup(GTK_LABEL(output_label), output->str);
		uploaded += (1024 * resp.interval) * upload_val;//in bytes
		downloaded += (1024 * resp.interval) * upload_val;//in bytes
		printf("seeders = %d leeches = %d update interval = %d uploaded = %.2f\n",resp.complete, resp.incomplete, resp.interval, mb);
	}
}

void send_request(GtkButton *button, gpointer user_data) {

	GObject *connect_button = gtk_builder_get_object(builder, "connect");
	GObject *output_label = gtk_builder_get_object(builder, "output_data");
	GObject *spinner = gtk_builder_get_object(builder, "spinner");

	const gchar *button_label = gtk_button_get_label(GTK_BUTTON(connect_button));

	if (strcmp(button_label, "Connect") == 0) {
		
		GString *output = g_string_new("");
		gtk_spinner_start(GTK_SPINNER(spinner));//start the spinning animation widgit
		gtk_button_set_label(GTK_BUTTON(connect_button), "Disconnect");

		g_string_printf(output, "<b>Update Interval (seconds): </b> %d\n<b>Seeders: </b> %d\n<b>Leeches: </b> %d\n<b>Uploaded (MB): </b>%ld", resp.interval, resp.complete, resp.incomplete, uploaded);
		gtk_label_set_markup(GTK_LABEL(output_label), output->str);

		//prepare the URL, and query string needed for a correct tracker responce.
		sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=0&downloaded=%lu&left=0&event=started&numwant=1&compact=1", info.url, e_hash, e_peer_id, info.size);	
	
		tracker_connect(&resp);
		
		//execute function at regular intervals
		id = g_timeout_add_seconds(resp.interval, (GSourceFunc)tracker_connect, NULL);
		
	} else { //Disconnect procedure
	
		gtk_spinner_stop(GTK_SPINNER(spinner));
		gtk_button_set_label(GTK_BUTTON(connect_button), "Connect");
		g_source_remove(id); //stop function from executing at regualr intervals	
		uploaded = 0; //reset upload amount
	}
}

void open_file(GtkFileChooser *fc, gpointer data) {
	
	GObject *label = gtk_builder_get_object(builder, "input_data");
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
	urle(e_peer_id, info.peer_id);
	
	//format some strings for better out put on the GUI (info hash and peer ID)	
	char val[2];
	char torrent_hash[56];
	char p_id[56];
	char client[8];
	int i;
	strcpy(client, info.peer_id);
	torrent_hash[0] = '\0';
	p_id[0] = '\0';
	client[8] = '\0';
	
	for(i = 0; i < 20; i++) {
		
		sprintf(val, "%02X", info.info_hash[i]);
		strncat(torrent_hash, val, 2);
	}	
	
	for(i = 8; i < 20; i++) {
	
		sprintf(val, "%02X", info.peer_id[i]);
		strncat(p_id, val, 2);
	}
	
	GString *output = g_string_new("");
	float size = (info.size / 1024) / 1024;
	g_string_printf(output, "<b>Info Hash: </b> %s\n<b>Peer_Id: </b>%s%s\n<b>Size (MB): </b>%.2f",torrent_hash, client, p_id, size );
	gtk_label_set_markup(GTK_LABEL(label), output->str);
}

/* Stop the GTK+ main loop function when the window is destroyed. */
void destroy(GtkWidget *window, gpointer data) {
	
	gtk_main_quit();
}

