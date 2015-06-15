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

#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define INVALID_FD (-1)

IoBlockReturn IoBlock_open(IoBlockHandle* handle, const char* filename, bool cacheable, bool readOnly)
{
    if(NULL == handle)
    {
        return IO_ERROR;
    }

    handle->_isReadOnly = readOnly;
    handle->_fd         = INVALID_FD;
    handle->_errorState = IO_ERROR;

    do
    {
        int args = 0;
        if(readOnly)
        {
            args = O_RDONLY;
        }
        else
        {
            args = O_CREAT | O_RDWR;
        }

        if(!cacheable)
        {
            // __O_DIRECT ?
            args |= O_SYNC;
        }

        handle->_fd = open(filename, args);
        if(INVALID_FD == handle->_fd)
        {
            perror("Open: ");
            break;
        }

        handle->_errorState = IO_SUCCESS;
    } while(0);

    return handle->_errorState;
}

void IoBlock_close(IoBlockHandle* handle)
{
    if((NULL != handle) && (IO_ERROR != handle->_errorState))
    {
        close(handle->_fd);
        handle->_fd = INVALID_FD;
    }
}


IoBlockReturn IoBlock_read(IoBlockHandle* handle, size_t address, void* buffer, size_t length)
{
    IoBlockReturn ret = IO_ERROR;
    do
    {
        if((NULL == buffer) || (NULL == handle))
            break;

        if(IO_ERROR == handle->_errorState)
            break;

        if(address != lseek(handle->_fd, address, SEEK_SET))
        {
            perror("Seek: ");
            break;
        }

        if(length != read(handle->_fd, buffer, length))
        {
            perror("Read: ");
            break;
        }

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

        if(address != lseek(handle->_fd, address, SEEK_SET))
        {
            perror("Seek: ");
            break;
        }

        if(length != write(handle->_fd, buffer, length))
        {
            perror("Write: ");
            break;
        }

        if(0 != fsync(handle->_fd))
        {
            perror("Sync: ");
            break;
        }

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

        struct stat fd_stats;
        if(0 != fstat(handle->_fd, &fd_stats))
        {
            perror("Stat: ");
            break;
        }

        if(S_ISBLK(fd_stats.st_mode))
        {
            ioctl(handle->_fd, BLKGETSIZE64, &sz);
            break;
        }
        else if(S_ISREG(fd_stats.st_mode))
        {
            sz = lseek(handle->_fd, 0, SEEK_END);
            break;
        }

    } while(0);

    return sz;
}
