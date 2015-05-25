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
client* newClient(char *_username, int sock)
{
	client* c = (client*) malloc(sizeof(client));
	c->username = strdup(_username);
	c->sock = sock;
	return c;
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
