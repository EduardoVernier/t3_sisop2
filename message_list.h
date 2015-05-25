#ifndef MESSAGE_LIST
#define MESSAGE_LIST

typedef struct message_el {
	char *text;
	char *room;
	char *username;
	int   id;
	struct message_el *next;
} message;

typedef struct message_li {
	message *start;
	message *end;
} message_list;

typedef struct message_li_room {
	char name[30];
	message_list *ml;
	struct message_li_room *next;
} room;

message_list* newMessageList ();
message* newMessage(char *_text, char *_room, char *_username, int _id);
void addNewMessage (message_list* ml, message* m);
room* getRoomByName(char *_name, room* root);
room* createRoom(char *_name, message_list *_ml, room* _room);

#endif
