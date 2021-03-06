/*                         S L A V E . C
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
/** @file slave.c
 *
 * $Id: slave.c 32029 2008-07-29 02:43:32Z starseeker $
 */

#ifndef TIE_PRECISION
# define TIE_PRECISION 0
#endif

#include "slave.h"
#include "load.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "adrt.h"
#include "camera.h"
#include "adrt.h"
#include "tie.h"
#include "tienet.h"
#include "render_util.h"

typedef struct adrt_slave_project_s
{
    tie_t tie;
    render_camera_t camera;
    uint16_t last_frame;
    uint8_t active;
} adrt_slave_project_t;

uint32_t adrt_slave_threads;
adrt_slave_project_t adrt_workspace_list[ADRT_MAX_WORKSPACE_NUM];

void 
adrt_slave_free() 
{
    uint16_t i;

    for (i = 0; i < ADRT_MAX_WORKSPACE_NUM; i++)
	if (adrt_workspace_list[i].active)
	{
//      render_camera_free (&camera);
	}
}

void
adrt_slave_work(tienet_buffer_t *work, tienet_buffer_t *result)
{
    TIE_3 pos, foc;
    unsigned char rm, op;
    uint32_t ind, wlen;
    uint16_t wid;

    /* Length of work data */
    wlen = work->ind;
    ind = 0;

    /* Get work type */
    TCOPY(uint8_t, work->data, ind, &op, 0);
    ind += 1;

    /* Workspace ID */
    TCOPY(uint16_t, work->data, ind, &wid, 0);
    ind += 2;

    /* This will get improved later with caching */
    TIENET_BUFFER_SIZE((*result), 3); /* Copy op and wid, 3 bytes */
    memcpy(result->data, &work->data[0], 3);
    result->ind = ind;

    switch (op) {
	case ADRT_WORK_INIT:
	{
	    render_camera_init (&adrt_workspace_list[wid].camera, adrt_slave_threads);
	    if( slave_load (&adrt_workspace_list[wid].tie, (struct adrt_load_info *)work->data, wlen-ind) != 0 )
		bu_exit (1, "Failed to load geometry. Going into a flaming tailspin\n");
	    tie_prep (&adrt_workspace_list[wid].tie);
	    render_camera_prep (&adrt_workspace_list[wid].camera);
	    printf ("ready.\n");
	    result->ind = 0;

	    /* Mark the workspace as active so it can be cleaned up when the time comes. */
	    adrt_workspace_list[wid].active = 1;
	}
	break;

	case ADRT_WORK_STATUS:
	{
	    double loadavg;

	    getloadavg (&loadavg, 1);
	    printf ("load average: %f\n", loadavg);
	}
	break;
 
	case ADRT_WORK_SELECT:
	{
	    uint8_t c;
	    char string[255];
	    uint32_t n, i, num;

	    /* reset */
	    TCOPY(uint8_t, work->data, ind, &c, 0);
	    ind += 1;
	    if (c)
		for (i = 0; i < slave_load_mesh_num; i++)
		    slave_load_mesh_list[i].flags = 0;

	    /* number of strings to match */
	    TCOPY(uint32_t, work->data, ind, &num, 0);
	    ind += 4;

	    for (i = 0; i < num; i++)
	    {
		/* string length */
		TCOPY(uint8_t, work->data, ind, &c, 0);
		ind += 1;

		/* string */
		memcpy(string, &work->data[ind], c);
		ind += c;

		/* set select flag */
		for (n = 0; n < slave_load_mesh_num; n++)
		    if (strstr(slave_load_mesh_list[n].name, string) || c == 1)
			slave_load_mesh_list[n].flags = (slave_load_mesh_list[n].flags & 0x1) | ((slave_load_mesh_list[n].flags & 0x2) ^ 0x2);
	    }

	    /* zero length result */
	    result->ind = 0;
	}  
	break;

	case ADRT_WORK_SHOTLINE:
	{
	    tie_ray_t ray;
	    void *mesg;
	    int dlen;

	    mesg = NULL;

	    /* coordinates */
	    TCOPY(TIE_3, work->data, ind, ray.pos.v, 0);
	    ind += sizeof (TIE_3);
	    TCOPY(TIE_3, work->data, ind, ray.dir.v, 0);
	    ind += sizeof (TIE_3);
#if 0
	    printf ("pos: %.3f %.3f %.3f ... dir %.3f %.3f %.3f\n", ray.pos.v[0], ray.pos.v[1], ray.pos.v[2], ray.dir.v[0], ray.dir.v[1], ray.dir.v[2]);
#endif
	    /* Fire the shot */
	    ray.depth = 0;
	    render_util_shotline_list (&adrt_workspace_list[wid].tie, &ray, &mesg, &dlen);

	    /* Make room for shot data */
	    TIENET_BUFFER_SIZE((*result), result->ind + dlen + 2*sizeof (TIE_3));
	    memcpy(&result->data[result->ind], mesg, dlen);
	    result->ind += dlen;

	    TCOPY(TIE_3, &ray.pos, 0, result->data, result->ind);
	    result->ind += sizeof (TIE_3);

	    TCOPY(TIE_3, &ray.dir, 0, result->data, result->ind);
	    result->ind += sizeof (TIE_3);

	    free (mesg);
	}
	break;

	case ADRT_WORK_SPALL:
	{
#if 0
	    tie_ray_t ray;
	    tfloat angle;
	    void *mesg;
	    int dlen;

	    mesg = NULL;

	    /* position */
	    TCOPY(TIE_3, data, ind, &ray.pos, 0);
	    ind += sizeof (TIE_3);

	    /* direction */
	    TCOPY(TIE_3, data, ind, &ray.dir, 0);
	    ind += sizeof (TIE_3);

	    /* angle */
	    TCOPY(tfloat, data, ind, &angle, 0);
	    ind += sizeof (tfloat);

	    /* Fire the shot */
	    ray.depth = 0;
	    render_util_spall_list(tie, &ray, angle, &mesg, &dlen);

	    /* Make room for shot data */
	    *res_len = sizeof (common_work_t) + dlen;
	    *res_buf = (void *)realloc(*res_buf, *res_len);

	    ind = 0;

	    memcpy(&((char *)*res_buf)[ind], mesg, dlen);

	    free (mesg);
#endif
	}
	break;

	case ADRT_WORK_FRAME_ATTR:
	{
	    uint16_t image_w, image_h, image_format;

	    /* Image Size */
	    TCOPY(uint16_t, work->data, ind, &image_w, 0);
	    ind += 2;
	    TCOPY(uint16_t, work->data, ind, &image_h, 0);
	    ind += 2;
	    TCOPY(uint16_t, work->data, ind, &image_format, 0);
	    ind += 2;

	    adrt_workspace_list[wid].camera.w = image_w;
	    adrt_workspace_list[wid].camera.h = image_h;
	    render_camera_prep (&adrt_workspace_list[wid].camera);
	    result->ind = 0;
	}
	break;

	case ADRT_WORK_FRAME:
	{
	    camera_tile_t tile;
	    uint8_t type;
	    tfloat fov;

	    /* Camera type */
	    TCOPY(uint8_t, work->data, ind, &type, 0);
	    ind += 1;

	    /* Camera fov */
	    TCOPY(tfloat, work->data, ind, &fov, 0);
	    ind += sizeof (tfloat);

	    /* Camera position */
	    TCOPY(TIE_3, work->data, ind, &pos, 0);
	    ind += sizeof (TIE_3);

	    /* Camera Focus */
	    TCOPY(TIE_3, work->data, ind, &foc, 0);
	    ind += sizeof (TIE_3);

	    /* Update Rendering Method if it has Changed */
	    rm = work->data[ind];
	    ind += 1;

	    if (rm != adrt_workspace_list[wid].camera.rm || rm & 1<<7)
	    {
		rm = rm & ((1<<7)-1);

		adrt_workspace_list[wid].camera.render.free (&adrt_workspace_list[wid].camera.render);

		switch (rm) {
		    case RENDER_METHOD_DEPTH:
			render_depth_init(&adrt_workspace_list[wid].camera.render);
			break;

		    case RENDER_METHOD_COMPONENT:
			render_component_init(&adrt_workspace_list[wid].camera.render);
			break;

		    case RENDER_METHOD_FLOS:
		    {
			TIE_3 frag_pos;

			/* Extract shot position and direction */
			TCOPY(TIE_3, work->data, ind, &frag_pos, 0);
			ind += sizeof (TIE_3);
			render_flos_init(&adrt_workspace_list[wid].camera.render, frag_pos);
		    }
		    break;

		    case RENDER_METHOD_GRID:
			render_grid_init(&adrt_workspace_list[wid].camera.render);
			break;

		    case RENDER_METHOD_NORMAL:
			render_normal_init(&adrt_workspace_list[wid].camera.render);
			break;

		    case RENDER_METHOD_PATH:
			render_path_init(&adrt_workspace_list[wid].camera.render, 12);
			break;

		    case RENDER_METHOD_PHONG:
			render_phong_init(&adrt_workspace_list[wid].camera.render);
			break;

		    case RENDER_METHOD_CUT:
		    {
			TIE_3 shot_pos, shot_dir;

			/* Extract shot position and direction */
			TCOPY(TIE_3, work->data, ind, &shot_pos, 0);
			ind += sizeof (TIE_3);

			TCOPY(TIE_3, work->data, ind, &shot_dir, 0);
			ind += sizeof (TIE_3);

			render_cut_init(&adrt_workspace_list[wid].camera.render, shot_pos, shot_dir);
		    }
		    break;

		    case RENDER_METHOD_SPALL:
		    {
			TIE_3 shot_pos, shot_dir;
			tfloat angle;

			/* Extract shot position and direction */
			TCOPY(TIE_3, work->data, ind, &shot_pos, 0);
			ind += sizeof (TIE_3);

			TCOPY(TIE_3, work->data, ind, &shot_dir, 0);
			ind += sizeof (TIE_3);

			TCOPY(tfloat, work->data, ind, &angle, 0);
			ind += sizeof (tfloat);

			render_spall_init (&adrt_workspace_list[wid].camera.render, shot_pos, shot_dir, angle);
		    }
		    break;

		    default:
			break;
		}

		adrt_workspace_list[wid].camera.rm = rm;
	    }

	    /* The portion of the image to be rendered */
	    ind = work->ind - sizeof (camera_tile_t);
	    TCOPY(camera_tile_t, work->data, ind, &tile, 0);
	    ind += sizeof (camera_tile_t);

	    /* Update camera if different frame */
	    if (tile.frame != adrt_workspace_list[wid].last_frame)
	    {
		adrt_workspace_list[wid].camera.type = type;
		adrt_workspace_list[wid].camera.fov = fov;
		adrt_workspace_list[wid].camera.pos = pos;
		adrt_workspace_list[wid].camera.focus = foc;
		render_camera_prep (&adrt_workspace_list[wid].camera);
	    }
	    adrt_workspace_list[wid].last_frame = tile.frame;

	    render_camera_render (&adrt_workspace_list[wid].camera, &adrt_workspace_list[wid].tie, &tile, result);
	}
	break;

	case ADRT_WORK_MINMAX:
	{
	    TCOPY(TIE_3, &adrt_workspace_list[wid].tie.min, 0, result->data, result->ind);
	    result->ind += sizeof (TIE_3);

	    TCOPY(TIE_3, &adrt_workspace_list[wid].tie.max, 0, result->data, result->ind);
	    result->ind += sizeof (TIE_3);
	}
	break;

	default:
	    break;
    }

#if 0
    gettimeofday(&tv, NULL);
    printf("[Work Units Completed: %.6d  Rays: %.5d k/sec %lld]\r", ++adrt_slave_completed, (int)((tfloat)tie->rays_fired / (tfloat)(1000 * (tv.tv_sec - adrt_slave_startsec + 1))), tie->rays_fired);
    fflush(stdout);
#endif
}

void 
adrt_slave(int port, char *host, int threads) 
{
    int i;
    adrt_slave_threads = threads;
    tienet_slave_init(port, host, adrt_slave_work, adrt_slave_free, ADRT_VER_KEY);

    /* Initialize all workspaces as inactive */
    for (i = 0; i < ADRT_MAX_WORKSPACE_NUM; i++)
	adrt_workspace_list[i].active = 0;

/*  slave_last_frame = 0; */
}

#if 0
void adrt_slave_mesg(void *mesg, unsigned int mesg_len) 
{
    short		op;
    TIE_3		foo;

    memcpy(&op, mesg, sizeof(short));

    switch (op) {
	case ADRT_WORK_SHOTLINE:
	{
	    int i, n, num, ind;
	    char name[256];
	    unsigned char c;

	    /* Reset all meshes hit flag */
	    for (i = 0; i < db.mesh_num; i++)
		db.mesh_list[i]->flags &= MESH_SELECT;

	    /* Read the data */
	    ind = sizeof(short);
	    memcpy(&num, &((unsigned char *)mesg)[ind], sizeof(int));

	    ind += sizeof(int);

	    for (i = 0; i < num; i++) 
	    {
		memcpy(&c, &((unsigned char *)mesg)[ind], 1);
		ind += 1;

		memcpy(name, &((unsigned char *)mesg)[ind], c);
		ind += c;

		/* set hit flag */
		for (n = 0; n < db.mesh_num; n++) {
		    if (!strcmp(db.mesh_list[n]->name, name)) {
			db.mesh_list[n]->flags |= MESH_HIT;
			continue;
		    }
		}
	    }
	}
	break;

#if 0 /* this isn't in the new version */
	case ISST_OP_SELECT:
	{
	    uint8_t c, t;
	    char string[256];
	    uint32_t n;

	    /* select or deslect */
	    memcpy(&t, &((uint8_t *)mesg)[2], 1);
	    /* string */
	    memcpy(&c, &((uint8_t *)mesg)[3], 1);
	    memcpy(string, &((uint8_t *)mesg)[4], c);

	    /* set select flag */
	    for (n = 0; n < db.mesh_num; n++)
		if (strstr(db.mesh_list[n]->name, string) || c == 1)
		    db.mesh_list[n]->flags = (db.mesh_list[n]->flags & MESH_SELECT) | t<<1;
	}
	break;
#endif ISST_OP_SELECT
	default:
	    break;
    }
}
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
