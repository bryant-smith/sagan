/* $Id$ */
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

/* sagan.h
 *
 * Sagan prototypes and definitions.
 *
 */

#include <syslog.h>

#if defined HAVE_LIBLOGNORM || defined WITH_BLUEDOT
#include <json.h>
#else
typedef void json_object;
#define json_object_to_json_string_ext(x, y) "{}"
#endif

#define PCRE_OVECCOUNT		 30

/* Various buffers used during configurations loading */

#define SENSOR_NAME		"default_sensor_name"
#define CLUSTER_NAME		"default_cluster_name"
#define MMAP_VERSION		2.0

#define USER_AGENT		"User-Agent: Sagan-Engine"

#define CLASSBUF		256
#define RULEBUF			5128
#define CONFBUF			65536

#define	MAX_SYSLOG_HOST		50
#define MAX_SYSLOG_FACILITY	15
#define MAX_SYSLOG_PRIORITY	20
#define MAX_SYSLOG_LEVEL	15
#define MAX_SYSLOG_TAG		64
#define MAX_SYSLOG_DATE		35
#define MAX_SYSLOG_TIME		35
#define MAX_SYSLOG_PROGRAM	50

#define MAX_SYSLOGMSG		32768

#define MAX_EVENTID		32

/* This is used in JSON message/program parsing */

#define JSON_MAX_OBJECTS        256
#define JSON_MAX_KEY_SIZE       32


#define DEFAULT_JSON_INPUT_MAP          "/usr/local/etc/sagan-rules/json-input.map"
#define INPUT_PIPE                      1
#define INPUT_JSON                      2

/* In very high preformance (over 100k EPS),  you may want to considering raising
   the MAX_SYSLOG_BATCH and setting it in the sagan.yaml.  This allows Sagan
   to "batch" logs together to avoid expensive mutex_lock/mutex_unlock calls. */

#define MAX_SYSLOG_BATCH	100
#define DEFAULT_SYSLOG_BATCH	1

#define MAXPATH 		255		/* Max path for files/directories */
#define MAXHOST         	255		/* Max host length */

#define MAX_SAGAN_MSG		 256		/* Max "msg" option size */

#define MAX_PCRE_SIZE		 1024		/* Max pcre length in a rule */
#define MAX_FIFO_SIZE		1048576		/* Max pipe/FIFO size in bytes/pages */

#define MAX_THREADS     	4096            /* Max system threads */

#define MAX_VAR_NAME_SIZE  	64		/* Max "var" name size */
#define MAX_VAR_VALUE_SIZE 	131072		/* Max "var" value size */

#define MAX_PCRE		10		/* Max PCRE within a rule */
#define MAX_CONTENT		30		/* Max 'content' within a rule */
#define MAX_EVENT_ID		25		/* Max 'event_id' within a rule */

#define MAX_META_CONTENT	5		/* Max 'meta_content' within a rule */
#define MAX_META_CONTENT_ITEMS	256		/* Max strings to look for in meta_content */
#define MAX_META_ITEM_SIZE	512		/* Max string size per meta_content type */

#define MAX_JSON_CONTENT        30              /* Max JSON content within a rule */
#define MAX_JSON_PCRE		10		/* Max JSON pcre within a rule */
#define MAX_JSON_META_CONTENT   10              /* Max JSON meta_content within a rule */
#define MAX_JSON_META_ITEM_SIZE 512
#define MAX_JSON_META_CONTENT_ITEMS 256
#define	MAX_JSON_MAP		20

#define MAX_FLEXBITS		10		/* Max 'flexbits' within a rule */
#define MAX_XBITS               10              /* Max 'xbits' within a rule */

#define MAX_FLOW_SIZE		32768
#define MAX_CHECK_FLOWS		512		/* Max amount of IP addresses to be checked in a flow */

#define MAX_REFERENCE		10		/* Max references within a rule */
#define MAX_PARSE_IP		30		/* Max IP to collect form log line via parse.c */

/* TODO: These need to be labeled better! These directly affect
   functions like is_notroutable(). Think before you alter */

#define MAXIP			64		/* Max IP length */
#define MAXIPBIT	     	16		/* Max IP length in bytes */

#define LOCKFILE 		"/var/run/sagan/sagan.pid"
#define SAGANLOG		"/var/log/sagan/sagan.log"
#define SAGANLOGPATH		"/var/log/sagan"
#define FIFO			"/var/run/sagan.fifo"
#define RULE_PATH		"/usr/local/etc/sagan-rules"

#define HOME_NET		"any"
#define EXTERNAL_NET		"any"

#define RUNAS			"sagan"

#define PLOG_INTERFACE		"eth0"
#define PLOG_FILTER		"port 514"
#define PLOG_LOGDEV		"/dev/log"

#define TRACK_TIME		1440

#define NORMAL			0
#define ERROR			1
#define WARN			2
#define DEBUG			3

#define DEFAULT_SYSLOG_FACILITY	LOG_AUTH
#define DEFAULT_SYSLOG_PRIORITY LOG_ALERT

#define IPv4	4
#define IPv6	6

#define DEFAULT_SMTP_SUBJECT 	"[Sagan]"

/* defaults if the user doesn't define */

#define MAX_PROCESSOR_THREADS   100

#define SUNDAY			1
#define MONDAY			2
#define TUESDAY			4
#define WEDNESDAY		8
#define THURSDAY		16
#define FRIDAY			32
#define SATURDAY		64

/* This is for loading/reloading Sagan log files */

#define OPEN			0
#define REOPEN			1

#define SAGAN_LOG		0
#define ALERT_LOG		1
#define ALL_LOGS		100

#define MD5_HASH_SIZE		32
#define SHA1_HASH_SIZE		40
#define SHA256_HASH_SIZE	64

#define MAX_FILENAME_SIZE	256
#define MAX_URL_SIZE		8192
#define MAX_USERNAME_SIZE	64
#define MAX_HOSTNAME_SIZE	255
#define MAX_EVENT_ID_SIZE	32

/* Locations of IPC/Share memory "files" */

#define IPC_DIRECTORY			"/dev/shm"

#define COUNTERS_IPC_FILE 		"sagan-counters.shared"
#define FLEXBIT_IPC_FILE 	        "sagan-flexbits.shared"
#define XBIT_IPC_FILE			"sagan-xbits.shared"
#define THRESH_BY_SRC_IPC_FILE 		"sagan-thresh-by-source.shared"
#define THRESH_BY_DST_IPC_FILE 		"sagan-thresh-by-destination.shared"
#define THRESH_BY_DSTPORT_IPC_FILE 	"sagan-thresh-by-destination-port.shared"
#define THRESH_BY_SRCPORT_IPC_FILE 	"sagan-thresh-by-source-port.shared"
#define THRESH_BY_USERNAME_IPC_FILE 	"sagan-thresh-by-username.shared"

#define AFTER2_IPC_FILE			"sagan-after2.shared"
#define THRESHOLD2_IPC_FILE             "sagan-threshold2.shared"
#define CLIENT_TRACK_IPC_FILE 		"sagan-track-clients.shared"

/* Default IPC/mmap sizes */

#define DEFAULT_IPC_CLIENT_TRACK_IPC	5000
#define DEFAULT_IPC_AFTER2_IPC		10000
#define DEFAULT_IPC_THRESHOLD2_IPC      10000
#define DEFAULT_IPC_FLEXBITS		10000
#define DEFAULT_IPC_XBITS		10000

#define	AFTER2				0
#define THRESHOLD2			1
#define FLEXBIT				2
#define XBIT				3

#define RULE_TYPE_ALERT			1	/* "alert" */
#define RULE_TYPE_DROP			2	/* "drop" */
#define RULE_TYPE_PASS			3	/* "pass" */

#define PARSE_HASH_MD5			1
#define	PARSE_HASH_SHA1			2
#define PARSE_HASH_SHA256		3
#define PARSE_HASH_ALL			4

#define NORMAL_RULE			0
#define DYNAMIC_RULE			1

#define XBIT_STORAGE_MMAP		0
#define XBIT_STORAGE_REDIS		1

#define	THREAD_NAME_LEN			16

#ifdef HAVE_LIBFASTJSON
#define MAX_METADATA			16
#endif

/* Outside WITH_BLUEDOT because used in arg passing */

#define BLUEDOT_JSON_SIZE               2048

/* For JSON input */

#if defined(HAVE_LIBFASTJSON)

#define MAX_JSON_DECODE_BASE64		5

#define JSON_INPUT_S_SOURCE             32
#define JSON_INPUT_S_SOURCE_MAX_SIZE    160
#define JSON_INPUT_S_SOURCE_MAX_COUNT   5

#define JSON_INPUT_PROGRAM		32
#define JSON_INPUT_PROGRAM_MAX_SIZE	160
#define JSON_INPUT_PROGRAM_MAX_COUNT	5

#define JSON_INPUT_MESSAGE		32
#define JSON_INPUT_MESSAGE_MAX_SIZE	160
#define JSON_INPUT_MESSAGE_MAX_COUNT	5

#define JSON_INPUT_FACILITY		32
#define JSON_INPUT_FACILITY_MAX_SIZE	160
#define JSON_INPUT_FACILITY_MAX_COUNT	5

#define JSON_INPUT_LEVEL		32
#define JSON_INPUT_LEVEL_MAX_SIZE	160
#define JSON_INPUT_LEVEL_MAX_COUNT	5

#define JSON_INPUT_PRIORITY		32
#define JSON_INPUT_PRIORITY_MAX_SIZE	160
#define JSON_INPUT_PRIORITY_MAX_COUNT	5

#define JSON_INPUT_TAG			64
#define JSON_INPUT_TAG_MAX_SIZE		160
#define JSON_INPUT_TAG_MAX_COUNT	5

#define JSON_INPUT_USERNAME		32
#define JSON_INPUT_USERNAME_MAX_SIZE	160
#define JSON_INPUT_USERNAME_MAX_COUNT	5

#define JSON_INPUT_TIME			32
#define JSON_INPUT_TIME_MAX_SIZE	160
#define JSON_INPUT_TIME_MAX_COUNT	5

#define JSON_INPUT_DATE			32
#define JSON_INPUT_DATE_MAX_SIZE	160
#define JSON_INPUT_DATE_MAX_COUNT	5

#define JSON_INPUT_SRCIP		32
#define JSON_INPUT_SRCIP_MAX_SIZE	160
#define JSON_INPUT_SRCIP_MAX_COUNT	5

#define JSON_INPUT_DSTIP		32
#define JSON_INPUT_DSTIP_MAX_SIZE	160
#define JSON_INPUT_DSTIP_MAX_COUNT	5

#define JSON_INPUT_SRCPORT		32
#define JSON_INPUT_SRCPORT_MAX_SIZE	160
#define JSON_INPUT_SRCPORT_MAX_COUNT	5

#define JSON_INPUT_DSTPORT		32
#define JSON_INPUT_DSTPORT_MAX_SIZE	160
#define JSON_INPUT_DSTPORT_MAX_COUNT	5

#define JSON_INPUT_MD5			32
#define JSON_INPUT_MD5_MAX_SIZE		160
#define JSON_INPUT_MD5_MAX_COUNT	5

#define JSON_INPUT_SHA1			32
#define JSON_INPUT_SHA1_MAX_SIZE	160
#define JSON_INPUT_SHA1_MAX_COUNT	5

#define JSON_INPUT_SHA256		32
#define JSON_INPUT_SHA256_MAX_SIZE	160
#define JSON_INPUT_SHA256_MAX_COUNT	5

#define JSON_INPUT_FILENAME		32
#define JSON_INPUT_FILENAME_MAX_SIZE	160
#define JSON_INPUT_FILENAME_MAX_COUNT	5

#define JSON_INPUT_HOSTNAME		32
#define JSON_INPUT_HOSTNAME_MAX_SIZE	160
#define JSON_INPUT_HOSTNAME_MAX_COUNT	5

#define JSON_INPUT_URL			32
#define JSON_INPUT_URL_MAX_SIZE		160
#define JSON_INPUT_URL_MAX_COUNT	5

#define JSON_INPUT_JA3			32
#define JSON_INPUT_JA3_MAX_SIZE		160
#define JSON_INPUT_JA3_MAX_COUNT	5

#define JSON_INPUT_FLOW_ID		32
#define JSON_INPUT_FLOW_ID_MAX_SIZE	160
#define JSON_INPUT_FLOW_ID_MAX_COUNT	5

#define JSON_INPUT_EVENT_ID		32
#define JSON_INPUT_EVENT_ID_MAX_SIZE	160
#define JSON_INPUT_EVENT_ID_MAX_COUNT	5

#define JSON_INPUT_PROTO		32
#define JSON_INPUT_PROTO_MAX_SIZE	160
#define JSON_INPUT_PROTO_MAX_COUNT	5

#endif
