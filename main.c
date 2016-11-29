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
#include <argp.h>

#include "util.h"
#include "book.h"
#include "gaiji.h"

/*
 * Local types
 */

typedef struct {
    char dict_path[256];
    char font_path[256];
    bool pretty_print;
    bool markup;
} Options;

/*
 * Local functions
 */

static error_t argp_parser(int key, char* arg, struct argp_state* state) {
    Options* options = (Options *)state->input;

    switch (key) {
        case 'd':
            if (arg != NULL) {
                strncpy(options->dict_path, arg, ARRSIZE(options->dict_path));
                options->dict_path[ARRSIZE(options->dict_path) - 1] = 0;
            }
            break;
        case 'f':
            if (arg != NULL) {
                strncpy(options->font_path, arg, ARRSIZE(options->font_path));
                options->font_path[ARRSIZE(options->font_path) - 1] = 0;
            }
            break;
        case 'm':
            options->markup = true;
            break;
        case 'p':
            options->pretty_print = true;
            break;
        case ARGP_KEY_END:
            if (*options->dict_path == 0) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

/*
 * Entry point
 */

int main(int argc, char *argv[]) {
    const struct argp_option argp_options[] = {
        { "dict",         'd', "PATH", 0,                   "dictionary resource to process",          0 },
        { "font",         'f', "PATH", OPTION_ARG_OPTIONAL, "definition file for font translation",    0 },
        { "pretty-print", 'p', NULL,   OPTION_ARG_OPTIONAL, "pretty print JSON output",                0 },
        { "markup",       'm', NULL,   OPTION_ARG_OPTIONAL, "markup JSON output with formatting tags", 0 },
        { }
    };

    const struct argp argp = {
        argp_options,
        argp_parser,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };

    Options options = { };
    argp_parse(&argp, argc, argv, 0, 0, &options);

    Gaiji_Context context;
    if (!gaiji_context_init(&context, *options.font_path == 0 ? NULL : options.font_path)) {
        return 1;
    }

    Book book;
    book_init(&book);

    const bool success =
        book_export(&book, &context, options.dict_path, options.markup) &&
        book_dump(&book, options.pretty_print, stdout);

    book_free(&book);

    gaiji_context_destroy(&context);
    return success ? 0 : 1;
}
