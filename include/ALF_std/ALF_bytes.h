/** \file ALF_bytes.h
* A simple structure for storing bytes.
**/

#ifndef ALF_bytes_h
#define ALF_bytes_h

#include "ALF_common.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct{
    uint8_t *bytes;
    size_t size;
} ALF_bytes;

/** Object initializer.
 * Params:

 * - const uint8_t *bytes: The bytes that will be stored. If NULL, the bytes will be initialized to zero.

 * - size_t size: The size of the stored bytes. If the bytes's size are greater than this param, it will be truncated. If equal to zero, the object will not be created and NULL will be returned.

 * Return value: The object. In case of error, NULL will be returned.
**/
ALF_bytes *ALF_bytes_init(const uint8_t *bytes, size_t size);


/** Object destroyer.
 * Params:

 * - ALF_bytes *bytesObj: The object to be freed.
**/
void ALF_bytes_free(ALF_bytes *bytesObj);

/// Return the size of the bytes.
size_t ALF_bytes_getSize(ALF_bytes *bytesObj);

/// Return a copy of the bytes. You have to free it after use.
uint8_t *ALF_bytes_getBytes(ALF_bytes *bytesObj);

/// Return the same bytes contained inside of the object. You must not free this.
const uint8_t *ALF_bytes_seeBytes(ALF_bytes *bytesObj);

/// Set a new set of bytes, deleting the old one.
bool ALF_bytes_setBytes(ALF_bytes *bytesObj, const uint8_t *bytes, size_t size);

/** Change the size.
 * If the size param is greater than the old size, the memory is allocated with zeros.

 * If size is smaller, the memory is truncated.

 * Returns true is size was successfully changed. false otherwise.
**/
bool ALF_bytes_setSize(ALF_bytes *bytesObj, size_t size);

/// Return the byte at the position provided. If position is greater than the size, returns the last byte.
uint8_t ALF_bytes_getByte(ALF_bytes *bytesObj, size_t position);

/// Change the byte at the position provided.
void ALF_bytes_changeByte(ALF_bytes *bytesObj, size_t position, uint8_t newByte);

#endif /* ALF_bytes_h */
