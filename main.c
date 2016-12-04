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

#include <string.h>
#include <stdbool.h>

#include "argparse/argparse.h"
#include "util.h"
#include "book.h"
#include "font.h"

/*
 * Local types
 */

typedef struct {
    char dict_path[256];
    char font_path[256];
    int  flags;
} Options;

/*
 * Entry point
 */

int main(int argc, const char *argv[]) {
    char* dict_path = NULL;
    char* font_path = NULL;
    int flags = 0;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('d', "dict", &dict_path, "dictionary resource to process", NULL, 0, 0),
        OPT_STRING('f', "font", &font_path, "font definition file for translation", NULL, 0, 0),
        OPT_BOOLEAN('p', "pretty-print", &flags, "output pretty-printed JSON", NULL, 0, 0),
        OPT_BOOLEAN('m', "markup", &flags, "output formatting tags", 0, 0, 0),
        OPT_BOOLEAN('s', "positions", &flags, "output positional data", NULL, 0, 0),
        OPT_BOOLEAN('t', "font-tags", &flags, "output missing font data tags", NULL, 0, 0),
        OPT_END()
    };

    struct argparse argparse;

    argparse_init(&argparse, options, NULL, 0);
    argparse_parse(&argparse, argc, argv);

    if (dict_path == NULL) {
        argparse_usage(&argparse);
        return 1;
    }

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
