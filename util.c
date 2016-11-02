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

#include "util.h"
#include "convert.h"

#include "eb/eb/eb.h"
#include "eb/eb/error.h"
#include "eb/eb/text.h"

#define MAX_TEXT 1024

char* read_book_data(EB_Book* book, const EB_Position* position, ReadMode mode) {
    if (eb_seek_text(book, position) != EB_SUCCESS) {
        return NULL;
    }

    char data[MAX_TEXT];
    ssize_t data_length = 0;
    EB_Error_Code error;

    switch (mode) {
        case READ_MODE_TEXT:
            error = eb_read_text(
                book,
                NULL,
                NULL,
                NULL,
                MAX_TEXT - 1,
                data,
                &data_length
            );
            break;
        case READ_MODE_HEADING:
            error = eb_read_heading(
                book,
                NULL,
                NULL,
                NULL,
                MAX_TEXT - 1,
                data,
                &data_length
            );
            break;
        default:
            return NULL;
    }

    return error == EB_SUCCESS ? eucjp_to_utf8(data) : NULL;
}

void free_book(Book* book) {
    for (int i = 0; i < book->subbook_count; ++i) {
        Subbook* subbook = book->subbooks + i;
        free(subbook->title);
        free(subbook->copyright);

        for (int j = 0; j < subbook->entry_count; ++j) {
            Entry* entry = subbook->entries + j;
            free(entry->heading);
            free(entry->text);
        }

        free(subbook->entries);
    }
}
