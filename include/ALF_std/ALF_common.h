/** \file ALF_common.h
* The common interface. 
* Has any definition that will be used in more than one lib file.
**/

#ifndef ALF_common_h
#define ALF_common_h

#include <stdlib.h>

#define _ALF_STR(x) #x

/// Converts things to string at compile-time.
#define ALF_STR(x) _ALF_STR(x)

#define _ALF_NUMBER_LENGTH(x) (sizeof(ALF_STR(x)) - 1)

/// Returns the amount of digits in a number.
#define ALF_NUMBER_LENGTH(x) _ALF_NUMBER_LENGTH(x)


#define ALF_CONC_(A, B) A##B
#define ALF_CONC(A, B) ALF_CONC_(A, B)

#define ALF_MAKE_VERSION(x, y) ALF_CONC(x, ALF_CONC(.,y))

#define ALF_BITS_IN_BYTE 8

/// Takes the left half of the variable and moves it to the right. The old right half is deleted.
#define ALF_MOVE_RIGHT(x) (x >> ((sizeof x)*ALF_BITS_IN_BYTE/2) )

/// Takes the right half of the variable and moves it to the left. The old left half is deleted.
#define ALF_MOVE_LEFT(x) (x << ((sizeof x)*ALF_BITS_IN_BYTE/2) )

/// Return the upper half of the variable.
#define ALF_HIGH_HALF(x) ALF_MOVE_LEFT(ALF_MOVE_RIGHT(x))

/// Return the lower half of the variable.
#define ALF_LOW_HALF(x) (x & ( (1L << ( (sizeof x)*ALF_BITS_IN_BYTE/2 ) ) - 1))

/// Copies the input_data into a new space of memory. The caller is responsible of free.
void *ALF_allocAndCopy(const void *input_data, size_t size);

#endif /* ALF_common_h */
