#include <stdio.h>
#include <stdlib.h>
#include "libfe.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s vdrhost:port\n", argv[0]);
    exit(1);
  }
  set_local_powerdown();
  return run_vdr_frontend(argv[1]);
}
