/**
 * Copyright (c) 2015, Bradley Kim Schleusner < bradschl@gmail.com >
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IO_BLOCK_H_
#define IO_BLOCK_H_

#include <stdio.h>
#include <stdbool.h>

typedef enum
{
    IO_SUCCESS,
    IO_ERROR
} IoBlockReturn;

typedef struct
{
    bool            _isReadOnly;
    int             _fd;
    IoBlockReturn   _errorState;
} IoBlockHandle;


IoBlockReturn IoBlock_open  (IoBlockHandle* handle, const char* filename, bool cacheable, bool readOnly);
void          IoBlock_close (IoBlockHandle* handle);

IoBlockReturn IoBlock_read  (IoBlockHandle* handle, size_t address, void* buffer, size_t length);
IoBlockReturn IoBlock_write (IoBlockHandle* handle, size_t address, const void* buffer, size_t length);

size_t        IoBlock_size  (IoBlockHandle* handle);

#endif /* IO_BLOCK_H_ */
