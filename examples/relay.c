#include <geoffrey.h>

void authenticated(geoffrey *g, char *message, void *data) {
    /* We have connected to IRC, lets join a channel */
    if (((char **)g->info)[1] != NULL) {
        gb_sendf(g->sock, "JOIN #%s\r\n", ((char **)g->info)[1]);
    }
}

void privmsg(geoffrey *g, char *message, char *data) {
    char *nick = gb_getnick(data);
    char *channel;
    char *msg;
    char *buf;
    
    if (strcmp(nick, ((char **)g->info)[0]) != 0) {
        channel = gb_getchannel(g, data);
        msg = gb_getmsg(data);
        
        if (channel != NULL) {
            buf = malloc(strlen(msg) + strlen(nick) + strlen(channel) + 8);
            sprintf(buf, "[%s] <%s>: %s", channel, nick, msg);
            free(channel);
        } else {
            buf = malloc(strlen(msg) + strlen(nick) + 5);
            sprintf(buf, "<%s>: %s", nick, msg);
        }
        
        gb_privmsg(g, ((char **)g->info)[0], buf);
        
        free(buf);
        free(msg);
    }
    
    free(nick);
}

int main(int argc, char **argv) {
    if (argc <= 5) {
        fprintf(stderr, "Usage: %s <server> <port> <nick> <relay> [<channel>]\nNever put # infront of the channel.\n", argv[0]);
        return 1;
    }
    
    char *info[2];
    info[0] = argv[4];
    
    /* Check for a 6th argument, which would be a optional channel */
    if (argc >= 6) {
        info[1] = argv[5];
    }
    
    char *host = argv[1];
    int port = atoi(argv[2]);
    char *nick = argv[3];
    
    geoffrey *g = gb_alloc(); /* Allocate memory */
    
    /* Inititalize geoffrey, passing it, server, port, nick, realname and any
       extra data that is passed to callbacks */
    gb_init(g, stderr, host, port, nick, "geoffrey relay", info);
    
    /* Register the default signals (helpers) this is, ping, line ->command,
       connect. The 1 is for debugging, to print every signal. */
    gb_registerHelpers(g, 1);
    
    /* Register some custom signals */
    gb_registerSignal(g, GB_AUTHED_SIG, authenticated);
    gb_registerSignal(g, GB_PRIVMSG_SIG, (gb_callback*)privmsg);
    
    gb_loop(g, 1); /* Create a loop, which blocks. The 1 says that we will
                      reconnect once disconnected. */
    gb_dealloc(g); /* Free all the memory geoffrey used */
    
    return 0;
}
