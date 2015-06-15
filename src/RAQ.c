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

#include <RAQ.h>
#include <stdlib.h>

RAQHandle RAQ_init(size_t totalSize, uint16_t minAccess, uint16_t maxAccess)
{
    RAQHandle handle;

    handle._accessQueue = g_queue_new();
    handle._rand = g_rand_new();

    size_t address = 0;
    size_t bytesLeft = totalSize;

    while(bytesLeft > 0)
    {
        RAQBlock* raq = (RAQBlock*) malloc(sizeof(RAQBlock));

        raq->address = address;
        if(bytesLeft < maxAccess)
        {
            raq->length = bytesLeft;
        }
        else
        {
            raq->length = (size_t) g_rand_int_range(handle._rand, (gint32) minAccess, ((gint32) maxAccess) + 1);
        }

        g_queue_push_tail(handle._accessQueue, raq);

        bytesLeft   -= raq->length;
        address     += raq->length;
    }

    return handle;
}

void RAQ_deinit(RAQHandle* handle)
{
    if(NULL != handle)
    {
        g_rand_free(handle->_rand);
        handle->_rand = NULL;

        while(TRUE != g_queue_is_empty(handle->_accessQueue))
        {
            free(g_queue_pop_head(handle->_accessQueue));
        }

        g_queue_free(handle->_accessQueue);
        handle->_accessQueue = NULL;
    }
}

bool RAQ_isEmpty(RAQHandle* handle)
{
    if(NULL == handle)
    {
        return true;
    }
    else
    {
        return TRUE == g_queue_is_empty(handle->_accessQueue);
    }
}

RAQBlock RAQ_popRandom(RAQHandle* handle)
{
    RAQBlock block;
    block.address = 0;
    block.length = 0;

    do
    {
        if(NULL == handle)
            break;

        guint queueLength = g_queue_get_length(handle->_accessQueue);
        guint element = g_rand_int_range(handle->_rand, 0, queueLength);

        RAQBlock* raq = g_queue_pop_nth(handle->_accessQueue, element);
        block = *raq;
        free(raq);
    } while(0);
    
    return block;
}
