#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("#ls|", .argleast = 1);
  int64_t signal;
       if (flag('s'))   signal =  flags[opt('s')].nums[flag('s')-1];
  else if (flag('#')) { signal = *flags[opt('#')].nums++; flag('#')--; }
  else                  signal = 15;
  /* if the option parser gets -9 -1 both are put in flag('#')
   * only the first is a signal, the other is a pid with - to mean process group 
   * in   kill -s 7 -9 -1   both -9 and -1 are pids
   * so   kill -9 -s 1 -4 81 -5   sends signal 1 to -9 -4 81 -5 */

  if (flag('l')) {
    puts("HUP INT QUIT ILL TRAP ABRT IOT BUS FPE KILL USR1 SEGV USR2 PIPE ALRM");
    puts("TERM STKFLT CHLD CLD CONT STOP TSTP TTIN TTOU URG XCPU XFSZ VTALRM PROF");
    puts("WINCH IO POLL PWR UNUSED SYS RT<N> RTMIN+<N> RTMAX-<N>");
  }
  else {
    if (argc == 1 && flag('#') == 0) return 1;
    for (size_t i = 0; i < flag('#'); i++)
      kill(-flags[opt('#')].nums[i], signal);
    while (*++argv) {
      char *end = NULL;
      pid_t pid = strtol(*argv, &end, 10);
      if (end) return 1;
      kill(pid, signal);
    }
  }
  return errno;
}
