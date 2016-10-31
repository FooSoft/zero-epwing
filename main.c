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

#include <stdio.h>
#include <iconv.h>
#include <string.h>
#include <stdlib.h>

#include "eb/eb/eb.h"
#include "eb/eb/error.h"
#include "eb/eb/text.h"

/*
   Constants
*/

#define MAX_ENTRY_HITS 128
#define MAX_ENTRY_TEXT 1024
#define MAX_ENTRY_HEADER 512

/*
    Array functions
*/

typedef struct {
    char header[MAX_ENTRY_HEADER];
    char text[MAX_ENTRY_TEXT];
} Entry;

typedef struct {
    Entry* ptr;
    size_t used;
    size_t size;
} Array;

void array_init(Array* arr, size_t init_size) {
    arr->ptr = (Entry *)malloc(init_size * sizeof(Entry));
    arr->used = 0;
    arr->size = init_size;
}

Entry* array_new(Array* arr) {
    if (arr->used == arr->size) {
        arr->size *= 2;
        arr->ptr = (Entry *)realloc(arr->ptr, arr->size * sizeof(Entry));
    }

    return &arr->ptr[arr->used++];
}

void array_free(Array* arr) {
    free(arr->ptr);
    arr->ptr = NULL;
    arr->used = arr->size = 0;
}

/*
    Local functions
*/

static const char * eucjp_to_utf8(const char src[]) {
    char src_buff[MAX_ENTRY_TEXT];
    size_t src_size = strlen(src);
    char * src_ptr = src_buff;
    memcpy(src_buff, src, src_size + 1);

    static char dst_buff[MAX_ENTRY_TEXT];
    size_t dst_size = MAX_ENTRY_TEXT;
    char * dst_ptr = dst_buff;
    *dst_buff = 0;

    iconv_t conv = iconv_open("UTF-8", "EUC-JP");
    if (conv != (iconv_t)-1) {
        if (iconv(conv, &src_ptr, &src_size, &dst_ptr, &dst_size) == (size_t)-1) {
            printf("error");
        }
    }
    else {
        printf("poop");
    }

    iconv_close(conv);
    return dst_buff;
}

static void dump_hits(EB_Book* book) {
    EB_Hit hits[MAX_ENTRY_HITS];
    int hit_count = 0;

    do {
        if (eb_hit_list(book, MAX_ENTRY_HITS, hits, &hit_count) != EB_SUCCESS) {
            fprintf(stderr, "error: could not get hit list\n");
            break;
        }

        for (int i = 0; i < hit_count; ++i) {
            char text[MAX_ENTRY_TEXT];
            ssize_t text_length = 0;

            eb_seek_text(book, &hits[i].text);
            eb_read_text(book, NULL, NULL, NULL, MAX_ENTRY_TEXT, text, &text_length);

            const char* text_utf8 = eucjp_to_utf8(text);
            (void) text_utf8;
        }
    }
    while (hit_count > 0);
}

static void dump_book(EB_Book* book) {
    if (eb_search_all_alphabet(book) == EB_SUCCESS) {
        dump_hits(book);
    }
    else {
        printf("notice: skiping alphabet search\n");
    }

    if (eb_search_all_kana(book) == EB_SUCCESS) {
        dump_hits(book);
    }
    else {
        printf("notice: skiping kana search\n");
    }

    if (eb_search_all_asis(book) == EB_SUCCESS) {
        dump_hits(book);
    }
    else {
        printf("notice: skiping asis search\n");
    }
}

static int process(const char path[]) {
    if (eb_initialize_library() != EB_SUCCESS) {
        fprintf(stderr, "error: failed to initialize library\n");
        return 1;
    }

    EB_Book book;
    eb_initialize_book(&book);

    do {
        if (eb_bind(&book, path) != EB_SUCCESS) {
            fprintf(stderr, "error: failed to bind book\n");
            break;
        }

        EB_Subbook_Code sub_codes[EB_MAX_SUBBOOKS];
        int sub_count = 0;

        if (eb_subbook_list(&book, sub_codes, &sub_count) != EB_SUCCESS) {
            fprintf(stderr, "error: failed to get sub-book list\n");
            break;
        }

        for (int i = 0; i < sub_count; ++i) {
            if (eb_set_subbook(&book, sub_codes[i]) == EB_SUCCESS) {
                dump_book(&book);
            }
            else {
                fprintf(stderr, "error: failed to set sub-book\n");
            }
        }
    }
    while (0);

    eb_finalize_book(&book);
    eb_finalize_library();
    return 0;
}

/*
    Entry point
*/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return 2;
    }

    return process(argv[1]);
}
