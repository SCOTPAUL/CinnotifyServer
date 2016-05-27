#pragma once

#include <stddef.h>
#include <libnotify/notify.h>
#include <glib.h>

/**
 * Sends a low priority notification to a libnotify compatible notification
 * server.
 * \param title the title of the notification
 * \param description the body of the notification
 * \param icon pointer to an icon, or NULL if default should be used
 */
void notify(const char *title, const char *description, GdkPixbuf *icon);

/**
 * Takes a JSON message in the Cinnotify format and handles the creation
 * and display of a notification based on it
 */
void create_and_send_notification(char *json_message);

