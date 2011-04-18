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
 * SECTION:element-ampvolume
 *
 * @see_also: ampsrc, ampaudiosink, ampvideoscale
 *
 * This element can mute, unmute or adjust the volume.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v ampsrc location=example.mpeg ! tee name=t ! queue ! ampvideoscale ! ampvideosink t. ! queue ! ampvolume ! ampaudiosink
 * ]| Play a media file on Sigma's board and control it with the remote
 * It will not work because you need an application to initialize DirectFB and
 * to handle the remote events.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstampvolume.h"
#include <cdefs_lpb.h>

/* number of steps we use for the mixer interface to go from 0.0 to 1.0 */
# define VOLUME_STEPS           16
#define VOLUME_MAX            1.0

GST_DEBUG_CATEGORY_STATIC (gst_amp_volume_debug);
#define GST_CAT_DEFAULT gst_amp_volume_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};

#define DEFAULT_PROP_MUTE       FALSE
#define DEFAULT_PROP_VOLUME     1.0;

enum
{
	PROP_0,
	PROP_MUTE,
	PROP_VOLUME
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

GST_BOILERPLATE (GstAmpVolume, gst_amp_volume, GstBaseTransform, GST_TYPE_BASE_TRANSFORM);

static void gst_amp_volume_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_amp_volume_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

// Added
static void gst_amp_volume_dispose (GObject * object);
static gboolean gst_amp_volume_src_event_handler (GstBaseTransform * basetransform, GstEvent * event);

static gboolean gst_amp_volume_execute_command (GstAmpVolume * ampvolume, struct SLPBCommand * command);


/* GObject vmethod implementations */

static void
gst_amp_volume_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
					     "AmpVolume",
					     "Resizes video frames",
					     "Scale the video in order that the source caps and the sink caps can negotiate",
					     "Romain Bichet <romain_bichet@sdesigns.com>");

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
}

/* initialize the ampfilter's class */
static void
gst_amp_volume_class_init (GstAmpVolumeClass * klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GstBaseTransformClass * gstbasetransform_class = GST_BASE_TRANSFORM_CLASS (klass);

	gobject_class->set_property = gst_amp_volume_set_property;
	gobject_class->get_property = gst_amp_volume_get_property;

	g_object_class_install_property (gobject_class, PROP_MUTE,
			g_param_spec_boolean ("mute", "Mute", "mute channel",
				DEFAULT_PROP_MUTE,
				G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_VOLUME,
			g_param_spec_double ("volume", "Volume", "volume factor, 1.0=100%",
				0.0, VOLUME_MAX, (13 / 16),
				G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));

	// Added	
	gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_amp_volume_dispose);
	gstbasetransform_class->src_event = GST_DEBUG_FUNCPTR (gst_amp_volume_src_event_handler);
	
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_amp_volume_init (GstAmpVolume * ampvolume,
		       GstAmpVolumeClass * gclass)
{
	ampvolume->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	ampvolume->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

	GST_INFO_OBJECT (ampvolume, "Function init.");

	ampvolume->dfb = NULL;	
	ampvolume->amp = NULL;

	// Retrieve the DirectFB interface
	if (DirectFBCreate(&ampvolume->dfb) != DFB_OK)
		goto cleanup;

	// Retrieve the amp instance
	// GetInterface does not work so _amp is global and should be
	// initialized elsewhere (ampsrc for example)!
	ampvolume->amp = _amp;

	// Retrieve the EventBuffer
	if (ampvolume->amp == NULL)
		goto cleanup;

	if (ampvolume->amp->GetEventBuffer(ampvolume->amp, &(ampvolume->amp_event)) != DFB_OK)
		goto cleanup;

	GST_INFO_OBJECT (ampvolume, "Initialization.");

	return;

cleanup:
	GST_INFO_OBJECT (ampvolume, "Cleanup.");
	gst_amp_volume_dispose (G_OBJECT (ampvolume));
}

static gboolean
gst_amp_volume_set_mute (GstAmpVolume * ampvolume)
{
	struct SLPBCommand command;
	struct SAdjustment adj;

	GST_INFO_OBJECT (ampvolume, "Function setMute.");
	
	adj.type = ADJ_MUTE;
	if (ampvolume->mute == TRUE)
		adj.value.mute = 1;
	else
		adj.value.mute = 0;

	command.cmd = Cmd_Adjust;
	command.common.control.adjustment = (struct SAdjustment *)&adj;

	return gst_amp_volume_execute_command (ampvolume, &command);
}

static gboolean
gst_amp_volume_set_volume (GstAmpVolume * ampvolume)
{
	// amp->volume is between 0 and 1.0
	// volume is between 0x80000000 and volume >> VOLUME_STEPS
	//
	// volume = 0x8000000 >> ( (1 - amp->volume) * VOLUME_STEPS);
	//           guint                 gdouble
	//
	
	struct SLPBCommand command;
	struct SAdjustment adj;
	guint volume, nshift;

	GST_INFO_OBJECT (ampvolume, "Function setVolume.");

	nshift = (int) ((1 - ampvolume->volume) * VOLUME_STEPS);
	volume = 0x80000000 >> nshift;

	adj.type = ADJ_VOLUME;
	adj.value.volume = volume;

	command.cmd = Cmd_Adjust;
	command.common.control.adjustment = (struct SAdjustment *)&adj;

	return gst_amp_volume_execute_command (ampvolume, &command);
}

static void
gst_amp_volume_set_property (GObject * object, guint prop_id,
			       const GValue * value, GParamSpec * pspec)
{
	GstAmpVolume * ampvolume = GST_AMP_VOLUME (object);

	switch (prop_id) {
		case PROP_MUTE:
			GST_OBJECT_LOCK (ampvolume);
			ampvolume->mute = g_value_get_boolean (value);
			gst_amp_volume_set_mute (ampvolume);
			GST_OBJECT_UNLOCK (ampvolume);
			break;
		case PROP_VOLUME:
			GST_OBJECT_LOCK (ampvolume);
			ampvolume->volume = g_value_get_double (value);
			gst_amp_volume_set_volume (ampvolume);
			GST_OBJECT_UNLOCK (ampvolume);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gst_amp_volume_get_property (GObject * object, guint prop_id,
			       GValue * value, GParamSpec * pspec)
{
	GstAmpVolume * ampvolume = GST_AMP_VOLUME (object);

	switch (prop_id) {
		case PROP_MUTE:
			GST_OBJECT_LOCK (ampvolume);
			g_value_set_boolean (value, ampvolume->mute);
			GST_OBJECT_UNLOCK (ampvolume);
			break;
		case PROP_VOLUME:
			GST_OBJECT_LOCK (ampvolume);
			g_value_set_double (value, ampvolume->volume);
			GST_OBJECT_UNLOCK (ampvolume);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

// In order to handle the EOS and to Finish the Media
static gboolean
gst_amp_volume_src_event_handler (GstBaseTransform * basetransform, GstEvent * event)
{
	GstAmpVolume * ampvolume;
	GstStructure * structure;
	gboolean ret;

	gfloat fvolume;

	ret = TRUE;

	ampvolume = GST_AMP_VOLUME_CAST (basetransform);

	GST_INFO_OBJECT (ampvolume, "handling event");

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_NAVIGATION:

			structure = gst_event_get_structure (event);

			if (gst_structure_has_field (structure, "mute"))
			{
				gst_structure_get_boolean (structure, "mute", &(ampvolume->mute));
				ret = gst_amp_volume_set_mute (ampvolume);
			}
			else if (gst_structure_has_field (structure, "volume"))
			{
				gst_structure_get_double (structure, "volume", &(ampvolume->volume));
				// TODO
				// Take into account the previous ret
				ret = gst_amp_volume_set_volume (ampvolume);
			}
			else
			{
			}

			break;
		default:
			break;
	}

	
	if (GST_BASE_TRANSFORM_CLASS (parent_class)->src_event)
		return (GST_BASE_TRANSFORM_CLASS (parent_class)->src_event (basetransform, event) && ret);
	else
		return ret;
}

static gboolean
gst_amp_volume_execute_command (GstAmpVolume * ampvolume, struct SLPBCommand * command)
{
	GST_INFO_OBJECT (ampvolume, "Function Execute Command.");

	gboolean res;
	DFBEvent event;
	char result[8192];

	res = FALSE;
	((struct SStatus *)result)->size = sizeof(result);

	command->common.dataSize = sizeof(struct SLPBCommand);
	command->common.mediaSpace = MEDIA_SPACE_LINEAR_MEDIA;   

	// If the AMP instance doesn't exist, exit.
	// If we are in a wrong (normaly it cannot be the case), exit.
	if ((ampvolume->amp == NULL) || (ampvolume->amp_event == NULL))
		return res;

	if (ampvolume->amp->ExecutePresentationCmd(ampvolume->amp, command, (struct SResult *)&result[0]) == DFB_OK)
	{
		ampvolume->amp_event->GetEvent(ampvolume->amp_event, &event);
		if (!IS_ERROR(((struct SResult *)result)->value))
			res = TRUE;
	}

	if (res == FALSE)
		GST_INFO_OBJECT (ampvolume, "Execute Command Failed.");

	return res;
}


static void
gst_amp_volume_dispose (GObject * object) {

	GstAmpVolume * ampvolume = GST_AMP_VOLUME (object);
	GST_INFO_OBJECT (ampvolume, "Disposing.");

	if(ampvolume->amp) {
		ampvolume->amp = NULL;
		_amp = NULL; // Because _amp is global
	}

	if(ampvolume->amp_event) {
		ampvolume->amp_event->Release(ampvolume->amp_event);
		ampvolume->amp_event = NULL;
	}

	if(ampvolume->dfb) {
		ampvolume->dfb->Release(ampvolume->dfb);
		ampvolume->dfb = NULL;
	}

	if (G_OBJECT_CLASS(parent_class)->dispose)
		G_OBJECT_CLASS (parent_class)->dispose (object);
	GST_INFO_OBJECT (ampvolume, "Dispose.");

}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_amp_volume_plugin_init (GstPlugin * ampvolume)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampvideoscale' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_volume_debug, "ampvolume",
			0, "amp volume");

	return gst_element_register (ampvolume, "ampvolume", GST_RANK_NONE,
			GST_TYPE_AMP_VOLUME);
}
