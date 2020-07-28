#include <stdarg.h>

typedef struct Message Message;
Message *message_new(const char *prefix, const char *command, ...) __attribute__((sentinel));
Message *message_parse(const char *line);
void message_dealloc(Message *message);

char *message_serialise(Message *message);

const char *message_get_prefix(Message *message);
void message_set_prefix(Message *message, const char *prefix);

const char *message_get_command(Message *message);
void message_set_command(Message *message, const char *command);

const char *message_get_parameter(Message *message, int index);
void message_append_parameter(Message *message, const char *parameter);
