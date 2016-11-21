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

#ifndef BOOK_H
#define BOOK_H

#include <stdbool.h>
#include <stdio.h>

#include "eb/eb/eb.h"

#include "gaiji.h"

/*
 * Types
 */

typedef struct {
    char* heading;
    char* text;
} Book_Entry;

typedef struct {
    char* title;
    char* copyright;

    Book_Entry* entries;
    int         entry_count;
    int         entry_capacity;
} Book_Subbook;

typedef struct {
    char character_code[32];
    char disc_code[32];

    Book_Subbook* subbooks;
    int           subbook_count;
} Book;

typedef enum {
    BOOK_MODE_TEXT,
    BOOK_MODE_HEADING,
} Book_Mode;

/*
 * Functions
 */

char* book_read(EB_Book* book, EB_Hookset* hookset, const EB_Position* position, Book_Mode mode, Gaiji_Table* table);
void book_free(Book* book);
void book_dump(Book* book, bool pretty_print, FILE* fp);

#endif /* BOOK_H */
