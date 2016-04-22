/*
 *  cal.c
 *  This file is part of posixutils.
 *
 *  Copyright (C) 2015 izabera
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
/* cal */

/* TODO:
 *   print full calendar on 3 columns 
 *   fix the 1900 issue since we don't really need it
 *   generate days' 2 letter names instead of hardcoding them */

void printheader (struct tm *time, bool printyear) {
  char buffer[22], spaces[10];
  size_t i, output;
  if (printyear) {
    time->tm_year-=1900;
    output = strftime(buffer, 80, "%B %Y", time);
    time->tm_year+=1900;
  }
  else output = strftime(buffer, 80, "%B", time);
  for (i = 0; i < (21-output)/2; i++) spaces[i] = ' ';
  spaces[i] = 0;
  printf("%s%s\n", spaces, buffer);
}

/* sakamoto method to get the day of week in gregorian calendar */
int dow (int y, int m, int d) {
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  y -= m < 3;
  return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}


#define isleap(year) ((!((year)%400)) || (((year)%100) && (!((year)%4))))
#define draw() {                                                              \
          for (i = 1; i <= monthlen[time->tm_mon]; i++)                       \
            array[i+shift-1] = i;                                             \
          for (i = 0; i < 6; i++) {                                           \
            for (j = 0; j < 6; j++)                                           \
              if (array[i*7+j] != 0) printf("%2d ", array[i*7+j]);            \
              else printf("   ");                                             \
            if (array[i*7+j] != 0) printf("%2d\n", array[i*7+j]);             \
            else printf("  \n");                                              \
          }                                                                   \
        }

void printmonth (struct tm *time) {
  int array[42] = {0}, i, j, shift,
      monthlen[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  shift = dow(time->tm_year, time->tm_mon, time->tm_mday);
  if (isleap(time->tm_year)) monthlen[1] = 29;

  if (time->tm_year > 1752) {
    draw();
  }
  else if (time->tm_year == 1752) {
    if (time->tm_mon > 9) {
     /* gregorian calendar starts here */
      draw();
    }
    else if (time->tm_mon == 9) {
      puts("       1  2 14 15 16");
      puts("17 18 19 20 21 22 23");
      puts("24 25 26 27 28 29 30");
      puts("\n\n");
    }
    else {
      shift = (shift+4) % 7;
      draw();
    }
  }
  else {
    shift = (shift+4) % 7;
    draw();
  }
}

/* FIXME */
#define generate_days()                                                       \
          struct tm *tmp;                                                     \
          char buffer[5];                                                     \
          for (tmp->tm_wday = 0; tmp->tm_wday < 7; tmp->tm_wday++) {          \
            strftime(buffer, 5, "%a", tmp);                                   \
            printf ("%s-",buffer);                                            \
          }                                                                   \
          printf("coso %s\n", days);


int main (int argc, char *argv[]) {
  struct tm *local_time;
  bool printall = false;
  time_t unix_time;
  time(&unix_time);
  local_time = localtime(&unix_time);

  /* saving the date to a human readable format */

  switch (argc) {
    case 1:
      local_time->tm_year += 1900;
      local_time->tm_mon++;
      break;
    case 2:
      local_time->tm_year = (unsigned) atoi(argv[1]);
      local_time->tm_mon = 1;
      printall = true;
      break;
    case 3:
      local_time->tm_mon = (unsigned) atoi(argv[1]);
      local_time->tm_year = (unsigned) atoi(argv[2]);
      break;
    default:
      fprintf(stderr, "Usage: %s [[month] year]\n", argv[0]);
      exit(EXIT_FAILURE);
  }
  if (local_time->tm_mon > 12 || local_time->tm_year < 0 || local_time->tm_year > 9999) {
    fprintf(stderr, "Usage: %s [[month] year]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char days[] = "Su Mo Tu We Th Fr Sa";
  /*char days[22];*/
  /*generate_days();*/

  if (printall) {
    do {
      printheader(local_time, false);
      puts(days);
      printmonth(local_time);
    } while (++local_time->tm_mon < 12);
  }
  else {
    printheader(local_time, true);
    puts(days);
    printmonth(local_time);
  }
  return 0;
}
