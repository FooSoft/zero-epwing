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

#include "eb/eb/eb.h"
#include "eb/eb/text.h"

#include "gaiji.h"

/*
 * Exported functions
 */

void gaiji_build_stub(char text[MAX_STUB_BYTES], int code, const Gaiji_context* context, Gaiji_width width) {
    sprintf(text, "!!!");
    (void)code;
    (void)text;
    (void)context;
    (void)width;
}

void gaiji_fixup_stub(char output[], int size, const char input[], const Gaiji_context* context) {
    (void)output;
    (void)size;
    (void)input;
    (void)context;
}
