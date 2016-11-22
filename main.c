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

#include "book.h"
#include "gaiji.h"

/*
 * Entry point
 */

int main(int argc, char *argv[]) {
    bool pretty_print = false;
    bool markup = false;

    char opt = 0;
    while ((opt = getopt(argc, argv, "pm")) != -1) {
        switch (opt) {
            case 'p':
                pretty_print = true;
                break;
            case 'm':
                markup = true;
                break;
            default:
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-p] dictionary_path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    Gaiji_Context context;
    gaiji_context_init(&context, "gaiji.json");

    Book book = {};
    book_export(&book, &context, argv[optind], markup);
    book_dump(&book, pretty_print, stdout);
    book_free(&book);

    gaiji_context_destroy(&context);
    return 0;
}
