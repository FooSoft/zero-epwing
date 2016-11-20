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

#define MAX_STUB_BYTES 10
#define MAX_TABLE_NAME 256

/*
 * Types
 */

typedef struct {
    int  code;
    char data[MAX_STUB_BYTES];
} Gaiji_entry;

typedef struct {
    char               title[MAX_TABLE_NAME];
    const Gaiji_entry* table_wide;
    int                count_wide;
    const Gaiji_entry* table_narrow;
    int                count_narrow;
} Gaiji_table;

typedef enum {
    GAIJI_WIDTH_WIDE,
    GAIJI_WIDTH_NARROW,
} Gaiji_width;

/*
 * Functions
 */

const Gaiji_table * gaiji_select_table(const char title[]);
void gaiji_build_stub(char text[MAX_STUB_BYTES], int code, const Gaiji_table* table, Gaiji_width width);
void gaiji_fixup_stub(char output[], int size, const char input[]);

#endif /* GAIJI_H */
