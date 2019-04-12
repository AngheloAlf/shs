/** \file ALF_jit.h
* A simple cross-platform wraper to use JIT recompilation.
**/

#ifndef ALF_jit_h
#define ALF_jit_h

#include "ALF_common.h"

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

typedef struct {
    uint64_t position;
    /**
     * Posible states:

     * 0x0: Normal. Data can be inserted.

     * 0x1: Ready for execution.
    **/
    uint8_t state;
    uint8_t *code;
} ALF_jit_buf;


/// Creates a buffer for use as a JIT.
/**
 * 
**/
ALF_jit_buf *ALF_jit_init(void);

/// Returns the aviable size for code.
/**
 * 
**/
uint64_t ALF_jit_getAvaibleSize(ALF_jit_buf *handler);

/// Inserts a instruction in the code.
/**
 * 
**/
int ALF_jit_addInstruction(ALF_jit_buf *handler, uint64_t size, uint64_t ins);

/// Inserts an immediate (number) in the code.
/**
 * 
**/
int ALF_jit_addImmediate(ALF_jit_buf *handler, uint64_t size, const void *value);

/// Prepares the code to be executed.
/**
 * 
**/
int ALF_jit_finalize(ALF_jit_buf *handler);

/// Free the memory.
/**
 * 
**/
long ALF_jit_free(ALF_jit_buf *handler);

/// Returns the last error.
/**
 * 
**/
const char *ALF_jit_getError(void);

/// Returns the last error code.
int ALF_jit_getErrorCode(void);

/// Returns the page size.
uint64_t ALF_jit_pageSize(void);

#endif /* ALF_jit_h */
