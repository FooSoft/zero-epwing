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

/*
 * Macros
 */

#ifndef UTIL_H
#define UTIL_H

#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

enum {
    FLAG_PRETTY_PRINT = 1 << 0,
    FLAG_HOOK_MARKUP  = 1 << 1,
    FLAG_POSITIONS    = 1 << 2,
    FLAG_FONT_TAGS    = 1 << 3,
};

#endif /* UTIL_H */
