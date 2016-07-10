#include "common.h"

int print_uptime() {
  struct sysinfo info;
  if (sysinfo(&info) == -1) return errno;
  time_t timenow = time(NULL);
  struct tm *tmnow = localtime(&timenow);
  printf(" %02d:%02d:%02d up ", tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec);

  int day, hour, min, users = 0;
  info.uptime /= 60;
  min = info.uptime % 60;
  info.uptime /= 60;
  hour = info.uptime % 24;
  day = info.uptime / 24;

  if (day) printf("%d day%s", day, plural(day));
  if (hour) printf("%2d:%02d", hour, min);
  else printf("%d min", min);

  struct utmpx *entry;
  setutxent();
  while ((entry = getutxent()))
    if (entry->ut_type == USER_PROCESS) users++;
  endutxent();

  printf(
      ",  %d user%s, "
      " load average: %.2f, %.2f, %.2f\n",
      users, plural(users),
      info.loads[0] / (65536 * 1.0), info.loads[1] / (65536 * 1.0), info.loads[2] / (65536 * 1.0)
  );
  return errno;
}
