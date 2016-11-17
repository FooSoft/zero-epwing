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

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "eb/eb/eb.h"

/*
   Dictionary structures
*/

#define MAX_ERROR 256

typedef struct {
    char* heading;
    char* text;
} TextBlock;

typedef struct {
    char* heading;
    char* text;
} Entry;

typedef struct {
    char* title;
    char* copyright;

    Entry* entries;
    int    entry_count;
    int    entry_cap;
} Subbook;

typedef struct {
    char character_code[32];
    char disc_code[32];

    Subbook* subbooks;
    int      subbook_count;
} Book;

/*
   Book helpers
*/

typedef enum {
    READ_MODE_TEXT,
    READ_MODE_HEADING,
} ReadMode;

char* read_book_data(EB_Book* book, const EB_Position* position, ReadMode mode);

void free_book(Book* book);
void dump_book(Book* book, bool pretty_print, FILE* fp);


#endif
