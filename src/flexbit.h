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

uint_fast8_t Flexbit_Type ( const char *type, uint_fast32_t linecount, const char *ruleset );

bool Flexbit_Count( int rule_position, const char *ip_src_char, const char *ip_dst_char );
bool Flexbit_Condition(int rule_position, const char *ip_src_char, const char *ip_dst_char, int src_port, int dst_port, const char *username, _Sagan_Proc_Syslog *SaganProcSyslog_LOCAL );
void Flexbit_Set(int rule_position, const char *ip_src_char, const char *ip_dst_char, int src_port, int dst_port, const char *username, const char *syslog_message );

