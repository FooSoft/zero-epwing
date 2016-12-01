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
#include <stdlib.h>
#include <assert.h>

#include "jansson/include/jansson.h"

#include "util.h"
#include "font.h"

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

static void parse_entry(Font_Entry* entry, const json_t* entry_json) {
    entry->code = json_integer_value(json_array_get(entry_json, 0));
    const char* utf8 = json_string_value(json_array_get(entry_json, 1));
    if (utf8 == NULL) {
        *entry->utf8 = 0;
    }
    else {
        strncpy(entry->utf8, utf8, ARRSIZE(entry->utf8));
		entry->utf8[ARRSIZE(entry->utf8) - 1] = 0;
    }
}

static void parse_entries(Font_Entry** entries, int* count, const json_t* entry_array_json) {
    *count = json_array_size(entry_array_json);
    if (*count == 0) {
        *entries = NULL;
    }
    else {
        *entries = malloc(sizeof(Font_Entry) * *count);
        for (int i = 0; i < *count; ++i) {
            parse_entry(*entries + i, json_array_get(entry_array_json, i));
        }
    }
}

static void parse_table(Font_Table* table, const json_t* table_json) {
    const char* name = json_string_value(json_object_get(table_json, "name"));
    if (name == NULL) {
        *table->name = 0;
    }
    else {
        strncpy(table->name, name, ARRSIZE(table->name));
        table->name[ARRSIZE(table->name) - 1] = 0;
    }

    parse_entries(
        (Font_Entry**)&table->table_wide,
        &table->table_wide_size,
        json_object_get(table_json, "wide")
    );

    parse_entries(
        (Font_Entry**)&table->table_narrow,
        &table->table_narrow_size,
        json_object_get(table_json, "narrow")
    );
}

static void parse_table_array(Font_Context* context, const json_t* table_array_json) {
    context->table_count = json_array_size(table_array_json);
    if (context->table_count == 0) {
        context->tables = NULL;
    }
    else {
        context->tables = malloc(sizeof(Font_Table) * context->table_count);
        for (int i = 0; i < context->table_count; ++i) {
            parse_table(context->tables + i, json_array_get(table_array_json, i));
        }
    }
}

/*
 * Exported functions
 */

const Font_Table* font_table_select(const Font_Context* context, const char name[]) {
    for (int i = 0; i < context->table_count; ++i) {
        const Font_Table* table = context->tables + i;
        if (strstr(name, table->name) != NULL) {
            return table;
        }
    }

    return NULL;
}

void font_stub_encode(char output[], int size, int code, const Font_Table* table, Font_Width width) {
    do {
        if (table == NULL) {
            break;
        }

        const Font_Entry* entries = NULL;
        int count = 0;

        switch (width) {
            case FONT_WIDTH_WIDE:
                entries = table->table_wide;
                count = table->table_wide_size;
                break;
            case FONT_WIDTH_NARROW:
                entries = table->table_narrow;
                count = table->table_narrow_size;
                break;
        }

        assert(entries != NULL);

        for (int i = 0; i < count; ++i) {
            const Font_Entry* entry = entries + i;
            if (entry->code == code) {
                encode_sequence(output, size, entry->utf8);
                return;
            }
        }

    }
    while (0);

    /* unable to decode, use output the unknown character symbol */
    encode_sequence(output, size, "\xef\xbf\xbd");
}

void font_stub_decode(char output[], int size, const char input[]) {
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

bool font_context_init(Font_Context* context, const char path[]) {
    context->table_count = 0;
    context->tables = NULL;

    if (path == NULL) {
        return true;
    }

    json_t* table_array_json = json_load_file(path, 0, NULL);
    if (table_array_json == NULL) {
        fprintf(stderr, "Failed to load font file %s\n", path);
        return false;
    }

    parse_table_array(context, table_array_json);
    json_decref(table_array_json);
    return true;
}

void font_context_destroy(Font_Context* context) {
    for (int i = 0; i < context->table_count; ++i) {
        Font_Table* table = context->tables + i;
        free((Font_Entry*)table->table_wide);
        free((Font_Entry*)table->table_narrow);
    }

    free(context->tables);

    context->tables = NULL;
    context->table_count = 0;
}
