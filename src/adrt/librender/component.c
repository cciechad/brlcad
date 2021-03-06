/*                     C O M P O N E N T . C
 * BRL-CAD / ADRT
 *
 * Copyright (c) 2007-2008 United States Government as represented by
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
/** @file component.c
 *
 */

#include "component.h"
#include "hit.h"
#include "adrt.h"
#include "adrt_struct.h"
#include <stdio.h>


void render_component_init(render_t *render) {
    render->work = render_component_work;
    render->free = render_component_free;
}


void render_component_free(render_t *render) {
}


static void* component_hit(tie_ray_t *ray, tie_id_t *id, tie_tri_t *tri, void *ptr) {
    adrt_mesh_t *mesh = (adrt_mesh_t *)(tri->ptr);

    ray->depth++;
    if (mesh->flags & (ADRT_MESH_SELECT|ADRT_MESH_HIT))
	return(mesh);

    return(0);
}


void render_component_work(render_t *render, tie_t *tie, tie_ray_t *ray, TIE_3 *pixel) {
    tie_id_t id;
    adrt_mesh_t *mesh;
    TIE_3 vec;
    tfloat angle;


    if ((mesh = (adrt_mesh_t *)tie_work(tie, ray, &id, component_hit, NULL))) {
	/* Flip normal to face ray origin (via dot product check) */
	if (ray->dir.v[0] * id.norm.v[0] + ray->dir.v[1] * id.norm.v[1] + ray->dir.v[2] * id.norm.v[2] > 0)
	    MATH_VEC_MUL_SCALAR(id.norm, id.norm, -1.0);

	/* shade solid */
	pixel->v[0] = mesh->flags & ADRT_MESH_HIT ? 0.8 : 0.2;
	pixel->v[1] = 0.2;
	pixel->v[2] = mesh->flags & ADRT_MESH_SELECT ? 0.8 : 0.2;
	MATH_VEC_SUB(vec, ray->pos, id.pos);
	MATH_VEC_UNITIZE(vec);
	MATH_VEC_DOT(angle, vec, id.norm);
	MATH_VEC_MUL_SCALAR((*pixel), (*pixel), (angle*0.8));
    } else if (ray->depth) {
	pixel->v[0] += 0.2;
	pixel->v[1] += 0.2;
	pixel->v[2] += 0.2;
    }
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
