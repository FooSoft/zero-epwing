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

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "book.h"
#include "font.h"

/*
 * Entry point
 */

int main(int argc, char *argv[]) {
    char* dict_path = NULL;
    char* font_path = NULL;
    int flags = 0;

    char c = 0;
    while ((c = getopt(argc, argv, "f:d:pmst")) != -1) {
        switch (c) {
            case 'f':
                font_path = optarg;
                break;
            case 'p':
                flags |= FLAG_PRETTY_PRINT;
                break;
            case 'm':
                flags |= FLAG_HOOK_MARKUP;
                break;
            case 's':
                flags |= FLAG_POSITIONS;
                break;
            case 't':
                flags |= FLAG_FONT_TAGS;
                break;
            default:
                abort();
                break;
        }
    }

    if (optind == argc) {
        fprintf(stderr, "dictionary must be provided provided\n");
        abort();
    }

    dict_path = argv[optind];

    Font_Context context;
    if (!font_context_init(&context, font_path == 0 ? NULL : font_path)) {
        return 1;
    }

    Book book;
    book_init(&book);

    const bool success =
        book_import(&book, &context, dict_path, flags) &&
        book_export(stdout, &book, flags);

    book_free(&book);

    font_context_destroy(&context);
    return success ? 0 : 1;
}
