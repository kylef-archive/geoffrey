#include <geoffrey.h>

geoffrey * gb_alloc(void) {
    return malloc(sizeof(geoffrey));
}

int gb_init(geoffrey * g, FILE *error, char *host, int port, char *nick, char *realname, void *info) {
    if (g == NULL) {
        return 0;
    }
    
    sock_start();
    
    if (host) {
        g->host = malloc(strlen(host) + 1);
        g->host = strcpy(g->host, host);
    }
    
    if (nick) {
        g->nick = malloc(strlen(nick) + 1);
        g->nick = strcpy(g->nick, nick);
    }
    
    if (realname) {
        g->realname = malloc(strlen(realname) + 1);
        g->realname = strcpy(g->realname, realname);
    }
    
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
    /* todo - unregister all signals */
    free(g->host);
    free(g->nick);
    free(g->realname);
    free(g);
    
    sock_stop();
}

void gb_registerSignal(geoffrey *g, char *message, gb_callback *handler) {
    gb_signal *s = malloc(sizeof(struct gb_signal_struct));
    
    s->message = malloc(strlen(message)+1);
    strcpy(s->message, message);
    
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
            } else {
                prev->next = NULL;
            }
            
            free(ptr->message);
            free(ptr);
        }
    }
}

void gb_runSignal(geoffrey *g, char *message, void *data) {
    gb_signal *ptr;
    for (ptr = g->signals; ptr != NULL; ptr = ptr->next) {
        if (strcasecmp(ptr->message, message) == 0) {
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
