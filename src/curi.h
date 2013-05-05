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

#ifndef CURI_H
#define CURI_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** an opaque handle to a parser */
typedef struct curi_handle_t* curi_handle;

/** error codes returned from this interface */
typedef enum 
{
    curi_status_success = 0, //!< No error
    curi_status_canceled, //!< A callback returned 0, stopping the operation 
    curi_status_error //!< An error occured
} curi_status;

typedef struct 
{
    int (*scheme)(void* userData, const char* scheme, size_t schemeLen);
    int (*userinfo)(void* userData, const char* userinfo, size_t userinfoLen);
    int (*host)(void* userData, const char* host, size_t hostLen);
    int (*port)(void* userData, const char* port, size_t portLen);
    int (*path)(void* userData, const char* path, size_t pathLen);
    int (*query)(void* userData, const char* query, size_t queryLen);
    int (*fragment)(void* userData, const char* fragment, size_t fragmentLen);
} curi_callbacks;

curi_handle curi_alloc(const curi_callbacks* callbacks, void* userData);

void curi_free(curi_handle handle);

curi_status curi_parse(curi_handle handle, const char* uri, size_t len);

#ifdef __cplusplus
}
#endif

#endif