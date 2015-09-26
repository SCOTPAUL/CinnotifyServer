#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "message_parser.h"

message * message_create(message *prev, char *header, char *content){
    message *msg = malloc(sizeof(message));
    char *headcpy = strdup(header);
    char *contcpy = strdup(content);

    msg->header = headcpy;
    msg->content = contcpy;

    if(prev != NULL) prev->next = msg;

    return msg;
}

void message_destroy(message *msg){
    free(msg->header);
    free(msg->content);

    if(msg->next != NULL){
        message_destroy(msg->next);
    }

    free(msg);
}

void get_message_regex(regex_t *reg_ptr){
    int reti = regcomp(reg_ptr, "\\[([[:alnum:]]+):([^]]*)\\]", REG_EXTENDED|REG_ICASE);
    if(reti){
        fprintf(stderr, "%s\n", "Couldn't compile regex");
    }
}

message * match_message_body(char *msg, regex_t *regex){
    int reti;
    regmatch_t matches[3];

    message *ptr = NULL;
    message *front = ptr;

    char *str = msg;
    int last_end = 0;
    while(!(reti = regexec(regex, str, 3, matches, 0))){
        char header[matches[1].rm_eo - matches[1].rm_so + 1];
        char content[matches[2].rm_eo - matches[2].rm_so + 1];
        strncpy(header, str + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        strncpy(content, str + matches[2].rm_so,  matches[2].rm_eo - matches[2].rm_so);

        header[matches[1].rm_eo - matches[1].rm_so] = '\0';
        content[matches[2].rm_eo - matches[2].rm_so] = '\0';

        ptr = message_create(ptr, header, content);
        if(front == NULL) front = ptr;

        last_end = matches[0].rm_eo;
        str = &str[last_end];
    }

    ptr->next = NULL;

    return front;
}
