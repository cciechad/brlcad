/*                   R E N D E R _ U T I L . C
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
/** @file render_util.c
 *
 */

#include "render_util.h"
#include "umath.h"
#include "adrt_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bu.h"

typedef struct render_segment_s {
    adrt_mesh_t *mesh;
    uint8_t complete;
    tfloat thickness;
} render_segment_t;


typedef struct render_shotline_s {
    render_segment_t *seglist;
    TIE_3 in_hit;
    uint32_t segnum;
    uint32_t segind;
} render_shotline_t;


/* Generate vector list for a spall cone given a reference angle */
void render_util_spall_vec(TIE_3 dir, tfloat angle, int vec_num, TIE_3 *vec_list) {
#if 0
    TIE_3 vec;
    tfloat radius, t;
    int i;


    /* Otherwise the cone would be twice the angle */
    angle *= 0.5;

    /* Figure out the rotations of the ray direction */
    vec = dir;
    vec.v[2] = 0;

    radius = sqrt(vec.v[0]*vec.v[0] + vec.v[1]*vec.v[1]);
    vec.v[0] /= radius;
    vec.v[1] /= radius;
  
    vec.v[0] = vec.v[1] < 0 ? 360.0 - acos(vec.v[0])*MATH_RAD2DEG : acos(vec.v[0])*MATH_RAD2DEG;

    /* triangles to approximate */
    for (i = 0; i < vec_num; i++) {
	t = angle * sin((i * 360 / vec_num) * MATH_DEG2RAD);
	vec_list[i].v[0] = cos((vec.v[0] + t) * MATH_DEG2RAD);
	vec_list[i].v[1] = sin((vec.v[0] + t) * MATH_DEG2RAD);

	t = angle * cos((i * 360 / vec_num) * MATH_DEG2RAD);
	vec_list[i].v[2] = cos(acos(dir.v[2]) + t * MATH_DEG2RAD);
    }
#endif
}


static void* shot_hit(tie_ray_t *ray, tie_id_t *id, tie_tri_t *tri, void *ptr) {
    adrt_mesh_t *mesh = (adrt_mesh_t *)(tri->ptr);
    render_shotline_t *shotline = (render_shotline_t *)ptr;
    uint32_t i;
    uint8_t found;

    /* Scan from segind to segnum to find a match */
    found = 0;
    for (i = shotline->segind; i < shotline->segnum; i++) {
	if (shotline->seglist[i].mesh == mesh) {
	    found = 1;
	    shotline->seglist[i].complete = 1;
	    shotline->seglist[i].thickness = sqrt(id->dist) - sqrt(shotline->seglist[i].thickness);

	    /* Advance to next !complete */
	    while (shotline->seglist[shotline->segind].complete && shotline->segind <= i)
		shotline->segind++;
	    break;
	}
    }

    if (!found) {
	/* Grow the shotline */
	shotline->seglist = (render_segment_t *)bu_realloc(shotline->seglist, (shotline->segnum + 1) * sizeof(render_segment_t), "Growing shotline in shot_hit");

	/* Assign */
	shotline->seglist[shotline->segnum].mesh = mesh;
	shotline->seglist[shotline->segnum].complete = 0;
	shotline->seglist[shotline->segnum].thickness = id->dist;

	/* In-hit */
	if (shotline->segnum == 0) {
	    shotline->in_hit = ray->dir;
	    MATH_VEC_MUL_SCALAR(shotline->in_hit, shotline->in_hit, id->dist);
	    MATH_VEC_ADD(shotline->in_hit, shotline->in_hit, ray->pos);
	}

	/* Increment */
	shotline->segnum++;
    }

    return(NULL);
}


void render_util_shotline_list(tie_t *tie, tie_ray_t *ray, void **data, int *dlen) {
    tie_id_t id;
    render_shotline_t shotline;
    uint32_t i;
    uint8_t c;

    shotline.seglist = NULL;
    shotline.segnum = 0;
    shotline.segind = 0;

    tie_work(tie, ray, &id, shot_hit, &shotline);

    /* result length */
    *dlen = 0;

    /* in-hit */
    *data = bu_realloc(*data, sizeof(TIE_3), "render_util_shotline_list: Growing in-hit");
    memcpy(&((char *)*data)[*dlen], &shotline.in_hit, sizeof(TIE_3));
    *dlen = sizeof(TIE_3);

    /* number of segments */
    *data = bu_realloc(*data, *dlen + sizeof(uint32_t), "render_util_shotline_list: Growing segment count");
    memcpy(&((char *)*data)[*dlen], &shotline.segnum, sizeof(uint32_t));
    *dlen += sizeof(uint32_t);

    for (i = 0; i < shotline.segnum; i++) {
/*    printf("i: %d, complete: %d, thickness: %.3f, name: %s\n", i, shotline.seglist[i].complete, shotline.seglist[i].thickness, shotline.seglist[i].mesh->name); */
	c = strlen(shotline.seglist[i].mesh->name) + 1;

	/* Grow the data */
	*data = bu_realloc(*data, *dlen + 1 + c + sizeof(uint32_t), "render_util_shotline_list");

	/* length of string */
	memcpy(&((char *)*data)[*dlen], &c, 1);
	*dlen += 1;

	/* string */
	memcpy(&((char *)*data)[*dlen], shotline.seglist[i].mesh->name, c);
	*dlen += c;

	/* thickness */
	memcpy(&((char *)*data)[*dlen], &shotline.seglist[i].thickness, sizeof(tfloat));
	*dlen += sizeof(tfloat);
    }

    /* Free shotline data */
    bu_free(shotline.seglist, "render_util_shotline_list: shotline data");
}


void render_util_spall_list(tie_t *tie, tie_ray_t *ray, tfloat angle, void **data, int *dlen) {
#if 0
    shotline_t shotline;
    tie_ray_t sray;
    tie_id_t id;
    int i, ind;
    unsigned char c;
    TIE_3 *vec_list, in, out;
 

    shotline.mesh_list = NULL;
    shotline.mesh_num = 0;

    MATH_VEC_SET(shotline.in, 0, 0, 0);
    MATH_VEC_SET(shotline.out, 0, 0, 0);

    /* Fire the center ray first */
    tie_work(tie, ray, &id, shot_hit, &shotline);
    in = shotline.in;
    out = shotline.out;

    sray.pos = shotline.in;

    /* allocate memory for 32 spall rays */
    vec_list = (TIE_3 *)malloc(32 * sizeof(TIE_3));
    if (!vec_list) {
	perror("vec_list");
	exit(1);
    }

    /* Fire the 32 spall rays from the first in-hit at full angle */
    render_util_spall_vec(ray->dir, angle, 32, vec_list);
    for (i = 0; i < 32; i++) {
	sray.dir = vec_list[i];
	tie_work(tie, &sray, &id, shot_hit, &shotline);
    }

    /* Fire the 16 spall rays from the first in-hit at half angle */
    render_util_spall_vec(ray->dir, angle*0.5, 16, vec_list);
    for (i = 0; i < 16; i++) {
	sray.dir = vec_list[i];
	tie_work(tie, &sray, &id, shot_hit, &shotline);
    }

    /* Fire the 12 spall rays from the first in-hit at quarter angle */
    render_util_spall_vec(ray->dir, angle*0.25, 12, vec_list);
    for (i = 0; i < 12; i++) {
	sray.dir = vec_list[i];
	tie_work(tie, &sray, &id, shot_hit, &shotline);
    }

    free(vec_list);

    shotline.in = in;
    shotline.out = out;

    ind = 0;

    *data = (void *)realloc(*data, 6*sizeof(tfloat) + sizeof(int));

    /* pack in hit */
    memcpy(&((char *)*data)[ind], &shotline.in, sizeof(TIE_3));
    ind += sizeof(TIE_3);

    /* pack out hit */
    memcpy(&((char *)*data)[ind], &shotline.out, sizeof(TIE_3));
    ind += sizeof(TIE_3);

    memcpy(&((char *)*data)[ind], &shotline.mesh_num, sizeof(int));
    ind += sizeof(int);

    for (i = 0; i < shotline.mesh_num; i++) {
	c = strlen(shotline.mesh_list[i]->name) + 1;

	*data = realloc(*data, ind + c + 2); /* 1 for length, 1 for null char */

	/* length */
	memcpy(&((char *)*data)[ind], &c, 1);
	ind += 1;

	/* name */
	memcpy(&((char *)*data)[ind], shotline.mesh_list[i]->name, c);
	ind += c;

/*    printf("hit[%d]: -%s-\n", i, shotline.mesh_list[i]->name); */
    }

    *dlen = ind;
#endif
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
