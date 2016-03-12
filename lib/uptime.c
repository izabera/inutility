#include <stdio.h>
#include <sys/sysinfo.h>
#include <utmpx.h>
#include <time.h>

#include "uptime.h"

int print_uptime() {
  struct sysinfo info;
  if (sysinfo(&info) == -1) return 1;
  time_t timenow = time(NULL);
  struct tm *tmnow = localtime(&timenow);
  int day, hour, min, users = 0;
  info.uptime /= 60;
  min = info.uptime % 60;
  info.uptime /= 60;
  hour = info.uptime % 24;
  day = info.uptime / 24;

  struct utmpx *entry;
  char *uptime = "";

  setutxent();
  while ((entry = getutxent()))
    if (entry->ut_type == USER_PROCESS) users++;
  endutxent();

  if (day) asprintf(&uptime, "%d day%s", day, plural(day));
  else if (hour) asprintf(&uptime, "%2d:%02d", hour, min);
  else asprintf(&uptime, "%d min", min);

  printf(
      " %02d:%02d:%02d "
      "up %s, "
      "%2d:%02d, "
      " %d user%s, "
      " load average: %.2f, %.2f, %.2f\n",
      tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec,
      uptime,
      hour, min,
      users, plural(users),
      info.loads[0] / (65536 * 1.0), info.loads[1] / (65536 * 1.0), info.loads[2] / (65536 * 1.0)
  );
  return 0;
}
