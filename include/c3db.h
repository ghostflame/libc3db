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
#define	Err				strerror( errno )
#endif

#ifndef Err3
#define	Err3( h )		c3db_error( h )
#endif


// the first one is opaque
typedef struct c3db_handle		C3HDL;
typedef struct c3db_point		C3PNT;
typedef struct c3db_result_set	C3RES;


struct c3db_point
{
	uint64_t			ts;
	double				val;
};

struct c3db_result_set
{
	uint64_t			from;
	uint64_t			to;
	uint64_t			period;
	C3PNT			*	points;
	uint32_t			count;
	int					rtype;
};


enum metric_vals
{
	C3DB_REQ_INVLD = -1,
	C3DB_REQ_MEAN,
	C3DB_REQ_MAX,
	C3DB_REQ_MIN,
	C3DB_REQ_COUNT,
	C3DB_REQ_SUM,
	C3DB_REQ_SPREAD,
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
	C3DB_TS_SEC = 0,		// 1234567890
	C3DB_TS_TVAL,			// 1234567890.123456
	C3DB_TS_USEC,			// 1234567890123456
	C3DB_TS_MAX
};


// for your edification
#define C3DB_FILE_EXTN		"c3db"
#define C3DB_FILE_EXTN_LEN	4
#define C3DB_CURR_VERSION	1

#define C3DB_HANDLE_OK( h )	( c3db_status( h ) == C3E_SUCCESS )

#define C3DB_RO				0
#define C3DB_RW				1


#define tt_to_us( _t, _u )	_u = ( (uint64_t) _t ) * 1000000
#define us_to_tt( _u, _t )	_t = (time_t) ( _u / 1000000 )

#define tv_to_us( _tv, _u )	_u = ( ( (uint64_t) _tv.tv_sec ) * 1000000 ) + ( (uint64_t) _tv.tv_usec )
#define us_to_tv( _u, _tv )	_tv.tv_sec = (time_t) ( _u / 1000000 ); _tv.tv_usec = (long) _u % 1000000



// init/shutdown
C3HDL *c3db_open( char *path, int rw );
C3HDL *c3db_create( char *path, int version, char *retention );
int c3db_close( C3HDL *h );

// queries
int c3db_read( C3HDL *h, time_t from, time_t to, int metric, C3RES *res );
int c3db_read_tv( C3HDL *h, struct timeval from, struct timeval to, int metric, C3RES *res );
int c3db_read_us( C3HDL *h, uint64_t from, uint64_t to, int metric, C3RES *res );

// updates
int c3db_write( C3HDL *h, int count, C3PNT *points );
int c3db_flush( C3HDL *h, int *written );

// error reporting
char *c3db_errstr( int errnum );
char *c3db_error( C3HDL *h );
int c3db_status( C3HDL *h );

// utils
uint64_t c3db_file_size( C3HDL *h );
int c3db_dump( C3HDL *h, FILE *to, int show_empty, int ts_fmt );
int c3db_dump_header( C3HDL *h, FILE *to, int ts_fmt );
int c3db_metric( char *name );
char *c3db_metric_name( int metric );

// zero-ing allocator
void *alloc3( size_t size );

#endif
