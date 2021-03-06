#include "util.h"

#include <string.h>
#include <ctype.h>
#include <time.h>

void getHttpDate(char *dst_http_date, size_t maxsize){
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(dst_http_date, maxsize, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

unsigned int string_hash(char *string){
    unsigned int hash = 7;
    size_t length = strlen(string);
    for(size_t i = 0; i < length; ++i){
        hash = hash*31 + string[i];
    }
    return hash;
}

int strcicmp(const char *a, const char *b){
    for(; *a && *b; a++, b++){
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0){
            return d;
        }
    }
    return 0;
}
