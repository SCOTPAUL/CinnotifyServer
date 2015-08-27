#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <libnotify/notify.h>
#include <regex.h>
#include "networking.h"
#include "message_parser.h"
#include "notify.h"

#define PORT "6525"
#define BACKLOG 15
#define BUFFER_SIZE 1000

void print_usage_and_quit(char *application_name);

void sigchld_handler(int sig){
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]){
    int sockfd, new_fd;
    struct sigaction sa;
    struct sockaddr_storage their_addr; // Address information of client
    socklen_t sin_size;
    char s[INET_ADDRSTRLEN];

    sockfd = get_listener_socket_file_descriptor(PORT);

    if(listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // Reap dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    }
    printf("%s\n", "server: waiting for connections");

    regex_t regex;
    get_message_regex(&regex);
    notify_init(argv[0]);
    while(1){
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_fd == -1){
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, &(((struct sockaddr_in *)&their_addr)->sin_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);


        if(!fork()){ // We are the child process
            close(sockfd);
            char *msg_body = get_message_body(new_fd);

            message *messages = match_message_body(msg_body, &regex);
            message *ptr = messages;

            char *title = NULL, *body = NULL;
            for(ptr = ptr->next; ptr != NULL; ptr = ptr->next){
                if(strcmp(ptr->header, "title") == 0) title = ptr->content;
                else if(strcmp(ptr->header, "desc") == 0) body = ptr->content;
            }

            if(title != NULL){
                if(body != NULL){
                    notify(title, body);
                }
                else{
                    notify(title, "");
                }
            }

            message_destroy(messages);
            free(msg_body);

            close(new_fd);
            exit(0);
        }
    }
    notify_uninit();
    regfree(&regex);
    close(new_fd);
    return 0;

}

void print_usage_and_quit(char *application_name){
    printf("Usage: %s\n", application_name);
    exit(1);
}
