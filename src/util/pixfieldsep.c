/*                   P I X F I E L D S E P . C
 * BRL-CAD
 *
 * Copyright (c) 2004-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/** @file pixfieldsep.c
 *
 * Separate an interlaced video image into two separate .pix files.
 *
 */

#include "common.h"

#include <stdlib.h>
#include "bio.h"

#include "bu.h"


FILE	*out1;
FILE	*out2;

char	*buf;
int	file_width = 720;
int	bytes_per_sample = 3;
int	doubleit = 0;

char	*even_file = "even.pix";
char	*odd_file = "odd.pix";

static char usage[] = "\
Usage: pixfieldsep [-w file_width] [-s square_size] [-# nbytes/pixel] \n\
	[-d] [even.pix odd.pix]\n";

int
get_args(int argc, register char **argv)
{
    register int c;

    while ( (c = bu_getopt( argc, argv, "ds:w:#:" )) != EOF )  {
	switch ( c )  {
	    case 'd':
		doubleit = 1;
		break;
	    case '#':
		bytes_per_sample = atoi(bu_optarg);
		break;
	    case 's':
		/* square file size */
		file_width = atoi(bu_optarg);
		break;
	    case 'w':
		file_width = atoi(bu_optarg);
		break;

	    default:		/* '?' */
		return(0);
	}
    }

    if ( bu_optind < argc )  {
	even_file = argv[bu_optind++];
    }
    if ( bu_optind < argc )  {
	odd_file = argv[bu_optind++];
    }

    if ( ++bu_optind <= argc )
	(void)fprintf( stderr, "pixfieldsep: excess argument(s) ignored\n" );

    return(1);		/* OK */
}

int
main(int argc, char **argv)
{
    register int	i;

    if ( !get_args( argc, argv ) )  {
	(void)fputs(usage, stderr);
	bu_exit ( 1, NULL );
    }

    if ( (out1 = fopen(even_file, "w")) == NULL )  {
	perror(even_file);
	bu_exit (1, NULL);
    }
    if ( (out2 = fopen(odd_file, "w")) == NULL )  {
	perror(odd_file);
	bu_exit (2, NULL);
    }

    buf = (char *)malloc( (file_width+1)*bytes_per_sample );

    for (;;)  {
	/* Even line */
	if ( fread( buf, bytes_per_sample, file_width, stdin ) != file_width )
	    break;
	for ( i=0; i <= doubleit; i++ )  {
	    if ( fwrite( buf, bytes_per_sample, file_width, out1 ) != file_width )  {
		perror("fwrite even");
		bu_exit (1, NULL);
	    }
	}
	/* Odd line */
	if ( fread( buf, bytes_per_sample, file_width, stdin ) != file_width )
	    break;
	for ( i=0; i <= doubleit; i++ )  {
	    if ( fwrite( buf, bytes_per_sample, file_width, out2 ) != file_width )  {
		perror("fwrite odd");
		bu_exit (1, NULL);
	    }
	}
    }
    bu_exit (0, NULL);
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
