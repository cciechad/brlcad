# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils java-pkg-opt-2 multilib
DESCRIPTION="Brlcad"
HOMEPAGE="http://brlcad.org"
SRC_URI="mirror://sourceforge/${PN}/${P}.tar.bz2"
LICENSE="LGPL BSD"
SLOT="0"
KEYWORDS="~amd64"

IUSE="examples unigraphics java X opengl tcl tk"
DEPEND="dev-tcltk/itcl"
RDEPEND="${DEPEND}"
#S="${WORKDIR}/${P}"

src_compile() {
	sed -i -e 's:\/usr\/local:\/usr\/lib64:' configure.ac || die 'sed failed'
	java-pkg-opt-2_pkg_setup
	if use java ; then
		myconf="--with-jdk=`java-config -O`/include"
	fi
	if use tcl ; then
		myconf="${myconf} --with-tcl=/usr/$(get_libdir)"
	fi
	if use tk ; then
		myconf="${myconf} --with-tk=/usr/$(get_libdir)"
	fi
	myconf="${myconf} --enable-regex-build=no --enable-png-build=no --enable-zlib-build=no \
		--enable-urt-build=no --enable-termlib-build=no --enable-tcl-build=no --enable-tk-build=no \
		--enable-itcl-build=no --enable-iwidgets-build=no --enable-blt-build=no --enable-tkimg-install=no"
	econf \
	${myconf} \
	$(use_with X x11) \
	$(use_with opengl ogl) \
	$(enable_with unigraphics unigraphics-build) \
	$(enable_with examples models-install) \
	$(enable_with smp parallel) || die "econf failed"
	emake || die "emake failed"
}

src_install() {
	emake DESTDIR="${D}" install || die "emake install failed"
	#einstall || die "einstall failed"
}
