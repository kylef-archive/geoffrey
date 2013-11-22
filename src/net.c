#include <geoffrey.h>

int gb_sendf(int sock, char *format, ...) {
    va_list args;
    char buf[513]; /* An IRC server *should* truncate messages to 512 */

    va_start(args, format);
    vsnprintf(buf, 512, format, args);
    va_end(args);

    return send(sock, buf, strlen(buf), 0);
}

int gb_recv(int sock, char *buffer, size_t buf_len, int timeout) {
    fd_set fds;
    struct timeval tv, *tvptr;
    int len;

    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (timeout == -1) {
        tvptr = NULL;
    } else {
        tvptr = &tv;
    }

    if (select(sizeof(fds)*8, &fds, NULL, NULL, tvptr) == 1) {
        len = recv(sock, buffer, buf_len, 0);
        if (len > 0) {
            return len;
        } else {
            return GB_NET_RWERR;
        }
    }

    return 0;
}

int gb_connect(geoffrey *g) {
    struct hostent *host;
    struct sockaddr_in addr;
    int connection;

    host = gethostbyname(g->host);
    if (!host) return GB_NET_NODNS;

    memcpy(&addr.sin_addr, host->h_addr, host->h_length);
    addr.sin_family = host->h_addrtype;
    addr.sin_port = htons(g->port);

    g->sock = socket(host->h_addrtype, SOCK_STREAM, 0);
    if (g->sock == -1) return GB_NET_NOSOCK;

    connection = connect(g->sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (connection != 0) {
        closesocket(g->sock);
        return GB_NET_NOCONN;
    }

    return GB_NET_CONN;
}

int gb_lineParser(geoffrey *g) {
    int len, index = 0;
    char buffer[NET_IO_BUF_SIZE];
    char *position;
    char *line;

    for (;;) {
        len = gb_recv(g->sock, &buffer[index], NET_IO_BUF_SIZE-index, -1);
        if (len == GB_NET_RWERR) return GB_NET_RWERR;

        len = len + index; /* The true length from the start of the buffer */
        while ((position = strstr(buffer, "\r\n")) != NULL) {
            index = position - buffer;

            /* Grab the line from the buffer */
            line = malloc(index + 1);
            memcpy(line, buffer, index);
            line[index] = 0; /* NULL terminated string */
            gb_runSignal(g, GB_LINE_SIG, (void*)line);
            free(line);

            /* Replace the line in the buffer */
            index = index + 2; /* Increment the index to after the CRLF */
            memcpy(buffer, &buffer[index], NET_IO_BUF_SIZE-index); /* Move data to front of buffer */
            len = len - index; /* Update the length (removing the line) */
            index = 0; /* Index is now at 0 as it was moved */
        }
    }
}

int gb_privmsg(geoffrey *g, char *to, char *message) {
    return gb_sendf(g->sock, "PRIVMSG %s :%s\r\n", to, message);
}

int gb_reply(geoffrey *g, char *line, char *message) {
    char *argv[3];
    char *buf = strdup(line);
    char *to;
    int ret;

    argv[0] = strtok(buf, " ");
    argv[1] = strtok(NULL, " ");
    argv[2] = strtok(NULL, " ");

    if ((to = gb_getchannel(g, line)) == NULL) {
        to = gb_getnick(argv[0]);
    }

    ret = gb_privmsg(g, to, message);

    free(to);
    free(buf);

    return ret;
}

void gb_disconnect(geoffrey *g, char *message) {
    g->alive = 0;
    gb_sendf(g->sock, "QUIT %s\r\n", message);
    closesocket(g->sock);
}
