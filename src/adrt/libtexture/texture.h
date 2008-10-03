/*                       T E X T U R E . H
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
/** @file texture.h
 *
 *  Comments -
 *      Texture Library - Main Include
 *
 */

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "texture_blend.h"
#include "texture_bump.h"
#include "texture_camo.h"
#include "texture_checker.h"
#include "texture_clouds.h"
#include "texture_gradient.h"
#include "texture_image.h"
#include "texture_mix.h"
#include "texture_stack.h"

#define TEXTURE_BLEND		0x0200
#define TEXTURE_BUMP		0x0201
#define TEXTURE_CAMO		0x0202
#define TEXTURE_CHECKER		0x0203
#define TEXTURE_CLOUDS		0x0204
#define TEXTURE_GRADIENT	0x0205
#define TEXTURE_IMAGE		0x0206
#define TEXTURE_MIX		0x0207
#define TEXTURE_REFLECT		0x0208
#define TEXTURE_STACK		0x0209

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
