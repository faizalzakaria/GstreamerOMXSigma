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
 * SECTION:element-ampfilter
 *
 * FIXME:Describe ampfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! ampfilter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/tag/tag.h>

#include "gstampfilter.h"
#include <assert.h>

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x...)  \
	err = x;        \
if (err != DFB_OK) {    \
	printf("\n\n%s <%d>:\n\t", __FILE__, __LINE__ );        \
	DirectFBError( #x, err );       \
	goto cleanup; \
} 


GST_DEBUG_CATEGORY_STATIC (gst_amp_filter_debug);
#define GST_CAT_DEFAULT gst_amp_filter_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
								    GST_PAD_SINK,
								    GST_PAD_ALWAYS,
								    GST_STATIC_CAPS ("video/mpegts; video/mpeg; video/x-ms-asf; video/x-wmv; video/x-mpeg; video/x-msvideo")
								   );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
								   GST_PAD_SRC,
								   GST_PAD_ALWAYS,
								   GST_STATIC_CAPS ("ANY")
								  );

GST_BOILERPLATE (GstAmpFilter, gst_amp_filter, GstElement, GST_TYPE_ELEMENT);

static void gst_amp_filter_set_property (GObject * object, guint prop_id,
					   const GValue * value, GParamSpec * pspec);
static void gst_amp_filter_get_property (GObject * object, guint prop_id,
					   GValue * value, GParamSpec * pspec);

static gboolean gst_amp_filter_set_caps (GstPad * pad, GstCaps * caps);
static GstFlowReturn gst_amp_filter_chain (GstPad * pad, GstBuffer * buf);


// Added
static GstStateChangeReturn gst_amp_filter_change_state (GstElement *element, GstStateChange transition);
static void gst_amp_filter_dispose (GObject * object);
static gboolean gst_amp_filter_sink_event (GstPad * pad, GstEvent * event);
static gboolean gst_amp_filter_post_command (GstAmpFilter * filter, struct SLPBCommand command, gboolean stopped);


/* GObject vmethod implementations */

static void
gst_amp_filter_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
					     "AmpFilter",
					     "Filter in order to use DCCHD",
					     "Instanciate an AMP instance and use it in order to playback using DCCHD",
					     "Romain Bichet <romain_bichet@sdesigns.com>");

	gst_element_class_add_pad_template (element_class,
					    gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class,
					    gst_static_pad_template_get (&sink_factory));
}

/* initialize the ampfilter's class */
static void
gst_amp_filter_class_init (GstAmpFilterClass * klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	GstElementClass *gstelement_class = GST_ELEMENT_CLASS(klass);

	gobject_class->set_property = gst_amp_filter_set_property;
	gobject_class->get_property = gst_amp_filter_get_property;

	g_object_class_install_property (gobject_class, PROP_SILENT,
					 g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
							       FALSE, G_PARAM_READWRITE));

	// Added	
	gstelement_class->change_state = gst_amp_filter_change_state; // To handle state changes
	gobject_class->dispose = gst_amp_filter_dispose; // To handle the uninit
	
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_amp_filter_init (GstAmpFilter * filter,
		       GstAmpFilterClass * gclass)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_setcaps_function (filter->sinkpad,
				      GST_DEBUG_FUNCPTR(gst_amp_filter_set_caps));
	gst_pad_set_getcaps_function (filter->sinkpad,
				      GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
	gst_pad_set_chain_function (filter->sinkpad,
				    GST_DEBUG_FUNCPTR(gst_amp_filter_chain));

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	gst_pad_set_getcaps_function (filter->srcpad,
				      GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));

	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
	filter->silent = FALSE;

	gst_pad_set_event_function (GST_PAD (filter->sinkpad), GST_DEBUG_FUNCPTR (gst_amp_filter_sink_event));

	filter->dfb = NULL;	
	filter->amp = NULL;
	filter->amp_event = NULL;
	filter->data_buffer = NULL;
	filter->media_format = (struct SMediaFormat) {0,};
	filter->pause = FALSE;
	filter->config_media = FALSE;

	IDirectFBDisplayLayer *main_video_layer = NULL;
	IDirectFBScreen *screen = NULL;
	DFBScreenMixerConfig mixer_config;
	DFBResult err;

	// Create a DirectFB interface
	DFBCHECK(DirectFBCreate(&filter->dfb));

	// Retrieve the main video layer interface
	DFBCHECK(filter->dfb->GetDisplayLayer(filter->dfb, EM86LAYER_MAINVIDEO, &main_video_layer));
	DFBCHECK(main_video_layer->SetCooperativeLevel(main_video_layer, DLSCL_EXCLUSIVE));

	// Mixer Configuration
	// (set by default)
	//
	// Retrieve the screen interface
	DFBCHECK(filter->dfb->GetScreen(filter->dfb, 0, &screen));
	
	DFBCHECK(screen->GetMixerConfiguration(screen, 0, &mixer_config));

	// Display only layers
	mixer_config.flags = DSMCONF_LAYERS;

	// Enable the main video layer
	DFB_DISPLAYLAYER_IDS_EMPTY(mixer_config.layers);
	DFB_DISPLAYLAYER_IDS_ADD(mixer_config.layers, EM86LAYER_MAINVIDEO);

	// Set the mixer configuration
	DFBCHECK(screen->SetMixerConfiguration(screen, 0, &mixer_config));

	// Cleanup
	// Screen & Main Video Layer
	screen->Release(screen);
	main_video_layer->Release(main_video_layer);
	screen = NULL;
	main_video_layer = NULL;

	// Initialize an Advanced Media Provider instance

	// Create an AMP interface
	DFBCHECK(filter->dfb->GetInterface(filter->dfb, "IAdvancedMediaProvider", NULL, (void*)0, (void *)&(filter->amp)));

	// Get the event buffer
	// Required to be done before OpenMedia call
	filter->media_format.mediaType = MTYPE_APP_UNKNOWN;
	filter->media_format.formatValid = 0;

	// Try to get the EventBuffer
	DFBCHECK(filter->amp->GetEventBuffer(filter->amp, &(filter->amp_event)));

	// Create and Set the dataBuffer
	DFBCHECK(filter->dfb->CreateDataBuffer(filter->dfb, NULL, &(filter->data_buffer) ));

	return;

cleanup:
	g_printerr ("Cleanup.....\n");
	gst_amp_filter_dispose (G_OBJECT (filter));
}

	static void
gst_amp_filter_set_property (GObject * object, guint prop_id,
			       const GValue * value, GParamSpec * pspec)
{
	GstAmpFilter *filter = GST_AMPFILTER (object);

	switch (prop_id) {
		case PROP_SILENT:
			filter->silent = g_value_get_boolean (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

	static void
gst_amp_filter_get_property (GObject * object, guint prop_id,
			       GValue * value, GParamSpec * pspec)
{
	GstAmpFilter *filter = GST_AMPFILTER (object);

	switch (prop_id) {
		case PROP_SILENT:
			g_value_set_boolean (value, filter->silent);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */
/* this function handles the link with other elements */
	static gboolean
gst_amp_filter_set_caps (GstPad * pad, GstCaps * caps)
{
	GstAmpFilter *filter;
	GstPad *otherpad;

	filter = GST_AMPFILTER (gst_pad_get_parent (pad));
	otherpad = (pad == filter->srcpad) ? filter->sinkpad : filter->srcpad;
	gst_object_unref (filter);

	return gst_pad_set_caps (otherpad, caps);
}

/* chain function
 * this function does the actual processing
 */
	static GstFlowReturn
gst_amp_filter_chain (GstPad * pad, GstBuffer * buf)
{
	GstAmpFilter *filter;
	unsigned int blen;
	filter = GST_AMPFILTER (GST_OBJECT_PARENT (pad));

	DFBEvent event;

	char buffer[8192];
	((struct SStatus *)buffer)->size = sizeof(buffer);
	((struct SStatus *)buffer)->mediaSpace = MEDIA_SPACE_UNKNOWN;

	// If the AMP instance doesn't exist, the function exits.
	if (filter->amp == NULL) { return gst_pad_push (filter->srcpad, buf);}

	// GST_BUFFER_SIZE casts the buffer before calculating the size
	if (filter->data_buffer->PutData(filter->data_buffer, buf->data, GST_BUFFER_SIZE(buf)) != DFB_OK) {
		g_printerr ("Cannot put the data\n");
		goto endMediaPresentation;
	}

	// At leat 1100x1024 bits of data are needed for the stream detection
	filter->data_buffer->GetLength(filter->data_buffer, &blen);

	// TODO Do we really need 1024 * 1100 ?
	if ((filter->config_media == FALSE) && (blen > 1100 * 1024)) {

		
		filter->config_media = TRUE; // In order to do these steps only once
		filter->amp->SetDataBuffer(filter->amp, filter->data_buffer, &(filter->media_format));

		if (filter->amp->OpenMedia(filter->amp, NULL, &(filter->media_format), NULL) != DFB_OK) {
			g_printerr ("Could not open media !!!\n");
			goto endMediaPresentation;
		}

		// Check the status
		filter->amp_event->WaitForEvent(filter->amp_event);
		filter->amp_event->GetEvent(filter->amp_event, &event);

		// If there is an event, upload status change
		// Otherwise, exit
		assert (filter->amp_event->HasEvent(filter->amp_event) != DFB_OK);
		filter->amp->UploadStatusChanges(filter->amp, (struct SStatus *)buffer, DFB_TRUE);

		if (IS_SUCCESS(((struct SStatus *)buffer)->lastCmd.result)) // succeeded
		{
			// Start Presentation
			if (filter->amp->StartPresentation(filter->amp, DFB_TRUE) == DFB_OK) {

				// Check the status
				filter->amp_event->WaitForEvent(filter->amp_event);
				filter->amp_event->GetEvent(filter->amp_event, &event);

				// If there is an event, upload status change
				// Otherwise, exit
				assert (filter->amp_event->HasEvent(filter->amp_event) != DFB_OK);
				filter->amp->UploadStatusChanges(filter->amp, (struct SStatus *)buffer, DFB_TRUE);

				if (IS_SUCCESS(((struct SStatus *)buffer)->lastCmd.result)) { g_print ("Playing...\n");}
				else
				{
					g_printerr ("Could not open media!!!\n");
					goto endMediaPresentation;
				}
			}
			else
			{
				g_printerr ("Unable to start presentation, abort!!!\n");
				goto endMediaPresentation;
			}
		}
		else
                {
                        g_printerr ("Could not open media!!!\n");
			goto endMediaPresentation;
                }

	}

	// In order to wait for AMP to process data
	if (blen > 128 * 1024) { /*g_print ("Waiting.............OK\n");*/ usleep (30000);}

	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);

endMediaPresentation:

	g_printerr ("EndMediaPresentation Cleanup...\n");
	gst_amp_filter_dispose (G_OBJECT(filter));

	/* just push out the incoming buffer without touching it */
	gst_pad_push (filter->srcpad, buf);
	return GST_FLOW_CUSTOM_ERROR;

}

/* Override the change_state function in order to print out the current status */
static GstStateChangeReturn
gst_amp_filter_change_state (GstElement *element, GstStateChange transition)
{
	GstAmpFilter *filter = GST_AMPFILTER (element);
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

        struct SLPBCommand command;
	command.common.dataSize = sizeof(struct SLPBCommand);
	command.common.mediaSpace = MEDIA_SPACE_LINEAR_MEDIA;   

	ret = parent_class->change_state (element, transition);
	// If the AMP instance doesn't exist, the function exits.
	// Same if the state change has already failed
	if ((ret == GST_STATE_CHANGE_FAILURE) || (filter->amp == NULL)) { return ret;}

	// Print out when the state is changing
	g_print ("Changing state : from %d to %d.\n", GST_STATE_TRANSITION_CURRENT (transition), GST_STATE_TRANSITION_NEXT (transition));

	// Send the command
	switch (transition) {
		case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
			if (filter->pause) { command.cmd = LPBCmd_PAUSE_OFF;}
			else { command.cmd = LPBCmd_PLAY;}
			filter->pause = FALSE;
			break;
		case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
			if (filter->config_media == FALSE) { gst_element_set_state (element, GST_STATE_PLAYING);}
			if (GST_STATE_PENDING (element) == GST_STATE_PAUSED) {
				command.cmd = LPBCmd_PAUSE_ON;
				filter->pause = TRUE;
				break;
			}
			else { return ret;}
		case GST_STATE_CHANGE_PAUSED_TO_READY:
			command.cmd = LPBCmd_STOP;
			filter->pause = FALSE;
			break;
		case GST_STATE_CHANGE_READY_TO_NULL:
			// If the user wants to stop the playback
			// before the EOS, the media has to be
			// finished in order to release AMP
			filter->data_buffer->Finish (filter->data_buffer);
			return ret;
		default:
			return ret;
	}

	// Post the command and check the result
	// we don't need that the playback is stopped so third argument
	// equals FALSE
	if (gst_amp_filter_post_command (filter, command, FALSE) == FALSE) { ret = GST_STATE_CHANGE_FAILURE;}

	return ret;
}

// In order to handle the EOS and to Finish the Media
static gboolean
gst_amp_filter_sink_event (GstPad * pad, GstEvent * event)
{
	gboolean res;
	GstAmpFilter *filter = GST_AMPFILTER (gst_pad_get_parent (pad));

	struct SLPBCommand command;
	command.common.dataSize = sizeof(struct SLPBCommand);
	command.common.mediaSpace = MEDIA_SPACE_LINEAR_MEDIA;   

	// If the AMP instance doesn't exist, the function exits.
	if (filter->amp == NULL) { return res;}

	switch (GST_EVENT_TYPE(event)) {
		case GST_EVENT_NEWSEGMENT:
			command.param2.speed = 10240 * 5;
			command.cmd = LPBCmd_SCAN_FORWARD;

			// TODO: FORWARD and REWIND are not working with -sdb
			// Execute command
			// gst_amp_filter_post_command (filter, command, FALSE);
			break;
		case GST_EVENT_EOS:
			g_print ("Handling EOS.........................\n");
			filter->data_buffer->Finish (filter->data_buffer);

			command.param2.speed = 10240;
			command.cmd = LPBCmd_STOP;

			// wait for the playback to be finished (third
			// argument equals TRUE) and post a stop command
			// exit the loop
			while (1) {
				if (gst_amp_filter_post_command (filter, command, TRUE) == TRUE) { break;}
				// In order not to call the loop too many times
				usleep (500000);
			}
			g_print ("EOS Handled.........................\n");
			break;
		default:
			break;
	}

	// Don't forget to propagate the event upstream
	res = gst_pad_push_event (filter->srcpad, event);

	// unref needed
	gst_object_unref (filter);

	return res;

}

static gboolean
gst_amp_filter_post_command (GstAmpFilter * filter, struct SLPBCommand command, gboolean stopped)
{
	// TODO in order not to set up res to FALSE when we do hasevent
	gboolean res = NULL;
	DFBEvent event;
	char buffer[8192];

	((struct SStatus *)buffer)->size = sizeof(buffer);
	((struct SStatus *)buffer)->mediaSpace = MEDIA_SPACE_UNKNOWN;

	// If the AMP instance doesn't exist, the function exits.
	if (filter->amp == NULL) { return res;}

	// Execute command
	// Waiting for AMP to be ready and playback is not stopped
	if ((filter->config_media == TRUE) && filter->amp && filter->amp_event && (filter->amp_event->HasEvent(filter->amp_event) == DFB_OK))
	{
		res = FALSE;

		// Check the status
		filter->amp_event->WaitForEvent(filter->amp_event);
		filter->amp_event->GetEvent(filter->amp_event, &event);

		if (filter->amp->UploadStatusChanges(filter->amp, (struct SStatus *)buffer, DFB_TRUE) == DFB_OK)
		{
			// if stopped = TRUE then we have to wait for the
			// playback to stop,so we check the right flag
			// otherwise we can just post the command
			if (
					(((struct SStatus *)buffer)->flags & SSTATUS_MODE)
					&& 
					(
					 	(stopped == FALSE)
						|| 
						((stopped == TRUE) && (((struct SStatus *)buffer)->mode.flags & SSTATUS_MODE_STOPPED))
					))
			{
				filter->amp->PostPresentationCmd(filter->amp, (struct SCommand *)&command);

				// Check the status
				filter->amp_event->WaitForEvent(filter->amp_event);
				filter->amp_event->GetEvent(filter->amp_event, &event);

				if (filter->amp->UploadStatusChanges(filter->amp, (struct SStatus *)buffer, DFB_TRUE) == DFB_OK)
				{
					if (IS_SUCCESS(((struct SStatus *)buffer)->lastCmd.result)) { res = TRUE;}
					else {g_printerr ("Post Command Failed\n");}
				}
				else { g_printerr ("cannot upload status changes.\n");}
			}
			else { 
				if (stopped == FALSE) { g_printerr ("Unable to post presentation command, abort!!!\n");}
			}
		}
		else { g_printerr ("Presentation Stopped. Unable to post command.\n");}
	}

	return res;

}

static void
gst_amp_filter_dispose (GObject * object) {
	g_print ("Finalizing...\n");
	GstAmpFilter *filter = GST_AMPFILTER (object);

	// Cleanup
	if(filter->amp) {
		filter->amp->Release(filter->amp);
		filter->amp = NULL;
		g_print ("Release AMP........................ OK\n");
	}


	if(filter->amp_event) {
		filter->amp_event->Release(filter->amp_event);
		filter->amp_event = NULL;
		g_print ("Release AMP Event........................ OK\n");
	}

	if (filter->data_buffer)
	{
		filter->data_buffer->Release(filter->data_buffer);
		filter->data_buffer = NULL;
		g_print ("Release dataBuffer........................ OK\n");
	}

	if(filter->dfb) {
		filter->dfb->Release(filter->dfb);
		filter->dfb = NULL;
		g_print ("Release DFB........................ OK\n");
	}

	g_print ("Finalized...\n");
	G_OBJECT_CLASS (parent_class)->dispose (object);
}
/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_amp_filter_plugin_init (GstPlugin * ampfilter)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampfilter' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_filter_debug, "ampfilter",
			0, "amp filter");

	return gst_element_register (ampfilter, "ampfilter", GST_RANK_NONE,
			GST_TYPE_AMPFILTER);
}
