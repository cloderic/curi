// Copyright (c) 2013 Clodéric Mars

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "curi.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

struct curi_handle_t 
{
    curi_callbacks callbacks;
    void* userData;
};

curi_handle curi_alloc(const curi_callbacks* callbacks, void* userData)
{
    if (callbacks == NULL)
    {
        // TODO: set an error string somewhere
        return NULL;
    }

    curi_handle hnd = (curi_handle)malloc(sizeof(struct curi_handle_t));
    if (hnd == NULL)
    {
        // TODO: set an error string somewhere
        return NULL;
    }

    memcpy(&hnd->callbacks, callbacks, sizeof(curi_callbacks));
    hnd->userData = userData;

    return hnd;
}

void curi_free(curi_handle handle)
{
    free(handle);
}

static curi_status read_scheme(curi_handle handle, const char* uri, size_t* offset, size_t len)
{
    size_t i = *offset;

    // 1st charcter should belong to [a-zA-Z]
    if (isalpha(uri[i]) == 0)
    {
        // TODO: set an error string somewhere
        return curi_status_error;
    }
    ++i;
    
    bool endFound = false;       
    while (i < len && !endFound)
    {
        switch (uri[i])
        {
        case ':': // scheme separator reached
            endFound = true;
            break;
        case '+': // valid special characters
        case '-':
        case '.':
            ++i;
            break;
        default:
            if (isalnum(uri[i]) == 0)
            {
                // TODO: set an error string somewhere
                return curi_status_error;
            }
            ++i;
        }
    }

    // end of uri reached
    if (handle->callbacks.curi_scheme)
    {
        if (handle->callbacks.curi_scheme(handle->userData,uri,i) == 0)
        {
            return curi_status_canceled;
        }
    }
    *offset = i;
    return curi_status_success;
}

curi_status curi_parse(curi_handle handle, const char* uri, size_t len)
{
    curi_status status = curi_status_success;

    size_t offset = 0;
    if (status == curi_status_success)
        status = read_scheme(handle,uri,&offset,len);

    return status; 
}
