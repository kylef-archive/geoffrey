#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "message.h"

typedef struct Parameter Parameter;

struct Parameter {
  Parameter *next;
  char value[1];
};

Parameter *parameter_new(const char *value) {
  size_t length = strlen(value);
  Parameter *parameter = calloc(1, sizeof(Parameter) + length);
  strncpy((parameter->value), value, length);
  return parameter;
}

struct Message {
  char *prefix;
  char *command;
  bool colon;

  Parameter *start_parameter;
  Parameter *end_parameter;
};

Message *message_alloc() {
  return calloc(1, sizeof(Message));
}

Message *message_init(Message *message, const char *prefix, const char *command) {
  message_set_prefix(message, prefix);
  message_set_command(message, command);
  return message;
}

void message_dealloc(Message *message) {
  free(message->prefix);
  free(message->command);

  Parameter *parameter = message->start_parameter;
  while (parameter != NULL) {
    Parameter *next = parameter->next;
    free(parameter);
    parameter = next;
  }

  free(message);
}

#define MAX_MESSAGE_SIZE 512

char *message_serialise(Message *message) {
  size_t position = 0;
  char *buffer = calloc(1, MAX_MESSAGE_SIZE);

  if (message->prefix) {
    buffer[position++] = ':';
    position += strlen(strncpy(&buffer[position], message->prefix, MAX_MESSAGE_SIZE - 0));
    buffer[position++] = ' ';
  }

  position += strlen(strncpy(&buffer[position], message->command, MAX_MESSAGE_SIZE - position));

  if (message->start_parameter) {
    for (Parameter *parameter = message->start_parameter; parameter != NULL; parameter = parameter->next) {
      buffer[position++] = ' ';

      if (parameter->next == NULL && (message->colon || (strchr(parameter->value, ' ') != NULL || parameter->value[0] == ':'))) {
        buffer[position++] = ':';
      }

      position += strlen(strncpy(&buffer[position], parameter->value, MAX_MESSAGE_SIZE - position));
    }
  }

  return buffer;
}

const char *message_get_prefix(Message *message) {
  return message->prefix;
}

void message_set_prefix(Message *message, const char *prefix) {
  if (message->prefix) {
    free(message->prefix);
  }

  message->prefix = prefix ? strdup(prefix) : NULL;
}

const char *message_get_command(Message *message) {
  return message->command;
}

void message_set_command(Message *message, const char *command) {
  if (message->command) {
    free(message->command);
  }

  message->command = strdup(command);
}

const char *message_get_parameter(Message *message, int index) {
  int count = 0;
  for (Parameter *parameter = message->start_parameter; parameter != NULL; parameter = parameter->next, ++count) {
    if (count == index) {
        return parameter->value;
    }
  }

  return NULL;
}

void message_append_parameter(Message *message, const char *parameter) {
  Parameter *param = parameter_new(parameter);

  if (message->start_parameter == NULL) {
    message->start_parameter = param;
  } else {
    message->end_parameter->next = param;
  }

  message->end_parameter = param;
}

Message *message_new(const char *prefix, const char *command, ...) {
  Message *message = message_alloc();
  message = message_init(message, prefix, command);

  va_list parameters;
  va_start(parameters, command);

  for (int index = 0;; ++index) {
    const char *parameter = va_arg(parameters, const char*);
    if (parameter == NULL) {
      break;
    }

    message_append_parameter(message, parameter);
  }

  va_end(parameters);

  return message;
}

Message *message_parse(const char *line) {
  char *buffer = strdup(line);

  Message *message = message_alloc();

  char *saveptr;

  if (line[0] == ':') {
    const char *prefix = strtok_r(&buffer[1], " ", &saveptr);
    const char *command = strtok_r(NULL, " ", &saveptr);
    message_init(message, prefix, command);
  } else {
    const char *command = strtok_r(buffer, " ", &saveptr);
    message_init(message, NULL, command);
  }


  char *parameter;
  while ((parameter = strtok_r(NULL, " ", &saveptr))) {
    if (parameter[0] == ':') {
      message->colon = true;

      if (saveptr[0] != 0) {
        saveptr[-1] = ' ';
      }

      message_append_parameter(message, ++parameter);
      break;
    }

    message_append_parameter(message, parameter);
  }

  free(buffer);
  return message;
}
