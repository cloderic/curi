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

#include "common.h"

static int decode(const char*);
static int parse_args(int, char**);

static int decode(const char* url)
{
    char   buf[2048];
    size_t len;

    if (curi_url_decode_nt(url, buf, sizeof(buf), &len) != curi_status_success)
    {
        fprintf(stderr, "curi_url_decode_nt() failed\n");
        return EXIT_FAILURE;
    }

    buf[len] = '\0';
    puts(buf);
    return EXIT_SUCCESS;
}

static int parse_args(int argc, char** argv)
{
    const char* progname = get_progname(*argv);

    if (argc != 2)
        goto wrongArgs;

    if (argv[1][0] != '-')
        return 0;

    if (argv[1][1] == '\0' || argv[1][2] != '\0')
        goto wrongArgs;

    switch (argv[1][1])
    {
        case 'h':
            fprintf(stdout, "usage: %s [-h] url\n", progname);
            return EXIT_OK;
        default:
            fprintf(stderr, "unknown option: '%c'\n", argv[1][1]);
            return EXIT_ERR;
    }

wrongArgs:
    fprintf(stderr, "wrong # args: must be \"%s [-h] url\"\n", progname);
    return EXIT_ERR;
}

int main(int argc, char** argv)
{
    int rc;

    if ((rc = parse_args(argc, argv)) < 0)
        return rc == EXIT_OK ? EXIT_SUCCESS : EXIT_FAILURE;

    return decode(argv[argc - 1]);
}
