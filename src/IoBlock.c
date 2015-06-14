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

#include <IoBlock.h>

IoBlockReturn IoBlock_open(IoBlockHandle* handle, const char* filename, bool cacheable, bool readOnly)
{
    if(NULL == handle)
    {
        return IO_ERROR;
    }

    handle->_isReadOnly = readOnly;
    handle->_file       = NULL;
    handle->_errorState = IO_ERROR;

    do
    {
        const char* args = NULL;
        if(readOnly)
        {
            args = "rb";
        }
        else
        {
            args = "wb";
        }

        handle->_file = fopen(filename, args);
        if(NULL == handle->_file)
        {
            break;
        }

        handle->_errorState = IO_SUCCESS;

        if(!cacheable)
        {
            setbuf(handle->_file, NULL);
        }
    } while(0);

    return handle->_errorState;
}

void IoBlock_close(IoBlockHandle* handle)
{
    if((NULL != handle) && (IO_ERROR != handle->_errorState))
    {
        fclose(handle->_file);
        handle->_file = NULL;
    }
}


IoBlockReturn IoBlock_read(IoBlockHandle* handle, size_t address, void* buffer, size_t length)
{
    IoBlockReturn ret = IO_ERROR;
    do
    {
        if((NULL == buffer) | (NULL == handle))
            break;

        if(IO_ERROR == handle->_errorState)
            break;

        if(0 != fseek(handle->_file, address, SEEK_SET))
            break;

        if(length != fread(buffer, 1, length, handle->_file))
            break;

        ret = IO_SUCCESS;
    } while(0);

    return ret;
}

IoBlockReturn IoBlock_write(IoBlockHandle* handle, size_t address, const void* buffer, size_t length)
{
    IoBlockReturn ret = IO_ERROR;
    do
    {
        if((NULL == buffer) | (NULL == handle))
            break;

        if(handle->_isReadOnly)
            break;

        if(IO_ERROR == handle->_errorState)
            break;

        if(0 != fseek(handle->_file, address, SEEK_SET))
            break;

        if(length != fwrite(buffer, 1, length, handle->_file))
            break;

        ret = IO_SUCCESS;
    } while(0);

    return ret;
}

size_t IoBlock_size(IoBlockHandle* handle)
{
    size_t sz = 0;
    do
    {
        if(NULL == handle)
            break;

        if(IO_ERROR == handle->_errorState)
            break;

        if(0 != fseek(handle->_file, 0, SEEK_END))
            break;

        sz = ftell(handle->_file);
    } while(0);

    return sz;
}
