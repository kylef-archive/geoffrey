#include "greatest/greatest.h"

SUITE(test_message);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(test_message);

  GREATEST_MAIN_END();
}
