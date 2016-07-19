#include "lib/common.h"

// todo: do this properly with /proc/meminfo
int main(int argc, char *argv[]) {
  options("bghkmt");
  struct sysinfo info;
  sysinfo(&info);
  puts("              total        used        free      shared  buff/cache   available");
  if (flag('h')) {
      printf("Mem:    %10.1f%c %10.1f%c %10.1f%c %10.1f%c %10.1f%c %10.1f%c\n"
             "Swap:   %10.1f%c %10.1f%c %10.1f%c\n",
             
             scale(info.totalswap), scale(info.totalswap - info.freeswap),
             scale(info.freeswap));
    if (flag('t'))
      printf("Tot:    %10.1f%c %10.1f%c %10.1f%c\n",
             scale(info.totalram + info.totalswap),
             scale(info.freeram + info.freeswap));
  }
  else {
    int shift;
         if (flag('b')) shift = 0;
    else if (flag('m')) shift = 20;
    else if (flag('g')) shift = 30;
    else                shift = 10;
    printf("Mem:    %11lu %11lu %11lu %11lu %11lu %11lu\n"
           "Swap:   %11lu %11lu %11lu\n",

           info.totalram >> shift, (info.totalram-info.freeram) >> shift,
           info.sharedram >> shift, (info.totalram - info.freeram - info.bufferram) >> shift,

           info.totalswap >> shift, (info.totalswap - info.freeswap) >> shift,
           info.freeswap >> shift);
    if (flag('t'))
      printf("Tot:    %11lu %11lu %11lu\n",
             (info.totalram + info.totalswap) >> shift,
             (info.freeram + info.freeswap) >> shift);
  }
  return errno;
}
