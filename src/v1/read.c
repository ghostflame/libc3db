#include "../c3_internal.h"


void __c3db_v1_choose_offset( int count, V1CFG *cfg, V1REQ *r )
{
	uint64_t os, oe, cstart;
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


void __c3db_v1_parse_min( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
	{
		if( b->ts >= res->to
		 || b->ts <  res->from )
			continue;

		res->points[k].ts  = b->ts;
		res->points[k].val = b->min;
	}
}

void __c3db_v1_parse_max( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
	{
		if( b->ts >= res->to
		 || b->ts <  res->from )
			continue;

		res->points[k].ts  = b->ts;
		res->points[k].val = b->max;
	}
}


void __c3db_v1_parse_sum( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
	{
		if( b->ts >= res->to
		 || b->ts <  res->from )
			continue;

		res->points[k].ts  = b->ts;
		res->points[k].val = b->sum;
	}
}

void __c3db_v1_parse_count( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
	{
		if( b->ts >= res->to
		 || b->ts <  res->from )
			continue;

		res->points[k].ts  = b->ts;
		res->points[k].val = b->count;
	}
}

void __c3db_v1_parse_mean( C3RES *res, V1BKT *data, int count, int offset )
{
	register int i, k;
	register V1BKT *b;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
	{
		if( b->ts >= res->to
		 || b->ts <  res->from )
			continue;

		res->points[k].ts  = b->ts;
		res->points[k].val = ( b->count ) ? b->sum / b->count : 0;
	}
}


void __c3db_v1_parse_spread( C3RES *res, V1BKT *data, int count, int offset )
{
	V1BKT *b;
	int i, k;

	for( b = data, k = offset, i = 0; i < count; i++, b++, k++ )
	{
		if( b->ts >= res->to
		 || b->ts <  res->from )
			continue;

		res->points[k].ts  = b->ts;
		res->points[k].val = b->max - b->min;
	}
}

__c3db_v1_parser *__c3db_v1_parser_fns[C3DB_REQ_END] = {
	__c3db_v1_parse_mean,
	__c3db_v1_parse_max,
	__c3db_v1_parse_min,
	__c3db_v1_parse_count,
	__c3db_v1_parse_sum,
	__c3db_v1_parse_spread
};




int c3db_v1_read( C3HDL *h, uint64_t from, uint64_t to, int metric, C3RES *res )
{
    __c3db_v1_parser *pfn;
	struct timeval tv;
    V1HDR *hdr;
    V1BKT *bkt;
    uint64_t t;
    V1REQ r;

    // limit the ranges - don't report on the future
	gettimeofday( &tv, NULL );
	tv_to_us( tv, t );

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
		us_to_tt( cfg->period, t );
		us_to_tt( ( cfg->period * cfg->count ), sec );
		fprintf( to, "  %2d   @ %6lds for %8lds\n", i, t, sec );
	}
}



int c3db_v1_dump( C3HDL *h, FILE *to, int show_empty, int ts_fmt )
{
	char *dfmt, *sfmt;
	float mean;
	V1HDR *hdr;
	V1CFG *cfg;
	V1BKT *bkt;
	int i, c;

	char *dfmts[3] = {
		"%10lu : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%10lu.%06lu : %8u %8.3g %8.3g %8.3g %8.3g\n",
		"%16lu : %8u %8.3g %8.3g %8.3g %8.3g\n"
	};

	char *sfmts[3] = {
		"%10s : %8s %8s %8s %8s %8s\n",
		"%17s : %8s %8s %8s %8s %8s\n",
		"%16s : %8s %8s %8s %8s %8s\n"
	};

	hdr = (V1HDR *) h->map;
	cfg = (V1CFG *) hdr->cfg;

	dfmt = dfmts[ts_fmt];
	sfmt = sfmts[ts_fmt];

	for( i = 0; i < hdr->bcount; i++, cfg++ )
	{
		fprintf( to, "Period:  %.3g    Count:  %u\n", ((double) cfg->period) / 1000000.0, cfg->count );
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
					fprintf( to, dfmt, bkt->ts / 1000000,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_TVAL:
					fprintf( to, dfmt, bkt->ts / 1000000, bkt->ts % 1000000,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
				case C3DB_TS_USEC:
					fprintf( to, dfmt, bkt->ts,
						bkt->count, mean, bkt->sum, bkt->min, bkt->max );
					break;
			}
		}
	}

	OK;
}
