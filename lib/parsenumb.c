#include <stdlib.h>
#include <strings.h>
#include "common.h"

int64_t parsenumb(const char *string) {
  char *end;
  int64_t num = strtoll(string, &end, 10);
  struct {
    char *name;
    int64_t value;
  } multipliers[] = {
    { "K"  , 1000LL                                     },
    { "KB" , 1000LL                                     },
    { "KiB", 1024LL                                     },

    { "M"  , 1000LL * 1000LL                            },
    { "MB" , 1000LL * 1000LL                            },
    { "MiB", 1024LL * 1024LL                            },

    { "G"  , 1000LL * 1000LL * 1000LL                   },
    { "GB" , 1000LL * 1000LL * 1000LL                   },
    { "GiB", 1024LL * 1024LL * 1024LL                   },

    { "T"  , 1000LL * 1000LL * 1000LL * 1000LL          },
    { "TB" , 1000LL * 1000LL * 1000LL * 1000LL          },
    { "TiB", 1024LL * 1024LL * 1024LL * 1024LL          },

    { "P"  , 1000LL * 1000LL * 1000LL * 1000LL * 1000LL },
    { "PB" , 1000LL * 1000LL * 1000LL * 1000LL * 1000LL },
    { "PiB", 1024LL * 1024LL * 1024LL * 1024LL * 1024LL },
  };
  for (size_t i = 0; i < arrsize(multipliers); i++)
    if (!strcasecmp(end, multipliers[i].name)) return num * multipliers[i].value;
  return num;
}
