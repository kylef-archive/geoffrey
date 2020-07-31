#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

typedef struct geoffrey_struct {
    /* Connection Infomation */
    int sock;
    char *host;
    int port;
    int alive;

    /* IRC Data */
    char *nick;
    char *realname;

    /* Signals */
    struct gb_signal_struct *signals;

    void *info; /* Optional infomation */

    /* Logging */
    int debug;
    FILE *error;
} geoffrey;

typedef void (gb_callback)(geoffrey *g, char *message, void *data);

geoffrey * gb_alloc(void);
int gb_init(geoffrey * g, int debug, FILE *error, char *host, int port, char *nick, char *realname, void *info);
void gb_finalize(geoffrey *g);
void gb_dealloc(geoffrey *g);
void gb_registerSignal(geoffrey *g, char *message, gb_callback *hander);
void gb_unregisterSignal(geoffrey *g, gb_callback *handler);
void gb_runSignal(geoffrey *g, char *message, void *data);
int gb_connect(geoffrey *g);
void gb_loop(geoffrey *g, int reconnect);
void gb_nick(geoffrey *g, char *nick);
char *gb_getnick(char *line);
char *gb_getmsg(char *line);
char *gb_getchannel(geoffrey *g, char *line);

/* Signals */
typedef struct gb_signal_struct {
    char *message;
    gb_callback *callback;
    struct gb_signal_struct *next;
} gb_signal;

#define GB_CONN_SIG "gb.connected"
#define GB_DCONN_SIG "gb.disconnected"
#define GB_LINE_SIG "gb.line"

#define GB_PRIVMSG_SIG "irc.privmsg"
#define GB_PING_SIG "irc.ping"
#define GB_AUTHED_SIG "irc.001" /* User Authenticated (not nickserv) */

/* Standard Helpers */
void gb_registerHelpers(geoffrey *g);

/* Networking */
#ifdef WIN32
    #include <winsock2.h>
    #define INET_ADDRSTRLEN 16
    #define sleep(x) Sleep((x)*1000)
    #define sock_start() WSADATA wsadata; WSAStartup(MAKEWORD(2,0), &wsadata)
    #define sock_stop() WSACleanup()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/select.h>
    #include <unistd.h>
    #define closesocket(x) close((x))
    #define sock_start()
    #define sock_stop()
#endif

enum gb_net_error {
    GB_NET_CONN,
    GB_NET_NOCONN,
    GB_NET_NOSOCK,
    GB_NET_NODNS,
    GB_NET_RWERR = -1
};

#define NET_IO_BUF_SIZE 8192

int gb_sendf(int sock, char *format, ...);
int gb_recv(int sock, char *buffer, size_t buf_len, int timeout);

/* Geoffrey specific networking functions */
int gb_connect(geoffrey *g);
int gb_lineParser(geoffrey *g);
int gb_privmsg(geoffrey *g, char *to, char *message);
int gb_reply(geoffrey *g, char *line, char *message);
void gb_disconnect(geoffrey *g, char *message);
