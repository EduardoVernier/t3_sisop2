#include "client_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

client_list* newClientList ()
{
	client_list* cl = (client_list*) malloc (sizeof(client_list));
	cl->start = cl->end = NULL;
	return cl;
}
client* newClient(char *_username, int sock, char* _room)
{
	client* c = (client*) malloc(sizeof(client));
	c->username = strdup(_username);
	c->sock = sock;
	c->room = strdup(_room);
	return c;
}

void setRoom(client_list *cl, char *_username, char *_room)
{
	client *c = cl->start;
	while (1)
	{
		if (c == NULL) break;
		if (strcmp (c->username, _username)==0)
			strcpy(c->room, _room);
		//write(c->sock, c->text, strlen(c->text));
		//printf("wrote> %s\n", m->text);
		if (c->next == NULL) break;
		c = c->next;
	}
}

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
