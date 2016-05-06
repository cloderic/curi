/*
 * Copyright (c) 2016 Svyatoslav Mishyn <juef@openmailbox.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <unistd.h>

#include "common.h"

enum
{
    FULLURI,
    PATH,
    QUERY
};

static int scheme(void*, const char*, size_t);
static int userinfo(void*, const char*, size_t);
static int host(void*, const char*, size_t);
static int portStr(void*, const char*, size_t);
static int path(void*, const char*, size_t);
static int path_segment(void*, const char*, size_t);
static int query(void*, const char*, size_t);
static int query_item_null(void*, const char*, size_t);
static int query_item_double(void*, const char*, size_t, double);
static int query_item_int(void*, const char*, size_t, long int);
static int query_item_str(void*, const char*, size_t, const char*, size_t);
static int fragment(void*, const char*, size_t);

static void init_curi_settings(curi_settings*);
static int  split(const char*, int, int);
static int  parse_args(int, char**, int*, int*);

static int scheme(void* data, const char* str, size_t len)
{
    return printf("scheme: %.*s\n", (int)len, str) > 0;
}

static int userinfo(void* data, const char* str, size_t len)
{
    return printf("user: %.*s\n", (int)len, str) > 0;
}

static int host(void* data, const char* str, size_t len)
{
    return printf("host: %.*s\n", (int)len, str) > 0;
}

static int portStr(void* data, const char* str, size_t len)
{
    return printf("port: %.*s\n", (int)len, str) > 0;
}

static int path(void* data, const char* str, size_t len)
{
    return printf("path: %.*s\n", (int)len, str) > 0;
}

static int path_segment(void* data, const char* str, size_t len)
{
    static size_t i;
    return printf("path[%zu]: %.*s\n", i++, (int)len, str) > 0;
}

static int query(void* data, const char* str, size_t len)
{
    return printf("query: %.*s\n", (int)len, str) > 0;
}

static int query_item_null(void* data, const char* str, size_t len)
{
    static size_t i;
    return printf("query[%zu]: %.*s\n", i++, (int)len, str) > 0;
}

static int query_item_double(void* data, const char* str, size_t len, double n)
{
    return printf("query.%.*s: %f\n", (int)len, str, n) > 0;
}

static int query_item_int(void* data, const char* str, size_t len, long int n)
{
    return printf("query.%.*s: %ld\n", (int)len, str, n) > 0;
}

static int query_item_str(void* data, const char* str, size_t len,
        const char* val, size_t vl)
{
    return printf("query.%.*s: %.*s\n", (int)len, str, (int)vl, val) > 0;
}

static int fragment(void* data, const char* str, size_t len)
{
    return printf("fragment: %.*s\n", (int)len, str) > 0;
}

static void init_curi_settings(curi_settings* s)
{
    curi_default_settings(s);
    s->scheme_callback            = scheme;
    s->userinfo_callback          = userinfo;
    s->host_callback              = host;
    s->portStr_callback           = portStr;
    s->path_callback              = path;
    s->path_segment_callback      = path_segment;
    s->query_callback             = query;
    s->query_item_null_callback   = query_item_null;
    s->query_item_double_callback = query_item_double;
    s->query_item_int_callback    = query_item_int;
    s->query_item_str_callback    = query_item_str;
    s->fragment_callback          = fragment;
}

static int split(const char* uri, int decode, int flag)
{
    curi_settings s;

    init_curi_settings(&s);
    s.url_decode = decode;

    if (flag == FULLURI
            && curi_parse_full_uri_nt(uri, &s, NULL) != curi_status_success)
        goto failure;
    else if (flag == PATH
            && curi_parse_path_nt(uri, &s, NULL) != curi_status_success)
        goto failure;
    else if (flag == QUERY
            && curi_parse_query_nt(uri, &s, NULL) != curi_status_success)
        goto failure;

    return EXIT_SUCCESS;

failure:
    fprintf(stderr, "split() failed\n");
    return EXIT_FAILURE;
}

static int parse_args(int argc, char** argv, int* decode, int* flag)
{
    const char* progname;
    int         c;

    *decode  = 0;
    *flag    = FULLURI;
    progname = get_progname(*argv);

    if (argc < 2 || argc > 4)
        goto wrongArgs;

    if (argc == 2 && argv[1][0] != '-')
        return 0;

    while ((c = getopt(argc, argv, "dhpq")) != -1)
    {
        switch (c)
        {
            case 'd':
                *decode = 1;
                break;
            case 'h':
                fprintf(stdout, "usage: %s [-dhpq] uri\n", progname);
                return EXIT_OK;
            case 'p':
                *flag = PATH;
                break;
            case 'q':
                *flag = QUERY;
                break;
            default:
                return EXIT_ERR;
        }
    }

    if (optind >= argc)
    {
        fprintf(stderr, "missing a uri\n");
        return EXIT_ERR;
    }

    return 0;

wrongArgs:
    fprintf(stderr, "wrong # args: must be \"%s [-dhpq] uri\"\n", progname);
    return EXIT_ERR;
}

int main(int argc, char** argv)
{
    int decode, flag;
    int rc;

    if ((rc = parse_args(argc, argv, &decode, &flag)) < 0)
        return rc == EXIT_OK ? EXIT_SUCCESS : EXIT_FAILURE;

    return split(argv[argc - 1], decode, flag);
}
