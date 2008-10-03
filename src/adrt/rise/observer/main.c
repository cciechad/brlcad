/*                          M A I N . C
 * BRL-CAD / ADRT
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
/** @file main.c
 *
 */

#include "common.h"

#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "bio.h"
#include "bu.h"

#include "rise.h"
#include "observer.h"
#include "tienet.h"


#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#ifdef HAVE_GETOPT_LONG
static struct option longopts[] =
{
    { "help",	no_argument,		NULL, 'h' },
    { "port",	required_argument,	NULL, 'p' },
    { "version",	no_argument,		NULL, 'v' },
};
#endif
static char shortopts[] = "Xhp:v";


static void finish(int sig) {
    printf("Collected signal %d, aborting!\n", sig);
    exit(EXIT_FAILURE);
}


static void help() {
    printf("%s\n", RISE_VER_DETAIL);
    printf("%s\n", "usage: rise_observer [options]\n\
  -v\t\tdisplay version\n\
  -h\t\tdisplay help\n\
  -P\t\tport number\n\
  -H ...\tconnect to master as observer\n");
}


int main(int argc, char **argv) {
    int		port = 0, c = 0;
    char		host[64], temp[64];


    signal(SIGINT, finish);

    if (argc == 1) {
	help();
	return EXIT_FAILURE;
    }

    /* Initialize strings */
    host[0] = 0;
    port = RISE_OBSERVER_PORT;

    /* Parse command line options */

    while ((c =
#ifdef HAVE_GETOPT_LONG
	    getopt_long(argc, argv, shortopts, longopts, NULL)
#else
	    getopt(argc, argv, shortopts)
#endif
	       )!= -1)
    {
	switch (c) {
	    case 'p':
		port = atoi(optarg);
		break;

	    case 'H':
		bu_strlcpy(host, optarg, sizeof(host));
		break;

	    case 'h':
		help();
		return EXIT_SUCCESS;

	    case 'v':
		printf("RISE %s - Copyright (C) U.S Army Research Laboratory (2003 - 2004)\n", VERSION);
		return EXIT_SUCCESS;

	    default:
		help();
		return EXIT_FAILURE;
	}
    }

    argc -= optind;
    argv += optind;

    if (argc)
	bu_strlcpy(host, argv[0], sizeof(host));

    if (host[0]) {
	printf("Observer mode: connecting to %s on port %d\n", host, port);
	rise_observer(host, port);
    } else {
	printf("must supply arguments: -H hostname\n");
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
