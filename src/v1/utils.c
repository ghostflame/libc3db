#include "../c3_internal.h"



// parse one chunk of a retention string
int __c3db_v1_parse_retain_part( char *str, int len, V1CFG *cfg )
{
  	unsigned long span, period, count;
	char *c, *s;

	if( !str || !*str || !len )
	  	return -1;

	period = strtoul( str, &c, 10 );
	if( *c != ':' )
	  	return -1;
	c++;

	span = strtoul( c, &s, 10 );

	// check if either is 0
	if( !period || !span )
		return -2;

	// *s might be a multiplier or \0
	switch( *s )
	{
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
		default:
				break;
	}

	count = span / period;

	if( !span )
	  	return -3;

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

