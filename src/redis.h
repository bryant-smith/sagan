/*
** Copyright (C) 2009-2025 Quadrant Information Security <quadrantsec.com>
** Copyright (C) 2009-2025 Champ Clark III <cclark@quadrantsec.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_LIBHIREDIS

#include <hiredis/hiredis.h>

#define   MAX_REDIS_KEY_SIZE            128


void Redis_Reader_Connect ( void );
void Redis_Writer (void);
void Redis_Writer_Init (void);
void Redis_Reader ( const char *redis_command, char *str, size_t size );

typedef struct _Sagan_Redis_Write _Sagan_Redis_Write;
struct _Sagan_Redis_Write
{
    char command[16];
    char *key;
    char *value;
    uint_fast32_t expire;
};

#endif
