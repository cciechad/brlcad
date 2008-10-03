/*                          M A I N . C
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
/** @file main.c
 *
 */

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#endif

#include "bio.h"
#include "bu.h"

#include "rise.h"
#include "slave.h"
#include "tienet.h"


#ifdef HAVE_GETOPT_LONG
static struct option longopts[] =
{
    { "help",	no_argument,		NULL, 'h' },
    { "port",	required_argument,	NULL, 'p' },
    { "threads",	required_argument,	NULL, 't' },
    { "version",	no_argument,		NULL, 'v' },
};
#endif
static char shortopts[] = "Xdhp:t:v";


static void finish(int sig) {
    bu_exit(EXIT_FAILURE, "Collected signal %d, aborting!\n", sig);
}


static void help() {
    printf("%s\n", RISE_VER_DETAIL);
    printf("%s", "usage: rise_slave [options] [host]\n\
  -v\t\tdisplay version\n\
  -h\t\tdisplay help\n\
  -P\t\tport number\n\
  -H ...\tconnect to master and shutdown when complete\n\
  -t ...\tnumber of threads to launch for processing\n");
}


int main(int argc, char **argv) {
    int		port = 0, c = 0, threads = 0;
    char		host[64], temp[64];


    /* Default Port */
    signal(SIGINT, finish);

    /* Initialize strings */
    host[0] = 0;
    port = 0;

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

	    case 't':
		bu_strlcpy(temp, optarg, sizeof(temp));
		threads = atoi(temp);
		if (threads < 0) threads = 0;
		if (threads > 32) threads = 32;
		break;

	    case 'h':
		help();
		return EXIT_SUCCESS;

	    case 'v':
		printf("%s\n", RISE_VER_DETAIL);
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

    if (!host[0]) {
	if (!port)
	    port = TN_SLAVE_PORT;
	printf("running as daemon.\n");
    } else {
	if (!port)
	    port = TN_MASTER_PORT;
    }

    rise_slave(port, host, threads);

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
