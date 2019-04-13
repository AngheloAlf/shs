#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void getHttpDate(char *dst_http_date, size_t maxsize);

unsigned int string_hash(char *string);

int strcicmp(const char *a, const char *b);

#endif /* UTIL_H */
