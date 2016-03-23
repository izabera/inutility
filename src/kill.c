#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("#l?s|");
  int64_t signal;
       if (flag('s'))   signal =  flags[opt('s')].nums[flag('s')-1];
  else if (flag('#')) { signal = *flags[opt('#')].nums++; flag('#')--; }
  else                  signal = 15;
  /* if the option parser gets -9 -1 both are put in flag('#')
   * only the first is a signal, the other is a pid with - to mean process group 
   * in   kill -s 7 -9 -1   both -9 and -1 are pids
   * so   kill -9 -s 1 -4 81 -5   sends signal 1 to -9 -4 81 -5 */

  char *end;
  if (flag('l')) {
    char *signals[128] = {
      "EXIT", "HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "BUS", "FPE",
      "KILL", "USR1", "SEGV", "USR2", "PIPE", "ALRM", "TERM", "STKFLT", "CHLD",
      "CONT", "STOP", "TSTP", "TTIN", "TTOU", "URG", "XCPU", "XFSZ", "VTALRM",
      "PROF", "WINCH", "POLL", "PWR", "SYS", [64] = "RTMAX"
    };
    if (flags[opt('l')].args[0]) {
      long int num = strtol(flags[opt('l')].args[0], &end, 10);
      if (*end || num < 0 || num > (long int)arrsize(signals)) return 1;
      if (signals[num]) puts(signals[num]);
      else printf("%ld\n", num);
    }
    else
      for (size_t i = 1; i < arrsize(signals); i++) {
        if (!signals[i]) continue;
        printf("%2zu) %s\n", i, signals[i]);
      }
  }
  else {
    if (argc == 1 && flag('#') == 0) return 1;
    for (size_t i = 0; i < flag('#'); i++)
      kill(-flags[opt('#')].nums[i], signal);
    while (*++argv) {
      pid_t pid = strtol(*argv, &end, 10);
      if (*end) return 1;
      kill(pid, signal);
    }
  }
  return errno;
}
