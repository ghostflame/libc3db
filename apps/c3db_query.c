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
	printf( " -d <sec>     Duration of query.\n" );
	printf( " -m <metric>  Metric to fetch (default: mean).\n" );
	printf( " -o <file>    Output filename (default: stdout).\n\n" );

	printf( "Time range may be specified in one of three ways:\n" );
	printf( "Start and end:        use -b and -e\n" );
	printf( "Start and duration:   use -b and -d\n" );
	printf( "End and duration:     use -e and -d\n\n" );

	exit( 0 );
}



int main( int ac, char **av )
{
  	time_t begin, end, from, to, dur;
  	char *out, *file, *metric;
	int oc, met, i;
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
	fh     = stdout;

	while( ( oc = getopt( ac, av, "hf:o:m:b:e:d:" ) ) != -1 )
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
			case 'b':
				begin = (time_t) strtoul( optarg, NULL, 10 );
				break;
			case 'e':
				end = (time_t) strtoul( optarg, NULL, 10 );
				break;
			case 'd':
				dur = (time_t) strtoul( optarg, NULL, 10 );
				break;
		}

	if( !file ) 
	{
		fprintf( stderr, "No C3DB file specified.\n" );
		return 1;
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


	if( c3db_read( h, from, to, met, &res ) )
	{
		fprintf( stderr, "Failed to query C3DB '%s' -- %s\n", file, c3db_error( h ) );
		return 1;
	}

	for( p = res.points, i = 0; i < res.count; i++, p++ )
		fprintf( fh, "ts: %10ld   %s: %f\n", p->ts, metric, p->val );

	c3db_close( h );

	return 0;
}
