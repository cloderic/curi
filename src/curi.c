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

#include <assert.h>
#include <ctype.h>
#include <string.h>

struct curi_handle_t 
{
    curi_callbacks callbacks;
    void* userData;
};

curi_handle curi_alloc(const curi_callbacks* callbacks, void* userData)
{
    curi_handle hnd = NULL;

    if (callbacks == NULL)
    {
        // TODO: set an error string somewhere
        return NULL;
    }

    hnd = (curi_handle)malloc(sizeof(struct curi_handle_t));
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

static const char end = '\0';

static const char* read_char(const char* uri, size_t len, size_t* offset)
{    
    const char* c = &end;
    if (*offset < len)
    {
        c = uri + *offset;
    }
    ++(*offset);
    return c;
}

#define TRY(status, offset, parse_fun_call) \
{ \
    size_t __TRY_initialOffset = *offset; \
    curi_status __TRY_tryStatus = parse_fun_call; \
    if (__TRY_tryStatus == curi_status_error) \
        *offset = __TRY_initialOffset; \
    else \
        status = __TRY_tryStatus; \
} \

static curi_status parse_alpha(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    if (isalpha(*read_char(uri,len,offset)))
        return curi_status_success;
    else
        return curi_status_error;
}

static curi_status parse_digit(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    if (isdigit(*read_char(uri,len,offset)))
        return curi_status_success;
    else
        return curi_status_error;
}

static curi_status parse_char(curi_handle handle, char c, const char* uri, size_t len, size_t* offset)
{
    if (*read_char(uri,len,offset) == c)
        return curi_status_success;
    else
        return curi_status_error;
}

static curi_status parse_scheme(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / ".")
    const size_t initialOffset = *offset;
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_alpha(handle,uri,len,offset);

    if (status == curi_status_success)
    {
        while (1)
        {
            status = curi_status_error;
            if (status == curi_status_error)
                TRY(status, offset, parse_alpha(handle, uri, len, offset));

            if (status == curi_status_error)
                TRY(status, offset, parse_digit(handle, uri, len, offset));

            if (status == curi_status_error)
                TRY(status, offset, parse_char(handle, '+', uri, len, offset));

            if (status == curi_status_error)
                TRY(status, offset, parse_char(handle, '-', uri, len, offset));

            if (status == curi_status_error)
                TRY(status, offset, parse_char(handle, '.', uri, len, offset));

            if (status == curi_status_error)
            {
                // end of scheme reached
                status = curi_status_success;
                if (handle->callbacks.scheme)
                {
                    if (handle->callbacks.scheme(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
                    {
                        status = curi_status_canceled;
                    }
                }
                break;
            }

        }
    }

    return status;
}

static curi_status parse_unreserved(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
    const char* c = read_char(uri,len,offset);
    switch (*c)
    {
        case '-':
        case '.':
        case '_':
        case '~':
            return curi_status_success;
        default:
            if (isalnum(*c))
                return curi_status_success;
            else
                return curi_status_error;
    }
}

static curi_status parse_hexdigit(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    const char* c = read_char(uri,len,offset);
    switch (*c)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            return curi_status_success;
        default:
            if (isdigit(*c))
                return curi_status_success;
            else
                return curi_status_error;
    }
}

static curi_status parse_pct_encoded(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // pct-encoded = "%" HEXDIG HEXDIG
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char(handle, '%', uri, len, offset);

    if (status == curi_status_success)
        status = parse_hexdigit(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_hexdigit(handle, uri, len, offset);

    return status;
}

static curi_status parse_sub_delims(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    //sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
    //             / "*" / "+" / "," / ";" / "="
    const char* c = read_char(uri,len,offset);
    switch (*c)
    {
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=':
            return curi_status_success;
        default:
            return curi_status_error;
    }
}

static curi_status parse_userinfo_ant_at(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // userinfo_and_at = userinfo "@"
    // userinfo = *( unreserved / pct-encoded / sub-delims / ":" )
    const size_t initialOffset = *offset;
    size_t afterUserinfoOffset;
    curi_status status;

    while (1)
    {
        status = curi_status_error;
        if (status == curi_status_error)
            TRY(status, offset, parse_unreserved(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_pct_encoded(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_sub_delims(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_char(handle, ':', uri, len, offset));

        if (status == curi_status_error)
        {
            // end of uri reached
            break;
        }
    }

    afterUserinfoOffset = *offset;
    
    status = parse_char(handle, '@', uri, len, offset);

    if (status == curi_status_success && handle->callbacks.userinfo)
    {
        if (handle->callbacks.userinfo(handle->userData, uri + initialOffset, afterUserinfoOffset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_reg_name(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // reg-name = *( unreserved / pct-encoded / sub-delims )
    while (1)
    {
        curi_status status = curi_status_error;
        if (status == curi_status_error)
            TRY(status, offset, parse_unreserved(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_pct_encoded(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_sub_delims(handle, uri, len, offset));

        if (status == curi_status_error)
        {
            // end of host reached
            return curi_status_success;
        }
    }
}

static curi_status parse_dec_octet(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // dec-octet = DIGIT                 ; 0-9
    //           / %x31-39 DIGIT         ; 10-99
    //           / "1" 2DIGIT            ; 100-199
    //           / "2" %x30-34 DIGIT     ; 200-249
    //           / "25" %x30-35          ; 250-255
    curi_status status = curi_status_success;
    int number;
    char numberStr[4] = {'\0', '\0', '\0', '\0'};
    size_t previousOffset;
    size_t i;

    for (i = 0 ; i < 3 ; ++i)
    {
        previousOffset = *offset;
        if (parse_digit(handle,uri,len,offset) !=  curi_status_success)
        {
            *offset = previousOffset;
            break;
        }
        numberStr[i] = uri[previousOffset];
    }

    number = atoi(numberStr);
    if (number >= 0 && number <= 255)
        return curi_status_success;
    else
        return curi_status_error;
}

static curi_status parse_IPv4address(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_dec_octet(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_char(handle, '.', uri, len, offset);

    if (status == curi_status_success)
        status = parse_dec_octet(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_char(handle, '.', uri, len, offset);

    if (status == curi_status_success)
        status = parse_dec_octet(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_char(handle, '.', uri, len, offset);

    if (status == curi_status_success)
        status = parse_dec_octet(handle, uri, len, offset);

    return status;
}

static curi_status parse_h16(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // h16 = 1*4HEXDIG
    curi_status status = curi_status_success;
    size_t i;

    if (status == curi_status_success)
        status = parse_hexdigit(handle, uri, len, offset);

    for (i = 0 ; i < 3 ; ++i)
        TRY(status,offset,parse_hexdigit(handle, uri, len, offset));

    return status;
}

static curi_status parse_h16_and_colon(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // h16_and_colon = h16 ":"
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_h16(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_char(handle, ':', uri, len, offset);

    return status;
}

static curi_status parse_ls32(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // ls32 = ( h16 ":" h16 ) / IPv4address
    curi_status status = curi_status_error;

    if (status == curi_status_error)
    {
        size_t initialOffset = *offset;
        curi_status tryStatus = curi_status_success;

        if (tryStatus == curi_status_success)
            tryStatus = parse_h16(handle, uri, len, offset);
        if (tryStatus == curi_status_success)
            tryStatus = parse_char(handle, ':', uri, len, offset);
        if (tryStatus == curi_status_success)
            tryStatus = parse_h16(handle, uri, len, offset);

        if (tryStatus == curi_status_error)
            *offset = initialOffset;
        else
            status = tryStatus;
    }

    if (status == curi_status_error)
        TRY(status,offset,parse_IPv4address(handle, uri, len, offset));

    return status;
}

static curi_status parse_IPv6address(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // IPv6address =                            6( h16 ":" ) ls32
    //             /                       "::" 5( h16 ":" ) ls32
    //             / [               h16 ] "::" 4( h16 ":" ) ls32
    //             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
    //             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
    //             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
    //             / [ *4( h16 ":" ) h16 ] "::"              ls32
    //             / [ *5( h16 ":" ) h16 ] "::"              h16
    //             / [ *6( h16 ":" ) h16 ] "::"

    // Let's simplify it to [ (":" / 1*7(h16_and_colon)) ":"] ( (0*6(h16_and_colon) ls32) / h16 )

    curi_status status = curi_status_success;

    if (status == curi_status_success)
    {
        size_t initialOffset = *offset;
        curi_status tryStatus = curi_status_error;

        if (tryStatus == curi_status_error)
            TRY(tryStatus,offset,parse_char(handle, ':', uri, len, offset));

        if (tryStatus == curi_status_error)
        {
            size_t i;
            tryStatus = curi_status_success;

            if (tryStatus == curi_status_success)
                tryStatus = parse_h16_and_colon(handle, uri, len, offset);

            for (i = 0 ; i < 6 ; ++i)
                TRY(tryStatus,offset,parse_h16_and_colon(handle, uri, len, offset));
        }

        if (tryStatus == curi_status_success)
            tryStatus = parse_char(handle, ':', uri, len, offset);

        if (tryStatus == curi_status_error)
            *offset = initialOffset;
        else
            status = tryStatus;
    }
    
    if (status == curi_status_success)
    {
        status = curi_status_error;

        if (status == curi_status_error)
        {
            size_t initialOffset = *offset;
            curi_status tryStatus = curi_status_success;
            size_t i;
            for (i = 0 ; i < 6 ; ++i)
                TRY(tryStatus,offset,parse_h16_and_colon(handle, uri, len, offset));

            if (tryStatus == curi_status_success)
                tryStatus = parse_ls32(handle, uri, len, offset);

            if (tryStatus == curi_status_error)
                *offset = initialOffset;
            else
                status = tryStatus;
        }

        if (status == curi_status_error)
            TRY(status,offset,parse_h16(handle, uri, len, offset));
    }

    return status;
}

static curi_status parse_IPvFuture(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char(handle, 'v', uri, len, offset);
    if (status == curi_status_success)
        status = parse_hexdigit(handle, uri, len, offset);
    if (status == curi_status_success)
        status = parse_char(handle, '.', uri, len, offset);
    if (status == curi_status_success)
    {
        status = curi_status_error;
        if (status == curi_status_error)
            TRY(status,offset,parse_unreserved(handle, uri, len, offset));
        if (status == curi_status_error)
            TRY(status,offset,parse_sub_delims(handle, uri, len, offset));
        if (status == curi_status_error)
            TRY(status,offset,parse_char(handle, ':', uri, len, offset));

        while(status == curi_status_success)
        {
            status = curi_status_error;
            if (status == curi_status_error)
                TRY(status,offset,parse_unreserved(handle, uri, len, offset));
            if (status == curi_status_error)
                TRY(status,offset,parse_sub_delims(handle, uri, len, offset));
            if (status == curi_status_error)
                TRY(status,offset,parse_char(handle, ':', uri, len, offset));
            if (status == curi_status_error)
            {
                status = curi_status_success;
                break;
            }
        }

    }

    return status;
}

static curi_status parse_IP_literal(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char(handle, '[', uri, len, offset);

    if (status == curi_status_success)
    {
        status = curi_status_error;

        if (status == curi_status_error)
            TRY(status, offset, parse_IPv6address(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_IPvFuture(handle, uri, len, offset));
    }

    if (status == curi_status_success)
        status = parse_char(handle, ']', uri, len, offset);

    return status;
}

static curi_status parse_host(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // host = IP-literal / IPv4address / reg-name
    const size_t initialOffset = *offset;
    curi_status status = curi_status_error;

    if (status == curi_status_error)
        TRY(status, offset, parse_IP_literal(handle, uri, len, offset));
    
    if (status == curi_status_error)
        TRY(status, offset, parse_IPv4address(handle, uri, len, offset));

    if (status == curi_status_error)
        TRY(status, offset, parse_reg_name(handle, uri, len, offset));

    if (status == curi_status_success)
    {
        if (handle->callbacks.host)
        {
            if (handle->callbacks.host(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
            {
                return curi_status_canceled;
            }
        }
    }
    return status;
}

static curi_status parse_port(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // port = *DIGIT
    const size_t initialOffset = *offset;

    while (1)
    {
        size_t subOffset = *offset;
        curi_status subStatus = parse_digit(handle,uri,len,&subOffset);

        if (subStatus == curi_status_success)
            *offset = subOffset;
        else
            break;
    }

    if (handle->callbacks.port)
    {
        if (handle->callbacks.port(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            return curi_status_canceled;
        }
    }
    return curi_status_success;
}

static curi_status parse_authority(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // authority = [ userinfo_and_at ] host [ ":" port ]

    curi_status status = curi_status_success;

    if (status == curi_status_success)
        TRY(status, offset, parse_userinfo_ant_at(handle, uri, len, offset));

    if (status == curi_status_success)
        status = parse_host(handle, uri, len, offset);

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;
        if (subStatus == curi_status_success)
            subStatus = parse_char(handle, ':', uri, len, &subOffset);
        if (subStatus == curi_status_success)
            subStatus = parse_port(handle, uri, len, &subOffset);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    return status;
}

static curi_status parse_pchar(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
    curi_status status = curi_status_error;
    if (status == curi_status_error)
        TRY(status, offset, parse_unreserved(handle, uri, len, offset));

    if (status == curi_status_error)
        TRY(status, offset, parse_pct_encoded(handle, uri, len, offset));

    if (status == curi_status_error)
        TRY(status, offset, parse_sub_delims(handle, uri, len, offset));

    if (status == curi_status_error)
        TRY(status, offset, parse_char(handle, ':', uri, len, offset));

    if (status == curi_status_error)
        TRY(status, offset, parse_char(handle, '@', uri, len, offset));

    return status;
}

static curi_status parse_segment(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // segment = *pchar

    curi_status status = curi_status_success;

    while (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = parse_pchar(handle,uri,len,&subOffset);

        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = curi_status_success;
        }
        else
            break;
    }

    return status;
}

static curi_status parse_segments(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // segments  = *( "/" segment )
    curi_status status = curi_status_success;

    while (1)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;

        if (subStatus == curi_status_success)
            subStatus = parse_char(handle, '/', uri, len, &subOffset);

        if (subStatus == curi_status_success)
            subStatus = parse_segment(handle, uri, len, &subOffset);

        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
        else
        {
            break;
        }
    }

    return status;
}

static curi_status parse_path_abempty(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // path-abempty  = *( "/" segment )
    const size_t initialOffset = *offset;

    curi_status status = parse_segments(handle,uri,len,offset);

    if (handle->callbacks.path)
    {
        if (handle->callbacks.path(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_segment_nz(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // segment = 1*pchar
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_pchar(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_segment(handle, uri, len, offset);

    return status;
}

static curi_status parse_path_absolute(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    const size_t initialOffset = *offset;
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char(handle, '/', uri, len, offset);

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;

        if (subStatus == curi_status_success)
            status = parse_segment_nz(handle, uri, len, &subOffset);

        if (subStatus == curi_status_success)
            status = parse_segments(handle, uri, len, &subOffset);

        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    if (status == curi_status_success && handle->callbacks.path)
    {
        if (handle->callbacks.path(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_path_rootless(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // path-rootless = segment-nz *( "/" segment )

    const size_t initialOffset = *offset;
    curi_status status = curi_status_success;

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;

        if (subStatus == curi_status_success)
            status = parse_segment_nz(handle,uri,len,&subOffset);

        if (subStatus == curi_status_success)
            status = parse_segments(handle,uri,len,&subOffset);

        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    if (status == curi_status_success && handle->callbacks.path)
    {
        if (handle->callbacks.path(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_path_empty(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // path-empty = 0<pchar>
    return curi_status_success;
}

static curi_status parse_hier_part(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // hier-part = "//" authority path-abempty
    //             / path-absolute
    //             / path-rootless
    //             / path-empty
    curi_status status = curi_status_error;

    if (status == curi_status_error)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;
        if (subStatus == curi_status_success)
            subStatus = parse_char(handle, '/', uri, len, &subOffset);
        if (subStatus == curi_status_success)
            subStatus = parse_char(handle, '/', uri, len, &subOffset);
        if (subStatus == curi_status_success)
            subStatus = parse_authority(handle, uri, len, &subOffset);
        if (subStatus == curi_status_success)
            subStatus = parse_path_abempty(handle, uri, len, &subOffset);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }
    
    if (status == curi_status_error)
        TRY(status,offset,parse_path_absolute(handle, uri,len,offset));

    if (status == curi_status_error)
        TRY(status,offset,parse_path_rootless(handle, uri,len,offset));

    if (status == curi_status_error)
        TRY(status,offset,parse_path_empty(handle, uri,len,offset));
    
    return status;
}

static curi_status parse_query_or_fragment(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // query = *( pchar / "/" / "?" )
    // fragment = *( pchar / "/" / "?" )

    while (1)
    {
        curi_status status = curi_status_error;
        if (status == curi_status_error)
            TRY(status, offset, parse_pchar(handle, uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_char(handle, '/', uri, len, offset));

        if (status == curi_status_error)
            TRY(status, offset, parse_char(handle, '?', uri, len, offset));

        if (status == curi_status_error)
        {
            // end of query reached
            return curi_status_success;
        }
    }
}

static curi_status parse_query(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    const size_t initialOffset = *offset;

    curi_status status = parse_query_or_fragment(handle, uri, len, offset);

    if (status == curi_status_success && handle->callbacks.query)
    {
        if (handle->callbacks.query(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_fragment(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    const size_t initialOffset = *offset;

    curi_status status = parse_query_or_fragment(handle, uri, len, offset);

    if (status == curi_status_success && handle->callbacks.fragment)
    {
        if (handle->callbacks.fragment(handle->userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_uri(curi_handle handle, const char* uri, size_t len, size_t* offset)
{
    // URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_scheme(handle, uri, len, offset);

    if (status == curi_status_success)
        status = parse_char(handle, ':', uri, len, offset);

    if (status == curi_status_success)
        status = parse_hier_part(handle, uri, len, offset);

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;
        if (subStatus == curi_status_success)
            subStatus = parse_char(handle,'?',uri,len,&subOffset);
        if (subStatus == curi_status_success)
            subStatus = parse_query(handle,uri,len,&subOffset);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;
        if (subStatus == curi_status_success)
            subStatus = parse_char(handle,'#',uri,len,&subOffset);
        if (subStatus == curi_status_success)
            subStatus = parse_fragment(handle,uri,len,&subOffset);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    return status;
}
curi_status curi_parse(curi_handle handle, const char* uri, size_t len)
{
    size_t offset = 0;
    curi_status status = parse_uri(handle,uri,len,&offset);

    if (status == curi_status_success && offset < len)
        // the URI weren't fully consumed
        // TODO: set an error string somewhere
        status = curi_status_error;

    return status; 
}
