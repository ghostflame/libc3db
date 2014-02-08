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
	time_t				ts;
	float				val;
};

struct c3db_result_set
{
	int					rtype;
	time_t				from;
	time_t				to;
	int					period;
	int					count;
	int32_t				_padding;
	C3PNT			*	points;
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
	C3E_MAX
};


// for your edification
#define C3DB_FILE_EXTN		"c3db"
#define C3DB_FILE_EXTN_LEN	4
#define C3DB_CURR_VERSION	1

#define C3DB_HANDLE_OK( h )	( c3db_status( h ) == C3E_SUCCESS )

#define C3DB_RO				0
#define C3DB_RW				1


// init/shutdown
C3HDL *c3db_open( char *path, int rw );
C3HDL *c3db_create( char *path, int version, char *retention );
int c3db_close( C3HDL *h );

// queries
int c3db_read( C3HDL *h, time_t from, time_t to, int metric, C3RES *res );

// updates
int c3db_write( C3HDL *h, int count, C3PNT *points );
int c3db_flush( C3HDL *h, int *written );

// error reporting
char *c3db_errstr( int errnum );
char *c3db_error( C3HDL *h );
int c3db_status( C3HDL *h );

// utils
uint64_t c3db_file_size( C3HDL *h );
int c3db_dump( C3HDL *h, FILE *to, int show_empty );
int c3db_dump_header( C3HDL *h, FILE *to );
int c3db_metric( char *name );
char *c3db_metric_name( int metric );

// zero-ing allocator
void *alloc3( size_t size );

#endif
