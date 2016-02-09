#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/cJSON/cJSON.h"
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

message * match_message_body(char *msg){
    char *title = NULL;
    char *content = NULL;
    cJSON *tmp;
    message *front = NULL;
    message *ptr = front;
    cJSON *root = cJSON_Parse(msg);

    if(root){
        tmp = cJSON_GetObjectItem(root, "title");
        if(tmp){
            title = tmp->valuestring;
            ptr = message_create(front, "title", title);
            if(!front) front = ptr;
        }
        
        tmp = cJSON_GetObjectItem(root, "desc");
        if(tmp){
            content = tmp->valuestring;
            ptr = message_create(front, "desc", content);
            if(!front) front = ptr;
        }
    }

    ptr->next = NULL;
    
    cJSON_Delete(root);
    return front;
}

