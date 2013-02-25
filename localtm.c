#include <stdio.h>
#include <time.h>


#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)

const unsigned short int __mon_yday[2][13] =
  {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
  };

/* Compute the `struct tm' representation of *T,
   offset OFFSET seconds east of UTC,
   and store year, yday, mon, mday, wday, hour, min, sec into *TP.
   Return nonzero if successful.  */
int
__offtime (t, offset, tp)
     const time_t *t;
     long int offset;
     struct tm *tp;
{
  long int days, rem, y;
  const unsigned short int *ip;

  days = *t / SECS_PER_DAY;
  rem = *t % SECS_PER_DAY;
  rem += offset;
  while (rem < 0)
    {
      rem += SECS_PER_DAY;
      --days;
    }
  while (rem >= SECS_PER_DAY)
    {
      rem -= SECS_PER_DAY;
      ++days;
    }
  tp->tm_hour = rem / SECS_PER_HOUR;
  rem %= SECS_PER_HOUR;
  tp->tm_min = rem / 60;
  tp->tm_sec = rem % 60;
  /* January 1, 1970 was a Thursday.  */
  tp->tm_wday = (4 + days) % 7;
  if (tp->tm_wday < 0)
    tp->tm_wday += 7;
  y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

  while (days < 0 || days >= (__isleap (y) ? 366 : 365))
    {
      /* Guess a corrected year, assuming 365 days per year.  */
      long int yg = y + days / 365 - (days % 365 < 0);

      /* Adjust DAYS and Y to match the guessed year.  */
      days -= ((yg - y) * 365
               + LEAPS_THRU_END_OF (yg - 1)
               - LEAPS_THRU_END_OF (y - 1));
      y = yg;
    }
  tp->tm_year = y - 1900;
  if (tp->tm_year != y - 1900)
    {
      /* The year cannot be represented due to overflow.  */
      //__set_errno (EOVERFLOW);
      return 0;
    }
  tp->tm_yday = days;
  ip = __mon_yday[__isleap(y)];
  for (y = 11; days < (long int) ip[y]; --y)
    continue;
  days -= ip[y];
  tp->tm_mon = y;
  tp->tm_mday = days + 1;
  return 1;
}

int diff_tm(struct tm *tm1, struct tm *tm2, int debug) {
        if (debug)
                printf("%04d %02d %02d %02d:%02d:%02d   %04d %02d %02d %02d:%02d:%02d\n", 
                        tm1->tm_year, tm1->tm_mon, tm1->tm_mday, tm1->tm_hour, tm1->tm_min, tm1->tm_sec,
                        tm2->tm_year, tm2->tm_mon, tm2->tm_mday, tm2->tm_hour, tm2->tm_min, tm2->tm_sec);

        return !(tm1->tm_year == tm2->tm_year &&
                 tm1->tm_mon == tm2->tm_mon &&
                 tm1->tm_mday == tm2->tm_mday &&
                 tm1->tm_hour == tm2->tm_hour &&
                 tm1->tm_min == tm2->tm_min &&
                 tm1->tm_sec == tm2->tm_sec);
}

int main() {
        int i = 0;
        struct tm tm1, tm2;
        time_t t;

        time(&t);

        for(; i < 3600 * 24 * 100; i ++ ) {
                __offtime(&t, 8*3600, &tm1);
                localtime_r(&t, &tm2);

                if(diff_tm(&tm1, &tm2, 0)) {
                        printf("diff at %d\n", t);
                        diff_tm(&tm1, &tm2, 1);
                }

                t += i;
        }

        return 0;
}

