#include <fate/timestamp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* See http://stackoverflow.com/a/1765088 */
fe_timestamp fe_timestamp_of_cpp_date_time(const char *date, const char *time) {
    char s_month[4];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    struct tm t = {0};

    memcpy(s_month, date, 3);
    s_month[3] = '\0';

    t.tm_sec  = strtoul(time,   NULL, 10);
    t.tm_min  = strtoul(time+3, NULL, 10);
    t.tm_hour = strtoul(time+6, NULL, 10);
    t.tm_mon  = (strstr(month_names, s_month)-month_names)/3;
    t.tm_mday = strtoul(date+4, NULL, 10);
    t.tm_year = strtoul(date+6, NULL, 10) - 1900;
    t.tm_isdst = -1;

    return mktime(&t);
}

