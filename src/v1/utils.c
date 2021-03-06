/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* utils.c - V1-specific C3DB helper functions                             *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



#include "../c3_internal.h"



// parse one chunk of a retention string
int __c3db_v1_parse_retain_part( char *str, int len, V1CFG *cfg )
{
	int64_t span, period, count;
	char *c, *s, u;

	if( !str || !*str || !len )
		return -1;

	period = strtoull( str, &c, 10 );

	u = tolower( *c );

	// m, u or n means msec, usec or nsec,
	// nothing means sec, so we may
	// have to convert to usec
	switch( u )
	{
		case 'm':
			ms_to_ns( period, period );
			c++;
			break;
		case 'u':
			us_to_ns( period, period );
			c++;
			break;
		case 'n':
			c++;
			break;
		case ':':
			tt_to_ns( period, period );
			break;
	}

	// and then check for a :
	if( *c++ != ':' )
		return -1;

	span = strtoull( c, &s, 10 );

	// check if either is 0
	if( !period || !span )
		return -2;

	u = tolower( *s );

	// *s might be a multiplier or \0
	switch( u )
	{
		case 's':
				tt_to_ns( span, span );
				break;
		case 'u':
				us_to_ns( span, span );
				break;
		case 'y':
				span *= 52;
		case 'w':
				span *= 7;
		case 'd':
				span *= 24;
		case 'h':
				span *= 60;
		case 'm':
				span *= 60;
		case 'n':
				tt_to_ns( span, span );
				break;
		default:
				return -3;
	}

	count = span / period;

	cfg->period = period;
	cfg->count  = count;

	return 0;
}


// parse a retention string
// 10:8d;300:1y
int c3db_v1_parse_retain_string( char *retain, V1CFG **cfg, int *count )
{
	V1CFG configs[128], *copy;
	char *p, *sc, *q;
	int cc, len;

	if( !retain || !*retain || !cfg || !count )
		return C3E_NULL_ARG;

	*cfg   = NULL;
	*count = 0;

	memset( configs, 0, 128 * sizeof( V1CFG ) );
	p  = retain;
	cc = 0;

	// walk along the string
	while( p && *p && cc < 127 )
	{
		q = p;
		if( ( sc = strchr( p, ';' ) ) )
		{
			len = sc - p;
			p   = sc + 1;
		}
		else
		{
			len = strlen( p );
			p   = NULL;
		}

		// read a chunk
		if( __c3db_v1_parse_retain_part( q, len, configs + cc ) )
			return C3E_BAD_RETAIN;

		cc++;
	}

	copy = (V1CFG *) alloc3( cc * sizeof( V1CFG ) );
	memcpy( copy, configs, cc * sizeof( V1CFG ) );

	*cfg   = copy;
	*count = cc;

	return 0;
}

