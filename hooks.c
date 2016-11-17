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

#include "hooks.h"
#include "eb/eb/eb.h"
#include "eb/eb/text.h"

/*
 *  Boilerplate
 */

#define HOOK_TAG_HELPER(FUNC_NAME, TAG_NAME) \
    static EB_Error_Code hook_##FUNC_NAME##_begin(\
        EB_Book*           book,\
        EB_Appendix*       appendix,\
        void*              container,\
        EB_Hook_Code       code,\
        int                argc,\
        const unsigned int argv[]\
    ) {\
        eb_write_text_string(book, "<" #TAG_NAME ">");\
        (void)appendix;\
        (void)container;\
        (void)code;\
        (void)argc;\
        (void)argv;\
        return 0;\
    }

#define HOOK_TAG_SINGLE(NAME) \
    HOOK_TAG_HELPER(NAME, NAME)

#define HOOK_TAG_PAIR(NAME) \
    HOOK_TAG_HELPER(NAME##_begin, #NAME); \
    HOOK_TAG_HELPER(NAME##_end, "/" #NAME);

#define MAX_REPLACEMENT 64

/*
 *  Paired tags
 */

HOOK_TAG_PAIR(candidate);         /* EB_HOOK_BEGIN_CANDIDATE */
HOOK_TAG_PAIR(color_bmp);         /* EB_HOOK_BEGIN_COLOR_BMP */
HOOK_TAG_PAIR(color_jpeg);        /* EB_HOOK_BEGIN_COLOR_JPEG */
HOOK_TAG_PAIR(decoration);        /* EB_HOOK_BEGIN_DECORATION */
HOOK_TAG_PAIR(emphasis);          /* EB_HOOK_BEGIN_EMPHASIS */
HOOK_TAG_PAIR(gray_graphic);      /* EB_HOOK_BEGIN_GRAY_GRAPHIC */
HOOK_TAG_PAIR(in_color_bmp);      /* EB_HOOK_BEGIN_IN_COLOR_BMP */
HOOK_TAG_PAIR(in_color_jpeg);     /* EB_HOOK_BEGIN_IN_COLOR_JPEG */
HOOK_TAG_PAIR(keyword);           /* EB_HOOK_BEGIN_KEYWORD */
HOOK_TAG_PAIR(mono_graphic);      /* EB_HOOK_BEGIN_MONO_GRAPHIC */
HOOK_TAG_PAIR(mpeg);              /* EB_HOOK_BEGIN_MPEG */
HOOK_TAG_PAIR(narrow);            /* EB_HOOK_BEGIN_NARROW */
HOOK_TAG_PAIR(no_newline);        /* EB_HOOK_BEGIN_NO_NEWLINE */
HOOK_TAG_PAIR(reference);         /* EB_HOOK_BEGIN_REFERENCE */
HOOK_TAG_PAIR(subscript);         /* EB_HOOK_BEGIN_SUBSCRIPT */
HOOK_TAG_PAIR(superscript);       /* EB_HOOK_BEGIN_SUPERSCRIPT */
HOOK_TAG_PAIR(wave);              /* EB_HOOK_BEGIN_WAVE */

/*
 *  Single tags
 */

HOOK_TAG_SINGLE(gb2312);          /* EB_HOOK_GB2312 */
HOOK_TAG_SINGLE(iso8859_1);       /* EB_HOOK_ISO8859_1 */
HOOK_TAG_SINGLE(narrow_jisx0208); /* EB_HOOK_NARROW_JISX0208 */
HOOK_TAG_SINGLE(newline);         /* EB_HOOK_NEWLINE */
HOOK_TAG_SINGLE(set_indent);      /* EB_HOOK_SET_INDENT */
HOOK_TAG_SINGLE(wide_jisx0208);   /* EB_HOOK_WIDE_JISX0208 */
HOOK_TAG_SINGLE(null);            /* EB_HOOK_NULL */

/*
 *  Gaiji nonsense
 */

static EB_Error_Code hook_narrow_font( /* EB_HOOK_NARROW_FONT */
    EB_Book*           book,
    EB_Appendix*       appendix,
    void*              container,
    EB_Hook_Code       code,
    int                argc,
    const unsigned int argv[]
) {
    (void)book;
    (void)appendix;
    (void)container;
    (void)code;
    (void)argc;
    (void)argv;

    /* char replacement[MAX_REPLACEMENT] = "?"; */

    /* /1* Check if there is a UTF-8 replacement for this gaiji code *1/ */
    /* if(get_gaiji_replacment_elem(subbook_directory, 'n', (unsigned short)argv[0]) != NULL) */
    /* { */
    /*     sprintf(replacement, "{#n%04X}", argv[0]); */
    /* } */
    /* else if(gaiji_option == GAIJI_OPTION_HTML_IMG) /1* Add HTML IMG tag *1/ */
    /* { */
    /*     get_character_html_img(replacement, book, SAVE_BMP, SAVE_NARROW, argv[0]); */
    /* } */
    /* else */
    /* { */
    /*     sprintf(replacement, "?"); */
    /* } */

    /* eb_write_text_string(book, replacement); */

    return 0;
}

static EB_Error_Code hook_wide_font( /* EB_HOOK_WIDE_FONT */
    EB_Book*           book,
    EB_Appendix*       appendix,
    void*              container,
    EB_Hook_Code       code,
    int                argc,
    const unsigned int argv[]
) {
    (void)book;
    (void)appendix;
    (void)container;
    (void)code;
    (void)argc;
    (void)argv;

    /* char replacement[MAXLEN_PATH] = ""; */

    /* /1* Check if there is a UTF-8 replacement for this gaiji code *1/ */
    /* if(get_gaiji_replacment_elem(subbook_directory, 'w', (unsigned short)argv[0]) != NULL) */
    /* { */
    /*     sprintf(replacement, "{#w%04X}", argv[0]); */
    /* } */
    /* else if(gaiji_option == GAIJI_OPTION_HTML_IMG) /1* Add HTML IMG tag *1/ */
    /* { */
    /*     get_character_html_img(replacement, book, SAVE_BMP, SAVE_WIDE, argv[0]); */
    /* } */
    /* else */
    /* { */
    /*     sprintf(replacement, "?"); */
    /* } */

    /* eb_write_text_string(book, replacement); */

    return 0;
}
