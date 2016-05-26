#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/cJSON/cJSON.h"
#include "message_parser.h"

struct message_field {
    char *header;
    char *content;
    struct message_field *next;
};

struct message {
    struct message_field *top;
};

Message *message_create(void){
    Message *msg = malloc(sizeof(Message));
    if(!msg){
        return NULL;
    }

    msg->top = NULL;

    return msg;
}

int message_field_add(Message *msg, char *header, char *content){
    struct message_field *msg_f = malloc(sizeof(struct message_field));
    if(!msg_f){
        return 0;
    }

    struct message_field *tmp = msg->top;
    char *headcpy = strdup(header);
    char *contcpy = strdup(content);

    msg_f->header = headcpy;
    msg_f->content = contcpy;

    msg->top = msg_f;
    msg_f->next = tmp;

    return 1;
}

int message_field_remove(Message *msg, char **header, char **content){
    struct message_field *msg_f = msg->top;
    if(!msg_f){
        return 0;
    }

    *header = msg_f->header;
    *content = msg_f->content;

    msg->top = msg_f->next;
    free(msg_f);

    return 1;
}

void message_destroy(Message *msg){
    struct message_field *next, *p = msg->top;
    free(msg);

    while(p){
        next = p->next;
        free(p->header);
        free(p->content);
        free(p);
        p = next;
    }
}

Message *match_message_body(char *json_msg){
    char *title = NULL;
    char *content = NULL;
    cJSON *tmp;
    Message *msg = message_create();
    if(!msg){
        return NULL;
    }

    cJSON *root = cJSON_Parse(json_msg);

    // TODO: Refactor this mess
    if(root){
        tmp = cJSON_GetObjectItem(root, "title");
        if(tmp){
            title = tmp->valuestring;
            message_field_add(msg, "title", title);
        }
        
        tmp = cJSON_GetObjectItem(root, "desc");
        if(tmp){
            content = tmp->valuestring;
            message_field_add(msg, "desc", content);
        }
        tmp = cJSON_GetObjectItem(root, "b64Icon");
        if(tmp){
            content = tmp->valuestring;
            message_field_add(msg, "b64Icon", content);
        }
    }

    cJSON_Delete(root);
    return msg;
}

