/*                     U N P A C K . H
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
/** @file unpack.h
 *
 *  Common Library - Unpacking Header
 *
 */

#ifndef _COMMON_UNPACK_H
#define _COMMON_UNPACK_H


#include "libutil/camera.h"
#include "cdb.h"
#include "env.h"
#include "texture.h"

/*** ONLY TEMPORARY ***/
#define COMMON_PACK_ENV_RM                0x0300
#define COMMON_PACK_ENV_IMAGESIZE         0x0301

#define COMMON_PACK_MESH_NEW              0x0400
/**********************/

typedef struct common_unpack_prop_node_s {
    char 			name[256];
    common_prop_t		prop;
} common_unpack_prop_node_t;


typedef struct common_unpack_texture_node_s {
    char			name[256];
    texture_t		*texture;
} common_unpack_texture_node_t;


typedef struct common_unpack_mesh_node_s {
    char			name[256];
    common_mesh_t		mesh;
} common_unpack_mesh_node_t;


extern	void	common_unpack(common_db_t *db, tie_t *tie, util_camera_t *camera, int socknum);
extern	void	common_unpack_free(common_db_t *db);

#endif

/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
