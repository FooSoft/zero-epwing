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

#ifndef FONT_H
#define FONT_H

#include <stdbool.h>

/*
 * Constants
 */

#define MAX_UTF8_BYTES 10
#define MAX_STUB_BYTES 32

/*
 * Types
 */

typedef struct {
    int  code;
    char utf8[MAX_UTF8_BYTES];
} Font_Entry;

typedef struct {
    char               name[256];
    const Font_Entry* table_wide;
    int                table_wide_size;
    const Font_Entry* table_narrow;
    int                table_narrow_size;
} Font_Table;

typedef struct {
    Font_Table* tables;
    int          table_count;
} Font_Context;

typedef enum {
    FONT_WIDTH_WIDE,
    FONT_WIDTH_NARROW,
} Font_Width;

/*
 * Functions
 */

bool font_context_init(Font_Context* context, const char path[]);
void font_context_destroy(Font_Context* context);

const Font_Table* font_table_select(const Font_Context* context, const char name[]);

void font_stub_encode(char output[], int size, int code, const Font_Table* table, Font_Width width);
void font_stub_decode(char output[], int size, const char input[]);

#endif /* FONT_H */
