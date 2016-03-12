#include <stdio.h>

#include "lib/flags.h"
#include "lib/uptime.h"

int main(int argc, char *argv[]) {
  options("");
  return print_uptime();
}
