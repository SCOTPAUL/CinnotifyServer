#include <libnotify/notify.h>
#include <glib.h>

void notify(const char *title, const char *description){
	NotifyNotification *notification = notify_notification_new(title, description, "dialog-information");
	notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);
	notify_notification_show(notification, NULL);
	g_object_unref(G_OBJECT(notification));
}
