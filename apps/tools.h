/**************************************************************************
* This code is licensed under the Apache License 2.0.  See ../LICENSE     *
* Copyright 2016 John Denholm                                             *
*                                                                         *
* tools.h - defines shared tools structures and values                    *
*                                                                         *
* Updates:                                                                *
**************************************************************************/



#ifndef C3DB_TOOLS_H
#define C3DB_TOOLS_H

#define DEFAULT_FILE	"default.c3db"
#define DEFAULT_RETAIN	"10:8d;60:72d"

typedef struct point_data PDATA;

struct point_data
{
	PDATA	*	next;
	C3PNT		point;
};

#endif

