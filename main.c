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
#include <string.h>

#include "convert.h"
#include "util.h"

#include "eb/eb/eb.h"
#include "eb/eb/error.h"

#define MAX_HITS 256

static void export_subbook_entries(EB_Book* eb_book, Subbook* subbook) {
    if (subbook->entry_cap == 0) {
        subbook->entry_cap = 16384;
        subbook->entries = malloc(subbook->entry_cap * sizeof(Entry));
    }

    EB_Hit hits[MAX_HITS];
    int hit_count = 0;

    do {
        if (eb_hit_list(eb_book, MAX_HITS, hits, &hit_count) != EB_SUCCESS) {
            continue;
        }

        for (int i = 0; i < hit_count; ++i) {
            EB_Hit* hit = hits + i;

            if (subbook->entry_count == subbook->entry_cap) {
                subbook->entry_cap *= 2;
                subbook->entries = realloc(subbook->entries, subbook->entry_cap * sizeof(Entry));
            }

            Entry* entry = subbook->entries + subbook->entry_count++;
            entry->heading = read_book_data(eb_book, &hit->heading, READ_MODE_HEADING);
            entry->text = read_book_data(eb_book, &hit->text, READ_MODE_TEXT);
        }
    }
    while (hit_count > 0);
}

static void export_subbook(EB_Book* eb_book, Subbook* subbook) {
    char title[EB_MAX_TITLE_LENGTH + 1];
    if (eb_subbook_title(eb_book, title) == EB_SUCCESS) {
        subbook->title = eucjp_to_utf8(title);
    }

    if (eb_have_copyright(eb_book)) {
        EB_Position position;
        if (eb_copyright(eb_book, &position) == EB_SUCCESS) {
            subbook->copyright = read_book_data(eb_book, &position, READ_MODE_TEXT);
        }
    }

    if (eb_search_all_alphabet(eb_book) == EB_SUCCESS) {
        export_subbook_entries(eb_book, subbook);
    }

    if (eb_search_all_kana(eb_book) == EB_SUCCESS) {
        export_subbook_entries(eb_book, subbook);
    }

    if (eb_search_all_asis(eb_book) == EB_SUCCESS) {
        export_subbook_entries(eb_book, subbook);
    }
}

static void export_book(const char path[], Book* book) {
    do {
        EB_Error_Code error;
        if ((error = eb_initialize_library()) != EB_SUCCESS) {
            strcpy(book->error, eb_error_message(error));
            break;
        }

        EB_Book eb_book;
        eb_initialize_book(&eb_book);

        if ((error = eb_bind(&eb_book, path)) != EB_SUCCESS) {
            strcpy(book->error, eb_error_message(error));
            eb_finalize_book(&eb_book);
            eb_finalize_library();
            break;
        }

        EB_Character_Code character_code;
        if (eb_character_code(&eb_book, &character_code) == EB_SUCCESS) {
            switch (character_code) {
                case EB_CHARCODE_ISO8859_1:
                    strcpy(book->character_code, "iso8859-1");
                    break;
                case EB_CHARCODE_JISX0208:
                    strcpy(book->character_code, "jisx0208");
                    break;
                case EB_CHARCODE_JISX0208_GB2312:
                    strcpy(book->character_code, "jisx0208/gb2312");
                    break;
                default:
                    strcpy(book->character_code, "invalid");
                    break;
            }
        }

        EB_Disc_Code disc_code;
        if (eb_disc_type(&eb_book, &disc_code) == EB_SUCCESS) {
            switch (disc_code) {
                case EB_DISC_EB:
                    strcpy(book->disc_code, "eb");
                    break;
                case EB_DISC_EPWING:
                    strcpy(book->disc_code, "epwing");
                    break;
                default:
                    strcpy(book->disc_code, "invalid");
                    break;
            }
        }

        EB_Subbook_Code sub_codes[EB_MAX_SUBBOOKS];
        if ((error = eb_subbook_list(&eb_book, sub_codes, &book->subbook_count)) == EB_SUCCESS) {
            if (book->subbook_count > 0) {
                book->subbooks = calloc(book->subbook_count, sizeof(Subbook));
                for (int i = 0; i < book->subbook_count; ++i) {
                    Subbook* subbook = book->subbooks + i;
                    if ((error = eb_set_subbook(&eb_book, sub_codes[i])) == EB_SUCCESS) {
                        export_subbook(&eb_book, subbook);
                    }
                    else {
                        strcpy(subbook->error, eb_error_message(error));
                    }
                }
            }
        }
        else {
            strcpy(book->error, eb_error_message(error));
        }

        eb_finalize_book(&eb_book);
        eb_finalize_library();
    }
    while(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s dictionary_path\n", argv[0]);
        return 2;
    }
    else {
        Book book = {};
        export_book(argv[1], &book);
        dump_book(&book, stdout);
        free_book(&book);
        return 1;
    }
}
