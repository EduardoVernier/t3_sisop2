#include "message_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Allocates and initializes a new message list */
message_list* newMessageList()
{
	message_list* ml = (message_list*) malloc (sizeof(message_list));
	ml->start = ml->end = NULL;
	return ml;
}

/* Allocates and initializes a new message */
message* newMessage(char *_text, char *_room, char *_username, int _id)
{
	message* m = (message*) malloc(sizeof(message));
	m->text = strdup(_text);
	m->room = strdup(_room);
	m->username = strdup(_username);
	m->id = _id;
	return m;
}

/* Adds a given message m to a message_list ml*/
void addNewMessage (message_list* ml, message* m)
{
	if (ml->start == NULL)
	{
		ml->start = m;
		ml->end = m;
	}
	else
	{
		ml->end->next = m;
		ml->end = m;
	}
}

/* Debugging function */
void printMessages (message_list *ml)
{
	message *m = ml->start;
	do {
		printf ("-> %s", m->text);
		m = m->next;
	}while(m != NULL);
}
