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
#include "eb/eb/eb.h"

/*
   Dictionary structures
*/

typedef struct {
    char* heading;
    char* text;
} TextBlock;

typedef struct {
    char* heading;
    int   heading_page;
    int   heading_offset;

    char* text;
    int   text_page;
    int   text_offset;

    int page;
    int offset;
} Entry;

typedef struct {
    char* title;
    char* copyright;

    Entry* entries;
    int    entry_count;

    char error[256];
} Subbook;

typedef struct {
    char character_code[256];
    char disc_code[256];

    Subbook* subbooks;
    int      subbook_count;

    char error[256];
} Book;

/*
   Array
*/

typedef struct {
    void** ptr;
    size_t used;
    size_t size;
} Array;

void array_init(Array* arr, size_t size);
void array_push(Array* arr, void* data);
void array_free(Array* arr);

#endif
