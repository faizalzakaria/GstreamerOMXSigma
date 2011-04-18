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
 * SECTION:element-ampsrc
 * @see_also: ampaudiosink, ampvideosink
 *
 * This element is able to play, given a url,  a file on Sigma's board.
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
#include "gstampsrc.h"

#include <assert.h>

GST_DEBUG_CATEGORY_STATIC (gst_amp_src_debug);
#define GST_CAT_DEFAULT gst_amp_src_debug

#define DEFAULT_PRESENTATION FALSE

/* Filter signals and args */
enum
{
	PROP_0,
	ARG_LOCATION,
	ARG_PRESENTATION
};


/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS_ANY);

GST_BOILERPLATE (GstAmpSrc, gst_amp_src, GstBaseSrc, GST_TYPE_BASE_SRC);

static void gst_amp_src_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec);
static void gst_amp_src_get_property (GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec);

// Added
static gboolean gst_amp_src_set_location (GstAmpSrc * src, const gchar * location);
static GstFlowReturn gst_amp_src_create (GstBaseSrc * src, guint64 offset, guint length, GstBuffer ** buffer);
static gboolean gst_amp_src_start (GstBaseSrc * basesrc);
static GstStateChangeReturn gst_amp_src_change_state (GstElement *element, GstStateChange transition);
static void gst_amp_src_dispose (GObject * object);
static gboolean gst_amp_src_execute_command (GstAmpSrc * src, struct SLPBCommand * command);
static gboolean gst_amp_src_do_seek (GstBaseSrc * src, GstSegment * segment);

/* GObject vmethod implementations */

static void
gst_amp_src_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
			"AMP Source",
			"Source/Audio/Video/Device",
			"Instanciate an AMP instance and handle the state changes.",
			"Romain Bichet <romain_bichet@sdesigns.com>");

	gst_element_class_add_pad_template (element_class,
			gst_static_pad_template_get (&src_factory));
}

/* initialize the ampsrc's class */
static void
gst_amp_src_class_init (GstAmpSrcClass * klass)
{
	GObjectClass *gobject_class;
	GstBaseSrcClass *gstbasesrc_class;
	GstElementClass *gstelement_class;

	gstelement_class = (GstElementClass *) klass;
	gobject_class = G_OBJECT_CLASS (klass);
	gstbasesrc_class = GST_BASE_SRC_CLASS (klass);

	gobject_class->set_property = gst_amp_src_set_property;
	gobject_class->get_property = gst_amp_src_get_property;

	g_object_class_install_property (gobject_class, ARG_LOCATION,
			g_param_spec_string ("location", "File Location",
				"Location of the file to read", NULL,
				G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
				GST_PARAM_MUTABLE_READY));

	g_object_class_install_property (gobject_class, ARG_PRESENTATION,
			g_param_spec_boolean ("presentation", "StartPresentation or StartPresentationTo",
				"Boolean to indicate which function to use. Warning: this feature only works with elementary streams.", DEFAULT_PRESENTATION,
				G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
				GST_PARAM_MUTABLE_PLAYING));

	gstbasesrc_class->create = GST_DEBUG_FUNCPTR (gst_amp_src_create);
	gstbasesrc_class->start = GST_DEBUG_FUNCPTR (gst_amp_src_start);
	gstbasesrc_class->do_seek = GST_DEBUG_FUNCPTR (gst_amp_src_do_seek);
	gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_amp_src_dispose);
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_amp_src_change_state);
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_amp_src_init (GstAmpSrc * src, GstAmpSrcClass * gclass)
{
	GstBaseSrc * basesrc;
	basesrc = GST_BASE_SRC (src);

	src->srcpad = gst_pad_new_from_static_template (&src_factory, "src");

	// Provide a Clock
	// Important if we want to add queues to the pipeline
	gst_element_set_clock (GST_ELEMENT (src), gst_system_clock_obtain());
	gst_base_src_set_live (basesrc, FALSE);

	src->presentation = DEFAULT_PRESENTATION;

	src->dfb = NULL;	
	src->amp = NULL;
	src->amp_event = NULL;
	src->state = GST_AMP_SRC_NULL;

	src->width = 0;
	src->height = 0;

	src->layer = NULL;
	IDirectFBDisplayLayer *main_video_layer = NULL;
	IDirectFBScreen *screen = NULL;
	DFBScreenMixerConfig mixer_config;

	// Create a DirectFB interface
	if (DirectFBCreate(&src->dfb) != DFB_OK)
		goto cleanup;

	// Retrieve the graphics layer
	DFBDisplayLayerConfig dlcfg;

	// Configure the graphic layer
	src->dfb->GetDisplayLayer(src->dfb, DLID_PRIMARY, &src->layer);

	src->layer->SetCooperativeLevel(src->layer, DLSCL_ADMINISTRATIVE);
	src->layer->SetBackgroundMode(src->layer, DLBM_DONTCARE);
	src->layer->EnableCursor(src->layer, 0);

	// TODO
	// If if these values can be retrieved from the AMP instance
	if (src->layer->GetConfiguration(src->layer, &dlcfg) != DFB_OK)
		goto cleanup;

	src->width = dlcfg.width;
	src->height = dlcfg.height;

	dlcfg.flags       = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE | DLCONF_PIXELFORMAT);
	dlcfg.buffermode  = DLBM_FRONTONLY;
	dlcfg.pixelformat = DSPF_ARGB;

	if (src->layer->SetConfiguration(src->layer, &dlcfg) != DFB_OK)
		goto cleanup;

	// Retrieve the main video layer interface
	if (src->dfb->GetDisplayLayer(src->dfb, EM86LAYER_MAINVIDEO, &main_video_layer) != DFB_OK)
		goto cleanup;

	if (main_video_layer->SetCooperativeLevel(main_video_layer, DLSCL_EXCLUSIVE) != DFB_OK)
		goto cleanup;

	// Mixer Configuration
	// Retrieve the screen interface
	if (src->dfb->GetScreen(src->dfb, 0, &screen) != DFB_OK)
		goto cleanup;
	
	if (screen->GetMixerConfiguration(screen, 0, &mixer_config) != DFB_OK)
		goto cleanup;

	// Display only layers
	mixer_config.flags = DSMCONF_LAYERS;

	// The main_video_layer is not added to the mixer.
	// The videosink should do it.
	DFB_DISPLAYLAYER_IDS_EMPTY(mixer_config.layers);

	// Set the mixer configuration
	if (screen->SetMixerConfiguration(screen, 0, &mixer_config) != DFB_OK)
		goto cleanup;

	// Cleanup
	// Screen & Main Video Layer
	screen->Release(screen);
	main_video_layer->Release(main_video_layer);
	screen = NULL;
	main_video_layer = NULL;

	// Initialize an Advanced Media Provider instance
	// Create an AMP interface
	if (src->dfb->GetInterface(src->dfb, "IAdvancedMediaProvider", "EM8630", (void*)0, (void **)&(src->amp)) != DFB_OK)
		goto cleanup;

	// Point the global variable to the AMP instance
	_amp = src->amp;

	// Get the event buffer
	// Required to be done before OpenMedia call
	if (src->amp->GetEventBuffer(src->amp, &(src->amp_event)) != DFB_OK)
		goto cleanup;


	GST_INFO_OBJECT (src, "Initialization.");

	return;

cleanup:
	GST_INFO_OBJECT (src, "Cleanup.");
	gst_amp_src_dispose (G_OBJECT (src));
}

static void
gst_amp_src_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec)
{
	GstAmpSrc *src;

	g_return_if_fail (GST_IS_BASE_SRC (object));
	src = GST_AMPSRC (object);

	GST_INFO_OBJECT (src, "Set Property.");

	switch (prop_id) {
		case ARG_LOCATION:
			gst_amp_src_set_location (src, g_value_get_string (value));
			break;
		case ARG_PRESENTATION:
			src->presentation = g_value_get_boolean (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gst_amp_src_get_property (GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec)
{
	GstAmpSrc *src = GST_AMPSRC (object);

	switch (prop_id) {
		case ARG_LOCATION:
			g_value_set_string (value, src->filename);
			break;
		case ARG_PRESENTATION:
			g_value_set_boolean (value, src->presentation);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */

static gboolean
gst_amp_src_set_location (GstAmpSrc * src, const gchar * location)
{
	GstState state;

	/* the element must be stopped in order to do this */
	GST_OBJECT_LOCK (src);
	state = GST_STATE (src);
	if (state != GST_STATE_READY && state != GST_STATE_NULL)
		goto wrong_state;
	GST_OBJECT_UNLOCK (src);

	g_free (src->filename);

	/* clear the filename if we get a NULL (is that possible?) */
	if (location == NULL) {
		src->filename = NULL;
	} else {
		/* we store the filename as received by the application. On Windoes this
		 * should be UTF8 */
		src->filename = g_strdup (location);
	}

	g_object_notify (G_OBJECT (src), "location");

	return TRUE;

	/* ERROR */
wrong_state:
	{
		g_warning ("Changing the `location' property on filesrc when a file is open is not supported.");
		GST_OBJECT_UNLOCK (src);
		return FALSE;
	}
}

static gboolean
gst_amp_src_do_seek (GstBaseSrc * basesrc, GstSegment * segment)
{
	GstAmpSrc * src;
	struct SLPBCommand command;
	gdouble rate;

	src = GST_AMP_SRC_CAST (basesrc);

	GST_INFO_OBJECT (src, "Function Do_Seek.");

	// If the AMP instance doesn't exist, the function exits.
	// Same if the playback has not started
	if ((src->amp == NULL) || (src->amp_event == NULL) || (src->state == GST_AMP_SRC_NULL) || (src->state == GST_AMP_SRC_READY) || (src->state == GST_AMP_SRC_STOPPED)) 
		goto end;

	// Parse the segment in order to know which command to
	// call
	rate = segment->rate;

	if (rate > 1.0)
		command.cmd = LPBCmd_SCAN_FORWARD;
	else if (rate < 1.0)
		command.cmd = LPBCmd_SCAN_BACKWARD;
	else
		command.cmd = LPBCmd_PLAY;

	command.param2.speed = 1024 * rate;

	// Execute command
	gst_amp_src_execute_command (src, &command);

end:
	if (GST_BASE_SRC_CLASS (parent_class)->do_seek)
		return GST_BASE_SRC_CLASS (parent_class)->do_seek (basesrc, segment);
	else
		return TRUE;
}

/* necessary to go to READY state */
static gboolean
gst_amp_src_start (GstBaseSrc * basesrc)
{
	GstAmpSrc *src;

	DFBEvent event;
	char amp_buffer[8192];
        struct SLPBCommand command;
	struct SMediaFormat media_format;
	struct SAdjustment adj;
	gboolean ret;

	src = GST_AMP_SRC_CAST (basesrc);
	ret = TRUE;

	((struct SStatus *)amp_buffer)->size = sizeof(amp_buffer);
	((struct SStatus *)amp_buffer)->mediaSpace = MEDIA_SPACE_UNKNOWN;

	media_format.mediaType = MTYPE_APP_UNKNOWN;
	media_format.formatValid = FALSE;

	GST_INFO_OBJECT (src, "Function Start.");

	// If state does not equal NULL, it means that the presentation has
	// already been started.
	// So we skip to the end
	if (src->state == GST_AMP_SRC_NULL)
	{
		if ((src->amp == NULL) || (src->amp_event == NULL))
			goto cleanup;

		if (src->amp->OpenMedia(src->amp, src->filename, &media_format, NULL) != DFB_OK)
			goto cleanup;

		// Wait for an event and consume it.
		src->amp_event->WaitForEvent(src->amp_event);
		src->amp_event->GetEvent(src->amp_event, &event);

		src->amp->UploadStatusChanges(src->amp, (struct SStatus *)amp_buffer, DFB_TRUE);

		if (!IS_SUCCESS(((struct SStatus *)amp_buffer)->lastCmd.result))
			goto cleanup;


		// Depending on the property PRESENTATION, the function
		// StartPresentation or StartPresentationTo is called.
		if (src->presentation == FALSE)
		{
			GST_INFO_OBJECT (src, "StartPresentation.");
			if (src->amp->StartPresentation(src->amp, DFB_TRUE) != DFB_OK) 
				goto cleanup;
		}
		else
		{
			// WARNING!
			// This feature only works with elementary streams
			GST_INFO_OBJECT (src, "StartPresentationTo.");

			IDirectFBSurface * primary;
			IDirectFBSurface * surface;
                        DVFrameCallback callback;
                        void *ctx;

			src->layer->GetSurface(src->layer, &primary);

			if (primary == NULL)
				goto cleanup;

			DFBRectangle area = {100,100,640,480};

			if (primary->GetSubSurface(primary, &area, &surface) != DFB_OK)
			{
				surface->Release(surface);
				goto cleanup;
			}

                        src->amp->StartPresentationTo(src->amp, surface, callback, ctx);

                        surface->Release(surface);
		}

		// Wait for an event and consume it.
		src->amp_event->WaitForEvent(src->amp_event);
		src->amp_event->GetEvent(src->amp_event, &event);

		src->amp->UploadStatusChanges(src->amp, (struct SStatus *)amp_buffer, DFB_TRUE);

		if (!IS_SUCCESS(((struct SStatus *)amp_buffer)->lastCmd.result))
			goto cleanup;

		// By default, mute
		// The GstAmpAudioSink, will unmute if it is in the pipeline
		adj.type = ADJ_MUTE;
		adj.value.mute = 1;

		command.cmd = Cmd_Adjust;
		command.common.control.adjustment = (struct SAdjustment *)&adj;

		ret = gst_amp_src_execute_command (src, &command);
	}
	else if (src->state == GST_AMP_SRC_STOPPED)
	{
		// If the state does not equal to NULL
		// it is not the first time this function is called
		// so OpenMedia is not needed
		// however a PLAY command is needed
		command.cmd = LPBCmd_PLAY;
		ret = gst_amp_src_execute_command (src, &command);
	}
	src->state = GST_AMP_SRC_READY;

	return ret;

cleanup:
	gst_amp_src_dispose (G_OBJECT(src));

	GST_INFO_OBJECT (src, "Could not start the presentation. Disposing.");
	return FALSE;
}

// This function is called after init and start.
// It actually does the processing while it returns GST_FLOW_OK.
static GstFlowReturn
gst_amp_src_create (GstBaseSrc * basesrc, guint64 offset, guint length, GstBuffer ** buffer)
{
	GstAmpSrc *src;
 	GstBuffer *buf;	
	GstCaps * caps;
	GstStructure * structure;

	DFBEvent event;
	char amp_buffer[8192];

	src = GST_AMP_SRC_CAST (basesrc);

	((struct SStatus *)amp_buffer)->size = sizeof(amp_buffer);
	((struct SStatus *)amp_buffer)->mediaSpace = MEDIA_SPACE_UNKNOWN;

	// Caps
	caps = gst_caps_new_empty ();
	structure = gst_structure_empty_new (gst_caps_to_string (gst_pad_get_caps (src->srcpad)));
	gst_structure_set (structure, "width", G_TYPE_INT, src->width, "height", G_TYPE_INT, src->height, NULL);
	gst_caps_append_structure (caps, structure);

	// The output is only empty buffers
	buf = gst_buffer_new (); // create buf
	gst_buffer_set_caps (buf, caps); // set caps
	gst_caps_unref (caps);
	*buffer = buf;

	// If the status has just stopped (state != ready), trigger an EOS.
	// The stop function will do the rest
	// If the status is playing, upadate the state.

	// Wait for the event but do not consume it (it is possible that it is
	// a command event.
	src->amp_event->WaitForEvent(src->amp_event);
	src->amp_event->GetEvent(src->amp_event, &event);

	if (src->state == GST_AMP_SRC_NULL)
		return GST_FLOW_UNEXPECTED;

	if (src->amp->UploadStatusChanges(src->amp, (struct SStatus *)amp_buffer, DFB_TRUE) == DFB_OK)
	{
		if (((struct SStatus *)amp_buffer)->flags & SSTATUS_MODE)
		{
			switch (((struct SStatus *)amp_buffer)->mode.flags) {
				case SSTATUS_MODE_STOPPED:
					if (src->state == GST_AMP_SRC_READY)
						return GST_FLOW_OK;
					src->state = GST_AMP_SRC_STOPPED;
					return GST_FLOW_UNEXPECTED;
				case SSTATUS_MODE_PLAYING:
					src->state = GST_AMP_SRC_PLAYING;
					break;
				default:
					break;
			}
			return GST_FLOW_OK;
		}
	}

	// Cleanup?
	GST_INFO_OBJECT (src, "Presentation Stopped. Unable to execute command.");
	return GST_FLOW_CUSTOM_ERROR;

}

// Override the change_state function in order to send the right command to
// the AMP instance
static GstStateChangeReturn
gst_amp_src_change_state (GstElement *element, GstStateChange transition)
{
	GstAmpSrc *src;
	GstStateChangeReturn ret;
	struct SLPBCommand command;

	src = GST_AMPSRC (element);
	if (GST_ELEMENT_CLASS (parent_class)->change_state)
		ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
	else
		ret = GST_STATE_CHANGE_SUCCESS;

	GST_INFO_OBJECT (src,"Changing state : from %d to %d.\n", GST_STATE_TRANSITION_CURRENT (transition), GST_STATE_TRANSITION_NEXT (transition));

	// If something went wrong before, continue but return a failure
	if ((src->amp == NULL) || (src->amp_event == NULL))
		return GST_STATE_CHANGE_FAILURE;

	// Here, we just handle the PAUSED/PLAYING changes.
	// Event Handler, Start and Stop functions do the rest.
	//
	// FIXME: For a GStreamer point of view of STOP command or a PAUSE
	// command are the same.
	// So for a STOP command, we send two commands : a PAUSE_OFF command
	// and then a STOP command.
	// Same for a PLAY command when the playback is stopped.
	switch (src->state) {
		case GST_AMP_SRC_NULL:
		case GST_AMP_SRC_READY:
		case GST_AMP_SRC_STOPPED:
			break;
		case GST_AMP_SRC_PAUSED:
		case GST_AMP_SRC_PLAYING:
		case GST_AMP_SRC_SCAN:
			switch (GST_STATE_PENDING (element)) {
				case GST_STATE_READY:
					// Stop the playback as soon as
					// possible
					command.cmd = LPBCmd_STOP;
					break;
				case GST_STATE_PAUSED:
					// If the playback is already PAUSED,
					// skip it
					if (src->state == GST_AMP_SRC_PAUSED)
						return ret;
					command.cmd = LPBCmd_PAUSE_ON;
					break;
				case GST_STATE_PLAYING:
					// If the playback is already PLAYING,
					// skip it
					if (src->state == GST_AMP_SRC_PLAYING)
						return ret;
					command.cmd = LPBCmd_PLAY;
					break;
				default:
					return ret;
			}
			if (gst_amp_src_execute_command (src, &command) == FALSE) 
				ret = GST_STATE_CHANGE_FAILURE;
			break;
		default:
			break;
	}
	return ret;
}

static gboolean
gst_amp_src_execute_command (GstAmpSrc * src, struct SLPBCommand * command)
{
	GST_INFO_OBJECT (src, "Function Execute Command.");

	gboolean res;
	DFBEvent event;
	char result[8192];

	res = FALSE;
	((struct SStatus *)result)->size = sizeof(result);

	command->common.dataSize = sizeof(struct SLPBCommand);
	command->common.mediaSpace = MEDIA_SPACE_LINEAR_MEDIA;   

	// If the AMP instance doesn't exist, exit.
	if ((src->amp == NULL) || (src->amp_event == NULL)) { return res;}

	// We skip all the stupid commands
	// Execept if the command is mute
	if (command->cmd != Cmd_Adjust)
	{
		switch (src->state) {
			case GST_AMP_SRC_NULL:
				return TRUE;
			case GST_AMP_SRC_READY:
				break;
			case GST_AMP_SRC_STOPPED:
				if ((command->cmd == LPBCmd_STOP) ||
						(command->cmd == LPBCmd_SCAN_FORWARD) ||
						(command->cmd == LPBCmd_SCAN_BACKWARD))
					return TRUE;
				break;
			case GST_AMP_SRC_PAUSED:
				if (command->cmd == LPBCmd_PAUSE_ON)
					return TRUE;
				break;
			case GST_AMP_SRC_PLAYING:
				if ((command->cmd == LPBCmd_PAUSE_OFF) || (command->cmd == LPBCmd_PLAY))
					return TRUE;
				break;
			case GST_AMP_SRC_SCAN:
				break;
			default:
				break;
		}
	}

	// ExecutePresentationCmd and not PostPresentationCmd
	// beacuse this one is synchronous
	if (src->amp->ExecutePresentationCmd(src->amp, command, (struct SResult *)&result[0]) == DFB_OK)
	{
		// Check the result
		src->amp_event->GetEvent(src->amp_event, &event);
		if (!IS_ERROR(((struct SResult *)result)->value))
			res = TRUE;
	}

	if (res == FALSE)
		GST_INFO_OBJECT (src, "Execute Command Failed.");
	else if (command->cmd != Cmd_Adjust)
	{
		// If the command was successfull, update the state
		switch (command->cmd) {
			case LPBCmd_STOP:
				src->state = GST_AMP_SRC_STOPPED;
				break;
			case LPBCmd_PAUSE_ON:
				src->state = GST_AMP_SRC_PAUSED;
				break;
			case LPBCmd_PAUSE_OFF:
			case LPBCmd_PLAY:
				if (src->state != GST_AMP_SRC_READY)
					src->state = GST_AMP_SRC_PLAYING;
				break;
			case LPBCmd_SCAN_FORWARD:
			case LPBCmd_SCAN_BACKWARD:
				src->state = GST_AMP_SRC_SCAN;
				break;
			default:
				break;
		}
	}

	return res;
}

static void
gst_amp_src_dispose (GObject * object)
{
	GstAmpSrc *src = GST_AMPSRC (object);

	if (src->layer) {
		src->layer->Release(src->layer);
		src->layer = NULL;
	}

	if(src->amp) {
		src->amp->Release(src->amp);
		src->amp = NULL;
		_amp = NULL; // Because _amp is global
	}

	if(src->amp_event) {
		src->amp_event->Release(src->amp_event);
		src->amp_event = NULL;
	}

	if(src->dfb) {
		src->dfb->Release(src->dfb);
		src->dfb = NULL;
	}

	if (G_OBJECT_CLASS(parent_class)->dispose)
		G_OBJECT_CLASS (parent_class)->dispose (object);
	GST_INFO_OBJECT (src, "Dispose.");
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_amp_src_plugin_init (GstPlugin * ampsrc)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampsrc' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_src_debug, "ampsrc",
			0, "amp source");

	return gst_element_register (ampsrc, "ampsrc", GST_RANK_NONE,
			GST_TYPE_AMPSRC);
}
