/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* c3_internal.h - Definitions and structures used within the library      *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



#ifndef C3DB_INT_H
#define C3DB_INT_H

#include <stdarg.h>
#include <features.h>
#include <sys/mman.h>
#include "../include/c3db.h"

#ifndef	Err
#define	Err			strerror( errno )
#endif

#define GETERRNO	h->errnocp = errno; h->errnum = C3E_SEE_ERRNO

#define	OK			return ( h->errnum = C3E_SUCCESS )
#define ERRNO		GETERRNO; return h->errnum
#define BAD( v )	return ( h->errnum = v )

#define C3DB_MAGIC	0x42443343		// "C3DB", little endian


// define the version-specific 
typedef int  c3_open_f   ( C3HDL * );
typedef int  c3_create_f ( C3HDL *, char * );
typedef int  c3_close_f  ( C3HDL * );
typedef int  c3_read_f   ( C3HDL *, int64_t, int64_t, C3RES * );
typedef int  c3_write_f  ( C3HDL *, int, C3PNT * );
typedef int  c3_flush_f  ( C3HDL *, int * );
typedef int  c3_dump_f   ( C3HDL *, FILE *, int, int );
typedef void c3_hdump_f  ( C3HDL *, FILE *, int );


// bring in version-specific headers
#include "v1/local.h"


enum c3db_states
{
	C3DB_ST_UNKNOWN = 0,
	C3DB_ST_OPEN,
	C3DB_ST_READONLY,
	C3DB_ST_CREATING,
	C3DB_ST_DIRTY,
	C3DB_ST_WRITING,
	C3DB_ST_CLOSED
};


struct c3db_handle
{
	void			*	map;
	void			*	hdr;

	char			*	fullpath;
	int					fd;
	int					state;
	uint64_t			fsize;


	int					updates;
	int					errnum;
	int					errnocp;

	uint16_t			version;
	uint16_t			hsize;

	c3_open_f		*	f_open;
	c3_create_f		*	f_create;
	c3_close_f		*	f_close;
	c3_read_f		*	f_read;
	c3_write_f		*	f_write;
	c3_flush_f		*	f_flush;
	c3_dump_f		*	f_dump;
	c3_hdump_f		*	f_hdump;
};



#endif
