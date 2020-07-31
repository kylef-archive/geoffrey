#include "greatest/greatest.h"

SUITE(test_message);
SUITE(test_helpers);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(test_message);
  RUN_SUITE(test_helpers);

  GREATEST_MAIN_END();
}
