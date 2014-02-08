#include "c3_internal.h"


int c3db_read( C3HDL *h, time_t from, time_t to, int metric, C3RES *res )
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
	if( !h )
		return C3E_BAD_HANDLE;

	// we only care about dirty
	if( h->state != C3DB_ST_DIRTY ) {
		OK;
	}

	return (h->f_flush)( h, written );
}


int c3db_close( C3HDL *h )
{
	if( !h )
		return C3E_BAD_HANDLE;

	return (h->f_close)( h );
}

int c3db_dump( C3HDL *h, FILE *to, int show_empty )
{
	if( !h )
	  	return C3E_BAD_HANDLE;

	if( h->state == C3DB_ST_DIRTY )
	  	BAD( C3E_BAD_STATE );

	return (h->f_dump)( h, to, show_empty );
}

int c3db_dump_header( C3HDL *h, FILE *to )
{
	if( !h )
	  	return C3E_BAD_HANDLE;

	(h->f_hdump)( h, to );

	return 0;
}

