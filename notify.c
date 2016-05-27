#include "notify.h"
#include "lib/cJSON/cJSON.h"

void free_icon_data(guchar *pixels, gpointer data){
    //unused
    (void) data;
    
    g_free(pixels);
}

void create_and_send_notification(char *json_message){
    cJSON *tmp, *icon;
    cJSON *root = cJSON_Parse(json_message);
    char *title = NULL, *content = NULL;

    GdkPixbuf *img_buf = NULL;
    int width, height, hasAlpha, rowLength; 
    char *b64data;
    guchar *data;
    gsize size;

    if(root){
        tmp = cJSON_GetObjectItem(root, "title");
        if(tmp){
            title = tmp->valuestring;
        }
        
        tmp = cJSON_GetObjectItem(root, "desc");
        if(tmp){
            content = tmp->valuestring;
        }
        icon = cJSON_GetObjectItem(root, "icon");
        if(icon){
            width = cJSON_GetObjectItem(icon, "width")->valueint;
            height = cJSON_GetObjectItem(icon, "height")->valueint;
            hasAlpha = cJSON_GetObjectItem(icon, "hasAlpha")->valueint;
            rowLength = cJSON_GetObjectItem(icon, "rowLength")->valueint;

            b64data = cJSON_GetObjectItem(icon, "b64data")->valuestring;
            data = g_base64_decode(b64data, &size);
            
            printf("Image has size: %lu\n", (unsigned long) size);

            img_buf = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB, hasAlpha, 8,  width, height, rowLength, &free_icon_data, NULL);
        }
    }

    if(title || content){
        notify(title, content, img_buf);
    }

    cJSON_Delete(root);
}

void notify(const char *title, const char *description, GdkPixbuf *icon){
	NotifyNotification *notification = notify_notification_new(title, description, "dialog-information");
	notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);
        
        if(icon){
            notify_notification_set_image_from_pixbuf(notification, icon);
        }
        
        notify_notification_show(notification, NULL);
	g_object_unref(G_OBJECT(notification));
}
