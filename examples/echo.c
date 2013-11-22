#include <geoffrey.h>

/* This is a simple IRC bot, it will echo any message it recives back to the sender */

void authenticated(geoffrey *g, char *message, void *data) {
    /* We have connected to IRC, lets join a channel */
    if (g->info != NULL) {
        gb_sendf(g->sock, "JOIN #%s\r\n", g->info);
    }
}

void privmsg_echo(geoffrey *g, char *message, char *data) {
    /* We have recived a PRIVMSG, lets echo it back to whoever sent it. */
    char *msg = gb_getmsg(data);
    gb_reply(g, data, msg);
    free(msg);
}

int main(int argc, char **argv) {
    if (argc <= 3) {
        fprintf(stderr, "Usage: %s <server> <port> <nick> [<channel>]\nNever put # infront of the channel.\n", argv[0]);
        return 1;
    }

    /* Check for a 5th argument, which would be a optional channel */
    char *info;
    if (argc == 5) {
        info = strdup(argv[4]);
    } else {
        info = NULL;
    }

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *nick = argv[3];

    geoffrey *g = gb_alloc(); /* Allocate memory */

    /* Inititalize geoffrey, passing it, server, port, nick, realname and any
       extra data that is passed to callbacks */
    gb_init(g, 1, stderr, host, port, nick, "geoffrey echo test", info);

    /* Register the default signals (helpers) this is, ping, line ->command,
       connect. The 1 is for debugging, to print every signal. */
    gb_registerHelpers(g);

    /* Register some custom signals */
    gb_registerSignal(g, GB_AUTHED_SIG, authenticated);
    gb_registerSignal(g, GB_PRIVMSG_SIG, (gb_callback*)privmsg_echo);

    gb_loop(g, 0); /* Create a loop, which blocks. The 0 says we will not
                      reconnect once disconnected. */
    gb_dealloc(g); /* Free all the memory geoffrey used */

    if (info != NULL) { /* If we used that info pointer, free it */
        free(info);
    }

    return 0;
}
