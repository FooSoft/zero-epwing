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

#include <stdio.h>
#include <eb/font.h>

/*
 * Types
 */

typedef struct {
    char* text;
    int   page;
    int   offset;
} Book_Block;

typedef struct {
    Book_Block heading;
    Book_Block text;
} Book_Entry;

typedef struct {
    unsigned char bitmap[EB_SIZE_WIDE_FONT_48];
} Book_Glyph;

typedef struct {
    Book_Glyph* glyphs;
    int         glyph_count;
    int         glyph_size;
    int         glyph_wide;
} Book_Font;

typedef struct {
    char*       title;
    Book_Block  copyright;

    Book_Entry* entries;
    int         entry_count;
    int         entry_alloc;

    Book_Font* fonts;
    int        font_count;
} Book_Subbook;

typedef struct {
    char          char_code[32];
    char          disc_code[32];
    Book_Subbook* subbooks;
    int           subbook_count;
} Book;

/*
 * Functions
 */

void book_init(Book* book);
void book_free(Book* book);
int book_import(Book* book, const char path[], int flags);
int book_export(FILE* fp, const Book* book, int flags);

#endif /* BOOK_H */
