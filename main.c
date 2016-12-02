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
 * Local functions
 */

static error_t argp_parser(int key, char* arg, struct argp_state* state) {
    Options* options = (Options*)state->input;

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
        case 'p':
            options->flags |= FLAG_PRETTY_PRINT;
            break;
        case 'm':
            options->flags |= FLAG_HOOK_MARKUP;
            break;
        case 't':
            options->flags |= FLAG_FONT_TAGS;
            break;
        case 's':
            options->flags |= FLAG_POSITIONS;
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
        { "dict",         'd', "PATH", 0,                   "dictionary resource to process",       0 },
        { "font",         'f', "PATH", OPTION_ARG_OPTIONAL, "definition file for font translation", 0 },
        { "pretty-print", 'p', NULL,   OPTION_ARG_OPTIONAL, "output pretty-printed JSON",           0 },
        { "markup",       'm', NULL,   OPTION_ARG_OPTIONAL, "output formatting tags",               0 },
        { "positions",    's', NULL,   OPTION_ARG_OPTIONAL, "output positional data",               0 },
        { "font-tags",    't', NULL,   OPTION_ARG_OPTIONAL, "output missing font data tags",        0 },
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

    Options options = {};
    argp_parse(&argp, argc, argv, 0, 0, &options);

    Font_Context context;
    if (!font_context_init(&context, *options.font_path == 0 ? NULL : options.font_path)) {
        return 1;
    }

    Book book;
    book_init(&book);

    const bool success =
        book_import(&book, &context, options.dict_path, options.flags) &&
        book_export(stdout, &book, options.flags);

    book_free(&book);

    font_context_destroy(&context);
    return success ? 0 : 1;
}
