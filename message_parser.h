#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

typedef struct message {
    char *header;
    char *content;
    struct message *next;
} message;

/**
 * Creates a new message, and allocates memory for the message's header and content.
 */
message * message_create(message *prev, char *header, char *content);

/**
 * Frees the memory used for the message, the header, content and so on if
 * msg->next points to another message.
 */
void message_destroy(message *msg);

message * match_message_body(char *msg);

#endif
