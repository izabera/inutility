#include "common.h"

static struct {
  char *name;
  int64_t value;
} numbers[] = {
  { ""   , 1LL                                        },
  { "K"  , 1000LL                                     },
  { "M"  , 1000LL * 1000LL                            },
  { "G"  , 1000LL * 1000LL * 1000LL                   },
  { "T"  , 1000LL * 1000LL * 1000LL * 1000LL          },
  { "P"  , 1000LL * 1000LL * 1000LL * 1000LL * 1000LL },
}, bytes[] = {
  { ""   , 1LL                                        },
  { "B"  , 1LL                                        },
                                                         
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

int64_t parsenumb(const char *string) {
  char *end;
  int64_t num = strtoll(string, &end, 10);
  for (size_t i = 0; i < arrsize(numbers); i++)
    if (!strcasecmp(end, numbers[i].name)) return num * numbers[i].value;
  return INT64_MIN;
}

int64_t parsebyte(const char *string) {
  char *end;
  int64_t num = strtoll(string, &end, 10);
  for (size_t i = 0; i < arrsize(bytes); i++)
    if (!strcasecmp(end, bytes[i].name)) return num * bytes[i].value;
  return INT64_MIN;
}
