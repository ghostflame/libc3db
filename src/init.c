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




C3HDL *c3db_open( char *path, int rw )
{
  	uint8_t buf[8];
	struct stat sb;
  	int f, mt, st;
	uint32_t *u;
	uint16_t *s;
	C3HDL *h;

	h = (C3HDL *) alloc3( sizeof( C3HDL ) );

	if( rw )
	{
		f  = O_RDWR;
		st = C3DB_ST_OPEN;
	}
	else
	{
#ifdef O_NOATIME
		f  = O_RDONLY|O_NOATIME;
#else
        f  = O_RDONLY;
#endif
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
		h->errnum = C3E_HDR_READ_FAIL;
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
	if( fstat( h->fd, &sb ) )
    {
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

    // mmap the file
    f  = ( rw ) ? PROT_READ|PROT_WRITE : PROT_READ;
    mt = ( rw ) ? MAP_SHARED : MAP_PRIVATE;

    h->map = mmap( NULL, h->fsize, f, mt, h->fd, 0 );

    if( h->map == MAP_FAILED )
    {
        GETERRNO;
        close( h->fd );
        return h;
    }

	// call the open function for this version
	(h->f_open)( h );

	h->state = st;
	return h;
}


C3HDL *c3db_create( char *path, int version, char *retention )
{
	C3HDL *h;

	h = (C3HDL *) alloc3( sizeof( C3HDL ) );

    h->fd = -1;

	// 0 means current
	h->version  = ( version ) ? version : C3DB_CURR_VERSION;
	h->fullpath = strdup( path );

	if( __set_version( h ) != 0 )
	  return h;

	// and call the creator function
	if( (h->f_create)( h, retention ) != C3E_SUCCESS )
        return h;

    // close it
    (h->f_close)( h );

    // and re-open rw
	return c3db_open( path, 1 );
}

