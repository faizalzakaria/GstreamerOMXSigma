# Makefile.in generated by automake 1.9.5 from Makefile.am.
# docs/pwg/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004, 2005  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



### this is the part you can customize if you need to

### These are all generic; we set all the variables we need

# this snippet is to be included by both our docbook manuals
# and gtk-doc API references

# it adds an upload target to each of these dir's Makefiles

# each Makefile.am should define the following variables:
# - DOC: the base name of the documentation
#        (faq, manual, pwg, gstreamer, gstreamer-libs)
# - FORMATS: the formats in which DOC is output
#            (html ps pdf)

# if you want to use it, make sure your ..sh/config file contains the
# correct User entry for the Host entry for the DOC_SERVER

# this file adds rules for installing html subtrees
# I really don't like this hack, but automake doesn't seem to want to
# install directory trees :(

srcdir = .
top_srcdir = ../..

pkgdatadir = $(datadir)/gstreamer
pkglibdir = $(libdir)/gstreamer
pkgincludedir = $(includedir)/gstreamer
top_builddir = ../..
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = i486-pc-linux-gnu
host_triplet = mips-unknown-linux-gnu
DIST_COMMON = $(srcdir)/../htmlinstall.mak $(srcdir)/../manuals.mak \
	$(srcdir)/../upload.mak $(srcdir)/Makefile.am \
	$(srcdir)/Makefile.in
subdir = docs/pwg
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/common/m4/as-ac-expand.m4 \
	$(top_srcdir)/common/m4/as-auto-alt.m4 \
	$(top_srcdir)/common/m4/as-compiler-flag.m4 \
	$(top_srcdir)/common/m4/as-docbook.m4 \
	$(top_srcdir)/common/m4/as-libtool-tags.m4 \
	$(top_srcdir)/common/m4/as-libtool.m4 \
	$(top_srcdir)/common/m4/as-python.m4 \
	$(top_srcdir)/common/m4/as-scrub-include.m4 \
	$(top_srcdir)/common/m4/as-version.m4 \
	$(top_srcdir)/common/m4/check.m4 \
	$(top_srcdir)/common/m4/gettext.m4 \
	$(top_srcdir)/common/m4/gst-arch.m4 \
	$(top_srcdir)/common/m4/gst-args.m4 \
	$(top_srcdir)/common/m4/gst-doc.m4 \
	$(top_srcdir)/common/m4/gst-error.m4 \
	$(top_srcdir)/common/m4/gst-feature.m4 \
	$(top_srcdir)/common/m4/gst-function.m4 \
	$(top_srcdir)/common/m4/gst-gettext.m4 \
	$(top_srcdir)/common/m4/gst-glib2.m4 \
	$(top_srcdir)/common/m4/gst-libxml2.m4 \
	$(top_srcdir)/common/m4/gst-plugindir.m4 \
	$(top_srcdir)/common/m4/gtk-doc.m4 \
	$(top_srcdir)/common/m4/iconv.m4 \
	$(top_srcdir)/common/m4/lib-ld.m4 \
	$(top_srcdir)/common/m4/lib-link.m4 \
	$(top_srcdir)/common/m4/lib-prefix.m4 \
	$(top_srcdir)/common/m4/pkg.m4 \
	$(top_srcdir)/common/m4/progtest.m4 $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
SOURCES =
DIST_SOURCES =
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = `echo $$p | sed -e 's|^.*/||'`;
am__installdirs = "$(DESTDIR)$(pwgdir)"
pwgDATA_INSTALL = $(INSTALL_DATA)
DATA = $(pwg_DATA)
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/missing --run aclocal-1.9
ACLOCAL_AMFLAGS = -I common/m4
AMDEP_FALSE = #
AMDEP_TRUE = 
AMTAR = ${SHELL} /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/missing --run tar
AR = mips-linux-gnu-ar
AUTOCONF = ${SHELL} /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/missing --run autoconf
AUTOHEADER = ${SHELL} /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/missing --run autoheader
AUTOMAKE = ${SHELL} /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/missing --run automake-1.9
AWK = gawk
BISON_PATH = /usr/bin/bison
BUILD_EXAMPLES_FALSE = #
BUILD_EXAMPLES_TRUE = 
BUILD_FAILING_TESTS_FALSE = 
BUILD_FAILING_TESTS_TRUE = #
BUILD_TESTS_FALSE = #
BUILD_TESTS_TRUE = 
CAT_ENTRY_END = 
CAT_ENTRY_START = 
CC = mips-linux-gnu-gcc -EL
CCAS = mips-linux-gnu-gcc -EL
CCASFLAGS = -fPIC -march=mips32r2 -Wa,-mips32r2 -g
CCDEPMODE = depmode=gcc3
CFLAGS = -fPIC -march=mips32r2 -Wa,-mips32r2 -g
CHECK_CFLAGS = 
CHECK_LIBS = -lcheck
CPP = mips-linux-gnu-gcc -EL -E
CPPFLAGS = 
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
DEPRECATED_CFLAGS = -DGST_DISABLE_DEPRECATED
DOCBOOK_ROOT = 
DOC_HTML_FALSE = 
DOC_HTML_TRUE = #
DOC_PDF_FALSE = 
DOC_PDF_TRUE = #
DOC_PS_FALSE = 
DOC_PS_TRUE = #
ECHO = echo
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = grep -E
ENABLE_DOCBOOK_FALSE = 
ENABLE_DOCBOOK_TRUE = #
ENABLE_GTK_DOC_FALSE = 
ENABLE_GTK_DOC_TRUE = #
ERROR_CFLAGS = -Wall
EXEEXT = 
FLEX_PATH = /usr/bin/flex
GETTEXT_PACKAGE = gstreamer-0.10
GLIB_CFLAGS = -pthread -I/users/phai/share/GSTReamer_compilation/include/glib-2.0 -I/users/phai/share/GSTReamer_compilation/lib/glib-2.0/include
GLIB_LIBS = -pthread -Wl,--export-dynamic -L/users/phai/share/GSTReamer_compilation/lib -lgobject-2.0 -lgthread-2.0 -lgmodule-2.0 -lrt -lglib-2.0  
GLIB_ONLY_CFLAGS = -I/users/phai/share/GSTReamer_compilation/include/glib-2.0 -I/users/phai/share/GSTReamer_compilation/lib/glib-2.0/include  
GLIB_ONLY_LIBS = -L/users/phai/share/GSTReamer_compilation/lib -lglib-2.0  
GLIB_REQ = 2.6
GMSGFMT = /usr/bin/msgfmt
GST_AGE = 2
GST_ALL_CFLAGS = -I$(top_srcdir)/libs -I$(top_srcdir) -I$(top_builddir) -pthread -I/users/phai/share/GSTReamer_compilation/include/glib-2.0 -I/users/phai/share/GSTReamer_compilation/lib/glib-2.0/include -I/users/phai/share/GSTReamer_compilation/include/libxml2   $(GST_OPTION_CFLAGS)
GST_ALL_LDFLAGS = -no-undefined
GST_ALL_LIBS = -pthread -Wl,--export-dynamic -L/users/phai/share/GSTReamer_compilation/lib -lgobject-2.0 -lgthread-2.0 -lgmodule-2.0 -lrt -lglib-2.0   
GST_CACHE_DIR = /users/phai/share/GSTReamer_compilation/var/cache/gstreamer-0.10
GST_CURRENT = 2
GST_DISABLE_ALLOC_TRACE_DEFINE = /* #undef GST_DISABLE_ALLOC_TRACE */
GST_DISABLE_ALLOC_TRACE_FALSE = 
GST_DISABLE_ALLOC_TRACE_TRUE = #
GST_DISABLE_ENUMTYPES_DEFINE = /* #undef GST_DISABLE_ENUMTYPES */
GST_DISABLE_ENUMTYPES_FALSE = 
GST_DISABLE_ENUMTYPES_TRUE = #
GST_DISABLE_GST_DEBUG_DEFINE = /* #undef GST_DISABLE_GST_DEBUG */
GST_DISABLE_GST_DEBUG_FALSE = 
GST_DISABLE_GST_DEBUG_TRUE = #
GST_DISABLE_INDEX_DEFINE = /* #undef GST_DISABLE_INDEX */
GST_DISABLE_INDEX_FALSE = 
GST_DISABLE_INDEX_TRUE = #
GST_DISABLE_LOADSAVE_DEFINE = /* #undef GST_DISABLE_LOADSAVE */
GST_DISABLE_LOADSAVE_FALSE = 
GST_DISABLE_LOADSAVE_TRUE = #
GST_DISABLE_NET_DEFINE = /* #undef GST_DISABLE_NET */
GST_DISABLE_NET_FALSE = 
GST_DISABLE_NET_TRUE = #
GST_DISABLE_PARSE_DEFINE = /* #undef GST_DISABLE_PARSE */
GST_DISABLE_PARSE_FALSE = 
GST_DISABLE_PARSE_TRUE = #
GST_DISABLE_PLUGIN_DEFINE = /* #undef GST_DISABLE_PLUGIN */
GST_DISABLE_PLUGIN_FALSE = 
GST_DISABLE_PLUGIN_TRUE = #
GST_DISABLE_REGISTRY_DEFINE = /* #undef GST_DISABLE_REGISTRY */
GST_DISABLE_REGISTRY_FALSE = 
GST_DISABLE_REGISTRY_TRUE = #
GST_DISABLE_TRACE_DEFINE = /* #undef GST_DISABLE_TRACE */
GST_DISABLE_TRACE_FALSE = 
GST_DISABLE_TRACE_TRUE = #
GST_DISABLE_URI_DEFINE = /* #undef GST_DISABLE_URI */
GST_DISABLE_URI_FALSE = 
GST_DISABLE_URI_TRUE = #
GST_GCOV_ENABLED_FALSE = 
GST_GCOV_ENABLED_TRUE = #
GST_HAVE_GLIB_2_8_DEFINE = #define GST_HAVE_GLIB_2_8 1
GST_HAVE_UNALIGNED_ACCESS_DEFINE = #define GST_HAVE_UNALIGNED_ACCESS 0
GST_LEVEL_DEFAULT = GST_LEVEL_NONE
GST_LIBVERSION = 2:0:2
GST_LIB_LDFLAGS = -export-symbols-regex _*\(gst_\|Gst\|GST_\).*
GST_LICENSE = LGPL
GST_LOADSAVE_DOC_TYPES = 
GST_LT_LDFLAGS =  -version-info 2:0:2
GST_MAJORMINOR = 0.10
GST_OBJ_CFLAGS = $(GST_ALL_CFLAGS)
GST_OBJ_LIBS = $(top_builddir)/gst/libgstreamer-0.10.la $(GST_ALL_LIBS)
GST_OPTION_CFLAGS = $(ERROR_CFLAGS) $(DEBUG_CFLAGS) $(PROFILE_CFLAGS) $(GCOV_CFLAGS) $(OPT_CFLAGS) $(DEPRECATED_CFLAGS)
GST_PACKAGE_NAME = GStreamer source release
GST_PACKAGE_ORIGIN = Unknown package origin
GST_PKG_DEPS = glib-2.0, gobject-2.0, gmodule-2.0, gthread-2.0
GST_PLUGIN_LDFLAGS = -module -avoid-version -export-symbols-regex '^_*gst_plugin_desc$$' -no-undefined
GST_PRINTF_EXTENSION_FORMAT_DEFINE = #define GST_PTR_FORMAT "P"
GST_REGISTRY_DOC_TYPES = 
GST_REVISION = 0
GST_VERSION = 0.10.2
GST_VERSION_MAJOR = 0
GST_VERSION_MICRO = 2
GST_VERSION_MINOR = 10
GST_VERSION_NANO = 0
GST_VERSION_RELEASE = 1
GTK_DOC_USE_LIBTOOL_FALSE = #
GTK_DOC_USE_LIBTOOL_TRUE = 
HAVE_CHECK_FALSE = #
HAVE_CHECK_TRUE = 
HAVE_CPU_ALPHA_FALSE = 
HAVE_CPU_ALPHA_TRUE = #
HAVE_CPU_ARM_FALSE = 
HAVE_CPU_ARM_TRUE = #
HAVE_CPU_HPPA_FALSE = 
HAVE_CPU_HPPA_TRUE = #
HAVE_CPU_I386_FALSE = 
HAVE_CPU_I386_TRUE = #
HAVE_CPU_IA64_FALSE = 
HAVE_CPU_IA64_TRUE = #
HAVE_CPU_M68K_FALSE = 
HAVE_CPU_M68K_TRUE = #
HAVE_CPU_MIPS_FALSE = #
HAVE_CPU_MIPS_TRUE = 
HAVE_CPU_PPC_FALSE = 
HAVE_CPU_PPC_TRUE = #
HAVE_CPU_S390_FALSE = 
HAVE_CPU_S390_TRUE = #
HAVE_CPU_SPARC_FALSE = 
HAVE_CPU_SPARC_TRUE = #
HAVE_CPU_X86_64_FALSE = 
HAVE_CPU_X86_64_TRUE = #
HAVE_DOCBOOK2HTML = 
HAVE_DOCBOOK2PS = 
HAVE_DVIPS = 
HAVE_EPSTOPDF = 
HAVE_FIG2DEV = 
HAVE_JADETEX = 
HAVE_LIBGNOMEUI_FALSE = 
HAVE_LIBGNOMEUI_TRUE = #
HAVE_MMAP_FALSE = 
HAVE_MMAP_TRUE = #
HAVE_PNGTOPNM = 
HAVE_PNMTOPS = 
HAVE_PS2PDF = 
HAVE_SYS_SOCKET_H_FALSE = #
HAVE_SYS_SOCKET_H_TRUE = 
HAVE_VALGRIND_FALSE = #
HAVE_VALGRIND_TRUE = 
HAVE_XMLLINT = 
HOST_CPU = mips
HTML_DIR = ${datadir}/gtk-doc/html
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
INTLLIBS = 
LDFLAGS = -L/users/phai/share/GSTReamer_compilation/lib 	-L/utils/release/sets/3_9_3_iptv_dev/csrootfs/cross_rootfs/lib
LIBDIR = /users/phai/share/GSTReamer_compilation/lib
LIBGNOMEUI_CFLAGS = 
LIBGNOMEUI_LIBS = 
LIBICONV = -liconv
LIBINTL = 
LIBOBJS = 
LIBS = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBXML2_REQ = 2.4.9
LIBXML_PKG = , libxml-2.0
LN_S = ln -s
LOCALEDIR = /users/phai/share/GSTReamer_compilation/share/locale
LTLIBICONV = -liconv
LTLIBINTL = 
LTLIBOBJS = 
MAINT = #
MAINTAINER_MODE_FALSE = 
MAINTAINER_MODE_TRUE = #
MAKEINFO = ${SHELL} /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/missing --run makeinfo
MKINSTALLDIRS = ./mkinstalldirs
MSGFMT = /usr/bin/msgfmt
MSGMERGE = /usr/bin/msgmerge
OBJEXT = o
PACKAGE = gstreamer
PACKAGE_BUGREPORT = 
PACKAGE_NAME = 
PACKAGE_STRING = 
PACKAGE_TARNAME = 
PACKAGE_VERSION = 
PATH_SEPARATOR = :
PERL_PATH = /usr/bin/perl
PKG_CONFIG = /usr/bin/pkg-config
PLUGINDIR = /users/phai/share/GSTReamer_compilation/lib/gstreamer-0.10
POSUB = po
PROFILE_CFLAGS = -g
PYTHON = /usr/bin/python
PYTHON_EXEC_PREFIX = ${exec_prefix}
PYTHON_PLATFORM = linux2
PYTHON_PREFIX = ${prefix}
PYTHON_VERSION = 2.6
RANLIB = mips-linux-gnu-ranlib
SET_MAKE = 
SHELL = /bin/bash
STRIP = mips-linux-gnu-strip
USE_NLS = yes
VALGRIND_CFLAGS = 
VALGRIND_LIBS = 
VALGRIND_PATH = /usr/bin/valgrind
VERSION = 0.10.2
XGETTEXT = /usr/bin/xgettext
XML_CATALOG = 
XML_CFLAGS = -I/users/phai/share/GSTReamer_compilation/include/libxml2  
XML_LIBS = -L/users/phai/share/GSTReamer_compilation/lib -lxml2  
XSLTPROC = 
XSLTPROC_FLAGS = 
ac_ct_AR = 
ac_ct_CC = 
ac_ct_RANLIB = 
ac_ct_STRIP = 
am__fastdepCC_FALSE = #
am__fastdepCC_TRUE = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build = i486-pc-linux-gnu
build_alias = i486-linux-gnu
build_cpu = i486
build_os = linux-gnu
build_vendor = pc
datadir = ${prefix}/share
docdir = $(datadir)/doc/gstreamer-0.10
exec_prefix = ${prefix}
host = mips-unknown-linux-gnu
host_alias = mips-linux-gnu
host_cpu = mips
host_os = linux-gnu
host_vendor = unknown
includedir = ${prefix}/include
infodir = ${prefix}/info
install_sh = /users/phai/share/GSTReamer_compilation/gstreamer-0.10.2/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${prefix}/man
mkdir_p = mkdir -p --
oldincludedir = /usr/include
pkgpyexecdir = ${pyexecdir}/gstreamer
pkgpythondir = ${pythondir}/gstreamer
plugindir = $(libdir)/gstreamer-0.10
prefix = /users/phai/share/GSTReamer_compilation
program_transform_name = s,x,x,
pyexecdir = ${exec_prefix}/lib/python2.6/site-packages
pythondir = ${prefix}/lib/python2.6/site-packages
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc
target_alias = mips-linux-gnu

# base name of doc
DOC = pwg
# formats defined for upload.mak
FORMATS = html ps pdf

# main xml file
MAIN = $(DOC).xml
# all xml sources
XML = $(notdir $(wildcard $(srcdir)/*.xml))
# base style sheet
CSS = base.css

# image sources
PNG_SRC = 
FIG_SRC = $(notdir $(wildcard $(srcdir)/*.fig))

# extra sources to copy in build directory
EXTRA_SRC = 

# intermediary build path
BUILDDIR = build
# same for images
BUILDIMAGESDIR = $(BUILDDIR)/images

# images
# right now, we only allow .png and .fig as source
# we might add more later if we feel the need

# PNG's can be source or built from .fig
PNG = $(strip $(PNG_SRC) $(FIG_SRC:.fig=.png))
# EPS .ps files can be built from .png or .fig
EPS = $(strip $(FIG_SRC:.fig=.ps) $(PNG_SRC:.png=.ps))
# PDF .pdf files can be built from .png or .fig
PDF = $(strip $(FIG_SRC:.fig=.pdf) $(PNG_SRC:.png=.pdf))

# where we expect to find images during building, whether by copying
# or by generating them
PNG_BUILT = $(foreach file, $(PNG), $(BUILDIMAGESDIR)/$(file))
EPS_BUILT = $(foreach file, $(EPS), $(BUILDIMAGESDIR)/$(file))
PDF_BUILT = $(foreach file, $(PDF), $(BUILDIMAGESDIR)/$(file))
SRC = $(XML) $(PNG_SRC) $(FIG_SRC) $(CSS) $(EXTRA_SRC)

# generate A4 docs
PAPER_LOCALE = nl_NL
HTML_DAT = 

# can we generate HTML ?
#HTML_DAT = html
HTML_TARGET = 
#HTML_TARGET = html/index.html
PS_DAT = 

# can we generate PS ?
#PS_DAT = $(DOC).ps
PDF_DAT = 

# can we generate PDF ?
#PDF_DAT = $(DOC).pdf

# these variables define the location of the online docs
DOC_SERVER = freedesktop.org
DOC_BASE = /srv/gstreamer.freedesktop.org/www/data/doc
DOC_URL = $(DOC_SERVER):$(DOC_BASE)

### this is the generic bit and you shouln't need to change this

# get the generic docbuilding Makefile stuff
# get the generic upload target

### this is standard automake stuff

# package up all the source
EXTRA_DIST = $(SRC)

# install documentation
pwgdir = $(docdir)/$(DOC)
pwg_DATA = $(PDF_DAT) $(PS_DAT)
all: all-am

.SUFFIXES:
$(srcdir)/Makefile.in: # $(srcdir)/Makefile.am $(srcdir)/../manuals.mak $(srcdir)/../upload.mak $(srcdir)/../htmlinstall.mak $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh \
		&& exit 0; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu  docs/pwg/Makefile'; \
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  docs/pwg/Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure: # $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4): # $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
uninstall-info-am:
install-pwgDATA: $(pwg_DATA)
	@$(NORMAL_INSTALL)
	test -z "$(pwgdir)" || $(mkdir_p) "$(DESTDIR)$(pwgdir)"
	@list='$(pwg_DATA)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=$(am__strip_dir) \
	  echo " $(pwgDATA_INSTALL) '$$d$$p' '$(DESTDIR)$(pwgdir)/$$f'"; \
	  $(pwgDATA_INSTALL) "$$d$$p" "$(DESTDIR)$(pwgdir)/$$f"; \
	done

uninstall-pwgDATA:
	@$(NORMAL_UNINSTALL)
	@list='$(pwg_DATA)'; for p in $$list; do \
	  f=$(am__strip_dir) \
	  echo " rm -f '$(DESTDIR)$(pwgdir)/$$f'"; \
	  rm -f "$(DESTDIR)$(pwgdir)/$$f"; \
	done
tags: TAGS
TAGS:

ctags: CTAGS
CTAGS:


distdir: $(DISTFILES)
	$(mkdir_p) $(distdir)/$(srcdir) $(distdir)/..
	@srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's|.|.|g'`; \
	list='$(DISTFILES)'; for file in $$list; do \
	  case $$file in \
	    $(srcdir)/*) file=`echo "$$file" | sed "s|^$$srcdirstrip/||"`;; \
	    $(top_srcdir)/*) file=`echo "$$file" | sed "s|^$$topsrcdirstrip/|$(top_builddir)/|"`;; \
	  esac; \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkdir_p) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) check-local
check: check-am
all-am: Makefile $(DATA) all-local
installdirs:
	for dir in "$(DESTDIR)$(pwgdir)"; do \
	  test -z "$$dir" || $(mkdir_p) "$$dir"; \
	done
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libtool clean-local mostlyclean-am

distclean: distclean-am
	-rm -f Makefile
distclean-am: clean-am distclean-generic distclean-libtool

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am: install-data-local install-pwgDATA

install-exec-am:

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-generic mostlyclean-libtool

pdf-am:

ps-am:

uninstall-am: uninstall-info-am uninstall-local uninstall-pwgDATA

.PHONY: all all-am all-local check check-am check-local clean \
	clean-generic clean-libtool clean-local distclean \
	distclean-generic distclean-libtool distdir dvi dvi-am html \
	html-am info info-am install install-am install-data \
	install-data-am install-data-local install-exec \
	install-exec-am install-info install-info-am install-man \
	install-pwgDATA install-strip installcheck installcheck-am \
	installdirs maintainer-clean maintainer-clean-generic \
	mostlyclean mostlyclean-generic mostlyclean-libtool pdf pdf-am \
	ps ps-am uninstall uninstall-am uninstall-info-am \
	uninstall-local uninstall-pwgDATA


### generate all documentation by default
# hook in html generation
all-local: html
html: $(HTML_TARGET)
ps: $(PS_DAT)
pdf: $(PDF_DAT)

debug:
	@echo "outputting some useful debug information"
	@echo "Source XML:"
	@echo "XML: '$(XML)'"
	@echo "CSS: '$(CSS)'"
	@echo "Source image files:"
	@echo "PNG_SRC: '$(PNG_SRC)'"
	@echo "FIG_SRC: '$(FIG_SRC)'"
	@echo "All used image files:"
	@echo "PNG: '$(PNG)'"
	@echo "EPS: '$(EPS)'"
	@echo "PDF: '$(PDF)'"
	@echo "All used image files in their built path:"
	@echo "PNG_BUILT: '$(PNG_BUILT)'"
	@echo "EPS_BUILT: '$(EPS_BUILT)'"
	@echo "PDF_BUILT: '$(PDF_BUILT)'"
	@echo "End result products:"
	@echo "HTML_DAT: '$(HTML_DAT)'"
	@echo "PS_DAT:   '$(PS_DAT)'"
	@echo "PDF_DAT:  '$(PDF_DAT)'"

# a rule to copy all of the source for docs into $(builddir)/build
$(BUILDDIR)/$(MAIN): $(XML) $(CSS) $(EXTRA_SRC)
	@-mkdir -p $(BUILDDIR)
	@if test "x$(EXTRA_SRC)" != "x"; then for a in $(EXTRA_SRC); do cp $(srcdir)/$$a $(BUILDDIR); done; fi
	@for a in $(XML); do cp $(srcdir)/$$a $(BUILDDIR); done
	@for a in $(CSS); do cp $(srcdir)/$$a $(BUILDDIR); done
	@cp ../version.entities $(BUILDDIR)
	@cp $(top_srcdir)/docs/url.entities $(BUILDDIR)

html/index.html: $(BUILDDIR)/$(MAIN) $(PNG_BUILT) $(FIG_SRC)
	@make check-local
	@echo "*** Generating HTML output ***"
	@-mkdir -p html
	@cp -f $(srcdir)/../image-png $(BUILDDIR)/image.entities
	@cd $(BUILDDIR) && docbook2html -o ../html -V '%use-id-as-filename%' $(MAIN)
	@test "x$(CSS)" != "x" && \
          echo "Copying .css files: $(CSS)" && \
          cp $(srcdir)/$(CSS) html
	@test "x$(PNG)" != "x" && \
          echo "Copying .png images: $(PNG_BUILT)" && \
	  mkdir -p html/images && \
          cp $(PNG_BUILT) html/images || true

$(DOC).ps: $(BUILDDIR)/$(MAIN) $(EPS_BUILT) $(PNG_SRC) $(FIG_SRC)
	@make check-local
	@echo "*** Generating PS output ***"
	@cp -f $(srcdir)/../image-eps $(BUILDDIR)/image.entities
	cd $(BUILDDIR) && docbook2ps -o .. $(MAIN)
#	export LC_PAPER=$(PAPER_LOCALE) && cd $(BUILDDIR) && xmlto ps -o .. $(MAIN)

$(DOC).pdf: $(DOC).ps
	@make check-local
	@echo "*** Generating PDF output ***"
	@ps2pdf $(DOC).ps

#$(DOC).pdf: $(MAIN) $(PDF) $(FIG_SRC)
#	@echo "*** Generating PDF output ***"
#	@cp -f $(srcdir)/../image-pdf image.entities
#	@export LC_PAPER=$(PAPER_LOCALE) && xmlto pdf $(MAIN)
#	@rm image.entities

clean-local:
	-$(RM) -r $(BUILDDIR)
	-$(RM) -r html
	-$(RM) $(DOC).ps
	-$(RM) $(DOC).pdf
	-$(RM) -r www

### image generation

# copy png from source dir png
$(BUILDIMAGESDIR)/%.png: $(srcdir)/%.png
	@echo "Copying $< to $@"
	@mkdir -p $(BUILDIMAGESDIR)
	@cp $< $@
# make png from fig
$(BUILDIMAGESDIR)/%.png: %.fig
	@echo "Generating $@ from $<"
	@mkdir -p $(BUILDIMAGESDIR)
	@fig2dev -Lpng $< $@

# make ps(EPS) from fig
$(BUILDIMAGESDIR)/%.ps: %.fig
	@echo "Generating $@ from $<"
	@mkdir -p $(BUILDIMAGESDIR)
	@fig2dev -Leps $< $@

# make pdf from fig
$(BUILDIMAGESDIR)/%.pdf: %.fig
	@echo "Generating $@ from $<"
	@mkdir -p $(BUILDIMAGESDIR)
	@fig2dev -Lpdf $< $@

# make pdf from png
$(BUILDIMAGESDIR)/%.pdf: %.png
	@echo "Generating $@ from $<"
	@mkdir -p $(BUILDIMAGESDIR)
	@cat $< | pngtopnm | pnmtops -noturn 2> /dev/null | epstopdf --filter --outfile $@ 2> /dev/null

# make ps(EPS) from png
$(BUILDIMAGESDIR)/%.ps: %.png
	@echo "Generating $@ from $<"
	@mkdir -p $(BUILDIMAGESDIR)
	@cat $< | pngtopnm | pnmtops -noturn > $@ 2> /dev/null

# make sure xml validates properly
check-local: $(BUILDDIR)/$(MAIN)
	@cp -f $(srcdir)/../image-png $(BUILDDIR)/image.entities
	cd $(BUILDDIR) && xmllint -noout -valid $(MAIN)

upload: $(FORMATS)
	@if test "x$(GST_VERSION_NANO)" = x0; then \
            export DOCVERSION=$(VERSION); \
        else export DOCVERSION=head; \
        fi; \
        export DIR=$(DOC_BASE)/gstreamer/$$DOCVERSION/$(DOC); \
	ssh $(DOC_SERVER) mkdir -p $$DIR; \
	if echo $(FORMATS) | grep html > /dev/null; then export SRC="$$SRC html"; fi; \
	if echo $(FORMATS) | grep ps > /dev/null; then export SRC="$$SRC $(DOC).ps"; fi; \
	if echo $(FORMATS) | grep pdf > /dev/null; then export SRC="$$SRC $(DOC).pdf"; fi; \
	echo Uploading $$SRC to $(DOC_SERVER):$$DIR; \
	rsync -rv -e ssh --delete $$SRC $(DOC_SERVER):$$DIR; \
	ssh $(DOC_SERVER) chmod -R g+w $$DIR; \
	echo Done

#install-data-local: html
#	$(mkinstalldirs) $(DESTDIR)$(docdir)/$(DOC)
#	cp -pr $(HTML_DAT) $(DESTDIR)$(docdir)/$(DOC)

#uninstall-local:
#	for part in $(HTML_DAT); do rm -rf $(DESTDIR)$(docdir)/$(DOC)/$$part; done
install-data-local:
uninstall-local:
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
