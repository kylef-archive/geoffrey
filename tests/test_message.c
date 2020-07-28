#include "greatest/greatest.h"
#include "../src/message.h"

TEST new(void) {
  Message *message = message_new(NULL, "PRIVMSG", "kyle", "Hello World", NULL);

  ASSERT_STR_EQ("PRIVMSG", message_get_command(message));
  ASSERT_STR_EQ("kyle", message_get_parameter(message, 0));
  ASSERT_STR_EQ("Hello World", message_get_parameter(message, 1));
  ASSERT_EQ(NULL, message_get_parameter(message, 2));

  message_dealloc(message);

  PASS();
}

TEST parse(void) {
  Message *message = message_parse("PRIVMSG kyle :Hello World");

  ASSERT_STR_EQ("PRIVMSG", message_get_command(message));
  ASSERT_STR_EQ("kyle", message_get_parameter(message, 0));
  ASSERT_STR_EQ("Hello World", message_get_parameter(message, 1));
  ASSERT_EQ(NULL, message_get_parameter(message, 2));

  message_dealloc(message);

  PASS();
}

TEST parse_colon_parameter_with_no_space(void) {
  Message *message = message_parse("PRIVMSG kyle :Hello");

  ASSERT_STR_EQ("PRIVMSG", message_get_command(message));
  ASSERT_STR_EQ("kyle", message_get_parameter(message, 0));
  ASSERT_STR_EQ("Hello", message_get_parameter(message, 1));
  ASSERT_EQ(NULL, message_get_parameter(message, 2));

  message_dealloc(message);

  PASS();
}

TEST parse_empty_colon_parameter(void) {
  Message *message = message_parse("PRIVMSG kyle :");

  ASSERT_STR_EQ("PRIVMSG", message_get_command(message));
  ASSERT_STR_EQ("kyle", message_get_parameter(message, 0));
  ASSERT_STR_EQ("", message_get_parameter(message, 1));
  ASSERT_EQ(NULL, message_get_parameter(message, 2));

  message_dealloc(message);

  PASS();
}

TEST parse_with_prefix(void) {
  Message *message = message_parse(":irc.example.com PRIVMSG kyle :Hello World");

  ASSERT_STR_EQ("irc.example.com", message_get_prefix(message));
  ASSERT_STR_EQ("PRIVMSG", message_get_command(message));
  ASSERT_STR_EQ("kyle", message_get_parameter(message, 0));
  ASSERT_STR_EQ("Hello World", message_get_parameter(message, 1));
  ASSERT_EQ(NULL, message_get_parameter(message, 2));

  message_dealloc(message);

  PASS();
}

TEST serialise(void) {
  Message *message = message_new(NULL, "PRIVMSG", "kyle", "Hello", NULL);

  char *str = message_serialise(message);
  ASSERT_STR_EQ("PRIVMSG kyle Hello", str);
  free(str);

  message_dealloc(message);

  PASS();
}

TEST serialise_last_parameter_with_space(void) {
  Message *message = message_new(NULL, "PRIVMSG", "kyle", "Hello World", NULL);

  char *str = message_serialise(message);
  ASSERT_STR_EQ("PRIVMSG kyle :Hello World", str);
  free(str);

  message_dealloc(message);

  PASS();
}

TEST serialise_last_parameter_with_colon(void) {
  Message *message = message_new(NULL, "PASS", ":secret", NULL);

  char *str = message_serialise(message);
  ASSERT_STR_EQ("PASS ::secret", str);
  free(str);

  message_dealloc(message);

  PASS();
}

TEST serialise_with_prefix(void) {
  Message *message = message_new("irc.example.com", "NOTICE", "kyle", "Hello World", NULL);

  char *str = message_serialise(message);
  ASSERT_STR_EQ(":irc.example.com NOTICE kyle :Hello World", str);
  free(str);

  message_dealloc(message);

  PASS();
}

TEST parse_serialise_remembers_colon(void) {
  Message *message = message_parse("PRIVMSG kyle :Hello");

  char *str = message_serialise(message);
  ASSERT_STR_EQ("PRIVMSG kyle :Hello", str);
  free(str);

  message_dealloc(message);

  PASS();
}

SUITE(test_message) {
  RUN_TEST(new);
  RUN_TEST(parse);
  RUN_TEST(parse_colon_parameter_with_no_space);
  RUN_TEST(parse_empty_colon_parameter);
  RUN_TEST(parse_with_prefix);
  RUN_TEST(serialise);
  RUN_TEST(serialise_last_parameter_with_space);
  RUN_TEST(serialise_last_parameter_with_colon);
  RUN_TEST(serialise_with_prefix);
  RUN_TEST(parse_serialise_remembers_colon);
}
