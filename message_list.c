#include "message_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

room* getRoomByName(char *_name, room* root){
    int i;
    for(i=0; i<strlen(_name); i++)
        if(_name[i] == '\n')
            _name[i] = '\0';

    room* navegate = root;
    while(navegate!=NULL){
        if(strcmp(navegate->name, _name) == 0)
            return navegate;
        navegate = navegate->next;
    }

    printf("\nNao achou %s.\n", _name);

    return NULL;
}

room* createRoom(char *_name, message_list *_ml, room* _root){
    room* Room = (room*) malloc (sizeof(room));
    Room->ml = _ml;
	Room->ml->start = NULL;
	Room->ml->end = NULL;
	strcpy(Room->name, _name);

    room* navegate = _root;
    if(navegate != NULL){
        while(navegate->next != NULL){
            navegate = navegate->next;
        }
        navegate->next = Room;
	}
	Room->next = NULL;

    printf("\nconseguiu criar ok: %s.\n", Room->name);

    return Room;
}

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
}
