#include <tommath.h>
#ifdef BN_MP_SHRINK_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://math.libtomcrypt.com
 */

/* shrink a bignum */
int mp_shrink (mp_int * a)
{
  mp_digit *tmp;
  if (a->alloc != a->used && a->used > 0) {
    if ((tmp = OPT_CAST(mp_digit) XREALLOC (a->dp, sizeof (mp_digit) * a->used)) == NULL) {
      return MP_MEM;
    }
    a->dp    = tmp;
    a->alloc = a->used;
  }
  return MP_OKAY;
}
#endif

/* $Source: /cvsroot/tcl/libtommath/bn_mp_shrink.c,v $ */
/* $Revision: 30092 $ */
/* $Date: 2008-01-25 17:37:10 -0500 (Fri, 25 Jan 2008) $ */
