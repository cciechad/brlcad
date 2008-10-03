/*                     C A D _ P A R E A . C
 * BRL-CAD
 *
 * Copyright (c) 2004-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/** @file cad_parea.c
 *
 *	cad_parea -- area of polygon
 *
 *  Author -
 *	D A Gwyn
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bu.h"

#include	"./vld_std.h"


typedef struct
{
    double	x;			/* X coordinate */
    double	y;			/* Y coordinate */
}	point;			/* polygon vertex */

static bool_t	GetArgs(int argc, char **argv), Input(register point *coop);
static void	Output(double result), Usage(void);


static void
Usage(void) 				/* print usage message */
{
    (void)printf( "usage: cad_parea[ -i input][ -o output]\n"
	);
}


int
main(int argc, char **argv)			/* "cad_parea" entry point */
    /* argument count */
    /* argument strings */
{
    point		previous;	/* previous point */
    point		current;	/* current point */
    point		first;		/* saved first point */
    register bool_t	saved;		/* "`first' valid" flag */
    double		sum;		/* accumulator */

    if ( !GetArgs( argc, argv ) )	/* process command arguments */
    {
	Output( 0.0 );
	return 1;
    }

    saved = false;
    sum = 0.0;

    while ( Input( &current ) )
    {
	/* scan input record */
	if ( !saved )
	{
	    /* first input only */
	    first = current;
	    saved = true;
	}
	else	/* accumulate cross-product */
	    sum += previous.x * current.y -
		previous.y * current.x;

	previous = current;
    }

    if ( saved )			/* normally true */
	sum += previous.x * first.y - previous.y * first.x;

    Output( sum / 2.0 );
    return 0;			/* success! */
}


static bool_t
GetArgs(int argc, char **argv)			/* process command arguments */
    /* argument count */
    /* argument strings */
{
    static bool_t	iflag = false;	/* set if "-i" option found */
    static bool_t	oflag = false;	/* set if "-o" option found */
    int		c;		/* option letter */

    while ( (c = bu_getopt( argc, argv, "i:o:" )) != EOF )
	switch ( c )
	{
	    case 'i':
		if ( iflag )
		{
		    (void)printf(
			"cad_parea: too many -i options\n"
			);
		    return false;
		}
		iflag = true;

		if ( strcmp( bu_optarg, "-" ) != 0
		     && freopen( bu_optarg, "r", stdin ) == NULL
		    )	{
		    (void)printf(
			"cad_parea: can't open \"%s\"\n",
			bu_optarg
			);
		    return false;
		}
		break;

	    case 'o':
		if ( oflag )
		{
		    (void)printf(
			"cad_parea: too many -o options\n"
			);
		    return false;
		}
		oflag = true;

		if ( strcmp( bu_optarg, "-" ) != 0
		     && freopen( bu_optarg, "w", stdout ) == NULL
		    )	{
		    (void)printf(
			"cad_parea: can't create \"%s\"\n",
			bu_optarg
			);
		    return false;
		}
		break;

	    case '?':
		Usage();	/* print usage message */
		return false;
	}

    return true;
}


static bool_t
Input(register point *coop)				/* input a coordinate record */
    /* -> input coordinates */
{
    char		inbuf[82];	/* input record buffer */

    while ( bu_fgets( inbuf, (int)sizeof inbuf, stdin ) != NULL )
    {
	/* scan input record */
	register int	cvt;	/* # converted fields */

	cvt = sscanf( inbuf, " %le %le", &coop->x, &coop->y );

	if ( cvt == 0 )
	    continue;	/* skip color, comment, etc. */

	if ( cvt == 2 )
	    return true;	/* successfully converted */

	(void)printf( "cad_parea: bad input:\n%s\n", inbuf
	    );
	Output( 0.0 );
	bu_exit( 2, NULL );		/* return false insufficient */
    }

    return false;			/* EOF */
}


static void
Output(double result)			/* output computed area */
    /* computed area */
{
    printf( "%g\n", result );
}

/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
