#include "client_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Allocates and initializes an empty client list */
client_list* newClientList ()
{
	client_list* cl = (client_list*) malloc (sizeof(client_list));
	cl->start = cl->end = NULL;
	return cl;
}

/* Allocates and initializes a new client */
client* newClient(char *_username, int sock, char* _room)
{
	client* c = (client*) malloc(sizeof(client));
	c->username = strdup(_username);
	c->sock = sock;
	c->room = strdup(_room);
	return c;
}

/* Appends a new client c in a given client_list */
void addNewClient(client_list *cl, client *c)
{
	if (cl->start == NULL)
	{
		cl->start = cl->end = c;
	}
	else
	{
		cl->end->next = c;
		cl->end = c;
	}

}

/* Sets the room in which the client is chatting*/
void setRoom(client_list *cl, char *_username, char *_room)
{
	client *c = cl->start;
	/* Client has to be found in the client_list cl */
	while (1)
	{
		if (c == NULL) break;
		if (strcmp (c->username, _username) == 0) // Found client
			strcpy(c->room, _room);
		if (c->next == NULL) break;
		c = c->next;
	}
}
