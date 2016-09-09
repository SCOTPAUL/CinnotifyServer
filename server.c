#include "main.h"
#include "crypto.h"

#define BACKLOG 15
#define BUFFER_SIZE 1000

int terminate = 0;

void print_usage_and_quit();

void sigchld_handler(__attribute__((unused)) int sig){
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void sigint_handler(__attribute__((unused)) int signum){
    terminate = 1;
    printf("Terminating\n");
}

int get_ip(char *buf, size_t buf_len){
    struct ifaddrs *addrs, *temp;
    int status = -1;
    
    getifaddrs(&addrs);
    temp = addrs;
    
    while(temp){
        if(!temp->ifa_addr) continue;
        
        if(strcmp(temp->ifa_name, "lo") != 0 && temp->ifa_addr->sa_family == AF_INET){
            status = getnameinfo(temp->ifa_addr, sizeof(struct sockaddr), buf, buf_len, NULL, 0, NI_NUMERICHOST);
            break;
        }

        temp = temp->ifa_next;
    }

    freeifaddrs(addrs);
    return status;
}

int main(int argc, char *argv[]){
    int sockfd, new_fd;
    struct sigaction sa;
    struct sockaddr_storage their_addr; // Address information of client
    socklen_t sin_size;
    char s[INET_ADDRSTRLEN];
    char ip_buf[BUFFER_SIZE];
    char *port = "6525";
    long port_num = 6525;

    int encrypted = 0;
    char *password;

    int option, option_index = 0;

    struct option long_options[] = {
        {"silent",  no_argument,       0, 's'},
        {"port",    required_argument, 0, 'p'},
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'v'},
        {"encrypt", required_argument, 0, 'e'},
        {0,         0,                 0,  0 } // End of array
    };

    while((option = getopt_long(argc, argv,"vshp:e:", long_options, &option_index)) != -1){
        switch(option){
            case 's':
                freopen("/dev/null", "w", stdout);
                freopen("/dev/null", "w", stderr);
                break;
            case 'p':
                // TODO: free this string!
                port = strdup(optarg);
                
                port_num = strtol(port, NULL, 10);

                char *ptr = port;
                while(*ptr){
                    if(!isdigit(*ptr)) print_usage_and_quit();
                    ++ptr;
                }
                break;
            case 'e':
                encrypted = 1;
                // TODO: free this string!
                password = strdup(optarg);
                break;
            case 'v':
                printf("%s - %s (Compiled %s %s)\n", NAME, VERSION, __DATE__, __TIME__);
                exit(0);
                break;
            case 'h': // Fallthrough
            default: print_usage_and_quit();
        }
    }


    struct sigaction sigint_action;
    sigint_action.sa_flags = 0;
    sigint_action.sa_handler = &sigint_handler;
    sigemptyset (&sigint_action.sa_mask);
    
    if (sigaction(SIGINT, &sigint_action, NULL) < 0) {
        perror("SIGINT error");
    }

    if(get_ip(ip_buf, BUFFER_SIZE) == 0){
        printf("This machine has address: %s\n", ip_buf);
    }
    else {
        printf("%s\n", "Getting IP address failed");
        exit(1);
    }

    sockfd = get_listener_socket_file_descriptor(port);

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
    
    start_service_broadcast((uint16_t) port_num);
    printf("%s\n", "server: waiting for connections");

    notify_init(argv[0]);
    while(!terminate){
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
          
            uint32_t message_size;
            char *msg_body = get_message_body(new_fd, &message_size);

            if(encrypted){
                crypto_init();
                
                char *message = malloc(message_size + 1);
                message[message_size] = '\0';

                decrypt(msg_body, message_size, password, message);
                
                create_and_send_notification(message);
                free(message);

                crypto_deinit();
            }
            else {
                create_and_send_notification(msg_body);
            }

            
            free(msg_body);
            close(new_fd);
            exit(0);
        }
    }
    
    stop_service_broadcast();
    notify_uninit();
    close(new_fd);
    close(sockfd);
    return 0;

}

void print_usage_and_quit(){
    printf("Usage: %s [OPTION...]\n\n"
            "-h --help                Displays this information\n"
            "-s --silent              Redirects outputs to /dev/null\n"
            "-e --encrypt <password>  Enables the decryption of messages\n"
            "-p --port <portnum>      Sets the binding port to <portnum>\n"
            "-v --version             Print current version number\n", 
            NAME);
    exit(1);
}


