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

/* zeek-intel.c (Formally "Bro")
*
* This allows Sagan to read in Zeek Intel files,  like those from Critical
* Stack (https://intel.brointel.com).
*
*/

/* TODO:  needs stats */


#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>


#include "sagan.h"
#include "sagan-defs.h"
#include "sagan-config.h"

#include "parsers/parsers.h"

#include "processors/zeek-intel.h"

#define MAX_BROINTEL_LINE_SIZE 10240

extern struct _SaganConfig *config;
extern struct _SaganCounters *counters;
extern struct _SaganDebug *debug;

struct _ZeekIntel_Intel_Addr *ZeekIntel_Intel_Addr;
struct _ZeekIntel_Intel_Domain *ZeekIntel_Intel_Domain;
struct _ZeekIntel_Intel_File_Hash *ZeekIntel_Intel_File_Hash;
struct _ZeekIntel_Intel_URL *ZeekIntel_Intel_URL;
struct _ZeekIntel_Intel_Software *ZeekIntel_Intel_Software;
struct _ZeekIntel_Intel_Email *ZeekIntel_Intel_Email;
struct _ZeekIntel_Intel_User_Name *ZeekIntel_Intel_User_Name;
struct _ZeekIntel_Intel_File_Name *ZeekIntel_Intel_File_Name;
struct _ZeekIntel_Intel_Cert_Hash *ZeekIntel_Intel_Cert_Hash;

pthread_mutex_t CounterBroIntelGenericMutex=PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************
 * ZeekIntel_Load_File - Loads BroIntel data and splits it up
 * into different arrays.
 * ***************************************************************************/

void ZeekIntel_Load_File ( void )
{

    FILE *zeekintel_file;

    char *value;
    char *type;
    char *description;

    bool found_flag;
    bool found_flag_array;

    char *tok = NULL; ;
    char *ptmp = NULL;

    uint_fast32_t line_count;
    uint_fast32_t i;

    unsigned char bits_ip[MAXIPBIT] = {0};

    char *zeekintel_filename = NULL;
    char brointelbuf[MAX_BROINTEL_LINE_SIZE] = { 0 };

    __atomic_store_n (&counters->zeekintel_dups, 0, __ATOMIC_SEQ_CST);

    zeekintel_filename = strtok_r(config->zeekintel_files, ",", &ptmp);

    while ( zeekintel_filename != NULL )
        {

            Sagan_Log(NORMAL, "Bro Intel Processor Loading File: %s.", zeekintel_filename);

            if (( zeekintel_file = fopen(zeekintel_filename, "r")) == NULL )
                {
                    Sagan_Log(ERROR, "[%s, line %d] Could not load Bro Intel file! (%s - %s)", __FILE__, __LINE__, zeekintel_filename, strerror(errno));
                }

            while(fgets(brointelbuf, MAX_BROINTEL_LINE_SIZE, zeekintel_file) != NULL)
                {

                    /* Skip comments and blank linkes */

                    if (brointelbuf[0] == '#' || brointelbuf[0] == 10 || brointelbuf[0] == ';' || brointelbuf[0] == 32 )
                        {
                            line_count++;
                            continue;
                        }
                    else
                        {

                            Remove_Return(brointelbuf);

                            value = strtok_r(brointelbuf, "\t", &tok);
                            type = strtok_r(NULL, "\t", &tok);
                            description = strtok_r(NULL, "\t", &tok);

                            if ( value == NULL || type == NULL || description == NULL )
                                {
                                    Sagan_Log(WARN, "[%s, line %d] Got invalid line at %d in %s", __FILE__, __LINE__, line_count, zeekintel_filename);
                                }

                            found_flag = 0;

                            if (!strcmp(type, "Intel::ADDR") && IP2Bit(value, bits_ip))
                                {

                                    found_flag = 1; 			/* Used to short circuit other 'type' lookups */
                                    found_flag_array = 0;		/* Used to short circuit/warn when dups are found.  This way we don't waste memory/CPU */

                                    /* Check for duplicates. */

                                    for (i=0; i < counters->zeekintel_addr_count; i++)
                                        {

                                            if ( !memcmp(ZeekIntel_Intel_Addr[i].bits_ip, bits_ip, MAXIPBIT) )
                                                {
                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::ADDR address %s in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);
                                                    __atomic_add_fetch(& counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {

                                            ZeekIntel_Intel_Addr = (_ZeekIntel_Intel_Addr *) realloc(ZeekIntel_Intel_Addr, (counters->zeekintel_addr_count+1) * sizeof(_ZeekIntel_Intel_Addr));

                                            if ( ZeekIntel_Intel_Addr == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_Addr. Abort!", __FILE__, __LINE__);
                                                }


                                            pthread_mutex_lock(&CounterBroIntelGenericMutex);
                                            memcpy( ZeekIntel_Intel_Addr[counters->zeekintel_addr_count].bits_ip, bits_ip, sizeof(bits_ip) );
                                            counters->zeekintel_addr_count++;
                                            pthread_mutex_unlock(&CounterBroIntelGenericMutex);
                                        }

                                }

                            if (!strcmp(type, "Intel::DOMAIN") && found_flag == 0)
                                {
                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;


                                    for (i=0; i < counters-> zeekintel_domain_count; i++)
                                        {
                                            if (!strcasecmp(ZeekIntel_Intel_Domain[i].domain, value))
                                                {
                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::DOMAIN '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);
                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {
                                            ZeekIntel_Intel_Domain = (_ZeekIntel_Intel_Domain *) realloc(ZeekIntel_Intel_Domain, (counters->zeekintel_domain_count+1) * sizeof(_ZeekIntel_Intel_Domain));

                                            if ( ZeekIntel_Intel_Domain == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_Domain. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_Domain[counters->zeekintel_domain_count].domain, value, sizeof(ZeekIntel_Intel_Domain[counters->zeekintel_domain_count].domain));

                                            __atomic_add_fetch(&counters->zeekintel_domain_count, 1, __ATOMIC_SEQ_CST);


                                        }

                                }

                            if (!strcmp(type, "Intel::FILE_HASH") && found_flag == 0)
                                {
                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_file_hash_count; i++)
                                        {

                                            if (!strcasecmp(ZeekIntel_Intel_File_Hash[i].hash, value))
                                                {

                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::FILE_HASH '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);

                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {

                                            ZeekIntel_Intel_File_Hash = (_ZeekIntel_Intel_File_Hash *) realloc(ZeekIntel_Intel_File_Hash, (counters->zeekintel_file_hash_count+1) * sizeof(_ZeekIntel_Intel_File_Hash));

                                            if ( ZeekIntel_Intel_File_Hash == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_File_Hash. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_File_Hash[counters->zeekintel_file_hash_count].hash, value, sizeof(ZeekIntel_Intel_File_Hash[counters->zeekintel_file_hash_count].hash));
                                            counters->zeekintel_file_hash_count++;

                                        }
                                }


                            if (!strcmp(type, "Intel::URL") && found_flag == 0)
                                {

                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_url_count; i++)
                                        {
                                            if (!strcasecmp(ZeekIntel_Intel_URL[i].url, value))
                                                {
                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::URL '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);
                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }


                                    if ( found_flag_array == 0 )
                                        {

                                            ZeekIntel_Intel_URL = (_ZeekIntel_Intel_URL *) realloc(ZeekIntel_Intel_URL, (counters->zeekintel_url_count+1) * sizeof(_ZeekIntel_Intel_URL));

                                            if ( ZeekIntel_Intel_URL == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_URL. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_URL[counters->zeekintel_url_count].url, value, sizeof(ZeekIntel_Intel_URL[counters->zeekintel_url_count].url));

                                            __atomic_add_fetch(&counters->zeekintel_url_count, 1, __ATOMIC_SEQ_CST);

                                        }

                                }


                            if (!strcmp(type, "Intel::SOFTWARE") && found_flag == 0)
                                {


                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_software_count++; i++)
                                        {

                                            if (!strcasecmp(ZeekIntel_Intel_Software[i].software, value))
                                                {

                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::SOFTWARE '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);
                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {

                                            ZeekIntel_Intel_Software = (_ZeekIntel_Intel_Software *) realloc(ZeekIntel_Intel_Software, (counters->zeekintel_software_count+1) * sizeof(_ZeekIntel_Intel_Software));

                                            if ( ZeekIntel_Intel_Software == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_Software. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_Software[counters->zeekintel_software_count].software, value, sizeof(ZeekIntel_Intel_Software[counters->zeekintel_software_count].software));

                                            __atomic_add_fetch(&counters->zeekintel_software_count, 1, __ATOMIC_SEQ_CST);


                                        }
                                }

                            if (!strcmp(type, "Intel::EMAIL") && found_flag == 0)
                                {

                                    To_LowerC(value);

                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_email_count; i++)
                                        {
                                            if (!strcasecmp(ZeekIntel_Intel_Email[i].email, value))
                                                {
                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::EMAIL '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);

                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;

                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {

                                            ZeekIntel_Intel_Email = (_ZeekIntel_Intel_Email *) realloc(ZeekIntel_Intel_Email, (counters->zeekintel_email_count+1) * sizeof(_ZeekIntel_Intel_Email));

                                            if ( ZeekIntel_Intel_Email == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_Email. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_Email[counters->zeekintel_email_count].email, value, sizeof(ZeekIntel_Intel_Email[counters->zeekintel_email_count].email));

                                            __atomic_add_fetch(&counters->zeekintel_email_count, 1, __ATOMIC_SEQ_CST);

                                            found_flag = 1;
                                        }

                                }


                            if (!strcmp(type, "Intel::USER_NAME") && found_flag == 0)
                                {
                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_user_name_count; i++)
                                        {
                                            if (!strcasecmp(ZeekIntel_Intel_User_Name[i].username, value))
                                                {
                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::USER_NAME '%s' in %s on line %.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);

                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {


                                            ZeekIntel_Intel_User_Name = (_ZeekIntel_Intel_User_Name *) realloc(ZeekIntel_Intel_User_Name, (counters->zeekintel_user_name_count+1) * sizeof(_ZeekIntel_Intel_User_Name));

                                            if ( ZeekIntel_Intel_User_Name == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_User_Name. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_User_Name[counters->zeekintel_user_name_count].username, value, sizeof(ZeekIntel_Intel_User_Name[counters->zeekintel_user_name_count].username));

                                            __atomic_add_fetch(&counters->zeekintel_user_name_count, 1, __ATOMIC_SEQ_CST);

                                        }
                                }

                            if (!strcmp(type, "Intel::FILE_NAME") && found_flag == 0)
                                {

                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_file_name_count; i++)
                                        {

                                            if (!strcasecmp(ZeekIntel_Intel_File_Name[i].file_name, value))
                                                {

                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::FILE_NAME '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);

                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }


                                    if ( found_flag_array == 0 )
                                        {

                                            ZeekIntel_Intel_File_Name = (_ZeekIntel_Intel_File_Name *) realloc(ZeekIntel_Intel_File_Name, (counters->zeekintel_file_name_count+1) * sizeof(_ZeekIntel_Intel_File_Name));

                                            if ( ZeekIntel_Intel_File_Name == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_File_Name. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_File_Name[counters->zeekintel_file_name_count].file_name, value, sizeof(ZeekIntel_Intel_File_Name[counters->zeekintel_file_name_count].file_name));

                                            __atomic_add_fetch(&counters->zeekintel_file_name_count, 1, __ATOMIC_SEQ_CST);

                                        }

                                }

                            if (!strcmp(type, "Intel::CERT_HASH") && found_flag == 0)
                                {
                                    To_LowerC(value);

                                    found_flag = 1;
                                    found_flag_array = 0;

                                    for (i=0; i < counters->zeekintel_cert_hash_count; i++)
                                        {
                                            if (!strcasecmp(ZeekIntel_Intel_Cert_Hash[i].cert_hash, value))
                                                {
                                                    Sagan_Log(WARN, "[%s, line %d] Got duplicate Intel::CERT_HASH '%s' in %s on line %d.", __FILE__, __LINE__, value, zeekintel_filename, line_count + 1);
                                                    __atomic_add_fetch(&counters->zeekintel_dups, 1, __ATOMIC_SEQ_CST);

                                                    found_flag_array = 1;
                                                }
                                        }

                                    if ( found_flag_array == 0 )
                                        {
                                            ZeekIntel_Intel_Cert_Hash = (_ZeekIntel_Intel_Cert_Hash *) realloc(ZeekIntel_Intel_Cert_Hash, (counters->zeekintel_cert_hash_count+1) * sizeof(_ZeekIntel_Intel_Cert_Hash));

                                            if ( ZeekIntel_Intel_Cert_Hash == NULL )
                                                {
                                                    Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for ZeekIntel_Intel_Cert_Hash. Abort!", __FILE__, __LINE__);
                                                }


                                            strlcpy(ZeekIntel_Intel_Cert_Hash[counters->zeekintel_cert_hash_count].cert_hash, value, sizeof(ZeekIntel_Intel_Cert_Hash[counters->zeekintel_cert_hash_count].cert_hash));

                                            __atomic_add_fetch(&counters->zeekintel_cert_hash_count, 1, __ATOMIC_SEQ_CST);

                                        }
                                }


                        }

                    line_count++;

                }
            fclose(zeekintel_file);
            zeekintel_filename = strtok_r(NULL, ",", &ptmp);
            line_count = 0;
        }

}

/*****************************************************************************
 * ZeekIntel_IPADDR - Search array for blacklisted IP addresses
 *****************************************************************************/

bool ZeekIntel_IPADDR ( unsigned char *ip, const char *ipaddr )
{

    uint_fast32_t i = 0;

    unsigned char ip_convert[MAXIPBIT] = { 0 };
    memset(ip_convert, 0, MAXIPBIT);
    memcpy(ip_convert, ip, MAXIPBIT);

    /* If RFC1918 and friends,  we can short circuit here */

    if ( is_notroutable(ip) )
        {

            if ( debug->debugbrointel )
                {
                    Sagan_Log(DEBUG, "[%s, line %d] %s is RFC1918, link local or invalid.", __FILE__, __LINE__, ipaddr );
                }

            return(false);
        }

    /* Search array for for the IP address */

    for ( i = 0; i < counters->zeekintel_addr_count; i++)
        {

            if ( !memcmp(ip_convert, ZeekIntel_Intel_Addr[i].bits_ip, MAXIPBIT) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found IP %u.", __FILE__, __LINE__, ip);
                        }

                    return(true);
                }

        }

    return(false);

}

/*****************************************************************************
 * ZeekIntel_IPADDR_All - Search and tests _all_ IP addresses within
 * a syslog_message (reguardless of lognorm/parse ip)!
 *****************************************************************************/

bool ZeekIntel_IPADDR_All ( const char *syslog_message, struct _Sagan_Lookup_Cache_Entry *lookup_cache, uint_fast8_t lookup_cache_size )
{

    uint_fast8_t i;
    uint_fast32_t b;

    for (i = 0; i < lookup_cache_size; i++)
        {


            if ( lookup_cache[i].status == 0 )
                {
                    return(false);
                }

            for ( b = 0; b < counters->zeekintel_addr_count; b++ )
                {

                    if ( !memcmp(ZeekIntel_Intel_Addr[b].bits_ip, lookup_cache[i].ip_bits, sizeof(ZeekIntel_Intel_Addr[b].bits_ip)))
                        {
                            return(true);
                        }
                }
        }

    return(false);
}

/*****************************************************************************
 * ZeekIntel_DOMAIN - Search DOMAIN array
 *****************************************************************************/

bool ZeekIntel_DOMAIN ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_domain_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_Domain[i].domain, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found domain %s.", __FILE__, __LINE__, ZeekIntel_Intel_Domain[i].domain);
                        }

                    return(true);
                }

        }

    return(false);

}

/*****************************************************************************
 * ZeekIntel_FILE_HASH - Search FILE_HASH array
 *****************************************************************************/

bool ZeekIntel_FILE_HASH ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_file_hash_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_File_Hash[i].hash, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found file hash %s.", __FILE__, __LINE__, ZeekIntel_Intel_File_Hash[i].hash);
                        }

                    return(true);
                }

        }

    return(false);

}

/*****************************************************************************
 * ZeekIntel_URL - Search URL array
 *****************************************************************************/

bool ZeekIntel_URL ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_url_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_URL[i].url, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found URL \"%s\".", __FILE__, __LINE__, ZeekIntel_Intel_URL[i].url);
                        }

                    return(true);
                }

        }

    return(false);
}

/*****************************************************************************
 * ZeekIntel_SOFTWARE - Search SOFTWARE array
 ****************************************************************************/

bool ZeekIntel_SOFTWARE ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_software_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_Software[i].software, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found software \"%s\".", __FILE__, __LINE__, ZeekIntel_Intel_Software[i].software);
                        }

                    return(true);
                }

        }

    return(false);
}

/*****************************************************************************
 * ZeekIntel_EMAIL - Search EMAIL array
 *****************************************************************************/

bool ZeekIntel_EMAIL ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_email_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_Email[i].email, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found e-mail address \"%s\".", __FILE__, __LINE__, ZeekIntel_Intel_Email[i].email);
                        }

                    return(true);
                }

        }

    return(false);
}

/*****************************************************************************
 * ZeekIntel_USER_NAME - Search USER_NAME array
 ****************************************************************************/

bool ZeekIntel_USER_NAME ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_user_name_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_User_Name[i].username, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found the username \"%s\".", __FILE__, __LINE__, ZeekIntel_Intel_User_Name[i].username);
                        }

                    return(true);
                }

        }

    return(false);
}

/****************************************************************************
 * ZeekIntel_FILE_NAME - Search FILE_NAME array
 ****************************************************************************/

bool ZeekIntel_FILE_NAME ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_file_name_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_File_Name[i].file_name, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found the file name \"%s\".", __FILE__, __LINE__, ZeekIntel_Intel_File_Name[i].file_name);
                        }

                    return(true);
                }

        }

    return(false);
}

/***************************************************************************
 * ZeekIntel_CERT_HASH - Search CERT_HASH array
 ***************************************************************************/

bool ZeekIntel_CERT_HASH ( const char *syslog_message )
{

    uint_fast32_t i;

    for ( i = 0; i < counters->zeekintel_cert_hash_count; i++)
        {

            if ( Sagan_stristr(syslog_message, ZeekIntel_Intel_Cert_Hash[i].cert_hash, false) )
                {
                    if ( debug->debugbrointel )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] Found the CERT_HASH \"%s\".", __FILE__, __LINE__, ZeekIntel_Intel_Cert_Hash[i].cert_hash);
                        }

                    return(true);
                }

        }

    return(false);
}

