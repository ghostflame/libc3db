/* VERSION AGNOSTIC CODE ONLY */

#include "c3_internal.h"


int __set_version( C3HDL *h )
{
	switch( h->version )
	{
		case 1:
			c3db_v1_set_version( h );
			return 0;
	}

	h->errnum = C3E_BAD_VERSION;
	return -1;
}


C3HDL *c3db_create( char *path, int version, char *retention )
{
	C3HDL *h;

	h = (C3HDL *) alloc3( sizeof( C3HDL ) );

	// 0 means current
	h->version  = ( version ) ? version : C3DB_CURR_VERSION;
	h->fullpath = strdup( path );

	if( __set_version( h ) != 0 )
	  return h;

	// and call the creator function
	(h->f_create)( h, retention );

	return h;
}



C3HDL *c3db_open( char *path, int rw )
{
  	uint8_t buf[8];
	struct stat sb;
	uint32_t *u;
	uint16_t *s;
  	int f, st;
	C3HDL *h;

	h = (C3HDL *) alloc3( sizeof( C3HDL ) );

	if( rw )
	{
		f  = O_RDWR;
		st = C3DB_ST_OPEN;
	}
	else
	{
		f  = O_RDONLY|O_NOATIME;
		st = C3DB_ST_READONLY;
	}

	if( ( h->fd = open( path, f ) ) < 0 )
	{
	  	GETERRNO;
		return h;
	}

	h->fullpath = strdup( path );

	if( read( h->fd, buf, 8 ) != 8 )
	{
	  	GETERRNO;
		return h;
	}

	u = (uint32_t *) buf;		// magic
	s = (uint16_t *) (buf + 4);	// version

	// check the magic matches
	if( *u != C3DB_MAGIC )
	{
		h->errnum = C3E_BAD_MAGIC;
		close( h->fd );
		return h;
	}

	// get the size
	if( fstat( h->fd, &sb ) ) {
		GETERRNO;
		close( h->fd );
		return h;
	}
	h->fsize   = sb.st_size;
	h->version = *s++;
	h->hsize   = *s;

	// set our functions
	if( __set_version( h ) != 0 )
	{
		close( h->fd );
		return h;
	}

	h->state   = st;

	// seek back to the start of the file
	// so the version opener can read the whole header
	lseek( h->fd, 0, SEEK_SET );

	// call the open function for this version
	(h->f_open)( h );

	return h;
}


