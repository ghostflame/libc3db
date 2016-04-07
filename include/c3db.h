#ifndef C3DB_HEADER_H
#define C3DB_HEADER_H

#define _GNU_SOURCE

#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#ifndef Err
#define Err				strerror( errno )
#endif

#ifndef Err3
#define Err3( h )		c3db_error( h )
#endif


// the first one is opaque
typedef struct c3db_handle		C3HDL;
typedef struct c3db_point		C3PNT;
typedef struct c3db_result_set	C3RES;


struct c3db_point
{
	int64_t				ts;
	float				val;
};

struct c3db_result_set
{
	int64_t				from;
	int64_t				to;
	int64_t				period;
	C3PNT			*	points;
	uint32_t			count;
	int					rtype;
};


enum metric_vals
{
	C3DB_REQ_INVLD = -1,
	C3DB_REQ_RAW = 0,
	C3DB_REQ_MEAN,
	C3DB_REQ_MAX,
	C3DB_REQ_MIN,
	C3DB_REQ_COUNT,
	C3DB_REQ_SUM,
	C3DB_REQ_SPREAD,
	C3DB_REQ_MIDDLE,
	C3DB_REQ_END
};


enum c3db_errnums
{
	C3E_SUCCESS = 0,
	C3E_BAD_HANDLE,
	C3E_SEE_ERRNO,
	C3E_BAD_COUNT,
	C3E_NULL_ARG,
	C3E_BAD_MAGIC,
	C3E_BAD_VERSION,
	C3E_BAD_STATE,
	C3E_READ_ONLY,
	C3E_BAD_RANGE,
	C3E_BAD_METRIC,
	C3E_BAD_RETAIN,
	C3E_BAD_FORMAT,
	C3E_HDR_READ_FAIL,
	C3E_MAX
};


enum c3db_ts_style
{
	C3DB_TS_INVLD = -1,
	C3DB_TS_SEC = 0,		// 1234567890
	C3DB_TS_TVAL,			// 1234567890.123456
	C3DB_TS_TSPC,			// 1234567890.123456789
	C3DB_TS_MSEC,			// 1234567890123
	C3DB_TS_USEC,			// 1234567890123456
	C3DB_TS_NSEC,			// 1234567890123456789 - native format
	C3DB_TS_MAX
};


// for your edification
#define C3DB_FILE_EXTN		"c3db"
#define C3DB_FILE_EXTN_LEN	4
#define C3DB_CURR_VERSION	1

#define C3DB_HANDLE_OK( h )	( c3db_status( h ) == C3E_SUCCESS )

#define C3DB_RO				0
#define C3DB_RW				1

// useful time conversion macros
#define tt_to_ns( _t, _n )		_n = ( (int64_t) _t ) * 1000000000
#define ns_to_tt( _n, _t )		_t = (time_t) ( _n / 1000000000 )

#define tt_to_us( _t, _u )		_u = ( (int64_t) _t ) * 1000000

#define tv_to_ns( _tv, _n )		_n = ( ( (int64_t) _tv.tv_sec ) * 1000000000 ) + ( (int64_t) _tv.tv_usec )
#define ns_to_tv( _n, _tv )		_tv.tv_sec = (time_t) ( _n / 1000000000 ); _tv.tv_usec = (long) ( ( _n % 1000000000 ) / 1000 )

#define ts_to_ns( _ts, _n )		_n = ( ( (int64_t) _ts.tv_sec ) * 1000000000 ) + ( (int64_t) _ts.tv_nsec )
#define ns_to_ts( _n, _ts )		_ts.tv_sec = (time_t) ( _n / 1000000000 ); _ts.tv_nsec = (long) ( _n % 1000000000 )

#define us_to_ns( _u, _n )		_n = _u * 1000
#define ns_to_us( _n, _u )		_u = _n / 1000

#define ms_to_ns( _m, _n )		_n = ( (int64_t) _m ) * 1000000
#define ns_to_ms( _n, _m )		_m = _m / 1000000



#define C3DB_RETAIN_FRAG		"[0-9]+[mMuUnN]?:[0-9]+[yYwWdDhHmMsSuUnN]"
#define C3DB_RETAIN_PATN		"^(" C3DB_RETAIN_FRAG ";)*" C3DB_RETAIN_FRAG "$"



// init/shutdown
C3HDL *c3db_open( char *path, int rw );
C3HDL *c3db_create( char *path, int version, char *retention );
int c3db_close( C3HDL *h );

// queries
int c3db_read_tt( C3HDL *h, time_t from, time_t to, int metric, C3RES *res );
int c3db_read_tv( C3HDL *h, struct timeval from, struct timeval to, int metric, C3RES *res );
int c3db_read_ts( C3HDL *h, struct timespec from, struct timespec to, int metric, C3RES *res );
int c3db_read_us( C3HDL *h, int64_t from, int64_t to, int metric, C3RES *res );
int c3db_read_ns( C3HDL *h, int64_t from, int64_t to, int metric, C3RES *res );

// default is nsec
#define c3db_read c3db_read_ns

// updates - always nsec
int c3db_write( C3HDL *h, int count, C3PNT *points );
int c3db_flush( C3HDL *h, int *written );

// error reporting
char *c3db_errstr( int errnum );
char *c3db_error( C3HDL *h );
int c3db_status( C3HDL *h );
int c3db_errno( C3HDL *h );

// utils
uint64_t c3db_file_size( C3HDL *h );
int c3db_dump( C3HDL *h, FILE *to, int show_empty, int ts_fmt );
int c3db_dump_header( C3HDL *h, FILE *to, int ts_fmt );
int c3db_metric( char *name );
char *c3db_metric_name( int metric );
int c3db_tsformat( char *name );
char *c3db_tsformat_name( int tsfmt );

// zero-ing allocator
void *alloc3( size_t size );

#endif
