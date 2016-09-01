#ifndef NETWORKING_H
#define NETWORKING_H

/**
 * Returns the file descriptor of a listening socket.
 *
 * If a connection cannot be established, the process is terminated.
 *
 * \param port Port number to listen on
 *
 * \returns The file descriptor of a stream socket
 */
int get_listener_socket_file_descriptor(char *port);

/**
 * Returns the message body of a notification transmission waiting on connected_socket.
 *
 * \param connected_socket The process' listening socket
 * \param message_size if not NULL, the size of the message will be written here
 *
 * \returns A null terminated string formatted as a Cinnotify message
 */
char * get_message_body(int connected_socket, uint32_t *message_size);

#endif
