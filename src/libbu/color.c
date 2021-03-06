/*                         C O L O R . C
 * BRL-CAD
 *
 * Copyright (c) 1997-2008 United States Government as represented by
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
/** @addtogroup color */
/** @{ */
/** @file color.c
 *
 *		Convert between RGB and HSV color models
 *
 *	R, G, and B are in {0, 1, ..., 255},
 *
 *	H is in [0.0, 360.0), and S and V are in [0.0, 1.0],
 *
 *	unless S = 0.0, in which case H = ACHROMATIC.
 *
 *	These two routines are adapted from:
 *	pp. 592-3 of J.D. Foley, A. van Dam, S.K. Feiner, and J.F. Hughes,
 *	_Computer graphics: principles and practice_, 2nd ed., Addison-Wesley,
 *	Reading, MA, 1990.
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "bu.h"
#include "vmath.h"


#define	ACHROMATIC	-1.0

#define	HUE	0
#define	SAT	1
#define	VAL	2

#define	RED	0
#define	GRN	1
#define	BLU	2


/**
 *			B U _ R G B _ T O _ H S V
 *
 */
void bu_rgb_to_hsv (unsigned char *rgb, fastf_t *hsv)
{
    fastf_t	red, grn, blu;
    fastf_t	*hue = &hsv[HUE];
    fastf_t	*sat = &hsv[SAT];
    fastf_t	*val = &hsv[VAL];
    fastf_t	max, min;
    fastf_t	delta;

    /*
     *	Compute value
     */
    max = min = red = ((fastf_t) rgb[RED]) / 255.0;

    grn = ((fastf_t) rgb[GRN]) / 255.0;
    if (grn < min)
	min = grn;
    else if (grn > max)
	max = grn;

    blu = ((fastf_t) rgb[BLU]) / 255.0;
    if (blu < min)
	min = blu;
    else if (blu > max)
	max = blu;

    *val = max;

    /*
     *	Compute saturation
     */
    delta = max - min;
    if (max > 0.0)
	*sat = delta / max;
    else
	*sat = 0.0;

    /*
     *	Compute hue
     */
    if (NEAR_ZERO(*sat, SMALL_FASTF))
	*hue = ACHROMATIC;
    else
    {
	if (NEAR_ZERO(red - max, SMALL_FASTF)) /* red == max */
	    *hue = (grn - blu) / delta;
	else if (NEAR_ZERO(grn - max, SMALL_FASTF)) /* grn == max */
	    *hue = 2.0 + (blu - red) / delta;
	else if (NEAR_ZERO(blu - max, SMALL_FASTF)) /* blu == max */
	    *hue = 4.0 + (red - grn) / delta;

	/*
	 *	Convert hue to degrees
	 */
	*hue *= 60.0;
	if (*hue < 0.0)
	    *hue += 360.0;
    }
}

/**
 *			B U _ H S V _ T O _ R G B
 *
 */
int bu_hsv_to_rgb (fastf_t *hsv, unsigned char *rgb)
{
    fastf_t	float_rgb[3];
    fastf_t	hue, sat, val;
    fastf_t	hue_frac;
    fastf_t	p, q, t;
    int		hue_int;

    hue = hsv[HUE];
    sat = hsv[SAT];
    val = hsv[VAL];

    if ((((hue < 0.0) || (hue > 360.0)) && (!NEAR_ZERO(hue - ACHROMATIC, SMALL_FASTF))) /* hue != ACHROMATIC */
	|| (sat < 0.0) || (sat > 1.0)
	|| (val < 0.0) || (val > 1.0)
	|| ((NEAR_ZERO(hue - ACHROMATIC, SMALL_FASTF)) && (sat > 0.0))) /* hue == ACHROMATIC */
    {
	bu_log("bu_hsv_to_rgb: Illegal HSV (%g, %g, %g)\n",
	       V3ARGS(hsv));
	return (0);
    }

    /* so hue == ACHROMATIC (or is ignored)	*/
    if (NEAR_ZERO(sat, SMALL_FASTF)) {
	VSETALL(float_rgb, val)
	    } else {
		if (NEAR_ZERO(hue - 360.0, SMALL_FASTF))
		    hue = 0.0;
		hue /= 60.0;
		hue_int = floor((double) hue);
		hue_frac = hue - hue_int;
		p = val * (1.0 - sat);
		q = val * (1.0 - (sat * hue_frac));
		t = val * (1.0 - (sat * (1.0 - hue_frac)));
		switch (hue_int) {
		    case 0: VSET(float_rgb, val, t, p); break;
		    case 1: VSET(float_rgb, q, val, p); break;
		    case 2: VSET(float_rgb, p, val, t); break;
		    case 3: VSET(float_rgb, p, q, val); break;
		    case 4: VSET(float_rgb, t, p, val); break;
		    case 5: VSET(float_rgb, val, p, q); break;
		    default:
			bu_log("%s:%d: This shouldn't happen\n",
			       __FILE__, __LINE__);
			bu_bomb("unexpected condition encountered in bu_hsv_to_rgb\n");
		}
	    }

    rgb[RED] = float_rgb[RED] * 255;
    rgb[GRN] = float_rgb[GRN] * 255;
    rgb[BLU] = float_rgb[BLU] * 255;

    return (1);
}

/**
 *			B U _ S T R _ T O _ R G B
 *
 */
int bu_str_to_rgb (char *str, unsigned char *rgb)
{
    int	num;
    int	r, g, b;

    r = g = b = -1;
    while (isspace(*str))
	++str;

    if (*str == '#')
    {
	if (strlen(++str) != 6)
	    return 0;
	num = sscanf(str, "%02x%02x%02x", (unsigned int *)&r, (unsigned int *)&g, (unsigned int *)&b);
#if 0
	bu_log("# notation: I read %d of %d, %d, %d\n", num, r, g, b);
#endif
    }
    else if (isdigit(*str))
    {
	num = sscanf(str, "%d/%d/%d", &r, &g, &b);
#if 0
	bu_log("slash separation: I read %d of %d, %d, %d\n", num, r, g, b);
#endif
	if (num == 1)
	{
	    num = sscanf(str, "%d %d %d", &r, &g, &b);
#if 0
	    bu_log("blank separation: I read %d of %d, %d, %d\n", num, r, g, b);
#endif
	}
	VSET(rgb, r, g, b);
	if ((r < 0) || (r > 255)
	    || (g < 0) || (g > 255)
	    || (b < 0) || (b > 255))
	    return 0;
    }
    else
	return 0;

    VSET(rgb, r, g, b);
    return 1;
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
