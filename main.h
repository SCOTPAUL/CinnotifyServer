#pragma once

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <libnotify/notify.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <regex.h>
#include <ctype.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "networking.h"
#include "notify.h"

#define NAME    "cinnotify-server"
#define VERSION "v0.2.1-alpha"

