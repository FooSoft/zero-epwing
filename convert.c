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
#include <errno.h>
#include <stdlib.h>

/*
 * Local functions
 */

/* https://stackoverflow.com/questions/2162390/iconv-encoding-conversion-problem */

static char * convert (const char *from_charset, const char *to_charset, const char *input) {
    size_t inleft, outleft, converted = 0;
    char *output, *outbuf, *tmp;
    const char *inbuf;
    size_t outlen;
    iconv_t cd;

    if ((cd = iconv_open (to_charset, from_charset)) == (iconv_t) -1)
        return NULL;

    inleft = strlen (input);
    inbuf = input;

    /* we'll start off allocating an output buffer which is the same size
     * as our input buffer. */
    outlen = inleft;

    /* we allocate 4 bytes more than what we need for nul-termination... */
    if (!(output = malloc (outlen + 4))) {
        iconv_close (cd);
        return NULL;
    }

    do {
        errno = 0;
        outbuf = output + converted;
        outleft = outlen - converted;

        converted = iconv (cd, (char **) &inbuf, &inleft, &outbuf, &outleft);
        if (converted != (size_t) -1 || errno == EINVAL) {
            /*
             * EINVAL  An  incomplete  multibyte sequence has been encoun­-
             *         tered in the input.
             *
             * We'll just truncate it and ignore it.
             */
            break;
        }

        if (errno != E2BIG) {
            /*
             * EILSEQ An invalid multibyte sequence has been  encountered
             *        in the input.
             *
             * Bad input, we can't really recover from this.
             */
            iconv_close (cd);
            free (output);
            return NULL;
        }

        /*
         * E2BIG   There is not sufficient room at *outbuf.
         *
         * We just need to grow our outbuffer and try again.
         */

        converted = outbuf - output;
        outlen += inleft * 2 + 8;

        if (!(tmp = realloc (output, outlen + 4))) {
            iconv_close (cd);
            free (output);
            return NULL;
        }

        output = tmp;
        outbuf = output + converted;
    } while (1);

    /* flush the iconv conversion */
    iconv (cd, NULL, NULL, &outbuf, &outleft);
    iconv_close (cd);

    /* Note: not all charsets can be nul-terminated with a single
     * nul byte. UCS2, for example, needs 2 nul bytes and UCS4
     * needs 4. I hope that 4 nul bytes is enough to terminate all
     * multibyte charsets? */

    /* nul-terminate the string */
    memset (outbuf, 0, 4);

    return output;
}

/*
 * Exported functions
 */

char* eucjp_to_utf8(const char src[]) {
    return convert("EUC-JP", "UTF-8", src);
}
