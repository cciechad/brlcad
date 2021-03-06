/*                        S C R I P T . L
 * BRL-CAD
 *
 * Copyright (c) 2005-2008 United States Government as represented by
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
/** @file script.l
 *
 */
%{
/*                        S C R I P T . C
 * BRL-CAD
 *
 * Copyright (c) 2005-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; see the file named COPYING for more
 * information.
 *
 */
/** @file script.c
 *
 */

#include "../tab/tokens.h"

%}

D	[0-9]
A	[a-zA-Z]
L	[a-zA-Z0-9_\/\.\(\)\[\]\{\}]

%%

-?{D}*\.{D}+e[\+-]{D}+	return FLOAT;
-?{D}*\.{D}+	return FLOAT;
-?0[Xx]{D}+	return INT;
-?0{D}+		return INT;
-?{D}+		return INT;
^!.*		return SHELL;
#.*		return COMMENT;
start		return START;
viewsize	return VIEWSIZE;
eye_pt		return EYEPT;
lookat_pt	return LOOKATPT;
viewrot		return VIEWROT;
orientation	return ORIENTATION;
end		return END;
tree		return TREE;
multiview	return MULTIVIEW;
anim		return ANIM;
matrix		return MATRIX;
rmul		return RMUL;
lmul		return LMUL;
rboth		return RBOTH;
rstack		return RSTACK;
rarc		return RARC;
clean		return CLEAN;
set		return SET;
width		return WIDTH;
height		return HEIGHT;
perspective	return PERSPECTIVE;
angle		return ANGLE;
ae		return AE;
opt		return OPT;
{A}[a-zA-Z0-9_]*	return STRING;
\/{A}{L}*	return PATH;
{A}{L}*		return PATH;
;		return SEMI;
[ \t\n]		;

%%

/*
 * Local Variables:
 * tab-width: 8
 * mode: C
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
