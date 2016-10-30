#include <stdio.h>
#include <string.h>

#include <eb/eb.h>
#include <eb/error.h>

void print_book_info(EB_Book * book) {
    printf("Book info for %s:\n", book->path);
    EB_Disc_Code disc_code;
    if (eb_disc_type(book, &disc_code) == EB_SUCCESS) {
        printf("\tBook type: ");
        switch (disc_code) {
            case EB_DISC_EB:
                printf("EB_DISC_EB\n");
                break;
            case EB_DISC_EPWING:
                printf("EB_DISC_EPWING\n");
                break;
            case EB_DISC_INVALID:
                printf("EB_DISC_INVALID\n");
                break;
            default:
                printf("undefined\n");
                break;
        }
    }
    else {
        perror("\tError: failed to get book type\n");
    }

    EB_Character_Code char_code;
    if (eb_character_code(book, &char_code) == EB_SUCCESS) {
        printf("\tCharacter type: ");
        switch (char_code) {
            case EB_CHARCODE_ISO8859_1:
                printf("EB_CHARCODE_ISO8859_1\n");
                break;
            case EB_CHARCODE_JISX0208:
                printf("EB_CHARCODE_JISX0208\n");
                break;
            case EB_CHARCODE_JISX0208_GB2312:
                printf("EB_CHARCODE_JISX0208_GB2312\n");
                break;
            case EB_CHARCODE_INVALID:
                printf("EB_CHARCODE_INVALID\n");
                break;
            default:
                printf("undefined\n");
                break;
        }
    }
    else {
        perror("\tError: failed to get book character code\n");
    }

    EB_Subbook_Code sub_codes[EB_MAX_SUBBOOKS];
    int sub_count = 0;

    if (eb_subbook_list(book, sub_codes, &sub_count) == EB_SUCCESS) {
        printf("\tFound %d sub-book(s):\n", sub_count);
        char title[EB_MAX_TITLE_LENGTH + 1];
        for (int i = 0; i < sub_count; ++i) {
            if (eb_subbook_title2(book, sub_codes[i], title) != EB_SUCCESS) {
                strcpy(title, "<unknown>");
            }

            printf("\t\t %d: %s\n", sub_codes[i], title);
        }
    }
    else {
        perror("\tError: could not get sub-book list\n");
    }

}

int dump(const char path[]) {
    if (eb_initialize_library() != EB_SUCCESS) {
        perror("error: failed to initialize library\n");
        return 1;
    }

    EB_Book book;
    if (eb_bind(&book, path) != EB_SUCCESS) {
        perror("error: failed to bind book\n");
        eb_finalize_book(&book);
        return 1;
    }

    print_book_info(&book);

    eb_finalize_book(&book);
    eb_finalize_library();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return 2;
    }

    return dump(argv[1]);
}
