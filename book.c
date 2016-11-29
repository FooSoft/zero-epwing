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

#include <string.h>

#include "book.h"
#include "hooks.h"
#include "convert.h"
#include "util.h"

#include "eb/eb/eb.h"
#include "eb/eb/error.h"
#include "eb/eb/text.h"

#include "jansson/include/jansson.h"

/*
 * Local types
 */

typedef enum {
    BOOK_MODE_TEXT,
    BOOK_MODE_HEADING,
} Book_Mode;

/*
 * Local functions
 */

static char* book_read(
    EB_Book*           book,
    EB_Hookset*        hookset,
    const EB_Position* position,
    Book_Mode          mode,
    const Gaiji_Table* table
) {
    if (eb_seek_text(book, position) != EB_SUCCESS) {
        return NULL;
    }

    char data[1024];
    ssize_t data_length = 0;
    EB_Error_Code error;

    switch (mode) {
        case BOOK_MODE_TEXT:
            error = eb_read_text(
                book,
                NULL,
                hookset,
                (void*)table,
                ARRSIZE(data) - 1,
                data,
                &data_length
            );
            break;
        case BOOK_MODE_HEADING:
            error = eb_read_heading(
                book,
                NULL,
                hookset,
                (void*)table,
                ARRSIZE(data) - 1,
                data,
                &data_length
            );
            break;
        default:
            return NULL;
    }

    if (error != EB_SUCCESS) {
        return NULL;
    }

    char * result = eucjp_to_utf8(data);
    if (result == NULL) {
        return NULL;
    }

    gaiji_stub_decode(result, strlen(result) + 1, result);
    return result;
}

static void entry_encode(Book_Entry* entry, json_t* entry_json) {
    json_object_set_new(entry_json, "heading", json_string(entry->heading));
    json_object_set_new(entry_json, "text", json_string(entry->text));
}

static void subbok_encode(Book_Subbook* subbook, json_t* subbook_json) {
    if (subbook->title != NULL) {
        json_object_set_new(subbook_json, "title", json_string(subbook->title));
    }

    if (subbook->copyright != NULL) {
        json_object_set_new(subbook_json, "copyright", json_string(subbook->copyright));
    }

    json_t* entry_json_array = json_array();
    for (int i = 0; i < subbook->entry_count; ++i) {
        json_t* entry_json = json_object();
        entry_encode(subbook->entries + i, entry_json);
        json_array_append(entry_json_array, entry_json);
        json_decref(entry_json);
    }

    json_object_set(subbook_json, "entries", entry_json_array);
    json_decref(entry_json_array);
}

static void book_encode(Book* book, json_t* book_json) {
    json_object_set_new(book_json, "characterCode", json_string(book->character_code));
    json_object_set_new(book_json, "discCode", json_string(book->disc_code));

    json_t* subbook_json_array = json_array();
    for (int i = 0; i < book->subbook_count; ++i) {
        json_t* subbook_json = json_object();
        subbok_encode(book->subbooks + i, subbook_json);
        json_array_append(subbook_json_array, subbook_json);
        json_decref(subbook_json);
    }

    json_object_set(book_json, "subbooks", subbook_json_array);
    json_decref(subbook_json_array);
}

static void subbook_entries_export(Book_Subbook* subbook, EB_Book* eb_book, EB_Hookset* eb_hookset, const Gaiji_Table* table) {
    if (subbook->entry_capacity == 0) {
        subbook->entry_capacity = 16384;
        subbook->entries = malloc(subbook->entry_capacity * sizeof(Book_Entry));
    }

    EB_Hit hits[256];
    int hit_count = 0;

    do {
        if (eb_hit_list(eb_book, ARRSIZE(hits), hits, &hit_count) != EB_SUCCESS) {
            continue;
        }

        for (int i = 0; i < hit_count; ++i) {
            EB_Hit* hit = hits + i;

            if (subbook->entry_count == subbook->entry_capacity) {
                subbook->entry_capacity *= 2;
                subbook->entries = realloc(subbook->entries, subbook->entry_capacity * sizeof(Book_Entry));
            }

            Book_Entry* entry = subbook->entries + subbook->entry_count++;
            entry->heading = book_read(eb_book, eb_hookset, &hit->heading, BOOK_MODE_HEADING, table);
            entry->text = book_read(eb_book, eb_hookset, &hit->text, BOOK_MODE_TEXT, table);
        }
    }
    while (hit_count > 0);
}

static void subbook_export(Book_Subbook* subbook, const Gaiji_Context* context, EB_Book* eb_book, EB_Hookset* eb_hookset) {
    const Gaiji_Table* table = NULL;
    char title[EB_MAX_TITLE_LENGTH + 1];
    if (eb_subbook_title(eb_book, title) == EB_SUCCESS) {
        subbook->title = eucjp_to_utf8(title);
        table = gaiji_table_select(context, subbook->title);
    }

    if (eb_have_copyright(eb_book)) {
        EB_Position position;
        if (eb_copyright(eb_book, &position) == EB_SUCCESS) {
            subbook->copyright = book_read(eb_book, eb_hookset, &position, BOOK_MODE_TEXT, table);
        }
    }

    if (eb_search_all_alphabet(eb_book) == EB_SUCCESS) {
        subbook_entries_export(subbook, eb_book, eb_hookset, table);
    }

    if (eb_search_all_kana(eb_book) == EB_SUCCESS) {
        subbook_entries_export(subbook, eb_book, eb_hookset, table);
    }

    if (eb_search_all_asis(eb_book) == EB_SUCCESS) {
        subbook_entries_export(subbook, eb_book, eb_hookset, table);
    }
}

/*
 * Exported functions
 */

void book_init(Book* book) {
    memset(book, 0, sizeof(Book));
}

void book_free(Book* book) {
    for (int i = 0; i < book->subbook_count; ++i) {
        Book_Subbook* subbook = book->subbooks + i;
        free(subbook->title);
        free(subbook->copyright);

        for (int j = 0; j < subbook->entry_count; ++j) {
            Book_Entry* entry = subbook->entries + j;
            free(entry->heading);
            free(entry->text);
        }

        free(subbook->entries);
    }
}

bool book_dump(Book* book, bool pretty_print, FILE* fp) {
    json_t* book_json = json_object();
    book_encode(book, book_json);

    char* output = json_dumps(book_json, pretty_print ? JSON_INDENT(4) : JSON_COMPACT);
    if (output != NULL) {
        fputs(output, fp);
    }
    free(output);

    json_decref(book_json);
    return output != NULL;
}


bool book_export(Book* book, const Gaiji_Context* context, const char path[], bool markup) {
    EB_Error_Code error;
    if ((error = eb_initialize_library()) != EB_SUCCESS) {
        fprintf(stderr, "Failed to initialize library: %s\n", eb_error_message(error));
        return false;
    }

    EB_Book eb_book;
    eb_initialize_book(&eb_book);

    EB_Hookset eb_hookset;
    eb_initialize_hookset(&eb_hookset);
    hooks_install(&eb_hookset, markup);

    if ((error = eb_bind(&eb_book, path)) != EB_SUCCESS) {
        fprintf(stderr, "Failed to bind book: %s\n", eb_error_message(error));
        eb_finalize_book(&eb_book);
        eb_finalize_hookset(&eb_hookset);
        eb_finalize_library();
        return false;
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
            book->subbooks = calloc(book->subbook_count, sizeof(Book_Subbook));
            for (int i = 0; i < book->subbook_count; ++i) {
                Book_Subbook* subbook = book->subbooks + i;
                if ((error = eb_set_subbook(&eb_book, sub_codes[i])) == EB_SUCCESS) {
                    subbook_export(subbook, context, &eb_book, &eb_hookset);
                }
                else {
                    fprintf(stderr, "Failed to set subbook: %s\n", eb_error_message(error));
                }
            }
        }
    }
    else {
        fprintf(stderr, "Failed to get subbook list: %s\n", eb_error_message(error));
    }

    eb_finalize_book(&eb_book);
    eb_finalize_hookset(&eb_hookset);
    eb_finalize_library();

    return true;
}
