#include <string.h>
#include <geoffrey.h>

geoffrey * gb_alloc(void) {
    return calloc(1, sizeof(geoffrey));
}

int gb_init(geoffrey * g, int debug, FILE *error, char *host, int port, char *nick, char *realname, void *info) {
    if (g == NULL) {
        return 0;
    }

    sock_start();

    if (host) {
        g->host = strdup(host);
    }

    if (nick) {
        g->nick = strdup(nick);
    }

    if (realname) {
        g->realname = strdup(realname);
    }

    g->debug = debug;
    g->error = error;
    g->alive = 0;
    g->port = port;
    g->info = info;
    g->signals = NULL;

    return 1;
}

void gb_finalize(geoffrey *g) {
    if (g->alive == 1) {
        g->alive = 0;
        closesocket(g->sock);
    }
}

void gb_dealloc(geoffrey *g) {
    /* unregister all signals */
    gb_signal *ptr = g->signals;
    while (ptr != NULL) {
        gb_signal *next = ptr->next;
        free(ptr);
        ptr = next;
    }

    free(g->host);
    free(g->nick);
    free(g->realname);
    free(g);

    sock_stop();
}

void gb_registerSignal(geoffrey *g, char *message, gb_callback *handler) {
    gb_signal *s = calloc(1, sizeof(struct gb_signal_struct));

    if (message != NULL) {
        s->message = strdup(message);
    } else {
        s->message = NULL;
    }

    s->callback = handler;

    s->next = g->signals;
    g->signals = s;
}

void gb_unregisterSignal(geoffrey *g, gb_callback *handler) {
    gb_signal *ptr, *prev;
    for (ptr = g->signals, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (handler == ptr->callback) {
            if (ptr->next != NULL) {
                if (prev == NULL) {
                    g->signals = ptr->next;
                } else {
                    prev->next = ptr->next;
                }
            } else if (prev != NULL) {
                prev->next = NULL;
            } else {
                g->signals = NULL;
            }

            if (ptr->message != NULL) free(ptr->message);
            free(ptr);
        }
    }
}

void gb_runSignal(geoffrey *g, char *message, void *data) {
    gb_signal *ptr;
    for (ptr = g->signals; ptr != NULL; ptr = ptr->next) {
        if (ptr->message == NULL || strcasecmp(ptr->message, message) == 0) {
            if (ptr->callback != NULL) {
                ptr->callback(g, message, data);
            }
        }
    }
}

void gb_loop(geoffrey *g, int reconnect) {
    if (g->alive) return;
    g->alive = 1;

    while (g->alive) {
        if (gb_connect(g) == GB_NET_CONN) {
            gb_runSignal(g, GB_CONN_SIG, NULL);
            gb_lineParser(g);
            gb_runSignal(g, GB_DCONN_SIG, NULL);
            close(g->sock);
            g->sock = 0;
        }

        if (!reconnect) {
            g->alive = 0;
            return;
        }

        if (g->alive) {
            sleep(15); /* Wait 15 seconds before reconnecting, unless we're quitting. */
        }
    }
}

void gb_nick(geoffrey *g, char *nick) {
    free(g->nick);

    if (nick) {
        g->nick = malloc(strlen(nick) + 1);
        g->nick = strcpy(g->nick, nick);
    }

    if (g->sock != 0) {
        gb_sendf(g->sock, "NICK %s\r\n", g->nick);
    }
}

char *gb_getnick(char *line) {
    int index = strchr(line, '!') - line;
    char *nick = malloc(index);

    memcpy(nick, &line[1], index - 1);
    nick[index - 1] = 0;

    return nick;
}

char *gb_getmsg(char *line) {
    int index = (strstr(line, " :") - line) + 2;
    int len = strlen(line) - index;
    char *msg = malloc(len + 1);

    memcpy(msg, &line[index], len);
    msg[len] = 0;

    return msg;
}

char *gb_getchannel(geoffrey *g, char *line) {
    char *argv[3];
    char *buf = strdup(line);
    char *channel;

    argv[0] = strtok(buf, " ");
    argv[1] = strtok(NULL, " ");
    argv[2] = strtok(NULL, " ");

    if (strcmp(argv[2], g->nick) == 0) {
        channel = NULL;
    } else {
        channel = strdup(argv[2]);
    }

    free(buf);
    return channel;
}
