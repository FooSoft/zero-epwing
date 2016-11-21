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

#include <assert.h>

#include "hooks.h"
#include "gaiji.h"
#include "util.h"

#include "eb/eb/eb.h"
#include "eb/eb/text.h"

/*
 * Macros
 */

#define HOOK_FUNC_NAME(NAME) hook_##NAME

#define HOOK_TAGGER(NAME) \
    static EB_Error_Code HOOK_FUNC_NAME(NAME)(\
        EB_Book*           book,\
        EB_Appendix*       appendix,\
        void*              container,\
        EB_Hook_Code       code,\
        int                argc,\
        const unsigned int argv[]\
    ) {\
        (void)appendix;\
        (void)container;\
        (void)code;\
        (void)argc;\
        (void)argv;\
        eb_write_text_string(book, "<" #NAME ">");\
        return 0;\
    }

/*
 * Local functions
 */

HOOK_TAGGER(begin_candidate);      /* EB_HOOK_BEGIN_CANDIDATE */
HOOK_TAGGER(begin_color_bmp);      /* EB_HOOK_BEGIN_COLOR_BMP */
HOOK_TAGGER(begin_color_jpeg);     /* EB_HOOK_BEGIN_COLOR_JPEG */
HOOK_TAGGER(begin_decoration);     /* EB_HOOK_BEGIN_DECORATION */
HOOK_TAGGER(begin_emphasis);       /* EB_HOOK_BEGIN_EMPHASIS */
HOOK_TAGGER(begin_gray_graphic);   /* EB_HOOK_BEGIN_GRAY_GRAPHIC */
HOOK_TAGGER(begin_in_color_bmp);   /* EB_HOOK_BEGIN_IN_COLOR_BMP */
HOOK_TAGGER(begin_in_color_jpeg);  /* EB_HOOK_BEGIN_IN_COLOR_JPEG */
HOOK_TAGGER(begin_keyword);        /* EB_HOOK_BEGIN_KEYWORD */
HOOK_TAGGER(begin_mono_graphic);   /* EB_HOOK_BEGIN_MONO_GRAPHIC */
HOOK_TAGGER(begin_mpeg);           /* EB_HOOK_BEGIN_MPEG */
HOOK_TAGGER(begin_narrow);         /* EB_HOOK_BEGIN_NARROW */
HOOK_TAGGER(begin_no_newline);     /* EB_HOOK_BEGIN_NO_NEWLINE */
HOOK_TAGGER(begin_reference);      /* EB_HOOK_BEGIN_REFERENCE */
HOOK_TAGGER(begin_subscript);      /* EB_HOOK_BEGIN_SUBSCRIPT */
HOOK_TAGGER(begin_superscript);    /* EB_HOOK_BEGIN_SUPERSCRIPT */
HOOK_TAGGER(begin_wave);           /* EB_HOOK_BEGIN_WAVE */
HOOK_TAGGER(end_candidate_group);  /* EB_HOOK_END_CANDIDATE_GROUP */
HOOK_TAGGER(end_candidate_leaf);   /* EB_HOOK_END_CANDIDATE_LEAF */
HOOK_TAGGER(end_color_graphic);    /* EB_HOOK_END_COLOR_GRAPHIC */
HOOK_TAGGER(end_decoration);       /* EB_HOOK_END_DECORATION */
HOOK_TAGGER(end_emphasis);         /* EB_HOOK_END_EMPHASIS */
HOOK_TAGGER(end_gray_graphic);     /* EB_HOOK_END_GRAY_GRAPHIC */
HOOK_TAGGER(end_in_color_graphic); /* EB_HOOK_SET_END_IN_COLOR_GRAPHIC */
HOOK_TAGGER(end_keyword);          /* EB_HOOK_END_KEYWORD */
HOOK_TAGGER(end_mono_graphic);     /* EB_HOOK_END_MONO_GRAPHIC */
HOOK_TAGGER(end_mpeg);             /* EB_HOOK_END_MPEG */
HOOK_TAGGER(end_narrow);           /* EB_HOOK_END_NARROW */
HOOK_TAGGER(end_no_newline);       /* EB_HOOK_END_NO_NEWLINE */
HOOK_TAGGER(end_reference);        /* EB_HOOK_END_REFERENCE */
HOOK_TAGGER(end_subscript);        /* EB_HOOK_END_SUBSCRIPT */
HOOK_TAGGER(end_superscript);      /* EB_HOOK_END_SUPERSCRIPT */
HOOK_TAGGER(end_wave);             /* EB_HOOK_END_WAVE */
HOOK_TAGGER(gb2312);               /* EB_HOOK_GB2312 */
HOOK_TAGGER(iso8859_1);            /* EB_HOOK_ISO8859_1 */
HOOK_TAGGER(narrow_jisx0208);      /* EB_HOOK_NARROW_JISX0208 */
HOOK_TAGGER(newline);              /* EB_HOOK_NEWLINE */
HOOK_TAGGER(set_indent);           /* EB_HOOK_SET_INDENT */
HOOK_TAGGER(wide_jisx0208);        /* EB_HOOK_WIDE_JISX0208 */
HOOK_TAGGER(null);                 /* EB_HOOK_NULL */

static EB_Error_Code hook_narrow_font( /* EB_HOOK_NARROW_FONT */
    EB_Book*           book,
    EB_Appendix*       appendix,
    void*              container,
    EB_Hook_Code       code,
    int                argc,
    const unsigned int argv[]
) {
    (void)appendix;
    (void)code;

    assert(argc > 0);
    char stub[MAX_STUB_BYTES];
    gaiji_stub_encode(stub, ARRSIZE(stub), argv[0], container, GAIJI_WIDTH_NARROW);
    eb_write_text_string(book, stub);

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
    (void)appendix;
    (void)code;

    assert(argc > 0);
    char stub[MAX_STUB_BYTES];
    gaiji_stub_encode(stub, ARRSIZE(stub), argv[0], container, GAIJI_WIDTH_WIDE);
    eb_write_text_string(book, stub);

    return 0;
}

/*
 * Local data
 */

static const EB_Hook s_hooks[] = {
    { EB_HOOK_BEGIN_CANDIDATE,      HOOK_FUNC_NAME(begin_candidate)      },
    { EB_HOOK_BEGIN_COLOR_BMP,      HOOK_FUNC_NAME(begin_color_bmp)      },
    { EB_HOOK_BEGIN_COLOR_JPEG,     HOOK_FUNC_NAME(begin_color_jpeg)     },
    { EB_HOOK_BEGIN_DECORATION,     HOOK_FUNC_NAME(begin_decoration)     },
    { EB_HOOK_BEGIN_EMPHASIS,       HOOK_FUNC_NAME(begin_emphasis)       },
    { EB_HOOK_BEGIN_GRAY_GRAPHIC,   HOOK_FUNC_NAME(begin_gray_graphic)   },
    { EB_HOOK_BEGIN_IN_COLOR_BMP,   HOOK_FUNC_NAME(begin_in_color_bmp)   },
    { EB_HOOK_BEGIN_IN_COLOR_JPEG,  HOOK_FUNC_NAME(begin_in_color_jpeg)  },
    { EB_HOOK_BEGIN_KEYWORD,        HOOK_FUNC_NAME(begin_keyword)        },
    { EB_HOOK_BEGIN_MONO_GRAPHIC,   HOOK_FUNC_NAME(begin_mono_graphic)   },
    { EB_HOOK_BEGIN_MPEG,           HOOK_FUNC_NAME(begin_mpeg)           },
    { EB_HOOK_BEGIN_NARROW,         HOOK_FUNC_NAME(begin_narrow)         },
    { EB_HOOK_BEGIN_NO_NEWLINE,     HOOK_FUNC_NAME(begin_no_newline)     },
    { EB_HOOK_BEGIN_REFERENCE,      HOOK_FUNC_NAME(begin_reference)      },
    { EB_HOOK_BEGIN_SUBSCRIPT,      HOOK_FUNC_NAME(begin_subscript)      },
    { EB_HOOK_BEGIN_SUPERSCRIPT,    HOOK_FUNC_NAME(begin_superscript)    },
    { EB_HOOK_BEGIN_WAVE,           HOOK_FUNC_NAME(begin_wave)           },
    { EB_HOOK_END_CANDIDATE_GROUP,  HOOK_FUNC_NAME(end_candidate_group)  },
    { EB_HOOK_END_CANDIDATE_LEAF,   HOOK_FUNC_NAME(end_candidate_leaf)   },
    { EB_HOOK_END_COLOR_GRAPHIC,    HOOK_FUNC_NAME(end_color_graphic)    },
    { EB_HOOK_END_DECORATION,       HOOK_FUNC_NAME(end_decoration)       },
    { EB_HOOK_END_EMPHASIS,         HOOK_FUNC_NAME(end_emphasis)         },
    { EB_HOOK_END_GRAY_GRAPHIC,     HOOK_FUNC_NAME(end_gray_graphic)     },
    { EB_HOOK_END_IN_COLOR_GRAPHIC, HOOK_FUNC_NAME(end_in_color_graphic) },
    { EB_HOOK_END_KEYWORD,          HOOK_FUNC_NAME(end_keyword)          },
    { EB_HOOK_END_MONO_GRAPHIC,     HOOK_FUNC_NAME(end_mono_graphic)     },
    { EB_HOOK_END_MPEG,             HOOK_FUNC_NAME(end_mpeg)             },
    { EB_HOOK_END_NARROW,           HOOK_FUNC_NAME(end_narrow)           },
    { EB_HOOK_END_NO_NEWLINE,       HOOK_FUNC_NAME(end_no_newline)       },
    { EB_HOOK_END_REFERENCE,        HOOK_FUNC_NAME(end_reference)        },
    { EB_HOOK_END_SUBSCRIPT,        HOOK_FUNC_NAME(end_subscript)        },
    { EB_HOOK_END_SUPERSCRIPT,      HOOK_FUNC_NAME(end_superscript)      },
    { EB_HOOK_END_WAVE,             HOOK_FUNC_NAME(end_wave)             },
    { EB_HOOK_GB2312,               HOOK_FUNC_NAME(gb2312)               },
    { EB_HOOK_ISO8859_1,            HOOK_FUNC_NAME(iso8859_1)            },
    { EB_HOOK_NARROW_FONT,          HOOK_FUNC_NAME(narrow_font)          },
    /* { EB_HOOK_NARROW_JISX0208,      HOOK_FUNC_NAME(narrow_jisx0208)      }, */
    { EB_HOOK_NEWLINE,              HOOK_FUNC_NAME(newline)              },
    { EB_HOOK_SET_INDENT,           HOOK_FUNC_NAME(set_indent)           },
    { EB_HOOK_WIDE_FONT,            HOOK_FUNC_NAME(wide_font)            },
    /* { EB_HOOK_WIDE_JISX0208,        HOOK_FUNC_NAME(wide_jisx0208)        }, */
    { EB_HOOK_NULL,                 HOOK_FUNC_NAME(null)                 },
};

/*
 * Exported functions
 */

void hooks_install(EB_Hookset* hookset) {
    for (unsigned i = 0; i < ARRSIZE(s_hooks); ++i) {
        eb_set_hook(hookset, s_hooks + i);
    }
}
