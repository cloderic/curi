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
#include <stdint.h>
#include <string.h>

void curi_default_settings(curi_settings* settings)
{
    memset(settings,0,sizeof(curi_settings));
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
    size_t __TRY_initialOffset = *(offset); \
    curi_status __TRY_tryStatus = parse_fun_call; \
    if (__TRY_tryStatus == curi_status_error) \
        *(offset) = __TRY_initialOffset; \
    else \
        status = __TRY_tryStatus; \
} \

static curi_status parse_alpha(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    if (isalpha(*read_char(uri,len,offset)))
        return curi_status_success;
    else
        return curi_status_error;
}

#define CASE_DIGIT \
    case '0': \
    case '1': \
    case '2': \
    case '3': \
    case '4': \
    case '5': \
    case '6': \
    case '7': \
    case '8': \
    case '9'

static curi_status parse_digit(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    switch (*read_char(uri,len,offset))
    {
        CASE_DIGIT:
            return curi_status_success;
        default:
            return curi_status_error;
    }  
}

static curi_status parse_char(char c, const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userDatat)
{
    if (*read_char(uri,len,offset) == c)
        return curi_status_success;
    else
        return curi_status_error;
}

static curi_status parse_scheme(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / ".")
    const size_t initialOffset = *offset;
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_alpha(uri, len, offset, settings, userData);

    if (status == curi_status_success)
    {
        while (1)
        {
            status = curi_status_error;
            if (status == curi_status_error)
                TRY(status, offset, parse_alpha(uri, len, offset, settings, userData));

            if (status == curi_status_error)
                TRY(status, offset, parse_digit(uri, len, offset, settings, userData));

            if (status == curi_status_error)
                TRY(status, offset, parse_char('+', uri, len, offset, settings, userData));

            if (status == curi_status_error)
                TRY(status, offset, parse_char('-', uri, len, offset, settings, userData));

            if (status == curi_status_error)
                TRY(status, offset, parse_char('.', uri, len, offset, settings, userData));

            if (status == curi_status_error)
            {
                // end of scheme reached
                status = curi_status_success;
                if (settings->scheme_callback)
                {
                    if (settings->scheme_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
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

static curi_status parse_unreserved(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
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

#define CASE_HEXDIGIT \
    CASE_DIGIT: \
    case 'a': \
    case 'b': \
    case 'c': \
    case 'd': \
    case 'e': \
    case 'f': \
    case 'A': \
    case 'B': \
    case 'C': \
    case 'D': \
    case 'E': \
    case 'F'

static curi_status parse_hexdigit(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    switch (*read_char(uri,len,offset))
    {
        CASE_HEXDIGIT:
            return curi_status_success;
        default:
            return curi_status_error;
    }
}

static curi_status parse_pct_encoded(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // pct-encoded = "%" HEXDIG HEXDIG
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char('%', uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_hexdigit(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_hexdigit(uri, len, offset, settings, userData);

    return status;
}

static curi_status parse_sub_delims(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
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

static curi_status parse_userinfo_ant_at(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
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
            TRY(status, offset, parse_unreserved(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_pct_encoded(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_sub_delims(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_char(':', uri, len, offset, settings, userData));

        if (status == curi_status_error)
        {
            // end of uri reached
            break;
        }
    }

    afterUserinfoOffset = *offset;
    
    status = parse_char('@', uri, len, offset, settings, userData);

    if (status == curi_status_success && settings->userinfo_callback)
    {
        if (settings->userinfo_callback(userData, uri + initialOffset, afterUserinfoOffset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_reg_name(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // reg-name = *( unreserved / pct-encoded / sub-delims )
    while (1)
    {
        curi_status status = curi_status_error;
        if (status == curi_status_error)
            TRY(status, offset, parse_unreserved(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_pct_encoded(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_sub_delims(uri, len, offset, settings, userData));

        if (status == curi_status_error)
        {
            // end of host reached
            return curi_status_success;
        }
    }
}

static curi_status parse_dec_octet(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
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
        if (parse_digit(uri, len, offset, settings, userData) !=  curi_status_success)
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

static curi_status parse_IPv4address(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_dec_octet(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_char('.', uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_dec_octet(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_char('.', uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_dec_octet(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_char('.', uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_dec_octet(uri, len, offset, settings, userData);

    return status;
}

static curi_status parse_h16(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // h16 = 1*4HEXDIG
    curi_status status = curi_status_success;
    size_t i;

    if (status == curi_status_success)
        status = parse_hexdigit(uri, len, offset, settings, userData);

    for (i = 0 ; i < 3 ; ++i)
        TRY(status,offset,parse_hexdigit(uri, len, offset, settings, userData));

    return status;
}

static curi_status parse_h16_and_colon(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // h16_and_colon = h16 ":"
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_h16(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_char(':', uri, len, offset, settings, userData);

    return status;
}

static curi_status parse_ls32(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // ls32 = ( h16 ":" h16 ) / IPv4address
    curi_status status = curi_status_error;

    if (status == curi_status_error)
    {
        size_t initialOffset = *offset;
        curi_status tryStatus = curi_status_success;

        if (tryStatus == curi_status_success)
            tryStatus = parse_h16(uri, len, offset, settings, userData);
        if (tryStatus == curi_status_success)
            tryStatus = parse_char(':', uri, len, offset, settings, userData);
        if (tryStatus == curi_status_success)
            tryStatus = parse_h16(uri, len, offset, settings, userData);

        if (tryStatus == curi_status_error)
            *offset = initialOffset;
        else
            status = tryStatus;
    }

    if (status == curi_status_error)
        TRY(status,offset,parse_IPv4address(uri, len, offset, settings, userData));

    return status;
}

static curi_status parse_IPv6address(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
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
            TRY(tryStatus,offset,parse_char(':', uri, len, offset, settings, userData));

        if (tryStatus == curi_status_error)
        {
            size_t i;
            tryStatus = curi_status_success;

            if (tryStatus == curi_status_success)
                tryStatus = parse_h16_and_colon(uri, len, offset, settings, userData);

            for (i = 0 ; i < 6 ; ++i)
                TRY(tryStatus,offset,parse_h16_and_colon(uri, len, offset, settings, userData));
        }

        if (tryStatus == curi_status_success)
            tryStatus = parse_char(':', uri, len, offset, settings, userData);

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
                TRY(tryStatus,offset,parse_h16_and_colon(uri, len, offset, settings, userData));

            if (tryStatus == curi_status_success)
                tryStatus = parse_ls32(uri, len, offset, settings, userData);

            if (tryStatus == curi_status_error)
                *offset = initialOffset;
            else
                status = tryStatus;
        }

        if (status == curi_status_error)
            TRY(status,offset,parse_h16(uri, len, offset, settings, userData));
    }

    return status;
}

static curi_status parse_IPvFuture(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char('v', uri, len, offset, settings, userData);
    if (status == curi_status_success)
        status = parse_hexdigit(uri, len, offset, settings, userData);
    if (status == curi_status_success)
        status = parse_char('.', uri, len, offset, settings, userData);
    if (status == curi_status_success)
    {
        status = curi_status_error;
        if (status == curi_status_error)
            TRY(status,offset,parse_unreserved(uri, len, offset, settings, userData));
        if (status == curi_status_error)
            TRY(status,offset,parse_sub_delims(uri, len, offset, settings, userData));
        if (status == curi_status_error)
            TRY(status,offset,parse_char(':', uri, len, offset, settings, userData));

        while(status == curi_status_success)
        {
            status = curi_status_error;
            if (status == curi_status_error)
                TRY(status,offset,parse_unreserved(uri, len, offset, settings, userData));
            if (status == curi_status_error)
                TRY(status,offset,parse_sub_delims(uri, len, offset, settings, userData));
            if (status == curi_status_error)
                TRY(status,offset,parse_char(':', uri, len, offset, settings, userData));
            if (status == curi_status_error)
            {
                status = curi_status_success;
                break;
            }
        }

    }

    return status;
}

static curi_status parse_IP_literal(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char('[', uri, len, offset, settings, userData);

    if (status == curi_status_success)
    {
        status = curi_status_error;

        if (status == curi_status_error)
            TRY(status, offset, parse_IPv6address(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_IPvFuture(uri, len, offset, settings, userData));
    }

    if (status == curi_status_success)
        status = parse_char(']', uri, len, offset, settings, userData);

    return status;
}

static curi_status parse_host(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // host = IP-literal / IPv4address / reg-name
    const size_t initialOffset = *offset;
    curi_status status = curi_status_error;

    if (status == curi_status_error)
        TRY(status, offset, parse_IP_literal(uri, len, offset, settings, userData));
    
    if (status == curi_status_error)
        TRY(status, offset, parse_IPv4address(uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status, offset, parse_reg_name(uri, len, offset, settings, userData));

    if (status == curi_status_success)
    {
        if (settings->host_callback)
        {
            if (settings->host_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
            {
                return curi_status_canceled;
            }
        }
    }
    return status;
}

static curi_status parse_port(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // port = *DIGIT
    const size_t initialOffset = *offset;

    while (1)
    {
        size_t subOffset = *offset;
        curi_status subStatus = parse_digit(uri, len, &subOffset, settings, userData);

        if (subStatus == curi_status_success)
            *offset = subOffset;
        else
            break;
    }

    if (settings->port_callback)
    {
        if (settings->port_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            return curi_status_canceled;
        }
    }
    return curi_status_success;
}

static curi_status parse_authority(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // authority = [ userinfo_and_at ] host [ ":" port ]

    curi_status status = curi_status_success;

    if (status == curi_status_success)
        TRY(status, offset, parse_userinfo_ant_at(uri, len, offset, settings, userData));

    if (status == curi_status_success)
        status = parse_host(uri, len, offset, settings, userData);

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;
        if (subStatus == curi_status_success)
            subStatus = parse_char(':', uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
            subStatus = parse_port(uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    return status;
}

static curi_status parse_pchar(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
    curi_status status = curi_status_error;
    if (status == curi_status_error)
        TRY(status, offset, parse_unreserved(uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status, offset, parse_pct_encoded(uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status, offset, parse_sub_delims(uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status, offset, parse_char(':', uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status, offset, parse_char('@', uri, len, offset, settings, userData));

    return status;
}

static curi_status parse_segment(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // segment = *pchar

    curi_status status = curi_status_success;

    while (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = parse_pchar(uri, len, &subOffset, settings, userData);

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

static curi_status parse_segments(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // segments  = *( "/" segment )
    curi_status status = curi_status_success;

    while (1)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;

        if (subStatus == curi_status_success)
            subStatus = parse_char('/', uri, len, &subOffset, settings, userData);

        if (subStatus == curi_status_success)
            subStatus = parse_segment(uri, len, &subOffset, settings, userData);

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

static curi_status parse_path_abempty(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // path-abempty  = *( "/" segment )
    const size_t initialOffset = *offset;

    curi_status status = parse_segments(uri, len, offset, settings, userData);

    if (settings->path_callback)
    {
        if (settings->path_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_segment_nz(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // segment = 1*pchar
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_pchar(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_segment(uri, len, offset, settings, userData);

    return status;
}

static curi_status parse_path_absolute(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    const size_t initialOffset = *offset;
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_char('/', uri, len, offset, settings, userData);

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;

        if (subStatus == curi_status_success)
            status = parse_segment_nz(uri, len, &subOffset, settings, userData);

        if (subStatus == curi_status_success)
            status = parse_segments(uri, len, &subOffset, settings, userData);

        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    if (status == curi_status_success && settings->path_callback)
    {
        if (settings->path_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_path_rootless(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // path-rootless = segment-nz *( "/" segment )

    const size_t initialOffset = *offset;
    curi_status status = curi_status_success;

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;

        if (subStatus == curi_status_success)
            status = parse_segment_nz(uri, len, &subOffset, settings, userData);

        if (subStatus == curi_status_success)
            status = parse_segments(uri, len, &subOffset, settings, userData);

        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    if (status == curi_status_success && settings->path_callback)
    {
        if (settings->path_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_path_empty(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // path-empty = 0<pchar>
    return curi_status_success;
}

static curi_status parse_hier_part(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
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
            subStatus = parse_char('/', uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
            subStatus = parse_char('/', uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
            subStatus = parse_authority(uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
            subStatus = parse_path_abempty(uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }
    
    if (status == curi_status_error)
        TRY(status,offset,parse_path_absolute(uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status,offset,parse_path_rootless(uri, len, offset, settings, userData));

    if (status == curi_status_error)
        TRY(status,offset,parse_path_empty(uri, len, offset, settings, userData));
    
    return status;
}

static curi_status parse_query_or_fragment(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // query = *( pchar / "/" / "?" )
    // fragment = *( pchar / "/" / "?" )

    while (1)
    {
        curi_status status = curi_status_error;
        if (status == curi_status_error)
            TRY(status, offset, parse_pchar(uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_char('/', uri, len, offset, settings, userData));

        if (status == curi_status_error)
            TRY(status, offset, parse_char('?', uri, len, offset, settings, userData));

        if (status == curi_status_error)
        {
            // end of query reached
            return curi_status_success;
        }
    }
}

static curi_status parse_query(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    const size_t initialOffset = *offset;

    curi_status status = parse_query_or_fragment(uri, len, offset, settings, userData);

    if (status == curi_status_success && settings->query_callback)
    {
        if (settings->query_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_fragment(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    const size_t initialOffset = *offset;

    curi_status status = parse_query_or_fragment(uri, len, offset, settings, userData);

    if (status == curi_status_success && settings->fragment_callback)
    {
        if (settings->fragment_callback(userData, uri + initialOffset, *offset - initialOffset) == 0)
        {
            status = curi_status_canceled;
        }
    }

    return status;
}

static curi_status parse_full_uri(const char* uri, size_t len, size_t* offset, const curi_settings* settings, void* userData)
{
    // URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    curi_status status = curi_status_success;

    if (status == curi_status_success)
        status = parse_scheme(uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_char(':', uri, len, offset, settings, userData);

    if (status == curi_status_success)
        status = parse_hier_part(uri, len, offset, settings, userData);

    if (status == curi_status_success)
    {
        size_t subOffset = *offset;
        curi_status subStatus = curi_status_success;
        if (subStatus == curi_status_success)
            subStatus = parse_char('?', uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
            subStatus = parse_query(uri, len, &subOffset, settings, userData);
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
            subStatus = parse_char('#', uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
            subStatus = parse_fragment(uri, len, &subOffset, settings, userData);
        if (subStatus == curi_status_success)
        {
            *offset = subOffset;
            status = subStatus;
        }
    }

    return status;
}
curi_status curi_parse_full_uri(const char* uri, size_t len, const curi_settings* settings /*= 0*/, void* userData /*= 0*/)
{
    size_t offset = 0;
    curi_status status;
    
    if (settings)
    {
        // parsing with the given settings
        status = parse_full_uri(uri, len, &offset, settings, userData); 
    }
    else
    {
        curi_settings defaultSettings;
        curi_default_settings(&defaultSettings);
        // parsing with default settings
        status = parse_full_uri(uri, len, &offset, &defaultSettings, userData); 
    }

    if (status == curi_status_success && *read_char(uri,len,&offset) != '\0')
        // the URI weren't fully consumed
        // TODO: set an error string somewhere
        status = curi_status_error;

    return status; 
}

curi_status curi_parse_full_uri_nt(const char* uri, const curi_settings* settings /*= 0*/, void* userData /*= 0*/)
{
    return curi_parse_full_uri(uri, SIZE_MAX, settings, userData);
}

curi_status curi_url_decode(const char* input, size_t inputLen, char* output, size_t outputCapacity, size_t* outputLen /*=0*/)
{
    curi_status status = curi_status_error;
    size_t inputOffset = 0;
    size_t outputOffset = 0;

    #define HEXTOI(x) (isdigit(x) ? x - '0' : tolower(x) - 'a' + 10)

    while ( outputOffset < outputCapacity ) 
    {
        status = curi_status_error;
        TRY(status, &inputOffset, parse_pct_encoded(input,inputLen,&inputOffset,0,0));

        if (status == curi_status_success)
        {
            output[outputOffset] = ((HEXTOI(input[inputOffset - 2]) << 4) | HEXTOI(tolower(input[inputOffset - 1])));
            ++outputOffset;
        }
        else
        {
            output[outputOffset] = *read_char(input, inputLen, &inputOffset);
            if (output[outputOffset] == '\0')
            {
                if (outputLen)
                    *outputLen = outputOffset;

                return curi_status_success;
            }
            else
            {
                ++outputOffset;
            }
        }
    }

    if (*read_char(input, inputLen, &inputOffset) == '\0')
    {
        if (outputLen)
            *outputLen = outputOffset;

        return curi_status_success;
    }
    else
    {
        return curi_status_error;
    }
}
