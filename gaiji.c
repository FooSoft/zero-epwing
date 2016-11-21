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
#include <stdbool.h>
#include <assert.h>

#include "util.h"
#include "gaiji.h"

/*
 * Macros
 */

#define GAIJI_TABLE(name, ents) {\
    name,\
    gaiji_table_##ents##_wide,\
    ARRSIZE(gaiji_table_##ents##_wide),\
    gaiji_table_##ents##_narrow,\
    ARRSIZE(gaiji_table_##ents##_narrow)\
}

/*
 * Local data
 */

#include "tables/gaiji_table_chujiten.h"
#include "tables/gaiji_table_daijirin.h"
#include "tables/gaiji_table_daijisen.h"
#include "tables/gaiji_table_genius.h"
#include "tables/gaiji_table_kojien.h"
#include "tables/gaiji_table_meikyojj.h"
#include "tables/gaiji_table_meikyou.h"
#include "tables/gaiji_table_snmkg99.h"
#include "tables/gaiji_table_wadai5.h"

static const Gaiji_Table gaiji_tables[] = {
    GAIJI_TABLE("ジーニアス英和辞典", genius),
    GAIJI_TABLE("スーパー大辞林", daijirin),
    GAIJI_TABLE("大辞泉", daijisen),
    GAIJI_TABLE("広辞苑第六版", kojien),
    GAIJI_TABLE("新和英大辞典", wadai5),
    GAIJI_TABLE("新明解国語辞典　", snmkg99),
    GAIJI_TABLE("新英和・和英中辞典", chujiten),
    GAIJI_TABLE("明鏡国語辞典", meikyojj),
};

/*
 * Local functions
 */

static int ascii_to_nibble(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'a' && c <= 'f') {
        return 0x0a + (c - 'a');
    }

    return 0;
}

static char nibble_to_ascii(int n) {
    const char hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    return n <= 0x0f ? hex[n] : 0;
}

static bool is_ascii_nibble(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static void encode_sequence(char output[], int size, const char utf8[]) {
    strncpy(output, "{#", size);
    int offset = strlen(output);

    for (int i = 0; i < MAX_UTF8_BYTES; ++i) {
        const unsigned char byte = utf8[i];
        if (byte == 0) {
            break;
        }

        if (offset >= size - 1) {
            break;
        }

        output[offset++] = nibble_to_ascii(byte >> 0x04);

        if (offset >= size - 1) {
            break;
        }

        output[offset++] = nibble_to_ascii(byte & 0x0f);
    }

    output[offset] = 0;
    strncat(output, "}", size);
}

/*
 * Exported functions
 */

const Gaiji_Table* gaiji_table_select(const char name[]) {
    for (unsigned i = 0; i < ARRSIZE(gaiji_tables); ++i) {
        const Gaiji_Table* table = gaiji_tables + i;
        if (strcmp(table->name, name) == 0) {
            return table;
        }
    }

    return NULL;
}

void gaiji_stub_encode(char output[], int size, int code, const Gaiji_Table* table, Gaiji_Width width) {
    do {
        if (table == NULL) {
            break;
        }

        const Gaiji_Entry* entries = NULL;
        int count = 0;

        switch (width) {
            case GAIJI_WIDTH_WIDE:
                entries = table->table_wide;
                count = table->count_wide;
                break;
            case GAIJI_WIDTH_NARROW:
                entries = table->table_narrow;
                count = table->count_narrow;
                break;
        }

        assert(entries != NULL);

        for (int i = 0; i < count; ++i) {
            const Gaiji_Entry* entry = entries + i;
            if (entry->code == code) {
                encode_sequence(output, size, entry->utf8);
                return;
            }
        }

    }
    while (0);

    strncpy(output, "<?>", size);
    output[size - 1] = 0;
}

void gaiji_stub_decode(char output[], int size, const char input[]) {
    const char* ptr_in = input;
    char* ptr_out = output;
    bool decode = false;

    while (*ptr_in != 0 && ptr_out - output < size - 1) {
        if (strncmp(ptr_in, "{#", 2) == 0) {
            decode = true;
            ptr_in += 2;
        }

        if (decode) {
            const char high_ascii = *ptr_in++;
            if (high_ascii == 0) {
                break;
            }

            const char low_ascii = *ptr_in++;
            if (low_ascii == 0) {
                break;
            }

            if (high_ascii == '}') {
                decode = false;
                --ptr_in;
            }
            else {
                assert(is_ascii_nibble(high_ascii));
                assert(is_ascii_nibble(low_ascii));
                *ptr_out++ = ascii_to_nibble(high_ascii) << 0x04 | ascii_to_nibble(low_ascii);
            }
        }
        else {
            *ptr_out++ = *ptr_in++;
        }
    }

    *ptr_out = 0;
}
