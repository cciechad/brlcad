/*                        C O M B _ C O L O R . C
 * BRL-CAD
 *
 * Copyright (c) 2008 United States Government as represented by
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
/** @file comb_color.c
 *
 * The comb_color command.
 *
 */

#include "ged.h"


int
ged_comb_color(struct rt_wdb *wdbp, int argc, const char *argv[])
{
    int				i;
    int				val;
    register struct directory	*dp;
    struct rt_db_internal	intern;
    struct rt_comb_internal	*comb;
    static const char *usage = "comb_color combination R G B";

    GED_CHECK_DATABASE_OPEN(wdbp, GED_ERROR);
    GED_CHECK_READ_ONLY(wdbp, GED_ERROR);

    /* initialize result */
    bu_vls_trunc(&wdbp->wdb_result_str, 0);
    wdbp->wdb_result = GED_RESULT_NULL;
    wdbp->wdb_result_flags = 0;

    /* must be wanting help */
    if (argc == 1) {
	wdbp->wdb_result_flags |= GED_RESULT_FLAGS_HELP_BIT;
	bu_vls_printf(&wdbp->wdb_result_str, "Usage: %s %s", argv[0], usage);
	return GED_OK;
    }

    if (argc != 5) {
	bu_vls_printf(&wdbp->wdb_result_str, "Usage: %s %s", argv[0], usage);
	return GED_ERROR;
    }

    GED_DB_LOOKUP(wdbp, dp, argv[1], LOOKUP_NOISY, GED_ERROR);
    GED_CHECK_COMB(wdbp, dp, GED_ERROR);
    GED_DB_GET_INTERNAL(wdbp, &intern, dp, (fastf_t *)NULL, &rt_uniresource, GED_ERROR);

    comb = (struct rt_comb_internal *)intern.idb_ptr;
    RT_CK_COMB(comb);

    for (i = 0; i < 3; ++i) {
	if (sscanf(argv[i+2], "%d", &val) != 1 || val < 0 || 255 < val) {
	    bu_vls_printf(&wdbp->wdb_result_str,"RGB value out of range: %s", argv[i + 2]);
	    rt_db_free_internal(&intern, &rt_uniresource);
	    return GED_ERROR;
	}
	else
	    comb->rgb[i] = val;
    }

    comb->rgb_valid = 1;
    GED_DB_PUT_INTERNAL(wdbp, dp, &intern, &rt_uniresource, GED_ERROR);

    return GED_OK;
}


/*
 * Local Variables:
 * tab-width: 8
 * mode: C
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
