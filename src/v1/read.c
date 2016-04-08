/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* read.c - V1-specific C3DB file querying                                 *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



#include "../c3_internal.h"


void __c3db_v1_choose_offset( int count, V1CFG *cfg, V1REQ *r )
{
	uint64_t os, oe;
	int64_t cstart;
	V1CFG *c;
	int i;

	for( c = cfg, i = 0; i < count; i++, c++ )
	{
		cstart = r->now - ( c->period * c->count );
		r->cfg = c;

		if( r->orig.from <= cstart )
			r->fetch.from = cstart;
		else
		{
			r->fetch.from  = r->orig.from;
			r->fetch.from -= ( r->orig.from % c->period );
			break;
		}
	}

	c = r->cfg;

	r->fetch.to = r->orig.to - ( r->orig.to % c->period );

	os = c3db_v1_config_offset( c, r->fetch.from );
	oe = c3db_v1_config_offset( c, r->fetch.to );

	if( os < oe )
	{
		r->ranges         = 1;
		r->range[0].start = os;
		r->range[0].count = ( oe - os ) / sizeof( V1BKT );
	}
	else
	{
		// wrap-around ranges
		r->ranges         = 2;

		r->range[0].start = c->offset;
		r->range[0].count = ( oe - c->offset ) / sizeof( V1BKT );

		r->range[1].start = os;
		r->range[1].count = c->count - ( ( os - c->offset ) / sizeof( V1BKT ) );
	}
}


typedef void __c3db_v1_parser ( C3RES *, V1BKT *, int, int );


#define tscheck( )		b->ts >= res->from && b->ts < res->to


void __c3db_v1_parse_min( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = b->min;
		}
}

void __c3db_v1_parse_max( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = b->max;
		}
}


void __c3db_v1_parse_sum( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = b->sum;
		}
}

void __c3db_v1_parse_count( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = b->count;
		}
}

void __c3db_v1_parse_mean( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = ( b->count ) ? b->sum / b->count : 0;
		}
}


void __c3db_v1_parse_spread( C3RES *res, V1BKT *data, int count, int offset )
{
	V1BKT *b;
	int i, k;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = b->max - b->min;
		}
}


void __c3db_v1_parse_middle( C3RES *res, V1BKT *data, int count, int offset )
{
	V1BKT *b;
	int i, k;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
		if( tscheck( ) )
		{
			res->points[k].ts  = b->ts;
			res->points[k].val = ( b->min / 2 ) + ( b->max / 2 );
		}
}

#undef tscheck

__c3db_v1_parser *__c3db_v1_parser_fns[C3DB_REQ_END] = {
	__c3db_v1_parse_mean, // raw has little meaning here - mean it is
	__c3db_v1_parse_mean,
	__c3db_v1_parse_max,
	__c3db_v1_parse_min,
	__c3db_v1_parse_count,
	__c3db_v1_parse_sum,
	__c3db_v1_parse_spread,
	__c3db_v1_parse_middle
};




int c3db_v1_read( C3HDL *h, int64_t from, int64_t to, int metric, C3RES *res )
{
	__c3db_v1_parser *pfn;
	struct timespec ts;
	V1HDR *hdr;
	V1BKT *bkt;
	int64_t t;
	V1REQ r;

	// limit the ranges - don't report on the future
	clock_gettime( CLOCK_REALTIME, &ts );
	ts_to_ns( ts, t );

	if( from > t )
		OK;

	if( to > t )
		to = t;

	memset( &r, 0, sizeof( V1REQ ) );
	r.now       = t;
	r.orig.to   = to;
	r.orig.from = from;

	hdr = (V1HDR *) h->map;

	__c3db_v1_choose_offset( hdr->bcount, (V1CFG *) hdr->cfg, &r );

	res->from   = r.fetch.from;
	res->to     = r.fetch.to;
	res->period = r.cfg->period;

	res->count  = ( res->to - res->from ) / res->period;
	res->points = (C3PNT *) alloc3( res->count * sizeof( C3PNT ) );

	// pick a parse function
	pfn = __c3db_v1_parser_fns[res->rtype];

	// handle range 0
	bkt = (V1BKT *) ( h->map + r.range[0].start );
	(*pfn)( res, bkt, r.range[0].count, 0 );

	if( r.ranges == 2 )
	{
		// handle range 1
		bkt = (V1BKT *) ( h->map + r.range[1].start );
		(*pfn)( res, bkt, r.range[1].count, r.range[0].count );
	}
 
	OK;
}


void c3db_v1_hdump( C3HDL *h, FILE *to, int ts_fmt )
{
	time_t t, sec;
	V1HDR *hdr;
	V1CFG *cfg;
	int i;

	hdr = (V1HDR *) h->map;
	cfg = (V1CFG *) hdr->cfg;

	fprintf( to, "File name:         %s\n",          h->fullpath );
	fprintf( to, "File size:         %lu bytes\n",   h->fsize );
	fprintf( to, "File version:      %hu\n",         h->version );
	fprintf( to, "Header size:       %hu bytes\n",   h->hsize );
	fprintf( to, "Retention count:   %d\n",          hdr->bcount );
	for( i = 0; i < hdr->bcount; i++, cfg++ )
	{
		ns_to_tt( cfg->period, t );
		ns_to_tt( ( cfg->period * cfg->count ), sec );
		fprintf( to, "  %2d   @ %6lds for %8lds\n", i, t, sec );
	}
}



int c3db_v1_dump( C3HDL *h, FILE *to, int show_empty, int ts_fmt )
{
	struct timespec ts;
	struct timeval tv;
	char *dfmt, *sfmt;
	uint64_t c, t;
	float mean;
	V1HDR *hdr;
	V1CFG *cfg;
	V1BKT *bkt;
	int i;

	char *dfmts[C3DB_TS_MAX] = {
		"%10ld : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%10ld.%06ld : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%10ld.%09ld : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%13ld : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%16ld : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%20ld : %8u %8.3g %8.3g %8.3g %8.3g\n"
	};

	char *sfmts[C3DB_TS_MAX] = {
		"%10s : %8s %8s %8s %8s %8s\n",
		"%17s : %8s %8s %8s %8s %8s\n",
		"%21s : %8s %8s %8s %8s %8s\n",
		"%13s : %8s %8s %8s %8s %8s\n",
		"%16s : %8s %8s %8s %8s %8s\n",
		"%20s : %8s %8s %8s %8s %8s\n",
	};

	hdr = (V1HDR *) h->map;
	cfg = (V1CFG *) hdr->cfg;

	dfmt = dfmts[ts_fmt];
	sfmt = sfmts[ts_fmt];

	for( i = 0; i < hdr->bcount; i++, cfg++ )
	{
		fprintf( to, "Period:  %.3g    Count:  %lu\n", ((double) cfg->period) / 1000000000.0, cfg->count );
		fprintf( to, sfmt, "Timestamp", "Count", "Mean", "Sum", "Min", "Max" );

		bkt = (V1BKT *) ( h->map + cfg->offset );

		for( c = 0; c < cfg->count; c++, bkt++ )
		{
			if( !show_empty && !bkt->count )
				continue;
			mean = ( bkt->count ) ? bkt->sum / bkt->count : 0.0;

			switch( ts_fmt )
			{
				case C3DB_TS_SEC:
					ns_to_tt( bkt->ts, t );
					fprintf( to, dfmt, t,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_TVAL:
					ns_to_tv( bkt->ts, tv );
					fprintf( to, dfmt, tv.tv_sec, tv.tv_usec,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_TSPC:
					ns_to_ts( bkt->ts, ts );
					fprintf( to, dfmt, ts.tv_sec, ts.tv_nsec,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_MSEC:
					ns_to_ms( bkt->ts, t );
					fprintf( to, dfmt, t,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_USEC:
					ns_to_us( bkt->ts, t );
					fprintf( to, dfmt, t,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_NSEC:
					fprintf( to, dfmt, bkt->ts,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
			}
		}
	}

	OK;
}

