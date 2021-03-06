# $Id: Makefile.am 29896 2008-01-13 23:39:10Z brlcad $

bin_PROGRAMS = \
	a-d \
	bw-d \
	c-d \
	d-a \
	d-bw \
	d-f \
	d-i \
	d-u \
	damdf \
	dauto \
	dauto2 \
	dconv \
	ddisp \
	dfft \
	dmod \
	dpeak \
	dsel \
	dstat \
	dwin \
	f-d \
	f-i \
	fhor \
	i-a \
	i-d \
	i-f \
	ihist \
	imod \
	istat \
	syn \
	u-a \
	u-bw \
	u-d \
	u-f \
	umod \
	ustat

AM_CPPFLAGS = ${TCL_CPPFLAGS}

a_d_SOURCES = a-d.c
a_d_LDADD = ${BU}

d_a_SOURCES = d-a.c
d_a_LDADD = ${BU}

bw_d_SOURCES = bw-d.c
bw_d_LDADD = ${BU}

c_d_SOURCES = c-d.c
c_d_LDADD = ${BU} ${LIBM}

d_i_SOURCES = d-i.c
d_i_LDADD = ${BU}

d_u_SOURCES = d-u.c
d_u_LDADD = ${BU}

d_bw_SOURCES = d-bw.c
d_bw_LDADD = ${BU}

d_f_SOURCES = d-f.c
d_f_LDADD = ${BU}

f_d_SOURCES = f-d.c
f_d_LDADD = ${BU}

f_i_SOURCES = f-i.c
f_i_LDADD = ${BU}

i_d_SOURCES = i-d.c
i_d_LDADD = ${BU}

i_f_SOURCES = i-f.c
i_f_LDADD = ${BU}

i_a_SOURCES = i-a.c
u_a_SOURCES = u-a.c

u_d_SOURCES = u-d.c
u_d_LDADD = ${BU}

u_f_SOURCES = u-f.c
u_f_LDADD =  ${BU}

u_bw_SOURCES = u-bw.c

#  Processors

dwin_SOURCES = dwin.c coswin.c hamwin.c
dwin_LDADD = ${BU} ${LIBM}

dauto_SOURCES = dauto.c
dauto_LDADD = ${BU}

dmod_SOURCES = dmod.c
dmod_LDADD = ${BU}

dfft_SOURCES = dfft.c interp.c butter.c
dfft_LDADD = ${FFT} ${BU}

dconv_SOURCES = dconv.c
dconv_LDADD = ${BU} ${FFT}

ddisp_SOURCES = ddisp.c
ddisp_LDADD = ${FB}

fhor_SOURCES = fhor.c
fhor_LDADD = ${FB}

imod_SOURCES = imod.c
imod_LDADD = ${BU} ${LIBM}

umod_SOURCES = umod.c
umod_LDADD = ${BU} ${LIBM}


#  Misc

dstat_SOURCES = dstat.c
dstat_LDADD = ${BU} ${LIBM}

dsel_SOURCES = dsel.c
dsel_LDADD = ${BU}

istat_SOURCES = istat.c
istat_LDADD = ${BU}

ustat_SOURCES = ustat.c
ustat_LDADD = ${BU}


#  Experimental

dpeak_SOURCES = dpeak.c
dpeak_LDADD = ${BU}

syn_SOURCES = syn.c
syn_LDADD = ${BU} ${LIBM}

damdf_SOURCES = damdf.c
damdf_LDADD = ${BU}

dauto2_SOURCES = dauto2.c
dauto2_LDADD = ${BU}

ihist_SOURCES = ihist.c
ihist_LDADD = ${BU}

# XXX need to review why these are not built
EXTRA_DIST = \
	d2-c.c \
	fhor2.c \
	fhor3.c \
	smod.c

DEPS=src/libfb src/libfft

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(a_d_OBJECTS) \
	$(bw_d_OBJECTS) \
	$(c_d_OBJECTS) \
	$(d_a_OBJECTS) \
	$(d_bw_OBJECTS) \
	$(d_f_OBJECTS) \
	$(d_i_OBJECTS) \
	$(d_u_OBJECTS) \
	$(damdf_OBJECTS) \
	$(dauto_OBJECTS) \
	$(dauto2_OBJECTS) \
	$(dconv_OBJECTS) \
	$(ddisp_OBJECTS) \
	$(dfft_OBJECTS) \
	$(dmod_OBJECTS) \
	$(dpeak_OBJECTS) \
	$(dsel_OBJECTS) \
	$(dstat_OBJECTS) \
	$(dwin_OBJECTS) \
	$(f_d_OBJECTS) \
	$(f_i_OBJECTS) \
	$(fhor_OBJECTS) \
	$(i_a_OBJECTS) \
	$(i_d_OBJECTS) \
	$(i_f_OBJECTS) \
	$(ihist_OBJECTS) \
	$(imod_OBJECTS) \
	$(istat_OBJECTS) \
	$(syn_OBJECTS) \
	$(u_a_OBJECTS) \
	$(u_bw_OBJECTS) \
	$(u_d_OBJECTS) \
	$(u_f_OBJECTS) \
	$(umod_OBJECTS) \
	$(ustat_OBJECTS) \
	$(bin_PROGRAMS)
