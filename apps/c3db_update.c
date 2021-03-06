/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* c3db_update.c - Update a C3DB file from the command line                *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



#include <c3db.h>
#include "tools.h"

void usage( void )
{
	printf( "Usage: c3db_update -h\n" );
	printf( "       c3db_update [OPTIONS] -f <name>\n\n" );

	printf( "Options:\n" );
	printf( " -h           Print this help.\n" );
	printf( " -V           More verbose output.\n" );
	printf( " -f <file>    Filename to write (default: %s)\n", DEFAULT_FILE );
	printf( " -i <file>    Input file containing points\n" );
	printf( " -I <file>    Input file containing usec timestamp points\n" );
	printf( " -p <point>   Data point ( <ts>:<value> )\n" );
	printf( " -P <point>   Data point ( <us>:<value> )\n\n" );

	printf( "Input file format should be lines of points in the same format as -p.\n\n" );

	exit( 0 );
}


int add_point( PDATA **list, char *str, int len, int convert )
{
	PDATA *new;
	char *c;

	if( !( c = memchr( str, ':', len ) ) )
	{
	  	fprintf( stderr, "String '%s' invalid - format is '<ts>:<value>'\n", str );
	  	return -1;
	}
	*c++ = '\0';

	new       = (PDATA *) malloc( sizeof( PDATA ) );
	new->next = *list;
	*list     = new;

	new->point.val = strtof( c, NULL );
	new->point.ts  = strtoll( str, NULL, 10 );
	if( convert )
		tt_to_ns( new->point.ts, new->point.ts );

	return 0;
}


int read_in_file( char *name, PDATA **list, int convert )
{
	size_t blen = 0;
  	char *buf = NULL;
	ssize_t len;
	int lc = 0;
	FILE *fh;

	if( !( fh = fopen( name, "r" ) ) )
	{
		fprintf( stderr, "Failed to open file '%s' -- %s\n",
			name, Err );
		return -1;
	}

	while( ( len = getline( &buf, &blen, fh ) ) != -1 )
	{
	  	lc++;

	  	if( !len )
		  	continue;

		if( buf[len - 1] == '\n' )
	  		// remove the newline
			buf[--len] = '\0';

		// don't error out, carry on
		if( add_point( list, buf, len, convert ) ) {
			fprintf( stderr, "Bad input on line %d of file '%s': %s\n", lc, name, buf );
		}
	}

	fclose( fh );
	return 0;
}




int main( int ac, char **av )
{
  	int oc, count, i, written, chat;
  	char *in, *inu, *file;
	PDATA *list, *p;
	C3PNT *points;
	C3HDL *h;

	file  = NULL;
	in    = NULL;
	inu   = NULL;
	list  = NULL;
	count = 0;
	chat  = 0;

	while( ( oc = getopt( ac, av, "hVf:I:i:p:P:" ) ) != -1 )
		switch( oc )
		{
			case 'h':
				usage( );
				break;
			case 'V':
				chat = 1;
				break;
			case 'p':
				if( add_point( &list, optarg, strlen( optarg ), 1 ) )
				{
					fprintf( stderr, "Invalid argument: %s\n", optarg );
					return 1;
				}
				break;
			case 'P':
				if( add_point( &list, optarg, strlen( optarg ), 0 ) )
				{
					fprintf( stderr, "Invalid argument: %s\n", optarg );
					return 1;
				}
				break;
			case 'f':
				file = strdup( optarg );
				break;
			case 'i':
				in = strdup( optarg );
				break;
			case 'I':
				inu = strdup( optarg );
				break;
		}

	if( !file ) 
	{
		fprintf( stderr, "No C3DB file specified.\n" );
		return 1;
	}

	if( in )
	{
		if( read_in_file( in, &list, 1 ) )
		{
			fprintf( stderr, "Could not read in file '%s'.\n", in );
			return 1;
		}
		else if( chat )
			printf( "Read input file '%s'\n", in );
	}
	else if( inu )
	{
		if( read_in_file( inu, &list, 1 ) )
		{
			fprintf( stderr, "Could not read in file '%s'.\n", inu );
			return 1;
		}
		else if( chat )
			printf( "Read input file '%s'\n", inu );
	}


	if( !list )
	{
		fprintf( stderr, "No data points given.\n" );
		return 1;
	}

	h = c3db_open( file, 1 );

	if( ! C3DB_HANDLE_OK( h ) )
	{
		fprintf( stderr, "Could not open C3DB '%s' -- %s\n", file, c3db_error( h ) );
		return 1;
	}

	// run through our points
	for( count = 0, p = list; p; p = p->next )
		count++;

	// make space
	points = (C3PNT *) malloc( count * sizeof( C3PNT ) );

	// and copy in the data
	for( i = 0, p = list; i < count; i++, p = p->next )
		points[i] = p->point;

	if( c3db_write( h, count, points ) )
		fprintf( stderr, "Failed to update C3DB '%s' -- %s\n", file, c3db_error( h ) );

	written = 0;
	if( c3db_flush( h, &written ) )
	  	fprintf( stderr, "Failed to flush updates to C3DB '%s' to disk -- %s\n",
					file, c3db_error( h ) );
	else if( chat )
	  	printf( "Wrote %d buckets to C3DB '%s'\n", written, file );


	c3db_close( h );

	return 0;
}
