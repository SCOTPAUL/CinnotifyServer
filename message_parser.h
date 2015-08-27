#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

typedef struct message {
    char *header;
    char *content;
    struct message *next;
} message;

message * message_create(char *header, char *content);
void message_destroy(message *msg);

void get_message_regex(regex_t *reg_ptr);
message * match_message_body(char *msg, regex_t *regex);

#endif
