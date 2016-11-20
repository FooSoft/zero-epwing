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
#include <assert.h>

#include "util.h"
#include "gaiji.h"

/*
 * Macros
 */

#define GAIJI_CONTEXT(name, ents) {\
    name,\
    gaiji_table_##ents##_wide,\
    ARRSIZE(gaiji_table_##ents##_wide),\
    gaiji_table_##ents##_narrow,\
    ARRSIZE(gaiji_table_##ents##_narrow)\
}

/*
 * Local data
 */

#include "gaiji_table_daijisen.h"

static const Gaiji_context gaiji_contexts[] = {
    GAIJI_CONTEXT("大辞泉", daijisen),
};

/*
 * Local functions
 */

static void encode_sequence(char output[], int size, const char utf8[]) {
    const char hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    strncpy(output, "{{", size);
    int offset = strlen(output);

    for (int i = 0; i < MAX_UTF8_BYTES; ++i) {
        const char c = utf8[i];
        if (c == 0) {
            break;
        }

        if (offset >= size - 1) {
            break;
        }

        output[offset++] = hex[c >> 0x08];

        if (offset >= size - 1) {
            break;
        }

        output[offset++] = hex[c & 0x0f];
    }

    output[offset] = 0;
    strncat(output, "}}", size);
}

/*
 * Exported functions
 */

const Gaiji_context * gaiji_select_table(const char name[]) {
    for (unsigned i = 0; i < ARRSIZE(gaiji_contexts); ++i) {
        const Gaiji_context* context = gaiji_contexts + i;
        if (strcmp(context->name, name) == 0) {
            return context;
        }
    }

    return NULL;
}

void gaiji_build_stub(char output[], int size, int code, const Gaiji_context* context, Gaiji_width width) {
    do {
        if (context == NULL) {
            break;
        }

        const Gaiji_entry* entries = NULL;
        int count = 0;

        switch (width) {
            case GAIJI_WIDTH_WIDE:
                entries = context->table_wide;
                count = context->count_wide;
                break;
            case GAIJI_WIDTH_NARROW:
                entries = context->table_narrow;
                count = context->count_narrow;
                break;
        }

        assert(entries != NULL);

        for (int i = 0; i < count; ++i) {
            const Gaiji_entry* entry = entries + i;
            if (entry->code == code) {
                encode_sequence(output, size, entry->utf8);
                return;
            }
        }

    }
    while (0);

    strncpy(output, "<?>", size);
    output[size - 1] = 0;
}

void gaiji_fixup_stub(char output[], int size, const char input[]) {
    (void)output;
    (void)size;
    (void)input;
}
