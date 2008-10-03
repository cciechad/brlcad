/*                     C D B . C
 * BRL-CAD / ADRT
 *
 * Copyright (c) 2002-2008 United States Government as represented by
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
/** @file cdb.c
 *
 *  Comments -
 *      Common Library - Database loader
 *
 */

#include "cdb.h"
#include <string.h>

#include "bio.h"
#include "canim.h"
#include "env.h"


int common_db_load(common_db_t *db, const char *path) {
    char proj_path[ADRT_NAME_SIZE], *path_ptr;

    /* Parse path out of proj file and chdir to it */
    strncpy(proj_path, path, sizeof(proj_path));

    path_ptr = strrchr(proj_path, '/');
    if (path_ptr) {
	path_ptr[0] = 0;
	chdir(proj_path);
    }


    /* Load Environment Data */
    common_env_init(&db->env);
    common_env_read(&db->env, path);
    common_env_prep(&db->env);

    /* Load Animation Data */
    common_anim_read(&db->anim, db->env.frames_file);

    return(0);
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
