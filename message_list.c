#include "message_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

message_list* newMessageList()
{
	message_list* ml = (message_list*) malloc (sizeof(message_list));
	ml->start = ml->end = NULL;
	return ml;
}

message* newMessage(char *_text, char *_room, char *_username, int _id)
{
	message* m = (message*) malloc(sizeof(message));
	m->text = strdup(_text);
	m->room = strdup(_room);
	m->username = strdup(_username);
	m->id = _id;
	return m;
}

void addNewMessage (message_list* ml, message* m)
{
	if (ml->start == NULL){
		ml->start = m;
		ml->end = m;
	}
	else{
		ml->end->next = m;
		ml->end = m;
	}
}

void printMessages (message_list *ml)
{
	message *m = ml->start;
	do{
		printf ("-> %s", m->text);
		m = m->next;
	}while(m != NULL);

	printf("\n...end print...\n");
}
