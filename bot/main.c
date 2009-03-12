#include <geoffrey.h>

/* Geoffrey Bell Bot (GBB) */
#define DEBUG 1

typedef struct gbb_bot_struct {
    char prefix;
} gbb_bot;

/* This function converts a privmsg signal into a command signal */
void gbb_commandSignal(geoffrey *g, char *message, char *data) {
    if (data[0] == ((gbb_bot *)g->info)->prefix) {
        char *command = strtok(data, " ");
        
        if (command != NULL) {
            int len = strlen(command);
            if (len > 1) {
                char *cmd = malloc(len + 4); /* Null (-! on command[0]) */
                strcpy(cmd, "cmd.");
                strcat(cmd, &command[1]);
                
                gb_runSignal(g, cmd, data);
                
                free(cmd);
            }
            free(command);
        }
    }
}

int main(int argc, char **argv) {
    gbb_bot info;
    info.prefix = '!';
    
    geoffrey *g = gb_alloc();
    gb_init(g, stderr, "irc.allshells.org", 6667, "bot", "geoffrey bot", &info);
    gb_registerHelpers(g, DEBUG);
    
    gb_registerSignal(g, GB_PRIVMSG_SIG, (gb_callback*)gbb_commandSignal);
    
    gb_loop(g, 1);
    gb_dealloc(g);
    
    return 0;
}
