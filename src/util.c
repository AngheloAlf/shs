#include "util.h"

#include <time.h>

void getHttpDate(char *dst_http_date, size_t maxsize){
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(dst_http_date, maxsize, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}
