# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgstamp.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
libgstamp_la_SOURCES = gstamp.c gstamp.h \
		       gstampsrc.c gstampsrc.h \
		       gstampfilter.c gstampfilter.h \
		       gstampaudiosink.c gstampaudiosink.h \
		       gstampvideosink.c gstampvideosink.h \
		       gstampdemux.c gstampdemux.h \
		       gstampvideoscale.c gstampvideoscale.h \
		       gstampvolume.c gstampvolume.h

DFB_DIR= /home/bichet/workspace/set_dcchd_black/directfb
RUA_DIR= /home/bichet/workspace/mrua/MRUA_src
DCCHD_DIR= /home/bichet/workspace/set_dcchd_black/dcchd

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstamp_la_CFLAGS = $(GST_CFLAGS) \
	-I$(DFB_DIR)/include -I$(DFB_DIR)/include/directfb -I$(DFB_DIR)/include/freetype2 -I$(DFB_DIR)/include/directfb-internal \
	-I$(RUA_DIR) \
	-I$(DCCHD_DIR)/core -I$(DCCHD_DIR)/dcchd -I$(DCCHD_DIR)/brd -I$(DCCHD_DIR)/mono -I$(DCCHD_DIR)/dvdvr -I$(DCCHD_DIR)/curacao -I$(DCCHD_DIR)/dtv \
	-I/home/bichet/workspace/set_dcchd_black/dcchd/sdesigns_8654

libgstamp_la_LIBADD = $(GST_LIBS)
libgstamp_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) \
	-L$(DFB_DIR)/lib -Wl,-rpath-link -Wl,$(DFB_DIR)/lib \
	-Wl,-rpath-link -Wl,$(RUA_DIR)/lib \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/core \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/mono \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dvdvr \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/brd \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/curacao \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/curacao/lib \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dtv \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dtv/tuner \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dtv/capture \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dtv/network \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dtv/acap \
	-Wl,-rpath-link -Wl,$(DCCHD_DIR)/dcchd
libgstamp_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstamp.h \
		 gstampsrc.h \
		 gstampfilter.h \
		 gstampaudiosink.h \
		 gstampvideosink.h \
		 gstampdemux.h \
		 gstampvideoscale.h \
		 gstampvolume.h
