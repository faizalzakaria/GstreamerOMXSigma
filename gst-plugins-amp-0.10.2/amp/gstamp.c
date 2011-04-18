/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) 2010 Romain Bichet <romain_bichet@sdesigns.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * This file is usefull to reference easily the elements of the package.
 * For the time being, the package contains:
 * GstAmpSrc
 * GstAmpAudioSink
 * GstAmpVideoSink
 * GstAmpFilter (Deprecated: Do not use it.)
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "gstamp.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gst_amp_src_plugin_init (plugin);
  gst_amp_filter_plugin_init (plugin);
  gst_amp_audio_sink_plugin_init (plugin);
  gst_amp_video_sink_plugin_init (plugin);
  gst_amp_demux_plugin_init (plugin);
  gst_amp_video_scale_plugin_init (plugin);
  gst_amp_volume_plugin_init (plugin);

  return TRUE;
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "amp"
#endif

/* gstreamer looks for this structure to register ampsrc
 * exchange the string 'Template ampsrc' with your ampsrc description
 */
GST_PLUGIN_DEFINE (
		GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		"amp",
		"amp implementation",
		plugin_init,
		VERSION,
		"LGPL",
		"GStreamer",
		"http://gstreamer.net/"
		)
