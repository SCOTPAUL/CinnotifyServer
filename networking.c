#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
* Cinnotify uses 32 bits at the start of a message to indicate the message size.
* This returns that value.
*/
uint32_t get_message_size(int connected_socket){
    int bytes_to_recv = 4;
    unsigned char buff[bytes_to_recv];

    int bytes_recieved = 0;
    bytes_recieved = recv(connected_socket, buff, bytes_to_recv, 0);
    bytes_to_recv -= bytes_recieved;
    while(bytes_to_recv > 0){
        bytes_recieved = recv(connected_socket, buff + (4 - bytes_to_recv), bytes_to_recv, 0);
        bytes_to_recv -= bytes_recieved;
    }

    return  (uint32_t) buff[0] << 24 |
            (uint32_t) buff[1] << 16 |
            (uint32_t) buff[2] << 8  |
            (uint32_t) buff[3];
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
    printf("listening on %s\n", s);

    freeaddrinfo(servinfo);

    return sockfd;
}
