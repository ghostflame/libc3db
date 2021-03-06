/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* utils.c - Non-versioned utilities and helper functions                  *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



/* NO VERSION-SPECIFIC CODE IN HERE PLEASE */

#include "c3_internal.h"

static char *c3db_metric_names[C3DB_REQ_END] =
{
	"raw",
	"mean",
	"max",
	"min",
	"count",
	"sum",
	"spread",
	"middle"
};

static char *c3db_timestamp_names[C3DB_TS_MAX] =
{
	"sec",
	"tval",
	"tspec",
	"msec",
	"usec",
	"nsec"
};


int c3db_metric( char *name )
{
	int i;

	for( i = 0; i < C3DB_REQ_END; i++ )
		if( !strcasecmp( name, c3db_metric_names[i] ) )
			return i;

	return C3DB_REQ_INVLD;
}

char *c3db_metric_name( int metric )
{
	if( metric > C3DB_REQ_INVLD && metric < C3DB_REQ_END )
		return c3db_metric_names[metric];

	return NULL;
}

int c3db_tsformat( char *name )
{
	int i;

	for( i = 0; i < C3DB_TS_MAX; i++ )
		if( !strcasecmp( name, c3db_timestamp_names[i] ) )
			return i;

	return C3DB_TS_INVLD;
}



static char *c3db_error_strings[C3E_MAX] =
{
	"Success",
	"Bad handle",
	"See errno instead",
	"Bad config count",
	"Null argument",
	"Bad magic value",
	"Bad db version",
	"Bad handle state",
	"Read-only handle",
	"Bad file range",
	"Bad request metric",
	"Bad retention string",
	"Bad time format",
	"Failed to read header"
};

char *c3db_errstr( int errnum )
{
	if( errnum < 0
	 || errnum >= C3E_MAX )
	 	return "Unknown error";

	return c3db_error_strings[errnum];
}

char *c3db_error( C3HDL *h )
{
	if( !h )
		return c3db_errstr( C3E_BAD_HANDLE );

	// special case - we kept errno
	if( h->errnum == C3E_SEE_ERRNO )
		return strerror( h->errnocp );

	return c3db_errstr( h->errnum );
}

int c3db_status( C3HDL *h )
{
	if( !h )
		return C3E_BAD_HANDLE;

	return h->errnum;
}

int c3db_errno( C3HDL *h )
{
	if( !h )
		return 0;

	return h->errnocp;
}

uint64_t c3db_file_size( C3HDL *h )
{
	return ( h ) ? h->fsize : 0;
}


static uint8_t mem_signal_array[128] =
{
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0
};



// force a null allocator
void *alloc3( size_t size )
{
	void *p = calloc( 1, size );

	if( !p )
		return (void *) &mem_signal_array;

	return p;
}


