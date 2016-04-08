/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* actions.c - wrappers for versioned action functions                     *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



#include "c3_internal.h"


int c3db_read_ns( C3HDL *h, int64_t from, int64_t to, int metric, C3RES *res )
{
	if( !h )
	  	return C3E_BAD_HANDLE;

	if( !res )
	  	return C3E_NULL_ARG;

	if( to <= from )
	  	return C3E_BAD_RANGE;

	if( metric <= C3DB_REQ_INVLD
	 || metric >= C3DB_REQ_END )
		return C3E_BAD_METRIC;

	memset( res, 0, sizeof( C3RES ) );
	res->rtype = metric;

	return (h->f_read)( h, from, to, metric, res );
}


int c3db_read_us( C3HDL *h, int64_t from, int64_t to, int metric, C3RES *res )
{
	return c3db_read_ns( h, from * 1000, to * 1000, metric, res );
}

int c3db_read_tt( C3HDL *h, time_t from, time_t to, int metric, C3RES *res )
{
	int64_t t, f;

	tt_to_ns( from, f );
	tt_to_ns( to,   t );

	return c3db_read_ns( h, f, t, metric, res );
}

int c3db_read_tv( C3HDL *h, struct timeval from, struct timeval to, int metric, C3RES *res )
{
	int64_t t, f;

	tv_to_ns( from, f );
	tv_to_ns( to,   t );

	return c3db_read_ns( h, f, t, metric, res );
}

int c3db_read_ts( C3HDL *h, struct timespec from, struct timespec to, int metric, C3RES *res )
{
	int64_t t, f;

	ts_to_ns( from, f );
	ts_to_ns( to,   t );

	return c3db_read_ns( h, f, t, metric, res );
}


int c3db_write( C3HDL *h, int count, C3PNT *points )
{
	if( !h )
	  	return C3E_BAD_HANDLE;

	if( !count )
	  	OK;

	switch( h->state )
	{
		case C3DB_ST_READONLY:
			BAD( C3E_READ_ONLY );
		case C3DB_ST_OPEN:
		case C3DB_ST_DIRTY:
			break;
		default:
			BAD( C3E_BAD_STATE );
	}

	h->state = C3DB_ST_DIRTY;

	return (h->f_write)( h, count, points );
}


int c3db_flush( C3HDL *h, int *written )
{
    int ret;

	if( !h )
		return C3E_BAD_HANDLE;

	// we only care about dirty
	if( h->state != C3DB_ST_DIRTY ) {
		OK;
	}

	ret = (h->f_flush)( h, written );

    if( ret == C3E_SUCCESS )
        h->state = C3DB_ST_OPEN;

    return ret;
}


int c3db_close( C3HDL *h )
{
	if( !h )
		return C3E_BAD_HANDLE;

    // prefer a version close function
	if( h->f_close )
		return (h->f_close)( h );

	// OK, now what - we have a file handle maybe
	if( h->fd >= 0 )
	{
		close( h->fd );
		h->fd = -1;
	}

    if( h->map )
        munmap( h->map, h->fsize );

	// we probably have a path
	if( h->fullpath )
	{
		free( h->fullpath );
		h->fullpath = NULL;
	}

	// that will do - further leaks are
	// something to track down later
	free( h );
	return 0;
}

int c3db_dump( C3HDL *h, FILE *to, int show_empty, int ts_fmt )
{
	if( !h )
	  	return C3E_BAD_HANDLE;

	if( h->state == C3DB_ST_DIRTY )
	  	BAD( C3E_BAD_STATE );

	if( ts_fmt < C3DB_TS_SEC || ts_fmt >= C3DB_TS_MAX )
		BAD( C3E_BAD_FORMAT );

	return (h->f_dump)( h, to, show_empty, ts_fmt );
}

int c3db_dump_header( C3HDL *h, FILE *to, int ts_fmt )
{
	if( !h )
	  	return C3E_BAD_HANDLE;

	if( ts_fmt < C3DB_TS_SEC || ts_fmt >= C3DB_TS_MAX )
		BAD( C3E_BAD_FORMAT );

	(h->f_hdump)( h, to, ts_fmt );

	return 0;
}

