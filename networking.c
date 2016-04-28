#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char * recv_all(int connected_socket , uint32_t size){
    uint32_t bytes_to_recv = size;
    uint32_t bytes_recieved = 0;
    char *buff = malloc(bytes_to_recv);

    bytes_recieved += recv(connected_socket, buff, bytes_to_recv, 0);
    while(bytes_recieved < bytes_to_recv){
        bytes_recieved += recv(connected_socket, buff + bytes_recieved, bytes_to_recv - bytes_to_recv, 0);
    }

    return buff;
}

static uint32_t char_array_to_uint(uint32_t num_bytes, char *buffer){
    uint32_t ret = 0;

    uint32_t i;
    for (i = 0; i < num_bytes; ++i) {
        ret <<= 8;
        ret |= (uint32_t) buffer[i];
    }

    return ret;
}

/**
 * Cinnotify uses 4 bytes at the start of a message to indicate the message size.
 * This returns that value.
 */
static uint32_t get_message_size(int connected_socket){
    char *buff = recv_all(connected_socket, 4);

    uint32_t ret = char_array_to_uint(4, buff);

    free(buff);
    return ret;
}

char * get_message_body(int connected_socket){
    uint32_t size = get_message_size(connected_socket);

    char *buff = malloc(size + 1);
    char *msg_body = recv_all(connected_socket, size);
    memcpy(buff, msg_body, size + 1);
    free(msg_body);
    buff[size] = '\0';
    return buff;
}

static void setup_addrinfo(struct addrinfo **servinfo, char *hostname, char *port, int flags){
    struct addrinfo hints;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = flags;

    if((rv = getaddrinfo(hostname, port, &hints, servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        freeaddrinfo(*servinfo);
        exit(1);
    }
}

int get_listener_socket_file_descriptor(char *port){
    int sockfd;
    struct addrinfo *servinfo, *p;
    char s[INET_ADDRSTRLEN];
    int yes = 1;

    setup_addrinfo(&servinfo, NULL, port, AI_PASSIVE);

    // Bind to the first possible result
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("socket");
            continue;
        }

        // Allow this port to be reused later
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit(1);
        }

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("connection");
            continue;
        }

        break;
    }

    if(p == NULL){
        fprintf(stderr, "%s\n", "server: failed to bind");
        exit(1);
    }

    inet_ntop(p->ai_family, &(((struct sockaddr_in *)p->ai_addr)->sin_addr), s, sizeof(s));
    printf("listening on %s:%s\n", s, port);

    freeaddrinfo(servinfo);

    return sockfd;
}
