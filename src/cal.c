#include "lib/common.h"

// http://www.tondering.dk/claus/cal/chrweek.php#calcdow
static int dow(int year, int month, int d) {
  int a = (13 - month) / 12;
  int y = year - a;
  int m = month + 12 * a - 1;

  if (y > 1752 || (year == 1752 && month > 8))
    return (d + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;
  return (5 + d + y + y / 4 + (31 * m) / 12) % 7;
}

#define isleap() (tm->tm_year % 4 ? 0 : tm->tm_year <= 1752 ? 1 : tm->tm_year % 100 ? 1 : tm->tm_year % 400 ? 0 : 1)
static char *printmon(struct tm *tm, int zero) {
  static char mon[6][7][3]; // static!
  int day = dow(tm->tm_year, tm->tm_mon, 1),
      lengths[] = { 31, 28+isleap(), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if (tm->tm_year == 1752 && tm->tm_mon == 8) {
    memcpy(mon, "       1  2 14 15 16\n" // this is stupid and useless
                "17 18 19 20 21 22 23\n"
                "24 25 26 27 28 29 30\n"
                "                    \n"
                "                    \n"
                "                    \n", 126);
    if (zero)
      for (int i = 0; i < 6; i++)
        for (int j = 0; j < 7; j++)
          mon[i][j][2] = 0;
  }
  else {
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 7; j++) {
        if ((i == 0 && j < day) || 1+i*7+j-day > lengths[tm->tm_mon]) memcpy(mon[i][j], "  ", 2);
        else snprintf(mon[i][j], 3, "%2d", 1+i*7+j-day);
        mon[i][j][2] = zero ? 0 : j < 6 ? ' ' : '\n';
      }
  }
  return (char*) mon;
}

int main(int argc, char *argv[]) {
  options("", .arglessthan = 3);
  struct tm *tm = localtime(&(time_t) { time(NULL) });
  tm->tm_year += 1900;
  char *months[] = { "January", "February", "March", "April", "May", "June",
                     "July", "August", "September", "October", "November", "December" },
       *days = "Su Mo Tu We Th Fr Sa";

#define rpad(x) ((int)(20 - strlen(months[x]))/2)
  switch (argc) {
    case  3: tm->tm_year = atoi(argv[2]);
             if ((tm->tm_mon = atoi(argv[1]) - 1) < 0 || tm->tm_mon > 11) return -1; /* fallthrough */
    case  1: printf("%*s %d\n%s\n", 17-rpad(tm->tm_mon), months[tm->tm_mon], tm->tm_year, days);
             fwrite_unlocked(printmon(tm, 0), 1, 126, stdout);
             break;

    case  2: tm->tm_year = atoi(argv[1]);
             printf("%35d\n\n", tm->tm_year);
             for (int i = 0; i < 4; i++) {
               printf("%*s%*s  %*s%*s  %*s%*s\n%s  %s  %s\n",
                   20-rpad(i*3+0), months[i*3+0], rpad(i*3+0), "",
                   20-rpad(i*3+1), months[i*3+1], rpad(i*3+1), "",
                   20-rpad(i*3+2), months[i*3+2], rpad(i*3+2), "",
                   days, days, days);

               char m[3][6][7][3];
               tm->tm_mon = i*3+0; memcpy(m[0], printmon(tm, 1), 126);
               tm->tm_mon = i*3+1; memcpy(m[1], printmon(tm, 1), 126);
               tm->tm_mon = i*3+2; memcpy(m[2], printmon(tm, 1), 126);

               for (int j = 0; j < 6; j++)
                 printf("%s %s %s %s %s %s %s  %s %s %s %s %s %s %s  %s %s %s %s %s %s %s\n",
                     m[0][j][0], m[0][j][1], m[0][j][2], m[0][j][3], m[0][j][4], m[0][j][5], m[0][j][6],
                     m[1][j][0], m[1][j][1], m[1][j][2], m[1][j][3], m[1][j][4], m[1][j][5], m[1][j][6],
                     m[2][j][0], m[2][j][1], m[2][j][2], m[2][j][3], m[2][j][4], m[2][j][5], m[2][j][6]);
             }
  }
  return errno;
}
