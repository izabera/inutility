#include "lib/common.h"
#include <sys/mman.h>

static volatile int gotsig;
static void sighandler(int s) { gotsig = s; }

int main(int argc, char *argv[]) {
  options(/*"f"*/ "pk|s|", .argleast = 2);
  // -f == --foreground
  // -p == --preserve-status
  // dunno why gnu doesn't have short opts for this
  int signal = flag('s') ? lastnum('s') : SIGTERM;
  double term_time = strtod(argv[1], NULL), kill_time = flag('k') ? lastnum('k') : 0;
  if (term_time < 0 || kill_time < 0) return -1;

  volatile char *map = mmap(NULL, 1, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  if (map) map[0] = 0; // silly thing to check whether exec succeeded
  pid_t pid;
  if ((pid = fork()) == -1) return errno;
  else if (pid == 0) {
    setpgid(0, 0); // this is needed to kill -pid on linux
    execvp(argv[2], argv+2);
    if (map) map[0] = 1;
    return errno;
  }

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sighandler;
  sigaction(SIGALRM, &sa, NULL);

  struct itimerval iterm = { .it_value = { (time_t) term_time, term_time - (time_t) term_time } },
                   ikill = { .it_value = { (time_t) kill_time, kill_time - (time_t) kill_time } };
  setitimer(ITIMER_REAL, &iterm, NULL);

  int status = 0, ret;
  ret = waitpid (pid, &status, 0);

  if (ret != -1) {
    if ((map && map[0]) || // execvp failed
        (WIFEXITED(status) && flag('p'))) return WEXITSTATUS(status);
    return errno;
  }

  kill(-pid, signal);
  if (!flag('k')) {
    while (waitpid(pid, &status, 0) == -1) continue;
    if (flag('p')) return WEXITSTATUS(status);
    else return 124; // copy what gnu does
  }

  sigaction(SIGALRM, &sa, NULL);
  setitimer(ITIMER_REAL, &ikill, NULL);
  ret = waitpid(pid, &status, 0);
  if (ret != -1 && WIFEXITED(status)) return errno;
  kill(-pid, SIGKILL);
  kill(0, SIGKILL);

  return 137;
}
