#include <c3db.h>
#include "tools.h"

void usage( void )
{
	printf( "Usage: c3db_dump -h\n" );
	printf( "       c3db_dump [OPTIONS] -f <name>\n\n" );

	printf( "Options:\n" );
	printf( " -h           Print this help.\n" );
	printf( " -H           Do not dump the header.\n" );
	printf( " -D           Do not dump the data.\n" );
	printf( " -E           Do not show empty slots.\n" );
	printf( " -t <fmt>     Select timestamp format.\n" );
	printf( " -f <name>    C3DB file to dump.\n" );
	printf( " -o <file>    Write to file rather than stdout.\n\n" );

	exit( 0 );
}


int main( int ac, char **av )
{
	int oc, empty, hdr, data, tf;
	char *target, *source;
	FILE *fh;
	C3HDL *h;

	target = NULL;
	source = NULL;
	empty  = 1;
	hdr    = 1;
	data   = 1;
	fh     = stdout;
	tf     = C3DB_TS_SEC;

	while( ( oc = getopt( ac, av, "hEHDt:f:o:" ) ) != -1 )
		switch( oc )
		{
			case 'h':
				usage( );
				break;
			case 'H':
				hdr = 0;
				break;
			case 'D':
				data = 0;
				break;
			case 'E':
				empty = 0;
				break;
			case 't':
				tf = c3db_tsformat( optarg );
				break;
			case 'f':
				source = strdup( optarg );
				break;
			case 'o':
				target = strdup( optarg );
				break;
		}

	if( !source )
	{
		fprintf( stderr, "No source C3DB specified.\n" );
		return 1;
	}

	// open readonly
	h = c3db_open( source, 0 );

	if( ! C3DB_HANDLE_OK( h ) )
	{
		fprintf( stderr, "Could not open C3DB '%s' -- %s\n", source, c3db_error( h ) );
		return 1;
	}

	if( target ) {
		if( !( fh = fopen( target, "w" ) ) )
		{
			fprintf( stderr, "Could not open output file '%s' -- %s\n", target, Err );
			c3db_close( h );
			return 1;
		}
	}

	// dump and close
	if( hdr )
		c3db_dump_header( h, fh, tf );
	if( data )
		c3db_dump( h, fh, empty, tf );

	c3db_close( h );

	// tidy up our output file maybe
	if( target )
		fclose( fh );

	return 0;
}
