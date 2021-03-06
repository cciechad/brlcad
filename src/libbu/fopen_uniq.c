/*                    F O P E N _ U N I Q . C
 * BRL-CAD
 *
 * Copyright (c) 2001-2008 United States Government as represented by
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
/** @addtogroup bu_log */
/** @{ */
/** @file fopen_uniq.c
 *
 * @brief Routine to open a unique filename.
 *
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>	/* for strerror() */
#include <ctype.h>
#include <math.h>
#include <string.h>

#ifdef HAVE_SYS_PARAM_H
#  include <sys/param.h>
#endif

#include "bu.h"

/**
 *  B U _ F O P E N _ U N I Q
 *@brief
 *  Open a file for output.  Assures that the file did not previously exist.
 *
 *  Typical Usages:
 @code
 *	static int n = 0;
 *	FILE *fp;
 *
 *	fp = bu_fopen_uniq("writing to %s for results", "output%d.pl", n++);
 *	...
 *	fclose(fp);
 *
 *
 *	fp = bu_fopen_uniq((char *)NULL, "output%d.pl", n++);
 *	...
 *	fclose(fp);
 @endcode
*/
FILE *
bu_fopen_uniq(const char *outfmt, const char *namefmt, int n)
{
    char filename[MAXPATHLEN];
    int fd;
    FILE *fp;

    if ( ! namefmt || ! *namefmt)
	bu_bomb("bu_uniq_file called with null string\n");

    bu_semaphore_acquire( BU_SEM_SYSCALL);

    /* NOTE: can't call bu_log because of the semaphore */

    snprintf(filename, MAXPATHLEN, namefmt, n);
    if ((fd = open(filename, O_RDWR|O_CREAT|O_EXCL, 0600)) < 0) {
	bu_exit(-1, "Cannot open %s, %s\n", filename, strerror(errno));
    }
    if ( (fp=fdopen(fd, "w")) == (FILE *)NULL) {
	bu_exit(-1, "%s", strerror(errno));
    }

    if (outfmt)
	fprintf(stderr, outfmt, filename);

    bu_semaphore_release( BU_SEM_SYSCALL);

    return fp;
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
