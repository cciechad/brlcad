/*                        C A M E R A . C
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
/** @file camera.c
 *
 * Brief description
 *
 */

/*                        C A M E R A . H
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
/** @file camera.h
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "bio.h"
#include "bu.h"

#include "component.h"
#include "cut.h"
#include "camera.h"
#include "libutil/umath.h"

pthread_t *render_tlist;

void* render_camera_render_thread(void *ptr);
static void render_camera_prep_ortho(render_camera_t *camera);
static void render_camera_prep_persp(render_camera_t *camera);
static void render_camera_prep_persp_dof(render_camera_t *camera);


void render_camera_init(render_camera_t *camera, int threads)
{
    camera->type = RENDER_CAMERA_PERSPECTIVE;

    camera->view_num = 1;
    camera->view_list = (render_camera_view_t *) bu_malloc (sizeof(render_camera_view_t), "render_camera_init");
    camera->dof = 0;
    camera->tilt = 0;

    /* The camera will use a thread for every cpu the machine has. */
    camera->thread_num = threads ? threads : bu_avail_cpus();
/* printf("threads: %d\n", camera->thread_num); */

    /* Initialize camera to rendering surface normals */
    render_normal_init(&camera->render);
    camera->rm = RENDER_METHOD_NORMAL;

    render_tlist = NULL;
    if (camera->thread_num > 1)
	render_tlist = (pthread_t *)bu_malloc(sizeof(pthread_t) * camera->thread_num, "render_tlist");
}


void render_camera_free(render_camera_t *camera)
{
    if (camera->thread_num > 1)
	bu_free(render_tlist, "render_tlist");
}


static void render_camera_prep_ortho(render_camera_t *camera)
{
    TIE_3 look, up, side, temp;
    tfloat angle, s, c;


    /* Generate standard up vector */
    up.v[0] = 0;
    up.v[1] = 0;
    up.v[2] = 1;

    /* Generate unitized look vector */
    MATH_VEC_SUB(look, camera->focus, camera->pos);
    MATH_VEC_UNITIZE(look);

    /* Make unitized up vector perpendicular to look vector */
    temp = look;
    MATH_VEC_DOT(angle, up, temp);
    MATH_VEC_MUL_SCALAR(temp, temp, angle);
    MATH_VEC_SUB(up, up, temp);
    MATH_VEC_UNITIZE(up);

    /* Generate a temporary side vector */
    MATH_VEC_CROSS(side, up, look);

    /* Apply tilt to up vector - negate angle to make positive angles clockwise */
    s = sin(-camera->tilt * MATH_DEG2RAD);
    c = cos(-camera->tilt * MATH_DEG2RAD);
    MATH_VEC_MUL_SCALAR(up, up, c);
    MATH_VEC_MUL_SCALAR(side, side, s);
    MATH_VEC_ADD(up, up, side);

    /* Create final side vector */
    MATH_VEC_CROSS(side, up, look);

    /* look direction */
    camera->view_list[0].top_l = look;

    /* fov is milimeters along the horizontal axis to display */
    /* left (side) */
    MATH_VEC_MUL_SCALAR(temp, side, (camera->aspect * camera->fov * 0.5));
    MATH_VEC_ADD(camera->view_list[0].pos, camera->pos, temp);
    /* and (up) */
    MATH_VEC_MUL_SCALAR(temp, up, (camera->fov * 0.5));
    MATH_VEC_ADD(camera->view_list[0].pos, camera->view_list[0].pos, temp);

    /* compute step vectors for camera position */

    /* X */
    MATH_VEC_MUL_SCALAR(camera->view_list[0].step_x, side, (-camera->fov * camera->aspect / (tfloat)camera->w));

    /* Y */
    MATH_VEC_MUL_SCALAR(camera->view_list[0].step_y, up, (-camera->fov / (tfloat)camera->h));
}


static void render_camera_prep_persp(render_camera_t *camera)
{
    TIE_3 look, up, side, temp, topl, topr, botl;
    tfloat angle, s, c;


    /* Generate unitized look vector */
    MATH_VEC_SUB(look, camera->focus, camera->pos);
    MATH_VEC_UNITIZE(look);

    /* Generate standard up vector */
    up.v[0] = 0;
    up.v[1] = 0;
    up.v[2] = 1;

    /* Make unitized up vector perpendicular to look vector */
    temp = look;
    MATH_VEC_DOT(angle, up, temp);
    MATH_VEC_MUL_SCALAR(temp, temp, angle);
    MATH_VEC_SUB(up, up, temp);
    MATH_VEC_UNITIZE(up);

    /* Generate a temporary side vector */
    MATH_VEC_CROSS(side, up, look);

    /* Apply tilt to up vector - negate angle to make positive angles clockwise */
    s = sin(-camera->tilt * MATH_DEG2RAD);
    c = cos(-camera->tilt * MATH_DEG2RAD);
    MATH_VEC_MUL_SCALAR(up, up, c);
    MATH_VEC_MUL_SCALAR(side, side, s);
    MATH_VEC_ADD(up, up, side);

    /* Create final side vector */
    MATH_VEC_CROSS(side, up, look);

    /* Compute sine and cosine terms for field of view */
    s = sin(camera->fov*MATH_DEG2RAD);
    c = cos(camera->fov*MATH_DEG2RAD);

    /* Up, Look, and Side vectors are complete, generate Top Left reference vector */
    topl.v[0] = s*up.v[0] + camera->aspect*s*side.v[0] + c*look.v[0];
    topl.v[1] = s*up.v[1] + camera->aspect*s*side.v[1] + c*look.v[1];
    topl.v[2] = s*up.v[2] + camera->aspect*s*side.v[2] + c*look.v[2];

    topr.v[0] = s*up.v[0] - camera->aspect*s*side.v[0] + c*look.v[0];
    topr.v[1] = s*up.v[1] - camera->aspect*s*side.v[1] + c*look.v[1];
    topr.v[2] = s*up.v[2] - camera->aspect*s*side.v[2] + c*look.v[2];

    botl.v[0] = -s*up.v[0] + camera->aspect*s*side.v[0] + c*look.v[0];
    botl.v[1] = -s*up.v[1] + camera->aspect*s*side.v[1] + c*look.v[1];
    botl.v[2] = -s*up.v[2] + camera->aspect*s*side.v[2] + c*look.v[2];

    MATH_VEC_UNITIZE(topl);
    MATH_VEC_UNITIZE(botl);
    MATH_VEC_UNITIZE(topr);

    /* Store Camera Position */
    camera->view_list[0].pos = camera->pos;

    /* Store the top left vector */
    camera->view_list[0].top_l = topl;

    /* Generate stepx and stepy vectors for sampling each pixel */
    MATH_VEC_SUB(camera->view_list[0].step_x, topr, topl);
    MATH_VEC_SUB(camera->view_list[0].step_y, botl, topl);

    /* Divide stepx and stepy by the number of pixels */
    MATH_VEC_MUL_SCALAR(camera->view_list[0].step_x, camera->view_list[0].step_x, 1.0 / camera->w);
    MATH_VEC_MUL_SCALAR(camera->view_list[0].step_y, camera->view_list[0].step_y, 1.0 / camera->h);
    return;
}


static void render_camera_prep_persp_dof(render_camera_t *camera)
{
    TIE_3 look, up, side, dof_look, dof_up, dof_side, dof_topl, dof_topr, dof_botl, temp, step_x, step_y, topl, topr, botl;
    tfloat angle, mag, sfov, cfov, sdof, cdof;
    uint32_t i, n;


    /* Generate unitized look vector */
    MATH_VEC_SUB(dof_look, camera->focus, camera->pos);
    MATH_VEC_UNITIZE(dof_look);

    /* Generate standard up vector */
    dof_up.v[0] = 0;
    dof_up.v[1] = 0;
    dof_up.v[2] = 1;

    /* Make unitized up vector perpendicular to look vector */
    temp = dof_look;
    MATH_VEC_DOT(angle, dof_up, temp);
    MATH_VEC_MUL_SCALAR(temp, temp, angle);
    MATH_VEC_SUB(dof_up, dof_up, temp);
    MATH_VEC_UNITIZE(dof_up);

    /* Generate a temporary side vector */
    MATH_VEC_CROSS(dof_side, dof_up, dof_look);

    /* Apply tilt to up vector - negate angle to make positive angles clockwise */
    sdof = sin(-camera->tilt * MATH_DEG2RAD);
    cdof = cos(-camera->tilt * MATH_DEG2RAD);
    MATH_VEC_MUL_SCALAR(dof_up, dof_up, cdof);
    MATH_VEC_MUL_SCALAR(dof_side, dof_side, sdof);
    MATH_VEC_ADD(dof_up, dof_up, dof_side);

    /* Create final side vector */
    MATH_VEC_CROSS(dof_side, dof_up, dof_look);

    /*
     * Generage a camera position, top left vector, and step vectors for each DOF sample
     */

    /* Obtain magnitude of reverse look vector */
    MATH_VEC_SUB(dof_look, camera->pos, camera->focus);
    MATH_VEC_MAG(mag, dof_look);
    MATH_VEC_UNITIZE(dof_look);


    /* Compute sine and cosine terms for field of view */
    sdof = sin(camera->dof*MATH_DEG2RAD);
    cdof = cos(camera->dof*MATH_DEG2RAD);


    /* Up, Look, and Side vectors are complete, generate Top Left reference vector */
    dof_topl.v[0] = sdof*dof_up.v[0] + sdof*dof_side.v[0] + cdof*dof_look.v[0];
    dof_topl.v[1] = sdof*dof_up.v[1] + sdof*dof_side.v[1] + cdof*dof_look.v[1];
    dof_topl.v[2] = sdof*dof_up.v[2] + sdof*dof_side.v[2] + cdof*dof_look.v[2];

    dof_topr.v[0] = sdof*dof_up.v[0] - sdof*dof_side.v[0] + cdof*dof_look.v[0];
    dof_topr.v[1] = sdof*dof_up.v[1] - sdof*dof_side.v[1] + cdof*dof_look.v[1];
    dof_topr.v[2] = sdof*dof_up.v[2] - sdof*dof_side.v[2] + cdof*dof_look.v[2];

    dof_botl.v[0] = -sdof*dof_up.v[0] + sdof*dof_side.v[0] + cdof*dof_look.v[0];
    dof_botl.v[1] = -sdof*dof_up.v[1] + sdof*dof_side.v[1] + cdof*dof_look.v[1];
    dof_botl.v[2] = -sdof*dof_up.v[2] + sdof*dof_side.v[2] + cdof*dof_look.v[2];

    MATH_VEC_UNITIZE(dof_topl);
    MATH_VEC_UNITIZE(dof_botl);
    MATH_VEC_UNITIZE(dof_topr);

    MATH_VEC_SUB(step_x, dof_topr, dof_topl);
    MATH_VEC_SUB(step_y, dof_botl, dof_topl);

    for (i = 0; i < RENDER_CAMERA_DOF_SAMPLES; i++)
    {
	for (n = 0; n < RENDER_CAMERA_DOF_SAMPLES; n++)
	{
	    /* Generate virtual camera position for this depth of field sample */
	    MATH_VEC_MUL_SCALAR(temp, step_x, ((tfloat)i/(tfloat)(RENDER_CAMERA_DOF_SAMPLES-1)));
	    MATH_VEC_ADD(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, dof_topl, temp);
	    MATH_VEC_MUL_SCALAR(temp, step_y, ((tfloat)n/(tfloat)(RENDER_CAMERA_DOF_SAMPLES-1)));
	    MATH_VEC_ADD(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, temp);
	    MATH_VEC_UNITIZE(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos);
	    MATH_VEC_MUL_SCALAR(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, mag);
	    MATH_VEC_ADD(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos, camera->focus);

	    /* Generate unitized look vector */
	    MATH_VEC_SUB(look, camera->focus, camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].pos);
	    MATH_VEC_UNITIZE(look);

	    /* Generate standard up vector */
	    up.v[0] = 0;
	    up.v[1] = 0;
	    up.v[2] = 1;

	    /* Make unitized up vector perpendicular to look vector */
	    temp = look;
	    MATH_VEC_DOT(angle, up, temp);
	    MATH_VEC_MUL_SCALAR(temp, temp, angle);
	    MATH_VEC_SUB(up, up, temp);
	    MATH_VEC_UNITIZE(up);

	    /* Generate a temporary side vector */
	    MATH_VEC_CROSS(side, up, look);

	    /* Apply tilt to up vector - negate angle to make positive angles clockwise */
	    sfov = sin(-camera->tilt * MATH_DEG2RAD);
	    cfov = cos(-camera->tilt * MATH_DEG2RAD);
	    MATH_VEC_MUL_SCALAR(up, up, cfov);
	    MATH_VEC_MUL_SCALAR(side, side, sfov);
	    MATH_VEC_ADD(up, up, side);

	    /* Create final side vector */
	    MATH_VEC_CROSS(side, up, look);

	    /* Compute sine and cosine terms for field of view */
	    sfov = sin(camera->fov*MATH_DEG2RAD);
	    cfov = cos(camera->fov*MATH_DEG2RAD);


	    /* Up, Look, and Side vectors are complete, generate Top Left reference vector */
	    topl.v[0] = sfov*up.v[0] + camera->aspect*sfov*side.v[0] + cfov*look.v[0];
	    topl.v[1] = sfov*up.v[1] + camera->aspect*sfov*side.v[1] + cfov*look.v[1];
	    topl.v[2] = sfov*up.v[2] + camera->aspect*sfov*side.v[2] + cfov*look.v[2];

	    topr.v[0] = sfov*up.v[0] - camera->aspect*sfov*side.v[0] + cfov*look.v[0];
	    topr.v[1] = sfov*up.v[1] - camera->aspect*sfov*side.v[1] + cfov*look.v[1];
	    topr.v[2] = sfov*up.v[2] - camera->aspect*sfov*side.v[2] + cfov*look.v[2];

	    botl.v[0] = -sfov*up.v[0] + camera->aspect*sfov*side.v[0] + cfov*look.v[0];
	    botl.v[1] = -sfov*up.v[1] + camera->aspect*sfov*side.v[1] + cfov*look.v[1];
	    botl.v[2] = -sfov*up.v[2] + camera->aspect*sfov*side.v[2] + cfov*look.v[2];

	    MATH_VEC_UNITIZE(topl);
	    MATH_VEC_UNITIZE(botl);
	    MATH_VEC_UNITIZE(topr);

	    /* Store the top left vector */
	    camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].top_l = topl;

	    /* Generate stepx and stepy vectors for sampling each pixel */
	    MATH_VEC_SUB(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].step_x, topr, topl);
	    MATH_VEC_SUB(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].step_y, botl, topl);

	    /* Divide stepx and stepy by the number of pixels */
	    MATH_VEC_MUL_SCALAR(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].step_x, camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].step_x, 1.0 / camera->w);
	    MATH_VEC_MUL_SCALAR(camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].step_y, camera->view_list[i*RENDER_CAMERA_DOF_SAMPLES+n].step_y, 1.0 / camera->h);
	}
    }
}


void render_camera_prep(render_camera_t *camera)
{
    /* Generate an aspect ratio coefficient */
    camera->aspect = (tfloat)camera->w / (tfloat)camera->h;

    if (camera->type == RENDER_CAMERA_ORTHOGRAPHIC)
	render_camera_prep_ortho(camera);

    if (camera->type == RENDER_CAMERA_PERSPECTIVE)
    {
	if (camera->dof <= 0.0)
	{
	    render_camera_prep_persp(camera);
	}
	else
	{
	    /* Generate camera positions for depth of field - Handle this better */
	    camera->view_num = RENDER_CAMERA_DOF_SAMPLES*RENDER_CAMERA_DOF_SAMPLES;
	    camera->view_list = (render_camera_view_t *)bu_malloc(sizeof(render_camera_view_t) * camera->view_num, "camera view");

	    render_camera_prep_persp_dof(camera);
	}
    }
}


void* render_camera_render_thread(void *ptr)
{
    render_camera_thread_data_t *td;
    int d, n, res_ind, scanline, v_scanline;
    TIE_3 pixel, accum, v1, v2;
    tie_ray_t ray;
    tfloat view_inv;


    td = (render_camera_thread_data_t *)ptr;
    view_inv = 1.0 / td->camera->view_num;


    res_ind = 0;
/* row, vertical */
/*
  for (i = td->tile->orig_y; i < td->tile->orig_y + td->tile->size_y; i++)
  {
*/
    while (1)
    {
	/* Determine if this scanline should be computed by this thread */
	pthread_mutex_lock(&td->mut);
	if (*td->scanline == td->tile->size_y)
	{
	    pthread_mutex_unlock(&td->mut);
	    return(0);
	}
	else
	{
	    scanline = *td->scanline;
	    (*td->scanline)++;
	}
	pthread_mutex_unlock(&td->mut);

	v_scanline = scanline + td->tile->orig_y;
	if (td->tile->format == RENDER_CAMERA_BIT_DEPTH_24)
	{
	    res_ind = 3*scanline*td->tile->size_x;
	}
	else if (td->tile->format == RENDER_CAMERA_BIT_DEPTH_128)
	{
	    res_ind = 4*scanline*td->tile->size_x;
	}


	/* optimization if there is no depth of field being applied */
	if (td->camera->view_num == 1)
	{
	    MATH_VEC_MUL_SCALAR(v1, td->camera->view_list[0].step_y, v_scanline);
	    MATH_VEC_ADD(v1, v1, td->camera->view_list[0].top_l);
	}


	/* scanline, horizontal, each pixel */
	for (n = td->tile->orig_x; n < td->tile->orig_x + td->tile->size_x; n++)
	{
	    /* depth of view samples */
	    if (td->camera->view_num > 1)
	    {
		MATH_VEC_SET(accum, 0, 0, 0);

		for (d = 0; d < td->camera->view_num; d++)
		{
		    MATH_VEC_MUL_SCALAR(ray.dir, td->camera->view_list[d].step_y, v_scanline);
		    MATH_VEC_ADD(ray.dir, ray.dir, td->camera->view_list[d].top_l);
		    MATH_VEC_MUL_SCALAR(v1, td->camera->view_list[d].step_x, n);
		    MATH_VEC_ADD(ray.dir, ray.dir, v1);

		    MATH_VEC_SET(pixel, RENDER_CAMERA_BGR, RENDER_CAMERA_BGG, RENDER_CAMERA_BGB);

		    ray.pos = td->camera->view_list[d].pos;
		    ray.depth = 0;
		    MATH_VEC_UNITIZE(ray.dir);

		    /* Compute pixel value using this ray */
		    td->camera->render.work(&td->camera->render, td->tie, &ray, &pixel);

		    MATH_VEC_ADD(accum, accum, pixel);
		}

		/* Find Mean value of all views */
		MATH_VEC_MUL_SCALAR(pixel, accum, view_inv);
	    }
	    else
	    {
		if (td->camera->type == RENDER_CAMERA_PERSPECTIVE)
		{
		    MATH_VEC_MUL_SCALAR(v2, td->camera->view_list[0].step_x, n);
		    MATH_VEC_ADD(ray.dir, v1, v2);

		    MATH_VEC_SET(pixel, RENDER_CAMERA_BGR, RENDER_CAMERA_BGG, RENDER_CAMERA_BGB);

		    ray.pos = td->camera->view_list[0].pos;
		    ray.depth = 0;
		    MATH_VEC_UNITIZE(ray.dir);

		    /* Compute pixel value using this ray */
		    td->camera->render.work(&td->camera->render, td->tie, &ray, &pixel);
		} else {
		    ray.pos = td->camera->view_list[0].pos;
		    ray.dir = td->camera->view_list[0].top_l;

		    MATH_VEC_MUL_SCALAR(v1, td->camera->view_list[0].step_x, n);
		    MATH_VEC_MUL_SCALAR(v2, td->camera->view_list[0].step_y, v_scanline);
		    MATH_VEC_ADD(ray.pos, ray.pos, v1);
		    MATH_VEC_ADD(ray.pos, ray.pos, v2);

		    MATH_VEC_SET(pixel, RENDER_CAMERA_BGR, RENDER_CAMERA_BGG, RENDER_CAMERA_BGB);
		    ray.depth = 0;

		    /* Compute pixel value using this ray */
		    td->camera->render.work(&td->camera->render, td->tie, &ray, &pixel);
		}
	    }


	    if (td->tile->format == RENDER_CAMERA_BIT_DEPTH_24)
	    {
		if (pixel.v[0] > 1) pixel.v[0] = 1;
		if (pixel.v[1] > 1) pixel.v[1] = 1;
		if (pixel.v[2] > 1) pixel.v[2] = 1;
		((char *)(td->res_buf))[res_ind+0] = (unsigned char)(255 * pixel.v[0]);
		((char *)(td->res_buf))[res_ind+1] = (unsigned char)(255 * pixel.v[1]);
		((char *)(td->res_buf))[res_ind+2] = (unsigned char)(255 * pixel.v[2]);
		res_ind += 3;
	    }
	    else if (td->tile->format == RENDER_CAMERA_BIT_DEPTH_128)
	    {
		tfloat alpha;

		alpha = 1.0;

		((tfloat *)(td->res_buf))[res_ind + 0] = pixel.v[0];
		((tfloat *)(td->res_buf))[res_ind + 1] = pixel.v[1];
		((tfloat *)(td->res_buf))[res_ind + 2] = pixel.v[2];
		((tfloat *)(td->res_buf))[res_ind + 3] = alpha;

		res_ind += 4;
	    }
/*          printf("Pixel: [%d, %d, %d]\n", rgb[0], rgb[1], rgb[2]); */

	}
    }

    return(0);
}


void render_camera_render(render_camera_t *camera, tie_t *tie, camera_tile_t *tile, tienet_buffer_t *result)
{
    render_camera_thread_data_t td;
    unsigned int i, scanline, ind;

    ind = result->ind;

    /* Allocate storage for results */
    if (tile->format == RENDER_CAMERA_BIT_DEPTH_24)
    {
	ind += 3 * tile->size_x * tile->size_y + sizeof(camera_tile_t);
    }
    else if (tile->format == RENDER_CAMERA_BIT_DEPTH_128)
    {
	ind += 4 * sizeof(tfloat) * tile->size_x * tile->size_y + sizeof(camera_tile_t);
    }

    TIENET_BUFFER_SIZE((*result), ind);

    TCOPY(camera_tile_t, tile, 0, result->data, result->ind);
    result->ind += sizeof(camera_tile_t);

    td.tie = tie;
    td.camera = camera;
    td.tile = tile;
    td.res_buf = &((char *)result->data)[result->ind];
    scanline = 0;
    td.scanline = &scanline;
    pthread_mutex_init(&td.mut, 0);

    /* Launch Render threads */
    if (camera->thread_num > 1)
    {
	for (i = 0; i < camera->thread_num; i++)
	    pthread_create(&render_tlist[i], NULL, render_camera_render_thread, &td);
	for (i = 0; i < camera->thread_num; i++)
	    pthread_join(render_tlist[i], NULL);
    } else {
	render_camera_render_thread(&td);
    }

    result->ind = ind;
    pthread_mutex_destroy(&td.mut);
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
