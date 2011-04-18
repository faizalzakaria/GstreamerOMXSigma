/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 Romain Bichet <<user@hostname.org>>
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
 * SECTION:element-ampvideoscale
 *
 * @see_also: ampsrc, ampvideosink, ampvolume
 *
 * This element can resize and move the video layer.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v ampsrc location=example.mpeg ! tee name=t ! queue ! ampvideoscale ! ampvideosink t. ! queue ! ampaudiosink
 * ]| Play a media file on Sigma's board and control it with the remote
 * It will not work because you need an application to initialize DirectFB and
 * to handle the remote events.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstampvideoscale.h"


GST_DEBUG_CATEGORY_STATIC (gst_amp_video_scale_debug);
#define GST_CAT_DEFAULT gst_amp_video_scale_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};

enum
{
	PROP_0,
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
								    GST_PAD_SINK,
								    GST_PAD_ALWAYS,
								    GST_STATIC_CAPS ("ANY")
								   );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
								   GST_PAD_SRC,
								   GST_PAD_ALWAYS,
								   GST_STATIC_CAPS ("ANY")
								  );

GST_BOILERPLATE (GstAmpVideoScale, gst_amp_video_scale, GstBaseTransform, GST_TYPE_BASE_TRANSFORM);

static GstFlowReturn gst_amp_video_scale_chain (GstPad * pad, GstBuffer * buf);

// Added
static void gst_amp_video_scale_dispose (GObject * object);
static gboolean gst_amp_video_scale_src_event_handler (GstBaseTransform * basetransform, GstEvent * event);

/* GObject vmethod implementations */

static void
gst_amp_video_scale_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
					     "AmpVideoScale",
					     "Resizes video frames",
					     "Scale the video in order that the source caps and the sink caps can negotiate",
					     "Romain Bichet <romain_bichet@sdesigns.com>");

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
}

/* initialize the ampfilter's class */
static void
gst_amp_video_scale_class_init (GstAmpVideoScaleClass * klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GstBaseTransformClass * gstbasetransform_class = GST_BASE_TRANSFORM_CLASS (klass);

	// Added	
	gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_amp_video_scale_dispose);
	gstbasetransform_class->src_event = GST_DEBUG_FUNCPTR (gst_amp_video_scale_src_event_handler);
	
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_amp_video_scale_init (GstAmpVideoScale * videoscale,
		       GstAmpVideoScaleClass * gclass)
{
	videoscale->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	videoscale->srcpad = gst_pad_new_from_static_template (&src_factory, "src");

//	gst_base_transform_set_in_place (GST_BASE_TRANSFORM (videoscale), TRUE);

	GST_INFO_OBJECT (videoscale, "Function init.");

	videoscale->dfb = NULL;	
	videoscale->amp = NULL;
	videoscale->main_video_layer = NULL;

	DFBDisplayLayerConfig lcfg;

	// Retrieve the DirectFB interface
	if (DirectFBCreate(&videoscale->dfb) != DFB_OK)
		goto cleanup;

	// Retrieve the main video layer interface
	if (videoscale->dfb->GetDisplayLayer(videoscale->dfb, EM86LAYER_MAINVIDEO, &videoscale->main_video_layer) != DFB_OK)
		goto cleanup;

	if (videoscale->main_video_layer->SetCooperativeLevel(videoscale->main_video_layer, DLSCL_EXCLUSIVE) != DFB_OK)
		goto cleanup;

	if (videoscale->main_video_layer->GetConfiguration(videoscale->main_video_layer, &lcfg) != DFB_OK)
		goto cleanup;

	// Initialization of the variables (size, position, ....)
	videoscale->dst_rect.x = 0;
	videoscale->dst_rect.y = 0;
	videoscale->dst_rect.w = lcfg.width;
	videoscale->dst_rect.h = lcfg.height;

	videoscale->xres = lcfg.width;
	videoscale->yres = lcfg.height;

	// Retrieve the amp instance
	// GetInterface does not work so _amp is global and should be
	// initialized elsewhere (ampsrc for example)!
	videoscale->amp = _amp;

	return;

cleanup:
	GST_INFO_OBJECT (videoscale, "Cleanup.");
	gst_amp_video_scale_dispose (G_OBJECT (videoscale));
}

// In order to handle the EOS and to Finish the Media
static gboolean
gst_amp_video_scale_src_event_handler (GstBaseTransform * basetransform, GstEvent * event)
{
	GstAmpVideoScale *videoscale;

	gboolean ret;
	gint width, height, x, y;
	GstStructure *structure;

	videoscale = GST_AMP_VIDEO_SCALE_CAST (basetransform);
	ret = FALSE;

	GST_INFO_OBJECT (videoscale, "handling event");

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_NAVIGATION:
			structure = gst_event_get_structure (event);

			// Retrieve the values of w, h, x and y.
			gst_structure_get_int (structure, "width", &(videoscale->dst_rect.w));
			gst_structure_get_int (structure, "height", &(videoscale->dst_rect.h));
			gst_structure_get_int (structure, "pointer_x", &(videoscale->dst_rect.x));
			gst_structure_get_int (structure, "pointer_y", &(videoscale->dst_rect.y));

			// Resize the video
			if (videoscale->main_video_layer->SetScreenRectangle(videoscale->main_video_layer, videoscale->dst_rect.x, videoscale->dst_rect.y, videoscale->dst_rect.w, videoscale->dst_rect.h) == DFB_OK)
				GST_INFO_OBJECT (videoscale, "New PAN: %d x %d  @  %d, %d  in  %d x %d  layer\n", videoscale->dst_rect.w, videoscale->dst_rect.h, videoscale->dst_rect.x, videoscale->dst_rect.y, videoscale->xres, videoscale->yres);
			else
				GST_INFO_OBJECT (videoscale, "FAILED to pan %d x %d  @  %d, %d  in  %d x %d  layer\n", videoscale->dst_rect.w, videoscale->dst_rect.h, videoscale->dst_rect.x, videoscale->dst_rect.y, videoscale->xres, videoscale->yres);

			break;
		default:
			break;
	}

	ret = TRUE;

	if (GST_BASE_TRANSFORM_CLASS (parent_class)->src_event)
		return (GST_BASE_TRANSFORM_CLASS (parent_class)->src_event (basetransform, event)) && ret;
	else
		return ret;
}

static void
gst_amp_video_scale_dispose (GObject * object) {

	GstAmpVideoScale * videoscale = GST_AMP_VIDEO_SCALE (object);
	GST_INFO_OBJECT (videoscale, "Disposing.");

	// Cleanup
	if (videoscale->main_video_layer) {
		videoscale->main_video_layer->Release(videoscale->main_video_layer);
		videoscale->main_video_layer = NULL;
	}

	if(videoscale->amp) {
		videoscale->amp = NULL;
		_amp = NULL; 
	}

	if(videoscale->dfb) {
		videoscale->dfb->Release(videoscale->dfb);
		videoscale->dfb = NULL;
	}

	if (G_OBJECT_CLASS(parent_class)->dispose)
		G_OBJECT_CLASS (parent_class)->dispose (object);
	GST_INFO_OBJECT (videoscale, "Dispose.");
}
/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_amp_video_scale_plugin_init (GstPlugin * ampvideoscale)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampvideoscale' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_video_scale_debug, "ampvideoscale",
			0, "amp videoscale");

	return gst_element_register (ampvideoscale, "ampvideoscale", GST_RANK_NONE,
			GST_TYPE_AMP_VIDEO_SCALE);
}
