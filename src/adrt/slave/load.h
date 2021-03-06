/*                         L O A D . H
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
/** @file load.h
 *
 * $Id: load.h 32029 2008-07-29 02:43:32Z starseeker $
 */

#ifndef _SLAVE_LOAD_H
#define _SLAVE_LOAD_H

#include <inttypes.h>
#include "adrt.h"
#include "adrt_struct.h"

int slave_load(tie_t *tie, struct adrt_load_info *, uint32_t dlen);

extern uint32_t slave_load_mesh_num;
extern adrt_mesh_t *slave_load_mesh_list;

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
