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

typedef struct {
    int* offsets;
    int  offset_count;
    int  offset_alloc;
} Page;

/*
 * Helper functions
 */

static char* book_read(EB_Book* book, EB_Hookset* hookset, const EB_Position* position, Book_Mode mode, const Font_Table* table, int flags) {
    if (eb_seek_text(book, position) != EB_SUCCESS) {
        return NULL;
    }

    char data[1024] = {};
    ssize_t data_length = 0;
    EB_Error_Code error;

    Hook_Params params = {};
    params.table = table;
    params.flags = flags;

    switch (mode) {
        case BOOK_MODE_TEXT:
            error = eb_read_text(
                book,
                NULL,
                hookset,
                &params,
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
                &params,
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

    font_stub_decode(result, strlen(result) + 1, result);
    return result;
}

static Book_Block book_read_content(EB_Book* book, EB_Hookset* hookset, const EB_Position* position, Book_Mode mode, const Font_Table* table, int flags) {
    Book_Block block = {};
    block.text = book_read(book, hookset, position, mode, table, flags);
    block.page = position->page;
    block.offset = position->offset;
    return block;
}

static void subbook_undupe(Book_Subbook* subbook) {
    int page_count = 0;
    for (int i = 0; i < subbook->entry_count; ++i) {
        const int page_index = subbook->entries[i].text.page + 1;
        if (page_count < page_index) {
            page_count = page_index;
        }
    }

    if (page_count == 0) {
        return;
    }

    Page* pages = calloc(page_count, sizeof(Page));

    for (int i = 0; i < subbook->entry_count; ++i) {
        Book_Entry* entry = subbook->entries + i;
        Page* page = pages + entry->text.page;

        int found = 0;
        for (int j = 0; j < page->offset_count; ++j) {
            if (entry->text.offset == page->offsets[j]){
                found = 1;
                break;
            }
        }

        if (found) {
            if (i + 1 < subbook->entry_count) {
                subbook->entries[i] = subbook->entries[subbook->entry_count - 1];
            }

            --subbook->entry_count;
            --i;

            continue;
        }

        if (page->offset_count + 1 >= page->offset_alloc) {
            if (page->offset_alloc == 0) {
                page->offset_alloc = 32;
                page->offsets = malloc(page->offset_alloc * sizeof(int));
            }
            else {
                const int offset_alloc_new = page->offset_alloc * 2;
                page->offsets = realloc(page->offsets, offset_alloc_new * sizeof(int));
                page->offset_alloc = offset_alloc_new;
            }
        }

        page->offsets[page->offset_count++] = entry->text.offset;
    }

    for (int i = 0; i < page_count; ++i) {
        free(pages[i].offsets);
    }

    free(pages);
}

static void book_undupe(Book* book) {
    for (int i = 0; i < book->subbook_count; ++i) {
        subbook_undupe(book->subbooks + i);
    }
}

/*
 * Encoding to JSON
 */

static void entry_encode(json_t* entry_json, const Book_Entry* entry, int flags) {
    if (entry->heading.text != NULL) {
        json_object_set_new(entry_json, "heading", json_string(entry->heading.text));
    }

    if (flags & FLAG_POSITIONS) {
        json_object_set_new(entry_json, "headingPage", json_integer(entry->heading.page));
        json_object_set_new(entry_json, "headingOffset", json_integer(entry->heading.offset));
    }

    if (entry->text.text != NULL) {
        json_object_set_new(entry_json, "text", json_string(entry->text.text));
    }

    if (flags & FLAG_POSITIONS) {
        json_object_set_new(entry_json, "textPage", json_integer(entry->text.page));
        json_object_set_new(entry_json, "textOffset", json_integer(entry->text.offset));
    }
}

static void subbook_encode(json_t* subbook_json, const Book_Subbook* subbook, int flags) {
    if (subbook->title != NULL) {
        json_object_set_new(subbook_json, "title", json_string(subbook->title));
    }

    if (subbook->copyright.text != NULL) {
        json_object_set_new(subbook_json, "copyright", json_string(subbook->copyright.text));
    }

    if (flags & FLAG_POSITIONS) {
        json_object_set_new(subbook_json, "copyrightPage", json_integer(subbook->copyright.page));
        json_object_set_new(subbook_json, "copyrightOffset", json_integer(subbook->copyright.offset));
    }

    json_t* entry_json_array = json_array();
    for (int i = 0; i < subbook->entry_count; ++i) {
        json_t* entry_json = json_object();
        entry_encode(entry_json, subbook->entries + i, flags);
        json_array_append_new(entry_json_array, entry_json);
    }

    json_object_set_new(subbook_json, "entries", entry_json_array);
}

static void book_encode(json_t* book_json, const Book* book, int flags) {
    json_object_set_new(book_json, "charCode", json_string(book->char_code));
    json_object_set_new(book_json, "discCode", json_string(book->disc_code));

    json_t* subbook_json_array = json_array();
    for (int i = 0; i < book->subbook_count; ++i) {
        json_t* subbook_json = json_object();
        subbook_encode(subbook_json, book->subbooks + i, flags);
        json_array_append_new(subbook_json_array, subbook_json);
    }

    json_object_set_new(book_json, "subbooks", subbook_json_array);
}

/*
 * Importing from EPWING
 */

static void subbook_entries_import(Book_Subbook* subbook, EB_Book* eb_book, EB_Hookset* eb_hookset, const Font_Table* table, int flags) {
    if (subbook->entry_alloc == 0) {
        subbook->entry_alloc = 16384;
        subbook->entries = malloc(subbook->entry_alloc * sizeof(Book_Entry));
    }

    EB_Hit hits[256] = {};
    int hit_count = 0;

    do {
        if (eb_hit_list(eb_book, ARRSIZE(hits), hits, &hit_count) != EB_SUCCESS) {
            continue;
        }

        for (int i = 0; i < hit_count; ++i) {
            EB_Hit* hit = hits + i;

            if (subbook->entry_count == subbook->entry_alloc) {
                subbook->entry_alloc *= 2;
                subbook->entries = realloc(subbook->entries, subbook->entry_alloc * sizeof(Book_Entry));
            }

            Book_Entry* entry = subbook->entries + subbook->entry_count++;
            entry->heading = book_read_content(eb_book, eb_hookset, &hit->heading, BOOK_MODE_HEADING, table, flags);
            entry->text = book_read_content(eb_book, eb_hookset, &hit->text, BOOK_MODE_TEXT, table, flags);
        }
    }
    while (hit_count > 0);
}

static void subbook_import(Book_Subbook* subbook, const Font_Context* context, EB_Book* eb_book, EB_Hookset* eb_hookset, int flags) {
    const Font_Table* table = NULL;
    char title[EB_MAX_TITLE_LENGTH + 1];
    if (eb_subbook_title(eb_book, title) == EB_SUCCESS) {
        subbook->title = eucjp_to_utf8(title);
        table = font_table_select(context, subbook->title);
    }

    if (eb_have_copyright(eb_book)) {
        EB_Position position;
        if (eb_copyright(eb_book, &position) == EB_SUCCESS) {
            subbook->copyright = book_read_content(eb_book, eb_hookset, &position, BOOK_MODE_TEXT, table, flags);
        }
    }

    if (eb_search_all_alphabet(eb_book) == EB_SUCCESS) {
        subbook_entries_import(subbook, eb_book, eb_hookset, table, flags);
    }

    if (eb_search_all_kana(eb_book) == EB_SUCCESS) {
        subbook_entries_import(subbook, eb_book, eb_hookset, table, flags);
    }

    if (eb_search_all_asis(eb_book) == EB_SUCCESS) {
        subbook_entries_import(subbook, eb_book, eb_hookset, table, flags);
    }
}

/*
 * imported functions
 */

void book_init(Book* book) {
    memset(book, 0, sizeof(Book));
}

void book_free(Book* book) {
    for (int i = 0; i < book->subbook_count; ++i) {
        Book_Subbook* subbook = book->subbooks + i;
        free(subbook->title);
        free(subbook->copyright.text);

        for (int j = 0; j < subbook->entry_count; ++j) {
            Book_Entry* entry = subbook->entries + j;
            free(entry->heading.text);
            free(entry->text.text);
        }

        free(subbook->entries);
    }

    memset(book, 0, sizeof(Book));
}

int book_export(FILE* fp, const Book* book, int flags) {
    json_t* book_json = json_object();
    book_encode(book_json, book, flags);

    char* output = json_dumps(book_json, flags & FLAG_PRETTY_PRINT ? JSON_INDENT(4) : JSON_COMPACT);
    if (output != NULL) {
        fputs(output, fp);
    }
    free(output);

    json_decref(book_json);
    return output != NULL;
}


int book_import(Book* book, const Font_Context* context, const char path[], int flags) {
    EB_Error_Code error;
    if ((error = eb_initialize_library()) != EB_SUCCESS) {
        fprintf(stderr, "Failed to initialize library: %s\n", eb_error_message(error));
        return 0;
    }

    EB_Book eb_book;
    eb_initialize_book(&eb_book);

    EB_Hookset eb_hookset;
    eb_initialize_hookset(&eb_hookset);
    hooks_install(&eb_hookset, flags);

    if ((error = eb_bind(&eb_book, path)) != EB_SUCCESS) {
        fprintf(stderr, "Failed to bind book: %s\n", eb_error_message(error));
        eb_finalize_book(&eb_book);
        eb_finalize_hookset(&eb_hookset);
        eb_finalize_library();
        return 0;
    }

    EB_Character_Code char_code;
    if ((error = eb_character_code(&eb_book, &char_code)) == EB_SUCCESS) {
        switch (char_code) {
            case EB_CHARCODE_ISO8859_1:
                strcpy(book->char_code, "iso8859-1");
                break;
            case EB_CHARCODE_JISX0208:
                strcpy(book->char_code, "jisx0208");
                break;
            case EB_CHARCODE_JISX0208_GB2312:
                strcpy(book->char_code, "jisx0208/gb2312");
                break;
            default:
                strcpy(book->char_code, "invalid");
                break;
        }
    }
    else {
        fprintf(stderr, "Failed to get character code: %s\n", eb_error_message(error));
    }

    EB_Disc_Code disc_code;
    if ((error = eb_disc_type(&eb_book, &disc_code)) == EB_SUCCESS) {
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
    else {
        fprintf(stderr, "Failed to get disc code: %s\n", eb_error_message(error));
    }

    EB_Subbook_Code sub_codes[EB_MAX_SUBBOOKS];
    if ((error = eb_subbook_list(&eb_book, sub_codes, &book->subbook_count)) == EB_SUCCESS) {
        if (book->subbook_count > 0) {
            book->subbooks = calloc(book->subbook_count, sizeof(Book_Subbook));
            for (int i = 0; i < book->subbook_count; ++i) {
                Book_Subbook* subbook = book->subbooks + i;
                if ((error = eb_set_subbook(&eb_book, sub_codes[i])) == EB_SUCCESS) {
                    subbook_import(subbook, context, &eb_book, &eb_hookset, flags);
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

    book_undupe(book);
    return 1;
}
