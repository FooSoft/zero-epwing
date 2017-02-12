/*
 * Copyright (c) 2017 Alex Yatskov <alex@foosoft.net>
 * Author: Alex Yatskov <alex@foosoft.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <getopt.h>

#include "util.h"
#include "book.h"

/*
 * Entry point
 */

int main(int argc, char *argv[]) {
    const struct option options[] = {
        { "pretty",    no_argument, NULL, 'p' },
        { "markup",    no_argument, NULL, 'm' },
        { "positions", no_argument, NULL, 's' },
        { "fonts",     no_argument, NULL, 'f' },
        { NULL,        0,           NULL,  0  },
    };

    char* dict_path = NULL;
    int flags = 0;

    int c = 0;
    while ((c = getopt_long(argc, argv, "f:d:pmst", options, NULL)) != -1) {
        switch (c) {
            case 'p':
                flags |= FLAG_PRETTY_PRINT;
                break;
            case 'm':
                flags |= FLAG_HOOK_MARKUP;
                break;
            case 's':
                flags |= FLAG_POSITIONS;
                break;
            case 'f':
                flags |= FLAG_FONTS;
                break;
            default:
                return 1;
        }
    }

    if (optind == argc) {
        fprintf(stderr, "error: dictionary path was not provided\n");
        return 1;
    }

    dict_path = argv[optind];

    Book book;
    book_init(&book);

    const int success =
        book_import(&book, dict_path, flags) &&
        book_export(stdout, &book, flags);

    book_free(&book);

    return success ? 0 : 1;
}
