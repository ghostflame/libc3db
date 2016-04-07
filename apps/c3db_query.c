#include <c3db.h>
#include "tools.h"

void usage( void )
{
	printf( "Usage: c3db_query -h\n" );
	printf( "       c3db_query [OPTIONS] -f <name>\n\n" );

	printf( "Options:\n" );
	printf( " -h           Print this help.\n" );
	printf( " -f <file>    C3DB filename.\n" );
	printf( " -b <ts>      Begin from this timestamp.\n" );
	printf( " -e <ts>      End at this timestamp.\n" );
	printf( " -B <usec>    Bring from this usec timestamp.\n" );
	printf( " -E <usec>    End at this usec timestamp.\n" );
	printf( " -d <sec>     Duration of query.\n" );
	printf( " -D <usec>    Duration of query in usec.\n" );
	printf( " -m <metric>  Metric to fetch (default: mean).\n" );
	printf( " -o <file>    Output filename (default: stdout).\n\n" );

	printf( "Time range may be specified in one of three ways:\n" );
	printf( "Start and end:        use -b and -e  OR  -B and -E\n" );
	printf( "Start and duration:   use -b and -d  OR  -B and -D\n" );
	printf( "End and duration:     use -e and -d  OR  -E and -D\n\n" );

	exit( 0 );
}


void us_check( int us )
{
	if( us == 1 )
		exit( fprintf( stderr, "Cannot mix usec and sec timings.\n" ) );
}


int main( int ac, char **av )
{
	int64_t begin, end, from, to, dur;
	char *out, *file, *metric;
	int oc, met, i, us;
	struct timeval tv;
	time_t tt;
	C3RES res;
	C3PNT *p;
	C3HDL *h;
	FILE *fh;

	file   = NULL;
	out    = NULL;
	metric = "mean";
	begin  = 0;
	end    = 0;
	dur    = 0;
	from   = 0;
	to     = 0;
	us     = 0;
	fh     = stdout;

	while( ( oc = getopt( ac, av, "hf:o:m:b:e:d:B:E:D:" ) ) != -1 )
		switch( oc )
		{
			case 'h':
				usage( );
				break;
			case 'f':
				file = strdup( optarg );
				break;
			case 'o':
				out = strdup( optarg );
				break;
			case 'm':
				metric = strdup( optarg );
				break;
			case 'B':
				us    = 1;
				begin = strtoll( optarg, NULL, 10 );
				break;
			case 'b':
				us_check( us );
				begin = strtoll( optarg, NULL, 10 );
				break;
			case 'E':
				us  = 1;
				end = strtoll( optarg, NULL, 10 );
				break;
			case 'e':
				us_check( us );
				end = strtoll( optarg, NULL, 10 );
				break;
			case 'D':
				us  = 1;
				dur = strtoll( optarg, NULL, 10 );
				break;
			case 'd':
				us_check( us );
				dur = strtoll( optarg, NULL, 10 );
				break;
		}

	if( !file ) 
	{
		fprintf( stderr, "No C3DB file specified.\n" );
		return 1;
	}

	if( us == 0 )
	{
		// spot the macro...
		tt_to_us( begin, begin );
		tt_to_us( end,   end   );
		tt_to_us( dur,   dur   );
	}

	if( end )
	{
		to = end;
	
		if( begin )
	  		from = begin;
		else if( dur )
		  	from = end - dur;
		else
			usage( );
	}
	else if( begin )
	{
		from = begin;
		if( dur )
		 	to = from + dur;
		else
			usage( );
	}

	if( to <= from )
		usage( );


	if( ( met = c3db_metric( metric ) ) == C3DB_REQ_INVLD )
	{
		fprintf( stderr, "Metric '%s' not recognised.\n", metric );
		return 1;
	}


	if( out )
	{
		if( !( fh = fopen( out, "w" ) ) )
		{
			fprintf( stderr, "Cannot open output file '%s' for writing -- %s\n",
				out, Err );
			return 1;
		}
	}


	h = c3db_open( file, C3DB_RO );
	if( ! C3DB_HANDLE_OK( h ) )
	{
		fprintf( stderr, "Could not open C3DB '%s' -- %s\n", file, c3db_error( h ) );
		return 1;
	}

	if( c3db_read_us( h, from, to, met, &res ) )
	{
		fprintf( stderr, "Failed to query C3DB '%s' -- %s\n", file, c3db_error( h ) );
		return 1;
	}

	if( us )
		for( p = res.points, i = 0; i < res.count; i++, p++ )
		{
			ns_to_tv( p->ts, tv );
			fprintf( fh, "ts: %10ld.%06ld   %s: %f\n", tv.tv_sec, tv.tv_usec, metric, p->val );
		}
	else
		for( p = res.points, i = 0; i < res.count; i++, p++ )
		{
			ns_to_tt( p->ts, tt );
			fprintf( fh, "ts: %10ld   %s: %f\n", tt, metric, p->val );
		}

	c3db_close( h );

	return 0;
}
