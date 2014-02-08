#ifndef C3DB_VERSION_1_H
#define C3DB_VERSION_1_H

// declare our functions
c3_create_f		c3db_v1_create;
c3_open_f		c3db_v1_open;
c3_close_f		c3db_v1_close;
c3_read_f		c3db_v1_read;
c3_write_f		c3db_v1_write;
c3_flush_f		c3db_v1_flush;
c3_dump_f		c3db_v1_dump;
c3_hdump_f		c3db_v1_hdump;


typedef struct	c3db_v1_config		V1CFG;
typedef struct	c3db_v1_header		V1HDR;
typedef struct	c3db_v1_bucket		V1BKT;
typedef struct	c3db_v1_update		V1UPD;
typedef struct	c3db_v1_request		V1REQ;
typedef struct	c3db_v1_timespan	V1SPN;
typedef struct	c3db_v1_offrange	V1RNG;


struct c3db_v1_config
{
	uint32_t			count;
	uint32_t			period;
	uint64_t			offset;
};


// maps straight into the file
struct c3db_v1_header
{
	uint32_t			magic;
	uint16_t			version;
	uint16_t			hsize;
	uint32_t			cksum;
	uint16_t			bcount;
	uint16_t			_padding;
	// this is now 8-byte-aligned
	unsigned char		cfg[0];
};

#define c3db_v1_GET_CONFIG( n )	( n >= h->bcount ) ? NULL : (V1CFG *) ( h->cfg + ( n * sizeof( V1CFG ) ) )


struct c3db_v1_bucket
{
	uint32_t			ts;
	uint32_t			count;
	float				sum;
	float				min;
	float				max;
};

struct c3db_v1_update
{
	V1UPD			*	next;
	uint64_t			offset;
	uint32_t			ts;
	int32_t				cfg_idx;
	V1BKT				orig;
	V1BKT				changed;
};

struct c3db_v1_timespan
{
	time_t				from;
	time_t				to;
};

struct c3db_v1_offrange
{
	uint64_t			start;
	uint64_t			end;
	int32_t				count;
	int32_t				_padding;
	V1BKT			*	data;
};

struct c3db_v1_request
{
	V1SPN				orig;
	V1SPN				fetch;
	V1CFG			*	cfg;
	uint32_t			now;
	int32_t				ranges;
	V1RNG				range[2];
};


// functions
void c3db_v1_set_version( C3HDL *h );

// utils
int c3db_v1_parse_retain_string( char *retain, V1CFG **cfg, int *count );
uint64_t c3db_v1_config_offset( V1CFG *cfg, time_t when );

#endif
