#include <c3db.h>
#include "tools.h"

void usage( void )
{
	printf( "Usage: c3db_create -h\n" );
	printf( "       c3db_create [OPTIONS] -f <name>\n\n" );

	printf( "Options:\n" );
	printf( " -h           Print this help.\n" );
	printf( " -V           More verbose output.\n" );
	printf( " -d           Delete after successful creation.\n" );
	printf( " -s           Report file size.\n" );
	printf( " -f <file>    Filename to write  (default: %s)\n", DEFAULT_FILE );
	printf( " -r <policy>  Retention policy   (default: %s)\n", DEFAULT_RETAIN );
	printf( " -v <vers>    Specify db version (default: %d)\n\n", C3DB_CURR_VERSION );

	printf( "Retention policy is a semi-colon-delimited list of time spec pairs.  Each\n" );
	printf( "pair is of the form 'seconds:duration' where duration is of the form\n" );
	printf( "<number>[y|w|d|h|m], years to minutes.\n" );

	exit( 0 );
}


int main( int ac, char **av )
{
	int oc, stats, l, vers, del, chat;
	char *ret, file[512];
	uint64_t sz;
	C3HDL *h;

	strncpy( file, DEFAULT_FILE, 512 );
	ret   = DEFAULT_RETAIN;
	chat  = 0;
	vers  = C3DB_CURR_VERSION;
	stats = 0;
	del   = 0;

	while( ( oc = getopt( ac, av, "hVsdv:r:f:" ) ) != -1 )
		switch( oc )
		{
			case 'h':
				usage( );
				break;
			case 'V':
				chat = 1;
				break;
			case 's':
				stats = 1;
				break;
			case 'd':
				del = 1;
				break;
			case 'r':
				ret = strdup( optarg );
				break;
			case 'f':
				strncpy( file, optarg, 511 )
				file[511] = '\0';
				break;
			case 'v':
				vers = atoi( optarg );
				break;
		}

	// add the file extn if it's not there and there is room
	l = strlen( file );
	if( strcmp( file + l - ( 1 + C3DB_FILE_EXTN_LEN ), "." C3DB_FILE_EXTN )
	 && ( l < ( 510 - C3DB_FILE_EXTN_LEN ) ) )
	  	snprintf( file + l, 512 - l, ".%s", C3DB_FILE_EXTN );

	h = c3db_create( file, vers, ret );

	if( ! C3DB_HANDLE_OK( h ) )
	{
		fprintf( stderr, "Could not create C3DB '%s' -- %s\n", file, c3db_error( h ) );
		c3db_close( h );
		return 1;
	}

	if( chat )
		printf( "Created C3DB '%s'.\n", file );

	if( stats )
	{
		sz = c3db_file_size( h );
		printf( "File size: %lu bytes.\n", sz );
	}

	c3db_close( h );

	if( del )
	{
		if( unlink( file ) )
		{
			fprintf( stderr, "Error removing file '%s' -- %s\n", file, Err );
			return 1;
		}
		else if( chat )
			printf( "Deleted C3DB '%s' after successful creation.\n", file );
	}

	return 0;
}
