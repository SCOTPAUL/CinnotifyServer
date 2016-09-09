/**
 * This file is a modified version of an example file which comes with avahi
 * the following comment is the original notice.
 */

/***
  This file is part of avahi.
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/alternative.h>
#include <avahi-common/thread-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>
#include <unistd.h>
#include <string.h>
#include "service.h"

static uint16_t port = 6525;
static AvahiClient *client = NULL;
static AvahiEntryGroup *group = NULL;
static AvahiThreadedPoll *thread_poll = NULL;
static char *name = NULL;
static void create_services(AvahiClient *c);
static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata) {
    assert(g == group || group == NULL);
    group = g;
    /* Called whenever the entry group state changes */
    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
            /* The entry group has been established successfully */
            fprintf(stderr, "Service '%s' successfully established.\n", name);
            break;
        case AVAHI_ENTRY_GROUP_COLLISION : {
            char *n;
            /* A service name collision with a remote service
             * happened. Let's pick a new name */
            n = avahi_alternative_service_name(name);
            avahi_free(name);
            name = n;
            fprintf(stderr, "Service name collision, renaming service to '%s'\n", name);
            /* And recreate the services */
            create_services(avahi_entry_group_get_client(g));
            break;
        }
        case AVAHI_ENTRY_GROUP_FAILURE :
            fprintf(stderr, "Entry group failure: %s\n", avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));
            /* Some kind of failure happened while we were registering our services */
            avahi_threaded_poll_quit(thread_poll);
            break;
        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}
static void create_services(AvahiClient *c) {
    char *n;
    int ret;
    assert(c);
    /* If this is the first time we're called, let's create a new
     * entry group if necessary */
    if (!group)
        if (!(group = avahi_entry_group_new(c, entry_group_callback, NULL))) {
            fprintf(stderr, "avahi_entry_group_new() failed: %s\n", avahi_strerror(avahi_client_errno(c)));
            goto fail;
        }
    /* If the group is empty (either because it was just created, or
     * because it was reset previously, add our entries.  */
    if (avahi_entry_group_is_empty(group)) {
        fprintf(stderr, "Adding service '%s'\n", name);
        if ((ret = avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0, name, "_cinnotify._tcp", NULL, NULL, port, NULL)) < 0) {
            if (ret == AVAHI_ERR_COLLISION)
                goto collision;
            fprintf(stderr, "Failed to add _cinnotify._tcp service: %s\n", avahi_strerror(ret));
            goto fail;
        }
        /* Tell the server to register the service */
        if ((ret = avahi_entry_group_commit(group)) < 0) {
            fprintf(stderr, "Failed to commit entry group: %s\n", avahi_strerror(ret));
            goto fail;
        }
    }
    return;
collision:
    /* A service name collision with a local service happened. Let's
     * pick a new name */
    n = avahi_alternative_service_name(name);
    avahi_free(name);
    name = n;
    fprintf(stderr, "Service name collision, renaming service to '%s'\n", name);
    avahi_entry_group_reset(group);
    create_services(c);
    return;
fail:
    avahi_threaded_poll_quit(thread_poll);
}
static void client_callback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata) {
    assert(c);
    /* Called whenever the client or server state changes */
    switch (state) {
        case AVAHI_CLIENT_S_RUNNING:
            /* The server has startup successfully and registered its host
             * name on the network, so it's time to create our services */
            create_services(c);
            break;
        case AVAHI_CLIENT_FAILURE:
            fprintf(stderr, "Client failure: %s\n", avahi_strerror(avahi_client_errno(c)));
            avahi_threaded_poll_quit(thread_poll);
            break;
        case AVAHI_CLIENT_S_COLLISION:
            /* Let's drop our registered services. When the server is back
             * in AVAHI_SERVER_RUNNING state we will register them
             * again with the new host name. */
        case AVAHI_CLIENT_S_REGISTERING:
            /* The server records are now being established. This
             * might be caused by a host name change. We need to wait
             * for our own records to register until the host name is
             * properly esatblished. */
            if (group)
                avahi_entry_group_reset(group);
            break;
        case AVAHI_CLIENT_CONNECTING:
            ;
    }
}

int start_service_broadcast(uint16_t service_port) {
    port = service_port;
    int error;
    int ret = 1;
    /* Allocate main loop object */
    if (!(thread_poll = avahi_threaded_poll_new())) {
        fprintf(stderr, "Failed to create threaded poll object.\n");
        goto fail;
    }

    char hostname[1024] = {0};
    gethostname(hostname, 1023);

    char tmp_name[2048] = {0};
    strncat(tmp_name, hostname, 1023);
    strncat(tmp_name, "-Cinnotify", 1023);

    name = avahi_strdup(tmp_name);
    /* Allocate a new client */
    client = avahi_client_new(avahi_threaded_poll_get(thread_poll), 0, client_callback, NULL, &error);
    /* Check wether creating the client object succeeded */
    if (!client) {
        fprintf(stderr, "Failed to create client: %s\n", avahi_strerror(error));
        goto fail;
    }
   /* Run the main loop */
    avahi_threaded_poll_start(thread_poll);
    ret = 0;
    return ret;

fail:
    /* Cleanup things */
    stop_service_broadcast();
    return ret;
}

void stop_service_broadcast(){
    printf("%s\n", "Shutting down service broadcast");
    
    if (client)
        avahi_client_free(client);
    if (thread_poll)
        avahi_threaded_poll_free(thread_poll);
    avahi_free(name);
}

