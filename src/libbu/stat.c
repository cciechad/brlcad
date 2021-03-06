/*                         S T A T . C
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
/** @addtogroup bu_log */
/** @{ */
/** @file stat.c
 *
 * Support routines for identifying properties of files and
 * directories such as whether they exist or are the same as another
 * given file.
 *
 */

#include "common.h"

#include <string.h>
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#ifdef HAVE_PWD_H
#  include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#  include <grp.h>
#endif
#include "bio.h"

#include "bu.h"

#ifndef R_OK
#  define R_OK 4
#endif
#ifndef W_OK
#  define W_OK 2
#endif
#ifndef X_OK
#  define X_OK 1
#endif


/**
 *			B U _ F I L E _ E X I S T S
 *
 *  @return	1	The given filename exists.
 *  @return	0	The given filename does not exist.
 */
int
bu_file_exists(const char *path)
{
    struct stat sbuf;

    if (bu_debug & BU_DEBUG_PATHS) {
	bu_log("Does [%s] exist? ", path);
    }

    if (!path) {
	if (bu_debug & BU_DEBUG_PATHS) {
	    bu_log("NO\n");
	}
	/* FAIL */
	return 0;
    }

    /* does it exist as a filesystem entity? */
    if ( stat( path, &sbuf ) == 0 ) {
	if (bu_debug & BU_DEBUG_PATHS) {
	    bu_log("YES\n");
	}
	/* OK */
	return 1;
    }

    if (bu_debug & BU_DEBUG_PATHS) {
	bu_log("NO\n");
    }
    /* FAIL */
    return 0;
}


/**
 * b u _ s a m e _ f i l e
 *
 * returns truthfully as to whether or not the two provided filenames
 * are the same file.  if either file does not exist, the result is
 * false.
 */
int
bu_same_file(const char *fn1, const char *fn2)
{
    struct stat sb1, sb2;

    if (!fn1 || !fn2) {
	return 0;
    }

    if (!bu_file_exists(fn1) || !bu_file_exists(fn2)) {
	return 0;
    }

    if ((stat(fn1, &sb1) == 0) &&
	(stat(fn2, &sb2) == 0) &&
	(sb1.st_dev == sb2.st_dev) &&
	(sb1.st_ino == sb2.st_ino)) {
	return 1;
    }

    return 0;
}


/**
 * b u _ s a m e _ f d
 *
 * returns truthfully as to whether or not the two provided file
 * descriptors are the same file.  if either file does not exist, the
 * result is false.
 */
int
bu_same_fd(int fd1, int fd2)
{
    struct stat sb1, sb2;

    if (fd1<0 || fd2<0) {
	return 0;
    }

    /* ares files the same inode on same device? */
    if ((fstat(fd1, &sb1) == 0) && (fstat(fd2, &sb2) == 0) && (sb1.st_dev == sb2.st_dev) && (sb1.st_ino == sb2.st_ino)) {
	return 1;
    }

    return 0;
}


/**
 * _ b u _ f i l e _ a c c e s s
 *
 * common guts to the file access functions that returns truthfully if
 * the current user has the ability permission-wise to access the
 * specified file.
 */
static int
_bu_file_access(const char *path, int access_level)
{
    struct stat sb;
    int mask;

    /* 0 is root or Windows user */
    uid_t uid = 0;

    /* 0 is wheel or Windows group */
    gid_t gid = 0;

    int usr_mask = S_IRUSR | S_IWUSR | S_IXUSR;
    int grp_mask = S_IRGRP | S_IWGRP | S_IXGRP;
    int oth_mask = S_IROTH | S_IWOTH | S_IXOTH;

    if (!path || (path[0] == '\0')) {
	return 0;
    }

    if (stat(path, &sb) == -1) {
	return 0;
    }

    if (access_level & R_OK) {
	mask = S_IRUSR | S_IRGRP | S_IROTH;
    }
    if (access_level & W_OK) {
	mask = S_IWUSR | S_IWGRP | S_IWOTH;
    }
    if (access_level & X_OK) {
	mask = S_IXUSR | S_IXGRP | S_IXOTH;
    }

#ifdef HAVE_GETEUID
    uid = geteuid();
#endif
#ifdef HAVE_GETEGID
    gid = getegid();
#endif

    if (sb.st_uid == uid) {
	/* we own it */
	return sb.st_mode & (mask & usr_mask);
    } else if (sb.st_gid == gid) {
	/* our primary group */
	return sb.st_mode & (mask & grp_mask);
    }

    /* search group database to see if we're in the file's group */
#if defined(HAVE_PWD_H) && defined (HAVE_GRP_H)
    {
	struct passwd *pwdb = getpwuid(uid);
	if (pwdb && pwdb->pw_name) {
	    int i;
	    struct group *grdb = getgrgid(sb.st_gid);
	    for (i = 0; grdb && grdb->gr_mem[i]; i++) {
		if (strcmp(grdb->gr_mem[i], pwdb->pw_name) == 0) {
		    /* one of our other groups */
		    return sb.st_mode & (mask & grp_mask);
		}
	    }
	}
    }
#endif

    /* check other */
    return sb.st_mode & (mask & oth_mask);;
}


/**
 * b u _ f i l e _ r e a d a b l e
 *
 * returns truthfully if current user can read the specified file or
 * directory.
 */
int
bu_file_readable(const char *path)
{
    return _bu_file_access(path, R_OK);
}


/**
 * b u _ f i l e _ w r i t a b l e
 *
 * returns truthfully if current user can write to the specified file
 * or directory.
 */
int
bu_file_writable(const char *path)
{
    return _bu_file_access(path, W_OK);
}


/**
 * b u _ f i l e _ e x e c u t a b l e
 *
 * returns truthfully if current user can run the specified file or
 * directory.
 */
int
bu_file_executable(const char *path)
{
    return _bu_file_access(path, X_OK);
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
