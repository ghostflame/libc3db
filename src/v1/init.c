#include "../c3_internal.h"


void c3db_v1_set_version( C3HDL *h )
{
	h->f_open   = c3db_v1_open;
	h->f_create = c3db_v1_create;
	h->f_close  = c3db_v1_close;
	h->f_read   = c3db_v1_read;
	h->f_write  = c3db_v1_write;
	h->f_flush  = c3db_v1_flush;
	h->f_dump   = c3db_v1_dump;
	h->f_hdump  = c3db_v1_hdump;
}


int c3db_v1_open( C3HDL *h )
{
    h->hdr = (V1HDR *) h->map;
	return 0;
}


int c3db_v1_create( C3HDL *h, char *retention )
{
	V1CFG *cfgs, *conf, *prev;
	uint32_t sum, *u;
	int cct, sz, i;
	V1HDR *hdr;

	if( c3db_v1_parse_retain_string( retention, &cfgs, &cct ) < 0
	 || cct == 0 )
	{
		h->errnum = C3E_BAD_RETAIN;
		return -1;
	}

	h->state = C3DB_ST_CREATING;

	// create a header
	sz			 = sizeof( V1HDR ) + ( cct * sizeof( V1CFG ) );
	hdr          = alloc3( sz );
	hdr->magic   = C3DB_MAGIC;
	hdr->version = h->version;
	hdr->hsize   = sz;
	hdr->bcount  = cct;

	// and copy the configs
	memcpy( hdr->cfg, cfgs, ( cct * sizeof( V1CFG ) ) );
	// don't need this any more
	free( cfgs );
	// but let's keep it as a pointer to the header configs
	cfgs = (V1CFG *) hdr->cfg;

	// that header is now the right size to be written
	// but we need to work out the offsets and checksum

	// the first one is easy
	cfgs[0].offset = hdr->hsize;
	conf           = cfgs;

	// the rest are incremental
	for( i = 1; i < cct; i++ )
	{
	  	conf = cfgs + i;
		prev = cfgs + i - 1;
		conf->offset = prev->offset + ( prev->count * sizeof( V1BKT ) );
	}

	// and finally the whole file size uses the last offset + size
	h->fsize = conf->offset + ( conf->count * sizeof( V1BKT ) );

	// now xor the header to get a checksum
	i   = hdr->hsize / sizeof( uint32_t );
	sum = 0;
	u   = (uint32_t *) hdr;
	while( i-- > 0 ) sum ^= *u++;

	hdr->cksum = sum;

	// create a file
	if( ( h->fd = open( h->fullpath, O_RDWR|O_CREAT|O_EXCL, 0644 ) ) < 0 )
	{
		GETERRNO;
fprintf( stderr, "Failed %s at open stage.\n", h->fullpath );
		free( hdr );
		return -2;
	}

	// allocate space
	if( ( i = posix_fallocate( h->fd, 0, h->fsize ) ) != 0 )
	{
		h->errnocp = i;
		h->errnum  = C3E_SEE_ERRNO;
		free( hdr );
		close( h->fd );
fprintf( stderr, "Failed %s at posix_fallocate.\n", h->fullpath );
		unlink( h->fullpath );
		return -3;
	}

	// and write the header
	if( write( h->fd, hdr, hdr->hsize ) != hdr->hsize )
	{
		GETERRNO;
fprintf( stderr, "Failed %s at write header.\n", h->fullpath );
		free( hdr );
		close( h->fd );
		unlink( h->fullpath );
		return -4;
	}

	// done
	h->state = C3DB_ST_OPEN;

	return 0;
}

int c3db_v1_close( C3HDL *h )
{
	if( h->state == C3DB_ST_DIRTY )
		c3db_v1_flush( h, NULL );

	if( h->fullpath )
		free( h->fullpath );

	if( h->map )
		munmap( h->map, h->fsize );

	if( h->fd >= 0 )
		close( h->fd );

	free( h );
	return 0;
}



