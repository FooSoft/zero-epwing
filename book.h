/*
 * Copyright (c) 2017 Alex Yatskov <alex@foosoft.net>
 * Author: Alex Yatskov <alex@foosoft.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    char bitmap[EB_SIZE_WIDE_FONT_48];
    int  code;
} Book_Glyph;

typedef struct {
    Book_Glyph* glyphs;
    int         width;
    int         height;
    int         count;
} Book_Glyph_Set;

typedef struct {
    Book_Glyph_Set wide;
    Book_Glyph_Set narrow;
} Book_Font;

typedef struct {
    char*       title;
    Book_Block  copyright;

    Book_Entry* entries;
    int         entry_count;
    int         entry_alloc;

    Book_Font fonts[4];
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
