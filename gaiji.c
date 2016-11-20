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

#include "util.h"
#include "gaiji.h"

/*
 * Macros
 */

#define GAIJI_TABLE(name, ents) {\
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

static const Gaiji_table gaiji_tables[] = {
    GAIJI_TABLE("大辞泉", daijisen),
};

/*
 * Exported functions
 */

const Gaiji_table * gaiji_select_table(const char name[]) {
    for (unsigned i = 0; i < ARRSIZE(gaiji_tables); ++i) {
        const Gaiji_table* table = gaiji_tables + i;
        if (strcmp(table->name, name) == 0) {
            return table;
        }
    }

    return NULL;
}

void gaiji_build_stub(char text[MAX_STUB_BYTES], int code, const Gaiji_table* table, Gaiji_width width) {
    if (table == NULL) {
        strcpy(text, "<?>");
        return;
    }

    (void)code;
    (void)text;
    (void)table;
    (void)width;
}

void gaiji_fixup_stub(char output[], int size, const char input[]) {
    (void)output;
    (void)size;
    (void)input;
}
