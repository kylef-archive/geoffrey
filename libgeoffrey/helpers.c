#include <geoffrey.h>

/* This function turns a line signal into a IRC Command signals. */
void gb_lineSignal(geoffrey *g, char *message, char *data) {
    char *buf = strdup(data);
    char *argv[2];
    char *m;
    int part;
    
    if (buf == NULL) {
        return;
    }
    
    argv[0] = strtok(buf, " ");
    argv[1] = strtok(NULL, " ");
    
    part = (data[0] == ':');
    m = malloc(strlen(argv[part]) + 5);
    strcpy(m, "irc.");
    strcat(m, argv[part]);
    
    gb_runSignal(g, m, (void*)data);
    
    free(m);
    free(buf);
}

/* This function sends USER and NICK, this should be done once we're connected. */
void gb_connectedSignal(geoffrey *g, char *message, void *data) {
    gb_sendf(g->sock, "USER %s 0 0 :%s\r\n", g->nick, g->realname);
    gb_sendf(g->sock, "NICK %s\r\n", g->nick);
}

/* This function responds to the PING irc command. */
void gb_pingSignal(geoffrey *g, char *message, void *data) {
    char *argv[2];
    char *buf = strdup(data);
    
    argv[0] = strtok(buf, " ");
    argv[1] = strtok(NULL, " ");
    
    gb_sendf(g->sock, "PONG %s\r\n", &argv[1][1]);
    free(buf);
}

/* Put the signal into the error file (geoffrey.error). */
void gb_debugSignal(geoffrey *g, char *message, char *data) {
    if (g->error != NULL) {
        if (data == NULL) {
            fprintf(g->error, "[signal] (%s)\n", message);
        } else {
            fprintf(g->error, "[signal] (%s) %s\n", message, data);
        }
    }
}

/* This function registered the generic helpers, which 90% of the time will be used. */
void gb_registerHelpers(geoffrey *g) {
    gb_registerSignal(g, GB_PING_SIG, gb_pingSignal);
    gb_registerSignal(g, GB_CONN_SIG, gb_connectedSignal);
    gb_registerSignal(g, GB_LINE_SIG, (gb_callback*)gb_lineSignal);
    
    if (g->debug) {
        gb_registerSignal(g, NULL, (gb_callback*)gb_debugSignal);
    }
}
