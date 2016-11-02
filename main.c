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

#define _BSD_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "convert.h"
#include "util.h"

#include "eb/eb/eb.h"
#include "eb/eb/error.h"
#include "eb/eb/text.h"

#define MAX_ENTRY_HITS 128
#define MAX_TEXT 1024
#define MAX_HEADING 512

typedef enum {
    READ_MODE_TEXT,
    READ_MODE_HEADING,
} ReadMode;

static void dump_hits(EB_Book* book) {
    EB_Hit hits[MAX_ENTRY_HITS];
    int hit_count = 0;

    do {
        if (eb_hit_list(book, MAX_ENTRY_HITS, hits, &hit_count) != EB_SUCCESS) {
            fprintf(stderr, "error: could not get hit list\n");
            break;
        }

        for (int i = 0; i < hit_count; ++i) {
            char text[MAX_TEXT];
            ssize_t text_length = 0;

            eb_seek_text(book, &hits[i].text);
            eb_read_text(book, NULL, NULL, NULL, MAX_TEXT, text, &text_length);

            const char* text_utf8 = eucjp_to_utf8(text);
            if (text_utf8 != NULL) {
                puts(text_utf8);
            }

            /* (void) text_utf8; */
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

static char* read_book_data(EB_Book* book, const EB_Position* position, ReadMode mode) {
    if (eb_seek_text(book, position) != EB_SUCCESS) {
        return NULL;
    }

    char data[1024];
    ssize_t data_length = 0;

    switch (mode) {
        case READ_MODE_TEXT:
            if (eb_read_text(book, NULL, NULL, NULL, 1023, data, &data_length) != EB_SUCCESS) {
                return NULL;
            }
            break;
        case READ_MODE_HEADING:
            if (eb_read_heading(book, NULL, NULL, NULL, 1023, data, &data_length) != EB_SUCCESS) {
                return NULL;
            }
            break;
        default:
            return NULL;
    }

    return eucjp_to_utf8(data);
}

static void export_subbook(EB_Book* book, Subbook* subbook_data) {
    char title[EB_MAX_TITLE_LENGTH + 1];
    if (eb_subbook_title(book, title) == EB_SUCCESS) {
        subbook_data->title = eucjp_to_utf8(title);
    }

    if (eb_have_copyright(book)) {
        EB_Position position;
        if (eb_copyright(book, &position) == EB_SUCCESS) {
            subbook_data->copyright = read_book_data(book, &position, READ_MODE_TEXT);
        }
    }

    puts(subbook_data->copyright);
}

static void export_book(const char path[], Book* book_data) {
    do {
        EB_Error_Code error;
        if ((error = eb_initialize_library()) != EB_SUCCESS) {
            strcpy(book_data->error, eb_error_message(error));
            break;
        }

        EB_Book book;
        eb_initialize_book(&book);

        if ((error = eb_bind(&book, path)) != EB_SUCCESS) {
            strcpy(book_data->error, eb_error_message(error));
            eb_finalize_book(&book);
            eb_finalize_library();
            break;
        }

        EB_Character_Code character_code;
        if (eb_character_code(&book, &character_code) == EB_SUCCESS) {
            switch (character_code) {
                case EB_CHARCODE_ISO8859_1:
                    strcpy(book_data->character_code, "iso8859-1");
                    break;
                case EB_CHARCODE_JISX0208:
                    strcpy(book_data->character_code, "jisx0208");
                    break;
                case EB_CHARCODE_JISX0208_GB2312:
                    strcpy(book_data->character_code, "jisx0208/gb2312");
                    break;
                default:
                    strcpy(book_data->character_code, "invalid");
                    break;
            }
        }

        EB_Disc_Code disc_code;
        if (eb_disc_type(&book, &disc_code) == EB_SUCCESS) {
            switch (disc_code) {
                case EB_DISC_EB:
                    strcpy(book_data->disc_code, "eb");
                    break;
                case EB_DISC_EPWING:
                    strcpy(book_data->disc_code, "epwing");
                    break;
                default:
                    strcpy(book_data->disc_code, "invalid");
                    break;
            }
        }

        EB_Subbook_Code sub_codes[EB_MAX_SUBBOOKS];
        if ((error = eb_subbook_list(&book, sub_codes, &book_data->subbook_count)) == EB_SUCCESS) {
            if (book_data->subbook_count > 0) {
                book_data->subbooks = calloc(book_data->subbook_count, sizeof(Subbook));
                for (int i = 0; i < book_data->subbook_count; ++i) {
                    Subbook* subbook_data = book_data->subbooks + i;
                    if ((error = eb_set_subbook(&book, sub_codes[i])) == EB_SUCCESS) {
                        export_subbook(&book, subbook_data);
                    }
                    else {
                        strcpy(subbook_data->error, eb_error_message(error));
                    }
                }
            }
        }
        else {
            strcpy(book_data->error, eb_error_message(error));
        }

        eb_finalize_book(&book);
        eb_finalize_library();
    }
    while(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return 2;
    }

    Book book_data = {};
    export_book(argv[1], &book_data);

    return 1;
}
