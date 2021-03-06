/*                     T E X T U R E _ I M A G E . C
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
/**
 * @file texture_image.c
 *
 *  Comments -
 *      Texture Library - Projects an Image onto a Surface
 *
 */

#include "texture_image.h"

#include <stdlib.h>
#include <string.h>
#include "umath.h"
#include "adrt_struct.h"

#include "bu.h"

void texture_image_init(texture_t *texture, short w, short h, unsigned char *image) {
    texture_image_t *td;

    texture->data = bu_malloc(sizeof(texture_image_t), "texture data");
    texture->free = texture_image_free;
    texture->work = (texture_work_t *)texture_image_work;

    td = (texture_image_t *)texture->data;
    td->w = w;
    td->h = h;
    td->image = (unsigned char *)bu_malloc(3*w*h, "texture image");
    memcpy(td->image, image, 3*w*h);
}


void texture_image_free(texture_t *texture) {
    texture_image_t *td;

    td = (texture_image_t *)texture->data;
    bu_free(td->image, "texture image");
    bu_free(texture->data, "texture data");
}


void texture_image_work(__TEXTURE_WORK_PROTOTYPE__) {
    texture_image_t *td;
    TIE_3 pt;
    tfloat u, v;
    int ind;


    td = (texture_image_t *)texture->data;


    /* Transform the Point */
    MATH_VEC_TRANSFORM(pt, id->pos, ADRT_MESH(mesh)->matinv);
    u = ADRT_MESH(mesh)->max.v[0] - ADRT_MESH(mesh)->min.v[0] > TIE_PREC ? (pt.v[0] - ADRT_MESH(mesh)->min.v[0]) / (ADRT_MESH(mesh)->max.v[0] - ADRT_MESH(mesh)->min.v[0]) : 0.0;
    v = ADRT_MESH(mesh)->max.v[1] - ADRT_MESH(mesh)->min.v[1] > TIE_PREC ? (pt.v[1] - ADRT_MESH(mesh)->min.v[1]) / (ADRT_MESH(mesh)->max.v[1] - ADRT_MESH(mesh)->min.v[1]) : 0.0;

    ind = 3*((int)((1.0 - v)*td->h)*td->w + (int)(u*td->w));

    pixel->v[0] = td->image[ind+2] / 255.0;
    pixel->v[1] = td->image[ind+1] / 255.0;
    pixel->v[2] = td->image[ind+0] / 255.0;
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
