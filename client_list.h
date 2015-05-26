#ifndef CLIENT_LIST
#define CLIENT_LIST

typedef struct client_el {
	int sock;
	char *username;
	char *room;
	struct client_el *next;	
} client;

typedef struct client_li {
	client *start;
	client *end;
} client_list;

client_list* newClientList ();
client* newClient(char *_user, int sock, char* _room);
void addNewClient(client_list *cl, client *c);
void setRoom(client_list *cl, char *_username, char *_room);

#endif
