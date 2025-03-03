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

/* alert.c
 *
 * Provides logging functionality in a 'snort like' format.  Usually in
 * the /var/log/sagan directory named 'alert'
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "sagan.h"

#include "alert.h"
#include "util-time.h"
#include "rules.h"
#include "references.h"
#include "sagan-config.h"

extern struct _Rule_Struct *rulestruct;
extern struct _SaganConfig *config;
extern struct _SaganCounters *counters;

void Alert_File( _Sagan_Event *Event )
{

    char tmpref[256];
    char timebuf[64];

    FILE *sagan_alert_stream;
    int sagan_alert_stream_int = 0;

    CreateTimeString(&Event->event_time, timebuf, sizeof(timebuf), 1);

    if (( sagan_alert_stream = fopen( config->sagan_alert_filepath, "a" )) == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Cannot open %s (%s). Abort", __FILE__, __LINE__, config->sagan_alert_filepath, strerror(errno));
        }

    sagan_alert_stream_int = fileno( sagan_alert_stream );

    File_Lock( sagan_alert_stream_int );

    fprintf(sagan_alert_stream, "\n[**] [1:%" PRIu64 ":%" PRIuFAST16 "] %s [**]\n", Event->sid, Event->rev, Event->f_msg);
    fprintf(sagan_alert_stream, "[Classification: %s] [Priority: %d] [%s]\n", Event->class, Event->pri, Event->host );
    fprintf(sagan_alert_stream, "[Alert Time: %s]\n", timebuf);

    fprintf(sagan_alert_stream, "%s ", Event->date);
    fprintf(sagan_alert_stream, "%s ", Event->time);
    fprintf(sagan_alert_stream, "%s:", Event->ip_src);
    fprintf(sagan_alert_stream, "%" PRIuFAST16 " ", Event->src_port);

    fprintf(sagan_alert_stream, " -> ");

    fprintf(sagan_alert_stream, "%s:", Event->ip_dst );
    fprintf(sagan_alert_stream, "%" PRIuFAST16 " ", Event->dst_port);

    fprintf(sagan_alert_stream, "%s ", Event->facility);
    fprintf(sagan_alert_stream, "%s ", Event->priority);
    fprintf(sagan_alert_stream, "%s", Event->program);

    fprintf(sagan_alert_stream, "\n");

    fprintf(sagan_alert_stream, "Message: %s\n", Event->message);

    if ( Event->rule_position != 0 )
        {

            Reference_Lookup( Event->rule_position, 0, tmpref, sizeof(tmpref) );

            if (strcmp(tmpref, "" ))
                {
                    fprintf(sagan_alert_stream, "%s\n", tmpref);
                }
        }

    File_Unlock( sagan_alert_stream_int );
    fclose(sagan_alert_stream);

}
