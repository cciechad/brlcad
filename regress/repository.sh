#                   R E P O S I T O R Y . S H
# BRL-CAD
#
# Copyright (c) 2008 United States Government as represented by
# the U.S. Army Research Laboratory.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following
# disclaimer in the documentation and/or other materials provided
# with the distribution.
#
# 3. The name of the author may not be used to endorse or promote
# products derived from this software without specific prior written
# permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
###
#
# Basic checks of the repository sources to make sure maintenance
# burden code clean-up problems don't creep in.
#
###

TOPSRC="$1"
if test "x$TOPSRC" = "x" ; then
    TOPSRC="."
fi

FAILED=0


# make sure nobody includes bio.h in a public header
echo "running bio.h public header check..."

if test ! -f "$TOPSRC/include/bio.h" ; then
    echo "Unable to find include/bio.h, aborting"
    exit 1
fi

FOUND="`grep '[^f]bio.h' $TOPSRC/include/*.h | grep -v 'include/bio.h'`"
if test "x$FOUND" = "x" ; then
    echo "-> bio.h check succeeded"
else
    echo "-> bio.h check FAILED"
    FAILED="`expr $FAILED + 1`"
fi


# make sure common.h is always included first
echo "running common.h inclusion order check..."

if test ! -f "$TOPSRC/include/common.h" ; then
    echo "Unable to find include/common.h, aborting"
    exit 1
fi

COMMONFILES="`find $TOPSRC -type f \( -name \*.c -o -name \*.cpp -o -name \*.cxx -o -name \*.h -o -name \*.y -o -name \*.l \) -not -regex '.*src/other.*' -not -regex '.*~' -not -regex '.*\.log' -not -regex '.*Makefile.*' -not -regex '.*cache.*' -not -regex '.*\.svn.*' -exec grep -n -I -e '#[[:space:]]*include' {} /dev/null \; | grep '\"common.h\"' | sed 's/:.*//g'`"

FOUND=
for file in $COMMONFILES ; do 
    if test -f "`echo $file | sed 's/\.c$/\.l/g'`" ; then 
	continue
    fi
    MATCH="`grep '#[[:space:]]*include' $file /dev/null | head -n 1 | grep -v '\"common.h\"' | sed 's/:.*//g'`"
    if test ! "x$MATCH" = "x" ; then
	echo "Header (common.h) out of order: $MATCH"
	FOUND=1
    fi
done

if test "x$FOUND" = "x" ; then
    echo "-> common.h check succeeded"
else
    echo "-> common.h check FAILED"
    FAILED="`expr $FAILED + 1`"
fi


exit $FAILED

# Local Variables:
# tab-width: 8
# mode: sh
# sh-indentation: 4
# sh-basic-offset: 4
# indent-tabs-mode: t
# End:
# ex: shiftwidth=4 tabstop=8
