#ifndef MESSAGE_LIST
#define MESSAGE_LIST

typedef struct message_el {
	char *text;
	char *room;
	//add receiver id
	struct message_el *next;	
} message;

typedef struct message_li {
	message *start;
	message *end;
} message_list;

message_list* newMessageList ();
message* newMessage(char *_text, char *_room);
void addNewMessage (message_list* ml, message* m);

#endif
