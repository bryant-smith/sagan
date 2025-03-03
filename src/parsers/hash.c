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

/*
 * hash.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "sagan.h"
#include "sagan-defs.h"
#include "version.h"
#include "sagan-config.h"
#include "parsers/parsers.h"

extern struct _SaganConfig *config;

void Parse_Hash(char *syslog_message, uint_fast8_t type, char *str, size_t size)
{

    char *ptmp=NULL;
    char *tok=NULL;
    char tmp[SHA256_HASH_SIZE+1];

    uint_fast32_t i;

    char *mod_string = malloc( config->message_buffer_size );

    if ( mod_string == NULL )
        {
            fprintf(stderr, "[%s, line %d] Fatal Error: Can't allocate memory! Abort!\n", __FILE__, __LINE__);
            exit(-1);
        }

    mod_string[0] = '\0';

    /* Remove anything we dont want */

    for (i=0; i<strlen(syslog_message); i++)
        {

            /* Remove everything.  Just want any hashes */

            if ( syslog_message[i] != '"' && syslog_message[i] != '(' && syslog_message[i] != ')' &&
                    syslog_message[i] != '[' && syslog_message[i] != ']' && syslog_message[i] != '<' &&
                    syslog_message[i] != '>' && syslog_message[i] != '{' && syslog_message[i] != '}' &&
                    syslog_message[i] != ',' && syslog_message[i] != '/' && syslog_message[i] != '@' &&
                    syslog_message[i] != '=' && syslog_message[i] != '-' && syslog_message[i] != '!' &&
                    syslog_message[i] != '|' && syslog_message[i] != '_' && syslog_message[i] != '+' &&
                    syslog_message[i] != '&' && syslog_message[i] != '%' && syslog_message[i] != '$' &&
                    syslog_message[i] != '~' && syslog_message[i] != '^' && syslog_message[i] != '\'' &&
                    syslog_message[i] != '.' )
                {

                    mod_string[i] = syslog_message[i];
                    mod_string[i+1] = '\0';

                }
            else
                {

                    mod_string[i] = ' ';
                    mod_string[i+1] = '\0';

                }

        }

    ptmp = strtok_r(mod_string, " ", &tok);

    while (ptmp != NULL )
        {

            Parse_Hash_Cleanup(ptmp, tmp, sizeof(tmp));

            if ( type == PARSE_HASH_MD5 || type == PARSE_HASH_ALL )
                {

                    if ( strlen(tmp) == MD5_HASH_SIZE )
                        {

                            if ( Validate_HEX(tmp) == true )
                                {
                                    snprintf(str, size, "%s", tmp);
                                    free(mod_string);
                                    return;
                                }
                        }

                }

            else if ( type == PARSE_HASH_SHA1 || type == PARSE_HASH_ALL )
                {
                    if ( strlen(tmp) == SHA1_HASH_SIZE )
                        {

                            if ( Validate_HEX(tmp) == true )
                                {
                                    snprintf(str, size, "%s", tmp);
                                    free(mod_string);
                                    return;
                                }
                        }
                }

            else if ( type == PARSE_HASH_SHA256 || type == PARSE_HASH_ALL )
                {

                    if ( strlen(tmp) == SHA256_HASH_SIZE )
                        {

                            if ( Validate_HEX(tmp) == true )
                                {
                                    snprintf(str, size, "%s", tmp);
                                    free(mod_string);
                                    return;
                                }
                        }
                }


            ptmp = strtok_r(NULL, " ", &tok);

        }

    tmp[0] = '\0';
    free(mod_string);
    snprintf(str, size, "%s", tmp);

}


void Parse_Hash_Cleanup(char *string, char *str, size_t size)
{

    char tmp[512];
    int i;
    char in[512] = { 0 };
    char tmp2[2];

    strlcpy(in, string, sizeof(in));

    int len = strlen(in);

    if ( ( in[strlen(in) - 1] ) == ',' || ( in[strlen(in) - 1] ) == '\'' )
        {
            strlcpy(tmp, in, len-1 );
            strlcpy(in, tmp, sizeof(in));
        }

    if ( in[0] == ',' || in[0] == '\'' || in[0] == ':' )
        {

            tmp[0] = '\0';

            for(i=1; i < strlen(in); i++)
                {
                    snprintf(tmp2, sizeof(tmp2), "%c", in[i]);
                    strcat(tmp, tmp2);
                }

            strlcpy(in, tmp, sizeof(in));
        }

    snprintf(str, size, "%s", in);

}

