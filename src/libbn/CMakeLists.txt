set(LIBBN_SOURCES
    anim.c
    axis.c
    bn_tcl.c
    complex.c
    font.c
    fortran.c
    globals.c
    list.c
    marker.c
    mat.c
    msr.c
    noise.c
    plane.c
    plot3.c
    poly.c
    qmath.c
    rand.c
    scale.c
    sphmap.c
    symbol.c
    tabdata.c
    tplot.c
    vectfont.c
    vector.c
    vers.c
    vert_tree.c
    wavelet.c
)

include_directories(
    ../../include
    ../other/tcl/generic
)

add_definitions(
    -DHAVE_CONFIG_H
    -DBRLCADBUILD
)

add_library(libbn ${LIBBN_SOURCES})
