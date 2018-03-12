
#include <stdio.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include <string.h>
#include <regex.h>
#include "urle.h"	//encode hash
#include "hash.h"	//get torrent meta info
#include "blex.h"	//generate a linked list

void destroy (GtkWidget*, gpointer);
void show_popup(GtkWidget *widget, GdkEvent *event);
void show_update(GtkWidget *widget, GdkEvent *event);
void show_about(GtkWidget *widget, GdkEvent *event);
void change_client();
void countdown();
void tracker_connect();
void convert_to_hex();
int open_file(GtkFileChooser *fc, gpointer data);
int send_request(GtkButton *button, gpointer data);
int regex_url();

struct torrent info = {{0}, {0}, {0}, 0};
struct responce resp = {{0}, {0}, -1, -1, -1};
char e_hash[64];		//for url encoded input to tracker
char e_peer_id[64];		//for url encoded input to tracker
char output_hash[64]; 	//for gui output
char output_peer_id[64];//for gui output
char output_url[64];	//for gui output
char request[512];
unsigned long uploaded = 0; //fake upload amount
unsigned long downloaded = 0; //fake download amount
double elapsed = 0; //time elapsed since an announce update was sent to the tracker 
FILE *torrent_file;
char user_update_flag = 0; //a Boolean that signifies id the user has set a custom update interval
int user_update = 0; //the amount of time a user has requested to perform regular updates to the tracker
char running = 0; // a Boolean flag that is set if the updates are running at the set update intervals or not
guint timer_id = 0;
GTimer *timer = NULL;

GtkBuilder *builder;
CURL *curl_handle = NULL;

int main (int argc, char *argv[]) {
	
	gtk_init(&argc, &argv);
	
	//use libcurl to connect to the tracker server and issue a request
	curl_handle = curl_easy_init();
	  
	/* Construct a GtkBuilder instance and load our UI description */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ratio-boost.glade", NULL);
	
	/*connect signals to callback functions */
	gtk_builder_connect_signals (builder, NULL);
        
    timer = g_timer_new();
    
    //main program loop	
	gtk_main();
	
    g_timer_destroy(timer);
	
	//free memory used by the curl easy interface
	curl_easy_cleanup(curl_handle);
	
	return 0;
}

//function to run at regular intervals to update labels on the GUI
void countdown() {
    
	GObject *upload_field = gtk_builder_get_object(builder, "upload_value");
	GObject *label_uploaded = gtk_builder_get_object(builder, "output_uploaded");
	GObject *label_update = gtk_builder_get_object(builder, "output_next_update");
	GString *output = g_string_new("");
	int upload_val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(upload_field));
    
    elapsed = g_timer_elapsed(timer, NULL);	//time elapse since timer has started
    
	uploaded += 1024 * upload_val;			//add amount uploaded in bytes
	float mb = (float)(uploaded / 1024) / 1024;
	g_string_printf(output, "%.2f", mb);
	gtk_label_set_text(GTK_LABEL(label_uploaded), output->str);
	g_string_printf(output, "%.0f", user_update - elapsed);
	gtk_label_set_text(GTK_LABEL(label_update), output->str);
	
	//countdown timer has reached 0 send data to toreent tracker
	if (user_update - elapsed <= 0) {
		
		tracker_connect();
	}
}

//show about dialog from popup menu
void show_about(GtkWidget *widget, GdkEvent *event) {
    
    GObject *about = gtk_builder_get_object(builder, "about_dialog");
    
    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_hide(GTK_WIDGET(about));
}

//show the right click popup menu
void show_popup(GtkWidget *widget, GdkEvent *event) {

	GObject *menu = gtk_builder_get_object(builder, "popup_menu");
	GObject *update = gtk_builder_get_object(builder, "update");
	GObject *client = gtk_builder_get_object(builder, "client");
	GdkEventButton *bevent = (GdkEventButton *) event;
  
	//disable the update option in the popup menu no torrent has been selected
	if (info.info_hash[0] == 0) {
		
		gtk_widget_set_sensitive(GTK_WIDGET(update), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(client), FALSE);
	
	} else {
	
		gtk_widget_set_sensitive(GTK_WIDGET(update), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(client), TRUE);
	}
		
    //right click
   if (bevent->button == 3) {  
       
       gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, bevent->button, bevent->time);
   }
}

void show_update(GtkWidget *widget, GdkEvent *event) {

	GObject *dialog = gtk_builder_get_object(builder, "update_dialog");
	GObject *spin = gtk_builder_get_object(builder, "update_spin_button");
	GObject *adj = gtk_builder_get_object(builder, "update_adj");
	GObject *label_interval = gtk_builder_get_object(builder, "output_update_interval");
	GString *output = g_string_new("");

	gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), resp.interval);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spin), GTK_ADJUSTMENT(adj));//fill the spin button with default update interval gained from the tracker
	
	int responce_id = gtk_dialog_run(GTK_DIALOG(dialog));
	
	//update GUI label to reflect new update interval
	if (responce_id == 0) {
		
		int val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
		user_update = val;
		
		g_string_printf(output, "%d", user_update);
		gtk_label_set_markup(GTK_LABEL(label_interval), output->str);
		
		//start countdown timer for the tracker_connect function to execute
        g_timer_start(timer); 
	}
	
	gtk_widget_hide(GTK_WIDGET(dialog));
}

//change the client from the pop up menu
void change_client(GtkMenuItem *item, GdkEvent *event) {
	
	const char *name = gtk_menu_item_get_label(item);
	GObject *label = gtk_builder_get_object(builder, "input_peer_id");

	if (strcmp(name, "KTorrent 5.1.0") == 0) {
		
		info.peer_id[0] = '-'; info.peer_id[1] = 'K'; info.peer_id[2] = 'T'; info.peer_id[3] = '5';
		info.peer_id[4] = '1'; info.peer_id[5] = '0'; info.peer_id[6] = '0'; info.peer_id[7] = '-';
	
	} else if (strcmp(name, "Transmission 2.8.2") == 0 ) {
		
		info.peer_id[0] = '-'; info.peer_id[1] = 'T'; info.peer_id[2] = 'R'; info.peer_id[3] = '2';
		info.peer_id[4] = '8'; info.peer_id[5] = '2'; info.peer_id[6] = '0'; info.peer_id[7] = '-';
	
	} else if (strcmp(name, "KTorrent 4.1.2") == 0) {
	
		info.peer_id[0] = '-'; info.peer_id[1] = 'K'; info.peer_id[2] = 'T'; info.peer_id[3] = '4';
		info.peer_id[4] = '1'; info.peer_id[5] = '2'; info.peer_id[6] = '0'; info.peer_id[7] = '-';
	}
	
	//re-encode urlencode the peer_id now that the client has been changed
	urle(e_peer_id, info.peer_id);
	
	//format some strings for better out put on the GUI (info hash and peer ID)	
	convert_to_hex();	

	gtk_label_set_text(GTK_LABEL(label), output_peer_id);
}

//perform connection to the torrent tracker
void tracker_connect() {

	GObject *connect_button = gtk_builder_get_object(builder, "connect");
	GObject *spinner = gtk_builder_get_object(builder, "spinner");
	GObject *download_field = gtk_builder_get_object(builder, "download_value");
	GObject *label_interval = gtk_builder_get_object(builder, "output_update_interval");
	GObject *label_seeders = gtk_builder_get_object(builder, "output_seeders");
	GObject *label_leechers = gtk_builder_get_object(builder, "output_leechers");
	GObject *label_uploaded = gtk_builder_get_object(builder, "output_uploaded");
	GObject *label_update = gtk_builder_get_object(builder, "output_next_update");
	GObject *message = gtk_builder_get_object(builder, "messagedialog1");
	int download_val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(download_field));
	
	//prepare the URL, and query string needed for a correct tracker response.
	sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=%ld&downloaded=%lu&left=0&event=&numwant=1&compact=1", info.url, e_hash, e_peer_id, uploaded, info.size);	
	
	FILE *output_file = tmpfile();

	if (curl_handle == NULL) {
		
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "CURL failed to initialize: NULL handle");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		printf("Curl failed to initialize: NULL Handel");
	}
	
	curl_easy_setopt(curl_handle, CURLOPT_URL, request);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, output_file);

	//preform the request
	curl_easy_perform(curl_handle);
	load_responce_info(output_file, &resp);
	fclose(output_file);
	
	if (resp.failure[0] != 0) {
		
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "Tracker response failure: see console output for more info");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		printf("response failure: %s\n", resp.failure);
		
		//stop running due to tracker responding with a failure
		running = 0;
		gtk_spinner_stop(GTK_SPINNER(spinner));
		gtk_button_set_label(GTK_BUTTON(connect_button), "Connect");
        g_timer_stop(timer);
		g_source_remove(timer_id);	//stop function from executing at regular intervals	
		uploaded = 0; 				//reset upload amount
		
	} else {
		
		if (user_update == 0) {
		
			user_update = resp.interval;
		}
		
		GString *output = g_string_new("");
		float mb = (float)(uploaded / 1024) / 1024;
		g_string_printf(output, "%d", user_update);
		gtk_label_set_text(GTK_LABEL(label_interval), output->str);
		g_string_printf(output, "%d", resp.complete);
		gtk_label_set_text(GTK_LABEL(label_seeders), output->str);
		g_string_printf(output, "%d", resp.incomplete);
		gtk_label_set_text(GTK_LABEL(label_leechers), output->str);
		g_string_printf(output, "%.2f", mb);
		gtk_label_set_text(GTK_LABEL(label_uploaded), output->str);
		g_string_printf(output, "%.0f",(int) user_update - elapsed);
		gtk_label_set_text(GTK_LABEL(label_update), output->str);
		
		downloaded += (1024 * resp.interval) * download_val;//in bytes
		printf("seeders = %d leeches = %d update interval = %d uploaded = %.2f\n",resp.complete, resp.incomplete, user_update, mb);
        g_timer_start(timer);//reset the countdown timer
	}
}

//connect button is clicked
int send_request(GtkButton *button, gpointer user_data) {

	GObject *connect_button = gtk_builder_get_object(builder, "connect");
	GObject *spinner = gtk_builder_get_object(builder, "spinner");
	GObject *message = gtk_builder_get_object(builder, "messagedialog1");
	
	//no torrent file selected exit function
	if (info.info_hash[0] == 0) {
		
		gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message), "Please select a valid torrent file");
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_hide(GTK_WIDGET(message));
		
		return 1;
	}
	
	//running flag, = 0 not running else = running
	if (running == 0) {
		
		running = 1;
		gtk_spinner_start(GTK_SPINNER(spinner));//start the spinning animation widget
		gtk_button_set_label(GTK_BUTTON(connect_button), "Disconnect");

		//prepare the URL, and query string needed for a correct tracker response.
		sprintf(request, "%s?info_hash=%s&peer_id=%s&port=51413&uploaded=0&downloaded=%lu&left=0&event=started&numwant=1&compact=1", info.url, e_hash, e_peer_id, info.size);	
	
		tracker_connect(&resp);
		
		//start countdown timer for the tracker_connect function to execute
        g_timer_start(timer); 
		
		//execute countdown function to execute ever second to update GUI labels
		timer_id = g_timeout_add_seconds(1, (GSourceFunc)countdown, NULL);
		
	} else { //Disconnect procedure
		
		running = 0;
		gtk_spinner_stop(GTK_SPINNER(spinner));
		gtk_button_set_label(GTK_BUTTON(connect_button), "Connect");
        g_timer_stop(timer);
		g_source_remove(timer_id);	//stop function from executing at regular intervals	
		uploaded = 0; 				//reset upload amount
	}
	
	return 0;
}

//file has been selected
int open_file(GtkFileChooser *fc, gpointer data) {
	
	GObject *label_info_hash = gtk_builder_get_object(builder, "input_info_hash");
	GObject *label_peer_id = gtk_builder_get_object(builder, "input_peer_id");
	GObject *label_size = gtk_builder_get_object(builder, "input_size");
	GObject *label_url = gtk_builder_get_object(builder, "input_url");
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
		printf("invalid torrent file or encoded response.\n");
		fclose(torrent_file);
	
		return 1;
	}

	//close file
	fclose(torrent_file);
	
	//convert some data to a format needed by the tracker
	urle(e_hash, info.info_hash);
	urle(e_peer_id, info.peer_id);
	
	//converts some data for display to the GUI
	convert_to_hex();
	
	//convert the annouce url from torrent struct to a basic url such as domain.com using posix regular expressions
	regex_url();
	
	float size = (info.size / 1024) / 1024;
	GString *output = g_string_new("");
	
	g_string_printf(output, "%.2f (MB)", size);
	gtk_label_set_text(GTK_LABEL(label_info_hash), output_hash);
	gtk_label_set_text(GTK_LABEL(label_peer_id), output_peer_id);
	gtk_label_set_text(GTK_LABEL(label_size), output->str);
	g_string_printf(output, "<a href=\"http://%s\">%s</a>", output_url, output_url);
	gtk_label_set_markup(GTK_LABEL(label_url), output->str);
	
	return 0;
}

// Stop the GTK+ main loop function when the window is destroyed.
void destroy(GtkWidget *window, gpointer data) {
	
	gtk_main_quit();
}

//converts the torrent info hash and peer id to hexadecimal strings
void convert_to_hex() {
	
	//format some strings for better out put on the GUI (info hash and peer ID)	
	char val[2];
	int i;
	
	//reset the output display strings so they don't get concatenated every time the function is called
	output_hash[0] = 0;
	output_peer_id[0] = 0;
	strncpy(output_peer_id, (char *) info.peer_id , 8);
	output_peer_id[8] = 0;//strncpy will not add a null byte so add one
	
	//copy bencoded torrent info hash into a more human readable format
	for(i = 0; i < 20; i++) {
		
		sprintf(val, "%02X", info.info_hash[i]);
		strncat(output_hash, val, 2);
	}	
	
	//copy bencoded torrent peer id into a more human readable format
	for(i = 8; i < 20; i++) {
	
		sprintf(val, "%02X", info.peer_id[i]);
		strncat(output_peer_id, val, 2);
	}
}

int regex_url() {

	//clear url in case it has already been written to. eg open file function has been opened before and written to it
	output_url[0] = '\0';
	puts(info.url);
	
	regex_t regex;
	int reti;
	regmatch_t pmatch;

	// Compile regular expression 
	reti = regcomp(&regex, "[[:alnum:]-]+\\.[[:alpha:]]{2,3}(\\/|:)", REG_EXTENDED);
	
	if (reti) {
		
		printf("Could not compile regex\n");
		return 1;
	}

	// Execute regular expression 
	reti = regexec(&regex, info.url, 1, &pmatch, 0);
	
	if (!reti) {
		
		int i;
		
		for (i = pmatch.rm_so; i < pmatch.rm_eo - 1; i++) {
			
			strncat(output_url, &info.url[i], 1);
		}
		
	} else if (reti == REG_NOMATCH) {
		
		puts("No match: regex expression could not match the url");
		output_url = "unknowen"
	}
	
	// Free memory allocated to the pattern buffer by regcomp() 
	regfree(&regex);
	
	return 0;	
}
