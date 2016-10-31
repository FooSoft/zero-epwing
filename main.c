#include <stdio.h>
#include <string.h>

#include "eb/eb/eb.h"
#include "eb/eb/error.h"
#include "eb/eb/text.h"

/* Constants */

const unsigned MAX_ENTRY_HITS = 128;
const unsigned MAX_ENTRY_TEXT = 1024;

/* Local functions */

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
            ssize_t text_length;

            eb_seek_text(book, &hits[i].text);
            eb_read_text(book, NULL, NULL, NULL, MAX_ENTRY_TEXT, text, &text_length);

            puts(text);
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

/* Entry point */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return 2;
    }

    return process(argv[1]);
}
