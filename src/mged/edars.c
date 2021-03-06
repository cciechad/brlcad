/*                         E D A R S . C
 * BRL-CAD
 *
 * Copyright (c) 1996-2008 United States Government as represented by
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
/** @file edars.c
 *
 * Functions -
 *	find_nearest_ars_pt - find which vertex of an ARS is nearest
 *			the ray from "pt" in the viewing direction (for vertex selection in MGED)
 *
 *  Author -
 *	John R. Anderson
 *
 */

#include "common.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "bio.h"
#include "vmath.h"
#include "nmg.h"
#include "raytrace.h"
#include "nurb.h"
#include "rtgeom.h"
#include "wdb.h"

#include "./mged.h"
#include "./mged_solid.h"
#include "./sedit.h"
#include "./mged_dm.h"

extern struct bn_tol mged_tol;

void
find_nearest_ars_pt(
    int *crv,
    int *col,
    struct rt_ars_internal *ars,
    point_t pick_pt,
    vect_t dir)
{
    int i, j;
    int pt_no;
    int closest_i=0, closest_j=0;
    fastf_t min_dist_sq=MAX_FASTF;

    RT_ARS_CK_MAGIC( ars );

    pt_no = 0;
    for ( i=0; i<ars->ncurves; i++ )
    {
	for ( j=0; j<ars->pts_per_curve; j++ )
	{
	    fastf_t dist_sq;

	    dist_sq = bn_distsq_line3_pt3( pick_pt, dir, &ars->curves[i][j*3] );
	    if ( dist_sq < min_dist_sq )
	    {
		min_dist_sq = dist_sq;
		closest_i = i;
		closest_j = j;
	    }
	    pt_no += 3;
	}
    }
    *crv = closest_i;
    *col = closest_j;
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
