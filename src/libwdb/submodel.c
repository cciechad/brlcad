/*                       S U B M O D E L . C
 * BRL-CAD
 *
 * Copyright (c) 1994-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/** @file submodel.c
 *
 *  Authors -
 *	John R. Anderson
 *
 */

#include "common.h"

#include "bio.h"

#include "bu.h"
#include "vmath.h"
#include "bn.h"
#include "rtgeom.h"
#include "raytrace.h"
#include "wdb.h"
#include "db.h"


/*
 *			M K _ S U B M O D E L
 *
 *  Create a submodel solid.
 *  If file is NULL or "", the treetop refers to the current database.
 *  Treetop is the name of a single database object in 'file'.
 *  meth is 0 (RT_PART_NUBSPT) or 1 (RT_PART_NUGRID).
 *  method 0 is what is normally used.
 */
int
mk_submodel(struct rt_wdb *fp, const char *name, const char *file, const char *treetop, int meth)
{
    struct rt_submodel_internal *in;

    BU_GETSTRUCT( in, rt_submodel_internal );
    in->magic = RT_SUBMODEL_INTERNAL_MAGIC;
    bu_vls_init( &in->file );
    if ( file )  bu_vls_strcpy( &in->file, file );
    bu_vls_init( &in->treetop );
    bu_vls_strcpy( &in->treetop, treetop );
    in->meth = meth;

    return wdb_export( fp, name, (genptr_t)in, ID_SUBMODEL, mk_conv2mm );
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
