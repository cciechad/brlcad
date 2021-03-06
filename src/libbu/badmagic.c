/*                      B A D M A G I C . C
 * BRL-CAD
 *
 * Copyright (c) 2004-2008 United States Government as represented by
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
/** @addtogroup magic */
/** @{ */
/** @file badmagic.c
 *
 * Routines involved with handling "magic numbers" used to identify
 * various in-memory data structures.
 *
 */

#include "common.h"

#include "bio.h"

#include "magic.h"


#define MAGICBUFSIZ 512


void
bu_badmagic(const unsigned long *ptr, unsigned long magic, const char *str, const char *file, int line)
{
    char buf[MAGICBUFSIZ];

    if ( !(ptr) )  {
	snprintf(buf, MAGICBUFSIZ, "ERROR: NULL %s pointer, file %s, line %d\n",
		 str, file, line );
	bu_bomb(buf);
    }
    if ( ((size_t)(ptr)) & (sizeof(unsigned long)-1) )  {
	snprintf(buf, MAGICBUFSIZ, "ERROR: x%lx mis-aligned %s pointer, file %s, line %d\n",
		 (unsigned long)ptr, str, file, line );
	bu_bomb(buf);
    }
    if ( *(ptr) != (unsigned long)(magic) )  {
	snprintf(buf, MAGICBUFSIZ, "ERROR: bad pointer x%lx: s/b %s(x%lx), was %s(x%lx), file %s, line %d\n",
		 (unsigned long)ptr,
		 str, magic,
		 bu_identify_magic( (unsigned long)*(ptr) ), *(ptr),
		 file, line );
	bu_bomb(buf);
    }
}

/** @} */
/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
