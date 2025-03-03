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

/* sagan-esmtp.h  */

#ifdef HAVE_LIBESMTP


#define ESMTPTO         32            /* 'To' buffer size max */
#define ESMTPFROM       32            /* 'From' buffer size max */
#define ESMTPSERVER     32            /* SMTP server size max */
#define MAX_EMAILSIZE   15360          /* Largest e-mail that can be sent */

const char *esmtp_cb ( void **, int *, void * );
int ESMTP_Thread( _Sagan_Event * );
int FixLF(_SaganConfig *, char *, char *);

#endif

