
plugin_LTLIBRARIES = libgstqtdemux.la

libgstqtdemux_la_CFLAGS = $(GST_PLUGINS_BASE_CFLAGS) $(GST_BASE_CFLAGS) $(GST_CFLAGS)
libgstqtdemux_la_LIBADD = \
    $(GST_PLUGINS_BASE_LIBS) \
    -lgstriff-@GST_MAJORMINOR@ \
    -lgstaudio-@GST_MAJORMINOR@ \
    -lgstrtp-@GST_MAJORMINOR@ \
    -lgsttag-@GST_MAJORMINOR@ \
    -lgstpbutils-@GST_MAJORMINOR@ \
    $(GST_BASE_LIBS) $(GST_LIBS) $(ZLIB_LIBS)
libgstqtdemux_la_LDFLAGS = ${GST_PLUGIN_LDFLAGS} 
libgstqtdemux_la_SOURCES = quicktime.c gstrtpxqtdepay.c \
	qtdemux.c qtdemux_types.c qtdemux_dump.c qtdemux_lang.c
libgstqtdemux_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = \
	qtatomparser.h   \
	qtdemux.h        \
	qtdemux_types.h  \
	qtdemux_dump.h   \
	qtdemux_fourcc.h \
	qtdemux_lang.h   \
	qtpalette.h      \
	gstrtpxqtdepay.h
