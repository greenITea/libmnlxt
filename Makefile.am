#ACLOCAL_AMFLAGS = -I m4

SUBDIRS=src include $(build_tests)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmnlxt.pc

${pkgconfig_DATA}: ${top_builddir}/config.status
