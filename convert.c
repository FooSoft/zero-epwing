/*
 * Copyright (c) 2017 Alex Yatskov <alex@foosoft.net>
 * Author: Alex Yatskov <alex@foosoft.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
