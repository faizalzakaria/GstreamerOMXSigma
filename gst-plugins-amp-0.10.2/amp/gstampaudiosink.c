/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 Romain Bichet romain_bichet@sdesigns.com>
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
 * SECTION:element-ampaudiosink
 * @see_also: ampsrc, ampvideosink
 *
 * This element will activate the audio output and handle audio related custom events from
 * the main application (pan and move).
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v ampsrc location=example.mpeg ! tee name=t ! queue ! ampvideoscale ! ampvideosink t. ! queue ! ampvolume ! ampaudiosink
 * ]| Play a media file on Sigma's board and control it with the remote.
 * It will not work because you need an application to initialize DirectFB and
 * to handle the remote events.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include "gstampaudiosink.h"

#include <assert.h>

GST_DEBUG_CATEGORY_STATIC (gst_amp_audio_sink_debug);
#define GST_CAT_DEFAULT gst_amp_audio_sink_debug

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
#define DEFAULT_PROP_MUTE TRUE

enum
{
	PROP_0,
	PROP_MUTE,
};

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS_ANY);

GST_BOILERPLATE (GstAmpAudioSink, gst_amp_audio_sink, GstBaseSink, GST_TYPE_BASE_SINK);

static void gst_amp_audio_sink_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_amp_audio_sink_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

// Added
static void gst_amp_audio_sink_dispose (GObject * object);
static gboolean gst_amp_audio_sink_event_handler (GstBaseSink * basesink, GstEvent * event);
static gboolean gst_amp_audio_sink_execute_command (GstAmpAudioSink * sink, struct SLPBCommand * command);

/* GObject vmethod implementations */

static void
gst_amp_audio_sink_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
			"AMP Audio Sink",
			"Sink/Audio/Device",
			"AMP Audio Sink which handle basic features (mute, unmute, ...).",
			"Romain Bichet <romain_bichet@sdesigns.com>");

	gst_element_class_add_pad_template (element_class,
			gst_static_pad_template_get (&sink_factory));
}

/* initialize the ampaudiosink's class */
static void
gst_amp_audio_sink_class_init (GstAmpAudioSinkClass * klass)
{
	GObjectClass *gobject_class;
	GstBaseSinkClass *gstbasesink_class;

	gobject_class = (GObjectClass *) klass;
	gstbasesink_class = (GstBaseSinkClass *) klass;

	gobject_class->set_property = gst_amp_audio_sink_set_property;
	gobject_class->get_property = gst_amp_audio_sink_get_property;

	g_object_class_install_property (gobject_class, PROP_MUTE,
			g_param_spec_boolean ("mute", "Mute", "mute channel",
				DEFAULT_PROP_MUTE,
				G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));

	gstbasesink_class->event = GST_DEBUG_FUNCPTR (gst_amp_audio_sink_event_handler);
	gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_amp_audio_sink_dispose);

}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_amp_audio_sink_init (GstAmpAudioSink * sink, GstAmpAudioSinkClass * gclass)
{
	GST_INFO_OBJECT (sink, "Function init.");

	sink->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_base_sink_set_sync (GST_BASE_SINK (sink), FALSE);

	sink->dfb = NULL;	
	sink->amp = NULL;
	sink->amp_event = NULL;

	sink->mute = DEFAULT_PROP_MUTE;
	sink->volume = 1.0 * (13.0 / 16.0);

	// Retrieve the DirectFB interface
	if (DirectFBCreate(&sink->dfb) != DFB_OK)
		goto cleanup;

	// Retrieve the amp instance
	// GetInterface does not work so _amp is global and should be
	// initialized elsewhere (ampsrc for example)!
	sink->amp = _amp;

	// Retrieve the EventBuffer
	if (sink->amp == NULL)
		goto cleanup;

	if (sink->amp->GetEventBuffer(sink->amp, &(sink->amp_event)) != DFB_OK)
		goto cleanup;

	GST_INFO_OBJECT (sink, "Initialization.");

	return;

cleanup:
	GST_INFO_OBJECT (sink, "Cleanup.");
	gst_amp_audio_sink_dispose (G_OBJECT (sink));
}

static void
gst_amp_audio_sink_set_property (GObject * object, guint prop_id,
			       const GValue * value, GParamSpec * pspec)
{
	GstAmpAudioSink * sink = GST_AMP_AUDIO_SINK_CAST (object);

	gboolean ret;
	struct SLPBCommand command;
	struct SAdjustment adj;

	switch (prop_id) {
		case PROP_MUTE:
			GST_OBJECT_LOCK (sink);
			sink->mute = g_value_get_boolean (value);

			adj.type = ADJ_MUTE;
			if (sink->mute == TRUE)
				adj.value.mute = 1;
			else
				adj.value.mute = 0;

			command.cmd = Cmd_Adjust;
			command.common.control.adjustment = (struct SAdjustment *)&adj;

			ret = gst_amp_audio_sink_execute_command (sink, &command);

			if (!ret)
			{
				GST_INFO_OBJECT (sink, "Execute Command Failed.");
				sink->mute = !sink->mute;
			}

			GST_OBJECT_UNLOCK (sink);

			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gst_amp_audio_sink_get_property (GObject * object, guint prop_id,
			       GValue * value, GParamSpec * pspec)
{
	GstAmpAudioSink * sink = GST_AMP_AUDIO_SINK_CAST (object);

	switch (prop_id) {
		case PROP_MUTE:
			GST_OBJECT_LOCK (sink);
			g_value_set_boolean (value, sink->mute);
			GST_OBJECT_UNLOCK (sink);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */
static gboolean
gst_amp_audio_sink_event_handler (GstBaseSink * basesink, GstEvent * event)
{
	GstAmpAudioSink *sink;
	gboolean ret;
	GstStructure * estructure, * cstructure;
	GstPad * sinkpad, * peer;

	struct SLPBCommand command;
	struct SAdjustment adj;

	sink = GST_AMP_AUDIO_SINK_CAST (basesink);
	ret = TRUE;

	GST_INFO_OBJECT (sink, "Function Event Handler.");

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CUSTOM_DOWNSTREAM_OOB:

			// Parse the event in order to know what command type
			// it is.
			// ADJ_MUTE or ADJ_VOLUME?
			//
			// Do not need to free the structure
			estructure = gst_event_get_structure (event);
			cstructure = gst_structure_empty_new ("audio/x-raw-int" );

			sinkpad = gst_element_get_pad (sink, "sink");
			peer = gst_pad_get_peer (sinkpad);


			if (g_strrstr (gst_structure_get_name (estructure), "MUTE"))
			{
				sink->mute = !sink->mute;
				gst_structure_set (cstructure, "mute", G_TYPE_BOOLEAN, sink->mute, NULL);
			}

			else if (g_strrstr (gst_structure_get_name (estructure), "VOLUME_UP"))
			{
				g_print ("sink->volume : %f\n", sink->volume);
				sink->volume += 0.1;
				if (sink->volume > 1.0)
					sink->volume = 1.0;
				g_print ("sink->volume : %f\n", sink->volume);
				gst_structure_set (cstructure, "volume", G_TYPE_DOUBLE, sink->volume, NULL);
			}

			else if (g_strrstr (gst_structure_get_name (estructure), "VOLUME_DOWN"))
			{
				sink->volume += - 0.1;
				if (sink->volume < 0)
					sink->volume = 0;
				g_print ("sink->volume : %f\n", sink->volume);
				gst_structure_set (cstructure, "volume", G_TYPE_DOUBLE, sink->volume, NULL);
			}
			else
			{
				gst_object_unref (sinkpad);
				gst_object_unref (peer);
				gst_structure_free (cstructure);
				break;
			}

			// Do not need to unref the event, so do not
			// need to free the structure
			ret = gst_pad_send_event (peer, gst_event_new_navigation (cstructure));

			gst_object_unref (sinkpad);
			gst_object_unref (peer);

			break;
		case GST_EVENT_NEWSEGMENT:
			// Unmute
			// Have to do it this way because if the volume
			// element is not present in the pipeline, we still
			// want to unmute the playback
			g_object_set (G_OBJECT(sink), "mute", FALSE, NULL);
			break;
		default:
			break;
	}

	if (GST_BASE_SINK_CLASS (parent_class)->event)
		return (GST_BASE_SINK_CLASS (parent_class)->event (basesink, event) && ret);
	else
		return ret;
}

static gboolean
gst_amp_audio_sink_execute_command (GstAmpAudioSink * sink, struct SLPBCommand * command)
{
	GST_INFO_OBJECT (sink, "Function Execute Command.");

	gboolean res;
	DFBEvent event;
	char result[8192];

	res = FALSE;
	((struct SStatus *)result)->size = sizeof(result);

	command->common.dataSize = sizeof(struct SLPBCommand);
	command->common.mediaSpace = MEDIA_SPACE_LINEAR_MEDIA;   

	// If the AMP instance doesn't exist, exit.
	// If we are in a wrong (normaly it cannot be the case), exit.
	if ((sink->amp == NULL) || (sink->amp_event == NULL))
		return res;

	if (sink->amp->ExecutePresentationCmd(sink->amp, command, (struct SResult *)&result[0]) == DFB_OK)
	{
		sink->amp_event->GetEvent(sink->amp_event, &event);
		if (!IS_ERROR(((struct SResult *)result)->value))
			res = TRUE;
	}

	if (res == FALSE)
		GST_INFO_OBJECT (sink, "Execute Command Failed.");

	return res;
}

static void
gst_amp_audio_sink_dispose (GObject * object)
{
	GstAmpAudioSink *sink = GST_AMPAUDIOSINK (object);

	if(sink->amp) {
		sink->amp = NULL;
		_amp = NULL; // Because _amp is global
	}

	if(sink->amp_event) {
		sink->amp_event->Release(sink->amp_event);
		sink->amp_event = NULL;
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
gst_amp_audio_sink_plugin_init (GstPlugin * ampaudiosink)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampaudiosink' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_audio_sink_debug, "ampaudiosink",
			0, "amp audio sink");

	return gst_element_register (ampaudiosink, "ampaudiosink", GST_RANK_NONE,
			GST_TYPE_AMPAUDIOSINK);
}
