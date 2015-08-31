#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * Sends a low priority notification to a libnotify compatible notification
 * server.
 */
void notify(const char *title, const char *description);

#endif
