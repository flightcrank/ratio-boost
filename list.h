
//linked list header file

struct element {
	
	char type;		//token type
	char value[256];	//token value
	int pos;		//char position in file
	struct element *next;	//next element in list
};

struct element * create_list();

struct element * find_last_element(struct element *start);

void print_all_elements(struct element *start);

struct element * remove_last_element(struct element *start);

void add_element(struct element *start, char type, char value[], int pos);
