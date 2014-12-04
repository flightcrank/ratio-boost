
//linked list header file

struct element {
	
	char type[32];
	int pos;
	struct element *next;
};

struct element * create_list();

void remove_last_element(struct element *start);

struct element * find_last_element(struct element *start);

void add_element(struct element *start, char type[], int pos);
