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

#ifndef RAQ_H_
#define RAQ_H_

#include <glib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    GQueue*         _accessQueue;
    GRand *         _rand;
} RAQHandle;

typedef struct
{
    size_t address;
    size_t length;
} RAQBlock;

/**
 * @brief Random Access Queue init
 *
 * @param totalSize Total number of bytes to access
 * @param minAccess Minimum access size in bytes
 * @param maxAccess Maximum access size in bytes
 * @return Handle required for all other RAQ_* functions
 */
RAQHandle   RAQ_init        (size_t totalSize, uint16_t minAccess, uint16_t maxAccess);
void        RAQ_deinit      (RAQHandle* handle);

/**
 * @brief Checks if the RAQ is empty
 *
 * @param handle RAQ instance handle
 * @return true if empty, else false
 */
bool        RAQ_isEmpty     (RAQHandle* handle);

/**
 * @brief Pops a random access block out of the queue
 * 
 * @param handle RAQ instance handle
 * @return Access block
 */
RAQBlock    RAQ_popRandom   (RAQHandle* handle);

#endif /* RAQ_H_ */
