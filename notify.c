#include <libnotify/notify.h>
#include <glib.h>

void notify(const char *title, const char *description){
	NotifyNotification *hello = notify_notification_new(title, description, "dialog-information");
	notify_notification_set_urgency(hello, NOTIFY_URGENCY_LOW);
	notify_notification_show(hello, NULL);
	g_object_unref(G_OBJECT(hello));
}
