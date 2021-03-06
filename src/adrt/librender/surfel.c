/*                        S U R F E L . C
 * BRL-CAD
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
/** @file surfel.c
 *
 * Brief description
 *
 */

/*                        S U R F E L . c
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
/** @file surfel.c
 *
 */

#include "component.h"
#include "cut.h"
#include "depth.h"
#include "flat.h"
#include "flos.h"
#include "surfel.h"
#include "hit.h"
#include "adrt_struct.h"

#include <stdio.h>
#include <stdlib.h>

#include "bu.h"

void render_surfel_init(render_t *render, uint32_t num, render_surfel_pt_t *list) {
    render_surfel_t *d;

    render->work = render_surfel_work;
    render->free = render_surfel_free;
    render->data = (render_surfel_t *)bu_malloc(sizeof(render_surfel_t), "render data");
    d = (render_surfel_t *)render->data;
    d->list = (render_surfel_pt_t *)bu_malloc(num * sizeof(render_surfel_pt_t), "data list");
    d->num = num;
    d->list = list;
}


void render_surfel_free(render_t *render) {
    render_surfel_t *d;

    d = (render_surfel_t *)render->data;
    bu_free(d->list, "data list");
    bu_free(render->data, "render data");
}


void render_surfel_work(render_t *render, tie_t *tie, tie_ray_t *ray, TIE_3 *pixel) {
    render_surfel_t *d;
    tie_id_t id;
    adrt_mesh_t *mesh;
    uint32_t i;
    tfloat dist_sq;

    d = (render_surfel_t *)render->data;

    if ((mesh = (adrt_mesh_t *)tie_work(tie, ray, &id, render_hit, NULL))) {
	for (i = 0; i < d->num; i++) {
	    dist_sq = (d->list[i].pos.v[0]-id.pos.v[0]) * (d->list[i].pos.v[0]-id.pos.v[0]) +
                (d->list[i].pos.v[1]-id.pos.v[1]) * (d->list[i].pos.v[1]-id.pos.v[1]) +
                (d->list[i].pos.v[2]-id.pos.v[2]) * (d->list[i].pos.v[2]-id.pos.v[2]);

	    if (dist_sq < d->list[i].radius*d->list[i].radius) {
		*pixel = d->list[i].color;
		break;
	    }
	}

	MATH_VEC_SET((*pixel), 0.8, 0.8, 0.8);
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
