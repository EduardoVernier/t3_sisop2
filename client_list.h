#ifndef CLIENT_LIST
#define CLIENT_LIST

typedef struct client_el {
	int sock;
	int online;
	char *username;
	struct client_el *next;	
} client;

typedef struct client_li {
	client *start;
	client *end;
} client_list;

client_list* newClientList ();
client* newClient(char *_user, int sock);

#endif
