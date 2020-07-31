#include "greatest/greatest.h"
#include "../geoffrey.h"

geoffrey *setup(void) {
  geoffrey *g = gb_alloc();
  gb_init(g, 0, stderr, "irc.example.com", 6667, "nick", "geoffrey test", NULL);
  gb_registerHelpers(g);
  return g;
}

TEST test_connected_signal(void) {
  geoffrey *g = setup();

  int fd[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) != 0) {
    FAIL();
  }
  g->sock = fd[0];

  gb_runSignal(g, "gb.connected", NULL);

  char buffer[512] = {};
  int length = read(fd[1], &buffer, sizeof(buffer));
  ASSERT_STR_EQ("USER nick 0 0 :geoffrey test\r\nNICK nick\r\n", buffer);

  gb_dealloc(g);
  close(fd[0]);
  close(fd[1]);

  PASS();
}

TEST test_ping_signal(void) {
  geoffrey *g = setup();

  int fd[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) != 0) {
    FAIL();
  }
  g->sock = fd[0];

  gb_runSignal(g, "gb.line", "PING :hello world");

  char buffer[512] = {};
  int length = read(fd[1], &buffer, sizeof(buffer));
  ASSERT_STR_EQ("PONG hello\r\n", buffer);

  gb_dealloc(g);
  close(fd[0]);
  close(fd[1]);

  PASS();
}

SUITE(test_helpers) {
  RUN_TEST(test_connected_signal);
  RUN_TEST(test_ping_signal);
}
