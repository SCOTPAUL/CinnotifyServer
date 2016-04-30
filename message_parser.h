#pragma once

typedef struct message Message;

/**
 * Frees the memory used for the message, the header, content and so on
 * \arg msg The message to be destroyed
 */
void message_destroy(Message *msg);

/**
 * Given a JSON representation of a Cinnotify notification, return
 * a Message representation
 *
 * \arg json_msg JSON representation of a Cinnotify App notification
 * \return Pointer to a Message representing the Cinnotify notification
 */
Message *match_message_body(char *json_msg);

/**
 * Pops a field off of the message stack, setting header and content to the related fields of the message field. If this is not possible, 0 is returned and header, content are not set.
 *
 * Note that the header and content arguments are dynamically allocated, and so must be freed after use.
 *
 * \arg msg The message to obtain a field from
 * \arg header Address of pointer to be pointed at dynamically allocated field header string
 * \arg header Address of pointer to be pointed at dynamically allocated field content string
 * \return 1 if successful, 0 if there are no more fields
 */
int message_field_remove(Message *msg, char **header, char **content);
