/*                         S T R L . C
 * BRL-CAD
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
/** @addtogroup vls */
/** @{ */
/** @file strl.c
 *
 * Compatibility routines to various string processing functions
 * including strlcat and strlcpy.
 *
 * @author
 *   Christopher Sean Morrison
 */

#include "common.h"

#include <string.h>
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

#include "bu.h"


/**
 * b u _ s t r l c a t / b u _ s t r l c a t m
 *
 * concatenate one string onto the end of another, returning the
 * length of the dst string after the concatenation.
 *
 * bu_strlcat() is a macro to bu_strlcatm() so that we can report the
 * file name and line number of any erroneous callers.
 */
size_t
bu_strlcatm(char *dst, const char *src, size_t size, const char *label)
{
    size_t srcsize;
    size_t dstsize;

    if (!dst && label) {
	bu_semaphore_acquire(BU_SEM_SYSCALL);
	fprintf(stderr, "WARNING: NULL destination string, size %ld [%s]\n", size, label);
	bu_semaphore_release(BU_SEM_SYSCALL);
    }
    if (!dst || !src || size <= 0) {
	return 0;
    }
    if (!label) {
	label = "bu_strlcat";
    }

    dstsize = strlen(dst);
    srcsize = strlen(src);

    if (dstsize == size - 1) {
	bu_semaphore_acquire(BU_SEM_SYSCALL);
	fprintf(stderr, "WARNING: [%s] concatenation string is already full at %ld chars\n", label, size-1);
	bu_semaphore_release(BU_SEM_SYSCALL);
    } else if (dstsize > size - 1) {
	/* probably missing null-termination or is not an initialized buffer */
	bu_semaphore_acquire(BU_SEM_SYSCALL);
	fprintf(stderr, "WARNING: [%s] concatenation string is already full, exceeds size (%ld > %ld)\n", label, dstsize, size-1);
	bu_semaphore_release(BU_SEM_SYSCALL);
    } else if (srcsize >= size - dstsize) {
	if (bu_debug) {
	    bu_semaphore_acquire(BU_SEM_SYSCALL);
	    fprintf(stderr, "WARNING: [%s] string truncation, exceeding %ld char max concatenating %ld chars (started with %ld)\n", label, size-1, srcsize, dstsize);
	    bu_semaphore_release(BU_SEM_SYSCALL);
	}
    }

#ifdef HAVE_STRLCAT
    /* don't return to ensure consistent null-termination behavior in following */
    (void)strlcat(dst, src, size);
#else
    (void)strncat(dst, src, size-strlen(dst)-1);
#endif

    /* be sure to null-terminate, contrary to strncat behavior */
    if (dstsize + srcsize < size-1) {
	dst[dstsize + srcsize] = '\0';
    } else {
	dst[size-1] = '\0'; /* sanity */
    }

    return strlen(dst);
}


/**
 * b u _ s t r l c p y / b u _ s t r l c p y m
 *
 * copies one string into another, returning the length of the dst
 * string after the copy.
 *
 * bu_strlcpy() is a macro to bu_strlcpym() so that we can report the
 * file name and line number of any erroneous callers.
 */
size_t
bu_strlcpym(char *dst, const char *src, size_t size, const char *label)
{
    size_t srcsize;

    
    if (!dst && label) {
	bu_semaphore_acquire(BU_SEM_SYSCALL);
	fprintf(stderr, "WARNING: NULL destination string, size %ld [%s]\n", size, label);
	bu_semaphore_release(BU_SEM_SYSCALL);
    }
    if (!dst || !src || size <= 0) {
	return 0;
    }
    if (!label) {
	label = "bu_strlcpy";
    }

    srcsize = strlen(src);

    if (bu_debug) {
	if (srcsize >= size ) {
	    bu_semaphore_acquire(BU_SEM_SYSCALL);
	    fprintf(stderr, "WARNING: [%s] string truncation, exceeding %ld char max copying %ld chars\n", label, size-1, srcsize);
	    bu_semaphore_release(BU_SEM_SYSCALL);
	}
    }

#ifdef HAVE_STRLCPY
    /* don't return to ensure consistent null-termination behavior in following */
    (void)strlcpy(dst, src, size);
#else
    (void)strncpy(dst, src, size-1);
#endif

    /* be sure to always null-terminate, contrary to strncpy behavior */
    if (srcsize < size-1) {
	dst[srcsize] = '\0';
    } else {
	dst[size-1] = '\0'; /* sanity */
    }

    return strlen(dst);
}


/**
 * b u _ s t r d u p  / b u _ s t r d u p m
 *
 * Given a string, allocate enough memory to hold it using bu_malloc(),
 * duplicate the strings, returns a pointer to the new string.
 *
 * bu_strdup() is a macro that includes the current file name and line
 * number that can be used when bu debugging is enabled.
 */
char *
bu_strdupm(register const char *cp, const char *label)
{
    register char	*base;
    register size_t	len;

    if (!cp && label) {
	bu_semaphore_acquire(BU_SEM_SYSCALL);
	fprintf(stderr, "WARNING: [%s] NULL copy buffer\n", label);
	bu_semaphore_release(BU_SEM_SYSCALL);
    }
    if (!label) {
	label = "bu_strdup";
    }

    len = strlen( cp )+1;
    base = bu_malloc( len, label);

    if (bu_debug&BU_DEBUG_MEM_LOG) {
	bu_semaphore_acquire(BU_SEM_SYSCALL);
	fprintf(stderr, "%8lx strdup%7ld \"%s\"\n", (long)base, (long)len, cp );
	bu_semaphore_release(BU_SEM_SYSCALL);
    }

    memcpy(base, cp, len);
    return(base);
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
