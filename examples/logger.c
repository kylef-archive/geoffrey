#include <geoffrey.h>

/* This will write every message sent into a channel into a file.
   Remember, the file will only be written to everytime we get a
   PING, or when we disconnect. A ctrl+c at the terminal will not
   write the file.
*/


struct log_info {
    char *channel;
    FILE *fp;
};

void authenticated(geoffrey *g, char *message, void *data) {
    /* We have connected to IRC, lets join a channel */
    gb_sendf(g->sock, "JOIN %s\r\n", ((struct log_info*)g->info)->channel);
}

/* When we get a PING, lets flush the file. */
void ping_flush(geoffrey *g, char *message, void *data) {
    fflush(((struct log_info*)g->info)->fp);
}

void privmsg(geoffrey *g, char *message, char *data) {
    /* We have recived a PRIVMSG, lets check it is from the channel,
       and then insert it into log file. */
    char *channel = gb_getchannel(g, data);
    
    if (channel == NULL) { /* Private Message */
        return;
    }
    
    if (strcmp(channel, ((struct log_info*)g->info)->channel) != 0) {
        /* Message not from the correct channel */
        free(channel);
        return;
    }
    
    char *nick = gb_getnick(data);
    char *msg = gb_getmsg(data);
    
    /* Lets append the nick, and message to the file. */
    fprintf(((struct log_info*)g->info)->fp, "%s: %s\n", nick, msg);
    
    free(channel);
    free(nick);
    free(msg);
}

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <server> <port> <nick> <channel> <file>\nNever put # infront of the channel.\n", argv[0]);
        return 1;
    }
    
    char *host = argv[1];
    int port = atoi(argv[2]);
    char *nick = argv[3];
    
    struct log_info info;
    info.channel = malloc(strlen(argv[4]) + 2);
    strcpy(info.channel , "#");
    strcat(info.channel , argv[4]);
    info.fp = fopen(argv[5], "w");
    
    if (info.fp == NULL) {
        fprintf(stderr, "Cannot open %s.\n", argv[4]);
        return 2;
    }
    
    fprintf(info.fp, "-- Logging started for channel: %s --\n", info.channel);
    
    geoffrey *g = gb_alloc(); /* Allocate memory */
    
    /* Inititalize geoffrey, passing it, server, port, nick, realname and any
       extra data that is passed to callbacks */
    gb_init(g, stderr, host, port, nick, "geoffrey logger", (void*)&info);
    
    /* Register the default signals (helpers) this is, ping, line ->command,
       connect. The 1 is for debugging, to print every signal. */
    gb_registerHelpers(g, 1);
    
    /* Register some custom signals */
    gb_registerSignal(g, GB_PING_SIG, ping_flush);
    gb_registerSignal(g, GB_AUTHED_SIG, authenticated);
    gb_registerSignal(g, GB_PRIVMSG_SIG, (gb_callback*)privmsg);
    
    gb_loop(g, 1); /* Create a loop, which blocks. The 1 states we will reconnect
                      when connection is disconnected. */
    gb_dealloc(g); /* Free all the memory geoffrey used */
    
    fprintf(info.fp, "-- Logging ended --\n");
    fclose(info.fp);
    free(info.channel);
    
    return 0;
}
