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

/* output.c
*
* This becomes a threaded operation.  This handles all I/O intensive output plugins
*/

#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "sagan.h"
#include "geoip.h"
#include "output.h"
#include "rules.h"
#include "sagan-config.h"
#include "json-handler.h"

#include "output-plugins/alert.h"
#include "output-plugins/external.h"
#include "output-plugins/fast.h"
#include "output-plugins/eve.h"

#ifdef WITH_SYSLOG
#include "output-plugins/syslog-handler.h"
#endif

#ifdef HAVE_LIBESMTP
#include "output-plugins/esmtp.h"
#endif

extern struct _SaganCounters *counters;
extern struct _Rule_Struct *rulestruct;
extern struct _SaganConfig *config;

bool nonthread_alert_lock = false;

pthread_mutex_t SaganOutputNonThreadMutex=PTHREAD_MUTEX_INITIALIZER;

void Output( _Sagan_Event *Event )
{

    /******************************/
    /* Single threaded operations */
    /******************************/

    char *alert_data = malloc ( config->message_buffer_size );

    if ( alert_data == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Error allocating memory.", __FILE__, __LINE__);
        }

    alert_data[0] = '\0';

    Format_JSON_Alert_EVE( Event, alert_data, config->message_buffer_size );

    /* Single threaded */

    pthread_mutex_lock(&SaganOutputNonThreadMutex);
    nonthread_alert_lock = true;

    counters->alert_total++; 	/* If alert file isn't enabled, we still want to collect the stats */

    if ( config->alert_flag && rulestruct[Event->rule_position].xbit_noalert == false )
        {
            Alert_File(Event);
        }

    if ( config->eve_flag && config->eve_alerts )
        {

            if ( rulestruct[Event->rule_position].xbit_noeve == false && rulestruct[Event->rule_position].flexbit_noeve == false )
                {
                    Alert_JSON( alert_data );
                }
        }

    if ( config->fast_flag )
        {
            Fast_File(Event);
        }

    nonthread_alert_lock = false;
    pthread_mutex_unlock(&SaganOutputNonThreadMutex);

    /* End single threaded */

    /****************************************************************************/
    /* Syslog output                                                            */
    /****************************************************************************/

#ifdef WITH_SYSLOG

    if ( config->sagan_syslog_flag )
        {
            Alert_Syslog( Event );
        }

#endif

    /****************************************************************************/
    /* SMTP/Email support (libesmtp)                                            */
    /****************************************************************************/

#ifdef HAVE_LIBESMTP

    if ( config->sagan_esmtp_flag && rulestruct[Event->rule_position].email_flag )
        {
            ESMTP_Thread( Event );
        }

#endif

    /****************************************************************************/
    /* External program via rule                                                */
    /****************************************************************************/

    if (  rulestruct[Event->rule_position].external_flag )
        {
            External_Thread( alert_data, rulestruct[Event->rule_position].external_program );
        }

    free(alert_data);

}

