/*
 * Copyright (C) 2016  Alex Yatskov <alex@foosoft.net>
 * Author: Alex Yatskov <alex@foosoft.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
