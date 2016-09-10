#include "lib/common.h"

// sakamoto method to get the day of week in gregorian calendar
static int dow (int y, int m, int d) {
  int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  y -= m < 2;
  return (y + y/4 - y/100 + y/400 + t[m] + d) % 7;
}

typedef union { char mon[6][7][3], str[127]; } mon;
#define isleap(x) (tm->tm_year % 4 ? 0 : tm->tm_year % 100 ? 1 : tm->tm_year % 400 ? 0 : 1)
static char *printmon(struct tm *tm, int zero) {
  static mon m = { .str = { 0 } }; // static! and null terminated
  int day = dow(tm->tm_year, tm->tm_mon, 1),
      lengths[] = { 31, 28+isleap(), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if (tm->tm_year < 1752 || (tm->tm_year == 1752 && tm->tm_mon < 8)) day = (day+4) % 7;
  if (tm->tm_year == 1752 && tm->tm_mon == 8) {
    memcpy(m.str, "       1  2 14 15 16\n" // this is stupid and useless
                  "17 18 19 20 21 22 23\n"
                  "24 25 26 27 28 29 30\n"
                  "                    \n"
                  "                    \n"
                  "                    \n", 126);
    if (zero)
      for (int i = 0; i < 6; i++)
        for (int j = 0; j < 7; j++)
          m.mon[i][j][2] = 0;
  }
  else {
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 7; j++) {
        if ((i == 0 && j < day) || 1+i*7+j-day > lengths[tm->tm_mon]) memcpy(m.mon[i][j], "  ", 2);
        else snprintf(m.mon[i][j], 3, "%2d", 1+i*7+j-day);
        m.mon[i][j][2] = zero ? 0 : j < 6 ? ' ' : '\n';
      }
  }
  return m.str;
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
             if ((tm->tm_mon = atoi(argv[1]) - 1) < 0 || tm->tm_mon > 11) return -1;
    case  1: printf("%*s %d\n%s\n", 17-rpad(tm->tm_mon), months[tm->tm_mon], tm->tm_year, days);
             fputs_unlocked(printmon(tm, 0), stdout);
             break;

    case  2: tm->tm_year = atoi(argv[1]);
             printf("%35d\n\n", tm->tm_year);
             for (int i = 0; i < 4; i++) {
               printf("%*s%*s  %*s%*s  %*s%*s\n%s  %s  %s\n",
                   20-rpad(i*3+0), months[i*3+0], rpad(i*3+0), "",
                   20-rpad(i*3+1), months[i*3+1], rpad(i*3+1), "",
                   20-rpad(i*3+2), months[i*3+2], rpad(i*3+2), "",
                   days, days, days);

               mon m0 = { .str = { 0 } }, m1 = { .str = { 0 } }, m2 = { .str = { 0 } };
               tm->tm_mon = i*3+0; memcpy(m0.str, printmon(tm, 1), 126);
               tm->tm_mon = i*3+1; memcpy(m1.str, printmon(tm, 1), 126);
               tm->tm_mon = i*3+2; memcpy(m2.str, printmon(tm, 1), 126);

               for (int j = 0; j < 6; j++)
                 printf("%s %s %s %s %s %s %s  %s %s %s %s %s %s %s  %s %s %s %s %s %s %s\n",
                     m0.mon[j][0], m0.mon[j][1], m0.mon[j][2], m0.mon[j][3],
                     m0.mon[j][4], m0.mon[j][5], m0.mon[j][6],
                     m1.mon[j][0], m1.mon[j][1], m1.mon[j][2], m1.mon[j][3],
                     m1.mon[j][4], m1.mon[j][5], m1.mon[j][6],
                     m2.mon[j][0], m2.mon[j][1], m2.mon[j][2], m2.mon[j][3],
                     m2.mon[j][4], m2.mon[j][5], m2.mon[j][6]);
             }
  }
  return errno;
}
