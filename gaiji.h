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

#ifndef GAIJI_H
#define GAIJI_H

/*
 * Constants
 */

#define MAX_UTF8_BYTES 9
#define MAX_STUB_BYTES 32
#define MAX_CONTEXT_NAME 256

/*
 * Types
 */

typedef struct {
    int  code;
    char utf8[MAX_UTF8_BYTES];
} Gaiji_Entry;

typedef struct {
    char               name[MAX_CONTEXT_NAME];
    const Gaiji_Entry* table_wide;
    int                count_wide;
    const Gaiji_Entry* table_narrow;
    int                count_narrow;
} Gaiji_Context;

typedef enum {
    GAIJI_WIDTH_WIDE,
    GAIJI_WIDTH_NARROW,
} Gaiji_Width;

/*
 * Functions
 */

const Gaiji_Context* gaiji_select_context(const char name[]);
void gaiji_build_stub(char output[], int size, int code, const Gaiji_Context* context, Gaiji_Width width);
void gaiji_fixup_stub(char output[], int size, const char input[]);

#endif /* GAIJI_H */
