#include "../c3_internal.h"


int __c3db_v1_update_cfg( C3HDL *h, C3PNT *pt, int cid, V1CFG *cfg )
{
	V1UPD *u, **uplist;
	time_t t;

	uplist = (V1UPD **) h->updates;

	// round off the timestamp
	t = pt->ts - ( pt->ts % cfg->period );

	for( u = uplist[cid]; u; u = u->next )
	  	if( u->ts == t )
		  	break;

	if( !u )
	{
		u = (V1UPD *) alloc3( sizeof( V1UPD ) );
		u->ts      = t;
		u->cfg_idx = cid;
		u->offset  = c3db_v1_config_offset( cfg, t );

		lseek( h->fd, u->offset, SEEK_SET );

		if( read( h->fd, &(u->orig), sizeof( V1BKT ) ) != sizeof( V1BKT ) )
		{
			GETERRNO;
			free( u );
			return h->errnum;
		}

		// copy that
		memcpy( &(u->changed), &(u->orig), sizeof( V1BKT ) );

		// and push that to the top
		u->next     = uplist[cid];
		uplist[cid] = u;
	}

	// new date?  the easy case
	if( u->changed.ts != t )
	{
	  	u->changed.ts    = t;
		u->changed.count = 1;
		u->changed.sum   = pt->val;
		u->changed.min   = pt->val;
		u->changed.max   = pt->val;
	}
	else
	{
		// we're updating a current valid point
		if( pt->val > u->changed.max )
		  	u->changed.max = pt->val;
		else if( pt->val < u->changed.min )
		  	u->changed.min = pt->val;

		u->changed.sum += pt->val;
		u->changed.count++;
	}

	OK;
}



int __c3db_v1_update( C3HDL *h, C3PNT *point )
{
	V1CFG *list;
	V1HDR *hdr;
	int i;

	hdr  = (V1HDR *) h->hdr;
	list = (V1CFG *) &(hdr->cfg);

	for( i = 0; i < hdr->bcount; i++ )
		__c3db_v1_update_cfg( h, point, i, list + i );

	return C3E_SUCCESS;
}



int c3db_v1_write( C3HDL *h, int count, C3PNT *points )
{
	int i;

	for( i = 0; i < count; i++ )
		if( __c3db_v1_update( h, points + i ) )
		  	return h->errnum;

	OK;
}





int c3db_v1_flush( C3HDL *h, int *written )
{
	V1UPD *up, *list, **all;
	V1HDR *hdr;
	int i, j;

	h->state = C3DB_ST_WRITING;

	hdr = (V1HDR *)  h->hdr;
	all = (V1UPD **) h->updates;

	for( i = 0, j = 0; i < hdr->bcount; i++ )
	{
		list = all[i];

		while( list )
		{
			up   = list;
			list = up->next;

			lseek( h->fd, up->offset, SEEK_SET );
			if( write( h->fd, &(up->changed), sizeof( V1BKT ) ) != sizeof( V1BKT ) )
			{
				ERRNO;
			}

			free( up );
			j++;
		}

		// and mark that update list empty
		all[i] = NULL;
	}

	if( written )
	  	*written = j;

	h->state = C3DB_ST_OPEN;

	OK;
}
