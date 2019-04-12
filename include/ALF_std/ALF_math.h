/** \file ALF_math.h
* Some math releated functions.
**/

#ifndef ALF_math_h
#define ALF_math_h

#include "ALF_common.h"

#include <stdint.h>
#include <stdbool.h>

/// Multiplies two numbers. The result is stored in result and carry params. Returns true if multiplies overflows, false otherwise.
bool ALF_MATH_multiply_64(uint64_t a, uint64_t b, uint64_t *result, uint64_t *carry);

///
bool ALF_MATH_multiply_32(uint32_t a, uint32_t b, uint32_t *result, uint32_t *carry);

///
bool ALF_MATH_multiply_16(uint16_t a, uint16_t b, uint16_t *result, uint16_t *carry);

///
bool ALF_MATH_multiply_8(uint8_t a, uint8_t b, uint8_t *result, uint8_t *carry);

/// Autodetects what function use depending on the type of the first parameter. If any parameter is of a different type than the first one, undefined behavior.
#define ALF_MATH_multiply(a, b, result, carry)  _Generic((a), \
                                        uint64_t: ALF_MATH_multiply_64, \
                                        uint32_t: ALF_MATH_multiply_32, \
                                        uint16_t: ALF_MATH_multiply_16, \
                                        uint8_t: ALF_MATH_multiply_8)(a, b, result, carry)

#endif /* ALF_math_h */
