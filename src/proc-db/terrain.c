/*                       T E R R A I N . C
 * BRL-CAD
 *
 * Copyright (c) 1991-2008 United States Government as represented by
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
/** @file terrain.c
 *
 * Create a random terrain spline model database.  Future additions to
 * this program would be to include random trees and objects to be
 * inserted into the database.
 *
 */

#include "common.h"

#include <stdlib.h>
#include <math.h>
#include "bio.h"

#include "bu.h"
#include "vmath.h"		/* BRL-CAD Vector macros */
#include "nurb.h"		/* BRL-CAD Spline data structures */
#include "raytrace.h"
#include "wdb.h"

fastf_t grid[10][10][3];

char *Usage = "This program ordinarily generates a database on stdout.\n\
	Your terminal probably wouldn't like it.";

void interpolate_data(void);

struct face_g_snurb *surfs[100];
int nsurf = 0;

struct rt_wdb *outfp;

#ifndef HAVE_DRAND48
/* simulate drand48() --  using 31-bit random() -- assumed to exist */
double drand48() {
    extern long random();
    return (double)random() / 2147483648.0; /* range [0, 1) */
}
#endif

int
main(int argc, char **argv)
{

    char * id_name = "terrain database";
    char * nurb_name = "terrain";
    int	i, j;
    fastf_t 	hscale;

    outfp = wdb_fopen("terrain.g");

    hscale = 2.5;


    while ((i=bu_getopt(argc, argv, "dh:")) != EOF) {
	switch (i) {
	    case 'd' : rt_g.debug |= DEBUG_MEM | DEBUG_MEM_FULL; break;
	    case 'h' :
		hscale = atof( bu_optarg );
		break;
	    default	:
		(void)fprintf(stderr,
			      "Usage: %s [-d] > database.g\n", *argv);
		return(-1);
	}
    }

    /* Create the database header record.
     * this solid will consist of three surfaces
     * a top surface, bottom surface, and the sides
     * (so that it will be closed).
     */

    mk_id( outfp, id_name);

    for ( i = 0; i < 10; i++)
	for ( j = 0; j < 10; j++)
	{
	    fastf_t		v;
	    fastf_t		drand48(void);

	    v = (hscale * drand48()) + 10.0;

	    grid[i][j][0] = i;
	    grid[i][j][1] = j;
	    grid[i][j][2] = v;

	}

    interpolate_data();

    mk_bspline( outfp, nurb_name, surfs);

    return 0;
}

/* Interpoate the data using b-splines */
void
interpolate_data(void)
{
    struct face_g_snurb *srf;
    fastf_t * data;
    fastf_t rt_nurb_par_edge();

    data = &grid[0][0][0];

    BU_GETSTRUCT( srf, face_g_snurb );

    rt_nurb_sinterp( srf, 4, data, 10, 10 );
    rt_nurb_kvnorm( &srf->u );
    rt_nurb_kvnorm( &srf->v );

    surfs[nsurf++] = srf;
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
