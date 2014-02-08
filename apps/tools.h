#ifndef C3DB_TOOLS_H
#define C3DB_TOOLS_H

#define DEFAULT_FILE	"default.c3db"
#define DEFAULT_RETAIN	"10:8d;1200:400d"

typedef struct point_data PDATA;

struct point_data
{
	PDATA	*	next;
	C3PNT		point;
};

#endif
