/** \file ALF_str.h
* Macros and functions concerning strings operations
**/

#ifndef ALF_str_h
#define ALF_str_h

#include "ALF_common.h"

#include <stdlib.h>
#include <stdint.h>

/// Python-like string.split()
/**
 * Params:

 * -char *string: The string to be splited.

 * -char *delimiters: The delimiter that is going to split the string.

 * Return value:

 * ->char **: An 'array' of strings. Each string is a part of the original string param.

**/
char **ALF_STR_split(const char *string, const char *delimiters);

/// Free an splitted string, like the returned from ALF_STR_split().
void ALF_STR_freeSplitted(char **splitted);

/// 
char* ALF_STR_changeExtension(const char* word, const char* newExt, size_t lenExt);

/** Finds the position of right inside the string. Usefull, for example, for searching a pair of parenthesis.
 
 * If there are multiple combinations of 'left' and 'right', it counts it until a pair is matched. 

 * The first 'left' should not be in the string.

 * The return value is part of the original 'string' parameter.

 * If not found or the 'string' parameter is NULL, NULL is returned.
**/
char *ALF_STR_searchCharPair(const char *string, char left, char right);

#endif /* ALF_str_h */
