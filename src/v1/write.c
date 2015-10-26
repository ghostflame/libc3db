#include "../c3_internal.h"


// iterate over points and configs updating the in-memory
// map of the database file
int c3db_v1_write( C3HDL *h, int count, C3PNT *points )
{
    register C3PNT *pt;
    register V1BKT *bk;
    register int j;
    V1BKT *base;
    V1CFG *cfg;
    V1HDR *hdr;
    time_t ts;
    int i;

    hdr = (V1HDR *) h->hdr;
    cfg = (V1CFG *) &(hdr->cfg);

    for( i = 0; i < hdr->bcount; i++, cfg++ )
    {
        pt   = points;
        // find the start of the buckets for this config
        base = (V1BKT *) ( h->map + cfg->offset );

        for( j = 0; j < count; j++, pt++ )
        {
            // get the timestamp and find the bucket
            ts  = pt->ts - ( pt->ts % cfg->period );

            // find the bucket to update
            bk = base + ( ( ts / cfg->period ) % cfg->count );

            // new or old?
            if( bk->ts != ts )
            {
                // new
                bk->ts    = ts;
                bk->count = 1;
                bk->sum   = pt->val;
                bk->min   = pt->val;
                bk->max   = pt->val;
            }
            else
            {
                // old, so update
                bk->count++;
                bk->sum += pt->val;

                if( pt->val > bk->max )
                    bk->max = pt->val;
                else if( pt->val < bk->min )
                    bk->min = pt->val;
            }
        }
    }

    h->updates += count;
    
    OK;
}


int c3db_v1_flush( C3HDL *h, int *written )
{
    if( h->map && h->updates > 0 )
    {
        if( msync( h->map, h->fsize, MS_ASYNC|MS_INVALIDATE ) )
        {
            ERRNO;
        }
    }

    OK;
}

