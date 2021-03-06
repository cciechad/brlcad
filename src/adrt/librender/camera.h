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

#ifndef _RENDER_CAMERA_H
#define _RENDER_CAMERA_H

#include "tie.h"
#include "tienet.h"
#include "render.h"

#define	RENDER_CAMERA_DOF_SAMPLES	13
#define RENDER_CAMERA_PERSPECTIVE	0x0
#define RENDER_CAMERA_ORTHOGRAPHIC	0x1
#define	RENDER_CAMERA_BGR		0.00
#define	RENDER_CAMERA_BGG		0.05
#define	RENDER_CAMERA_BGB		0.15

#define RENDER_CAMERA_BIT_DEPTH_24	0
#define RENDER_CAMERA_BIT_DEPTH_128	1


typedef struct render_camera_view_s
{
    TIE_3 step_x;
    TIE_3 step_y;
    TIE_3 pos;
    TIE_3 top_l;
} render_camera_view_t;


typedef struct render_camera_s
{
    uint8_t type;
    TIE_3 pos;
    TIE_3 focus;
    tfloat tilt;
    tfloat fov;
    tfloat aspect;
    tfloat dof;
    uint8_t thread_num;
    uint16_t view_num;
    render_camera_view_t *view_list;
    render_t render;
    uint32_t rm;
    uint16_t w;
    uint16_t h;
} render_camera_t;


typedef struct camera_tile_s
{
    uint16_t orig_x;
    uint16_t orig_y;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t format;
    uint16_t frame;
} camera_tile_t;


typedef struct render_camera_thread_data_s
{
    render_camera_t *camera;
    tie_t *tie;
    camera_tile_t *tile;
    void *res_buf;
    unsigned int *scanline;
    pthread_mutex_t mut;
} render_camera_thread_data_t;


void render_camera_init(render_camera_t *camera, int threads);
void render_camera_free(render_camera_t *camera);
void render_camera_prep(render_camera_t *camera);
void render_camera_render(render_camera_t *camera, tie_t *tie, camera_tile_t *tile, tienet_buffer_t *result);

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
