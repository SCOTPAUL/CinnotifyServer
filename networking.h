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
* Finds the size of the body of the Cinnotify message
*
* \param connected_socket Listening server socket with active connection
*
* \returns Size of the main body of the message
*/
uint32_t get_message_size(int connected_socket);

#endif
