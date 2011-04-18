/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 Romain Bichet <romain_bichet@sdesigns.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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

/**
 * SECTION:element-ampvideosink
 *
 * @see_also: ampsrc, ampaudiosink
 *
 * This element will activate the video output and handle video related custom events from
 * the main application (pan and move).
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v ampsrc location=example.mpeg ! tee name=t ! queue ! ampvideosink t. ! queue ! ampaudiosink
 * ]| Play a media file on Sigma's board and control it with the remote
 * It will not work because you need an application to initialize DirectFB and
 * to handle the remote events.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include "gstampvideosink.h"

GST_DEBUG_CATEGORY_STATIC (gst_amp_video_sink_debug);
#define GST_CAT_DEFAULT gst_amp_video_sink_debug

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS_ANY);

GST_BOILERPLATE (GstAmpVideoSink, gst_amp_video_sink, GstBaseSink, GST_TYPE_BASE_SINK);

// Added
static void gst_amp_video_sink_dispose (GObject * object);
static gboolean gst_amp_video_sink_event_handler (GstBaseSink * sink, GstEvent * event);

/* GObject vmethod implementations */
static void
gst_amp_video_sink_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
			"AMP Video Sink",
			"Sink/Video/Device",
			"AMP Video Sink which handle basic features (mute, unmute, ...).",
			"Romain Bichet <romain_bichet@sdesigns.com>");

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
}

/* initialize the ampvideosink's class */
static void
gst_amp_video_sink_class_init (GstAmpVideoSinkClass * klass)
{
	GObjectClass *gobject_class;
	GstBaseSinkClass *gstbasesink_class;
	GstElementClass * gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstbasesink_class = (GstBaseSinkClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gstbasesink_class->event = GST_DEBUG_FUNCPTR (gst_amp_video_sink_event_handler);
	gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_amp_video_sink_dispose);
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_amp_video_sink_init (GstAmpVideoSink * sink, GstAmpVideoSinkClass * gclass)
{
	GST_INFO_OBJECT (sink, "Function init.");
	sink->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

	gst_base_sink_set_sync (GST_BASE_SINK (sink), FALSE);

	sink->dfb = NULL;	
	sink->amp = NULL;

	sink->pan_mode = FALSE;

	IDirectFBDisplayLayer * main_video_layer;
	IDirectFBScreen * screen = NULL;
	DFBScreenMixerConfig mixer_config;
	DFBDisplayLayerConfig lcfg;

	// Retrieve the DirectFB interface
	if (DirectFBCreate(&sink->dfb) != DFB_OK)
		goto cleanup;

	// Retrieve the main video layer interface
	if (sink->dfb->GetDisplayLayer(sink->dfb, EM86LAYER_MAINVIDEO, &main_video_layer) != DFB_OK)
		goto cleanup;

	if (main_video_layer->SetCooperativeLevel(main_video_layer, DLSCL_EXCLUSIVE) != DFB_OK)
		goto cleanup;

	if (main_video_layer->GetConfiguration(main_video_layer, &lcfg) != DFB_OK)
		goto cleanup;

	// Initialization of the variables (size, position, ....)
	sink->xres = lcfg.width;
	sink->yres = lcfg.height;

	sink->x = 0;
	sink->y = 0;
	sink->width = lcfg.width;
	sink->height = lcfg.height;

	// Mixer Configuration
	// (set by default)
	//
	// Retrieve the screen interface
	if (sink->dfb->GetScreen(sink->dfb, 0, &screen) != DFB_OK)
		goto cleanup;
	
	if (screen->GetMixerConfiguration(screen, 0, &mixer_config) != DFB_OK)
		goto cleanup;

	DFB_DISPLAYLAYER_IDS_ADD(mixer_config.layers, EM86LAYER_MAINVIDEO);

	// Set the mixer configuration
	if (screen->SetMixerConfiguration(screen, 0, &mixer_config) != DFB_OK)
		goto cleanup;

	// Cleanup Screen
	screen->Release(screen);
	screen = NULL;

	main_video_layer->Release(main_video_layer);
	main_video_layer = NULL;

	// Retrieve the amp instance
	// GetInterface does not work so _amp is global and should be
	// initialized elsewhere (ampsrc for example)!
	sink->amp = _amp;

	GST_INFO_OBJECT (sink, "Initialization.");

	return;

cleanup:
	GST_INFO_OBJECT (sink, "Cleanup.");
	gst_amp_video_sink_dispose (G_OBJECT (sink));
}

static gboolean
gst_amp_video_sink_event_handler (GstBaseSink * basesink, GstEvent * event)
{
	GstAmpVideoSink * sink;
	gboolean ret;

	GstStructure * estructure, * cstructure;
	GstPad * sinkpad, * peer;
	gint x, y;
	gint width, height;

	sink = GST_AMP_VIDEO_SINK_CAST (basesink);

	GST_INFO_OBJECT (sink, "Function Event Handler.");

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CUSTOM_DOWNSTREAM_OOB:

			// Do not need to free the structure
			estructure = gst_event_get_structure (event);

			// PAN mode on or off ?
			if (g_strrstr (gst_structure_get_name (estructure), "PAN"))
			{
				sink->pan_mode = !sink->pan_mode;
				if (sink->pan_mode)
					GST_INFO_OBJECT (sink, "Pan Mode: ON");
				else
					GST_INFO_OBJECT (sink, "Pan Mode: ON");
			}

			// If PAN mode is on
			else if (sink->pan_mode)
			{
				sinkpad = gst_element_get_pad (sink, "sink");
				peer = gst_pad_get_peer (sinkpad);

				cstructure = gst_structure_empty_new ("video/x-raw-rgb" );
	
				// We copy the old values.
				x = sink->x;
				y = sink->y;
				width = sink->width;
				height = sink->height;

				if (g_strrstr (gst_structure_get_name (estructure), "PREVIOUS"))
				{
					x += - width / 4;
					y += - height / 4; 
					width += width / 2;
					height += height / 2;
				}
				else if (g_strrstr (gst_structure_get_name (estructure), "NEXT"))
				{
					x += + width / 4;
					y += + height / 4; 
					width += - width / 2;
					height += - height / 2;
				}
				else if (g_strrstr (gst_structure_get_name (estructure), "CURSOR_UP"))
				{
					y += - sink->yres / 16;
				}
				else if (g_strrstr (gst_structure_get_name (estructure), "CURSOR_DOWN"))
				{
					y += sink->yres / 16;
				}
				else if (g_strrstr (gst_structure_get_name (estructure), "CURSOR_LEFT"))
				{
					x += - sink->xres / 16;
				}
				else if (g_strrstr (gst_structure_get_name (estructure), "CURSOR_RIGHT"))
				{
					x += sink->xres / 16;
				}
				else
				{
					GST_INFO_OBJECT (sink, "Unrecognized Type.");
					break;
				}

				// Check if the operation is possible.
				// If not and PREVIOUS, correct it.
				// Otherwise, do nothing
				if ((x < 0) || (width < (sink->xres / 16)) || ((x + width) > sink->xres) || (y < 0) || (height < (sink->yres / 16)) || ((y + height) > sink->yres))
				{
					GST_INFO_OBJECT (sink, "Invalid PAN size/position: %d x %d  @  %d, %d  in  %d x %d  screen\n", width, height, x, y, sink->xres, sink->yres);
					// If the user wants dezoom, correct
					// the position or the size in order
					// to dezoom properly
					if (g_strrstr (gst_structure_get_name (estructure), "PREVIOUS"))
					{
						if (width > sink->xres)
							width = sink->xres;

						if (height > sink->yres)
							height = sink->yres;

						if (x + width > sink->xres)
							x = sink->xres - width;

						if (y + height > sink->yres)
							y = sink->yres - height;

						if (x < 0)
							x = 0;

						if (y < 0)
							y = 0;

					}
					// If it is not possible to move the
					// screen, leave it like it was
					// before;
					else
					{
						break;	
					}
				}

				// Store the new values
				sink->x = x;
				sink->y = y;
				sink->width = width;
				sink->height = height;

				// Set the structure and send the event to the
				// peer pad (GstAmpVideoScale).
				gst_structure_set (cstructure, "width", G_TYPE_INT, sink->width, "height", G_TYPE_INT, sink->height, "pointer_x", G_TYPE_INT, sink->x, "pointer_y", G_TYPE_INT, sink->y, NULL);

				// Do not need to unref the event, so do not
				// need to free the structure
				ret = gst_pad_send_event (peer, gst_event_new_navigation (cstructure));

				gst_object_unref (sinkpad);
				gst_object_unref (peer);
			}
			break;
		default:
			break;
	}

	if (GST_BASE_SINK_CLASS (parent_class)->event)
		return GST_BASE_SINK_CLASS (parent_class)->event (basesink, event) && ret;
	else
		return ret;
				

}

static void
gst_amp_video_sink_dispose (GObject * object)
{
	GstAmpVideoSink *sink = GST_AMPVIDEOSINK (object);

	if(sink->amp) {
		sink->amp = NULL;
		_amp = NULL; // Because _amp is global
	}

	if(sink->dfb) {
		sink->dfb->Release(sink->dfb);
		sink->dfb = NULL;
	}

	if (G_OBJECT_CLASS(parent_class)->dispose)
		G_OBJECT_CLASS (parent_class)->dispose (object);
	GST_INFO_OBJECT (sink, "Dispose.");
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_amp_video_sink_plugin_init (GstPlugin * ampvideosink)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampvideosink' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_video_sink_debug, "ampvideosink",
			0, "amp video sink");

	return gst_element_register (ampvideosink, "ampvideosink", GST_RANK_NONE,
			GST_TYPE_AMPVIDEOSINK);
}
