/*
 * Copyright (C) 2016  Alex Yatskov <alex@foosoft.net>
 * Author: Alex Yatskov <alex@foosoft.net>
 * Data based on eplkup by Christopher Brochtrup
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

#ifndef GAIJI_TABLE_SNMKG99_H
#define GAIJI_TABLE_SNMKG99_H

#include "../gaiji.h"

static const Gaiji_Entry gaiji_table_snmkg99_narrow[] = { };

static const Gaiji_Entry gaiji_table_snmkg99_wide[] = {
    { 0xB467, { 0xE2, 0x87, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }, /* ⇒ cb4960 */
    { 0xB468, { 0xE2, 0x87, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }, /* ⇔ cb4960 */
    { 0xB55F, { 0xE2, 0x81, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }, /* ⁎ cb4960 */
    { 0xB560, { 0xE2, 0x81, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }, /* ⁑ cb4960 */
};

#endif /* GAIJI_TABLE_SNMKG99_H */
