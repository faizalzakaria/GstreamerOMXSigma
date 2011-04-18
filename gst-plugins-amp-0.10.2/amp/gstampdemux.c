/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *               2000,2001,2002,2003,2004,2005 Wim Taymans <wim@fluendo.com>
 *               2007 Wim Taymans <wim.taymans@gmail.com>
 *
 * gsttee.c: Tee element, one in N out
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
 * SECTION:element-tee
 * @see_also: #GstIdentity
 *
 * Split data to multiple pads.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gstampdemux.h"

#include <string.h>

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (gst_amp_demux_debug);
#define GST_CAT_DEFAULT gst_amp_demux_debug


/* lock to protect request pads from being removed while downstream */
#define GST_AMP_DEMUX_DYN_LOCK(demux) g_mutex_lock ((demux)->dyn_lock)
#define GST_AMP_DEMUX_DYN_UNLOCK(demux) g_mutex_unlock ((demux)->dyn_lock)

enum
{
	PROP_0,
	PROP_ALLOC_PAD,
};

static GstStaticPadTemplate amp_demux_src_template = GST_STATIC_PAD_TEMPLATE ("src%d",
		GST_PAD_SRC,
		GST_PAD_REQUEST,
		GST_STATIC_CAPS_ANY);

GST_BOILERPLATE (GstAmpDemux, gst_amp_demux, GstElement, GST_TYPE_ELEMENT);

/* structure and quark to keep track of which pads have been pushed */
static GQuark push_data;

typedef struct
{
	gboolean pushed;
	GstFlowReturn result;
	gboolean removed;
} PushData;

static GstPad *gst_amp_demux_request_new_pad (GstElement * element,
		GstPadTemplate * temp, const gchar * unused);
static void gst_amp_demux_release_pad (GstElement * element, GstPad * pad);

static void gst_amp_demux_finalize (GObject * object);
static void gst_amp_demux_set_property (GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec);
static void gst_amp_demux_get_property (GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec);
static void gst_amp_demux_dispose (GObject * object);

static GstFlowReturn gst_amp_demux_chain (GstPad * pad, GstBuffer * buffer);
static gboolean gst_amp_demux_sink_activate_push (GstPad * pad, gboolean active);


static void
gst_amp_demux_base_init (gpointer g_class)
{
	GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);

	gst_element_class_set_details_simple (gstelement_class,
			"AMP demuxer",
			"Codec/Demuxer",
			"demux amp streams",
			"Erik Walthinsen <omega@cse.ogi.edu>, " "Wim Taymans <wim@fluendo.com>");
	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&sinktemplate));
	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&amp_demux_src_template));

	push_data = g_quark_from_static_string ("amp_demux-push-data");
}

static void
gst_amp_demux_dispose (GObject * object)
{
	GstAmpDemux *demux = GST_AMP_DEMUX (object);
	GList *item;

restart:
	for (item = GST_ELEMENT_PADS (object); item; item = g_list_next (item)) {
		GstPad *pad = GST_PAD (item->data);
		if (GST_PAD_IS_SRC (pad)) {
			gst_element_release_request_pad (GST_ELEMENT (object), pad);
			goto restart;
		}
	}

	if (G_OBJECT_CLASS(parent_class)->dispose)
		G_OBJECT_CLASS (parent_class)->dispose (object);
	GST_INFO_OBJECT (demux, "Dispose.");
}

static void
gst_amp_demux_finalize (GObject * object)
{
	GstAmpDemux *demux;

	demux = GST_AMP_DEMUX (object);

	g_mutex_free (demux->dyn_lock);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_amp_demux_class_init (GstAmpDemuxClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = G_OBJECT_CLASS (klass);
	gstelement_class = GST_ELEMENT_CLASS (klass);

	gobject_class->finalize = gst_amp_demux_finalize;
	gobject_class->set_property = gst_amp_demux_set_property;
	gobject_class->get_property = gst_amp_demux_get_property;
	gobject_class->dispose = gst_amp_demux_dispose;

	g_object_class_install_property (gobject_class, PROP_ALLOC_PAD,
			g_param_spec_object ("alloc-pad", "Allocation Src Pad",
				"The pad used for gst_pad_alloc_buffer", GST_TYPE_PAD,
				G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	gstelement_class->request_new_pad =
		GST_DEBUG_FUNCPTR (gst_amp_demux_request_new_pad);
	gstelement_class->release_pad = GST_DEBUG_FUNCPTR (gst_amp_demux_release_pad);
}

static void
gst_amp_demux_init (GstAmpDemux * demux, GstAmpDemuxClass * g_class)
{
	demux->dyn_lock = g_mutex_new ();

	demux->sinkpad = gst_pad_new_from_static_template (&sinktemplate, "sink");

#if 0
	gst_pad_set_setcaps_function (demux->sinkpad,
			GST_DEBUG_FUNCPTR (gst_pad_proxy_setcaps));
	gst_pad_set_getcaps_function (demux->sinkpad,
			GST_DEBUG_FUNCPTR (gst_pad_proxy_getcaps));
	gst_pad_set_activatepush_function (demux->sinkpad,
			GST_DEBUG_FUNCPTR (gst_amp_demux_sink_activate_push));
#endif

	gst_pad_set_chain_function (demux->sinkpad, GST_DEBUG_FUNCPTR (gst_amp_demux_chain));
	gst_element_add_pad (GST_ELEMENT (demux), demux->sinkpad);
}

static GstPad *
gst_amp_demux_request_new_pad (GstElement * element, GstPadTemplate * templ,
		const gchar * unused)
{
	gchar *name;
	GstPad *srcpad;
	GstAmpDemux *demux;
	gboolean res;
	PushData *data;

	demux = GST_AMP_DEMUX (element);

	GST_DEBUG_OBJECT (demux, "requesting pad");

	GST_OBJECT_LOCK (demux);
	name = g_strdup_printf ("src%d", demux->pad_counter++);

	srcpad = gst_pad_new_from_template (templ, name);
	g_free (name);

	/* install the data, we automatically free it when the pad is disposed because
	 * of _release_pad or when the element goes away. */
	data = g_new0 (PushData, 1);
	data->pushed = FALSE;
	data->result = GST_FLOW_NOT_LINKED;
	data->removed = FALSE;
	g_object_set_qdata_full (G_OBJECT (srcpad), push_data, data, g_free);

	GST_OBJECT_UNLOCK (demux);

	gst_pad_set_setcaps_function (srcpad,
			GST_DEBUG_FUNCPTR (gst_pad_proxy_setcaps));
	gst_pad_set_getcaps_function (srcpad,
			GST_DEBUG_FUNCPTR (gst_pad_proxy_getcaps));
	gst_element_add_pad (GST_ELEMENT_CAST (demux), srcpad);

	return srcpad;
}

static void
gst_amp_demux_release_pad (GstElement * element, GstPad * pad)
{
	GstAmpDemux *demux;
	PushData *data;
	gboolean changed = FALSE;

	demux = GST_AMP_DEMUX (element);

	GST_DEBUG_OBJECT (demux, "releasing pad");

	/* wait for pending pad_alloc to finish */
	GST_AMP_DEMUX_DYN_LOCK (demux);
	data = g_object_get_qdata (G_OBJECT (pad), push_data);

	GST_OBJECT_LOCK (demux);
	/* mark the pad as removed so that future pad_alloc fails with NOT_LINKED. */
	data->removed = TRUE;
	if (demux->allocpad == pad) {
		demux->allocpad = NULL;
		changed = TRUE;
	}
	GST_OBJECT_UNLOCK (demux);

	gst_pad_set_active (pad, FALSE);

	gst_element_remove_pad (GST_ELEMENT_CAST (demux), pad);
	GST_AMP_DEMUX_DYN_UNLOCK (demux);

	if (changed) {
		g_object_notify (G_OBJECT (demux), "alloc-pad");
	}
}

static void
gst_amp_demux_set_property (GObject * object, guint prop_id, const GValue * value,
		GParamSpec * pspec)
{
	GstAmpDemux *demux = GST_AMP_DEMUX (object);

	GST_OBJECT_LOCK (demux);
	switch (prop_id) {
	case PROP_ALLOC_PAD:
	{
		GstPad *pad = g_value_get_object (value);
		GST_OBJECT_LOCK (pad);
		if (GST_OBJECT_PARENT (pad) == GST_OBJECT_CAST (object))
			demux->allocpad = pad;
		else
			GST_WARNING_OBJECT (object, "Tried to set alloc pad %s which"
					" is not my pad", GST_OBJECT_NAME (pad));
		GST_OBJECT_UNLOCK (pad);
		break;
	}
	default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
	}
	GST_OBJECT_UNLOCK (demux);
}

static void
gst_amp_demux_get_property (GObject * object, guint prop_id, GValue * value,
		GParamSpec * pspec)
{
	GstAmpDemux *demux = GST_AMP_DEMUX (object);

	GST_OBJECT_LOCK (demux);
	switch (prop_id) {
	case PROP_ALLOC_PAD:
	g_value_set_object (value, demux->allocpad);
	break;
	default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
	}
	GST_OBJECT_UNLOCK (demux);
}


static GstFlowReturn
gst_amp_demux_do_push (GstAmpDemux * demux, GstPad * pad, gpointer data)
{
	GstFlowReturn res;
	GstBuffer * buf;
	GstCaps * caps;
	GstStructure * structure;
	gchar * name;

	// We need to copy the buffer because it is not the same capabilities
	// for the two output pads
	buf = gst_buffer_copy (GST_BUFFER_CAST (data));
	name = gst_pad_get_name (pad);

	caps = gst_caps_make_writable (gst_buffer_get_caps (buf));
	structure = gst_caps_steal_structure (caps, 0);

	if (g_strcmp0 (name, "src0") == 0)
	{
		// Audio
		caps = gst_caps_new_simple ("audio/x-raw-int", NULL);
	}
	else if (g_strcmp0 (name, "src1") == 0)
	{
		// Video
		gst_structure_set_name (structure, "video/x-raw-rgb");
		gst_caps_append_structure (caps, structure);
	}

	gst_buffer_set_caps (buf, caps);
	gst_caps_unref (caps);

	return gst_pad_push (pad, buf);
}

static void
clear_pads (GstPad * pad, GstAmpDemux * demux)
{
	PushData *data;

	data = g_object_get_qdata ((GObject *) pad, push_data);

	/* the data must be there or we have a screwed up internal state */
	g_assert (data != NULL);

	data->pushed = FALSE;
	data->result = GST_FLOW_NOT_LINKED;
}

static GstFlowReturn
gst_amp_demux_handle_data (GstAmpDemux * demux, gpointer data)
{
	GList *pads;
	guint32 cookie;
	GstFlowReturn ret, cret;

	GST_OBJECT_LOCK (demux);
	pads = GST_ELEMENT_CAST (demux)->srcpads;

	/* special case for zero pads */
	if (G_UNLIKELY (!pads))
		goto no_pads;

	/* special case for just one pad that avoids reffing the buffer */
	if (!pads->next) {
		GstPad *pad = GST_PAD_CAST (pads->data);

		GST_OBJECT_UNLOCK (demux);

		if (pad == demux->pull_pad) {
			ret = GST_FLOW_OK;
		} else {
			ret = gst_pad_push (pad, GST_BUFFER_CAST (data));
		}
		return ret;
	}

	/* mark all pads as 'not pushed on yet' */
	g_list_foreach (pads, (GFunc) clear_pads, demux);

restart:
	cret = GST_FLOW_NOT_LINKED;
	pads = GST_ELEMENT_CAST (demux)->srcpads;
	cookie = GST_ELEMENT_CAST (demux)->pads_cookie;

	while (pads) {
		GstPad *pad;
		PushData *pdata;

		pad = GST_PAD_CAST (pads->data);

		/* get the private data, something is really wrong with the internal state
		 * when it is not there */
		pdata = g_object_get_qdata ((GObject *) pad, push_data);
		g_assert (pdata != NULL);

		if (G_LIKELY (!pdata->pushed)) {
			/* not yet pushed, release lock and start pushing */
			gst_object_ref (pad);
			GST_OBJECT_UNLOCK (demux);

			GST_LOG_OBJECT (demux, "Starting to push %s %p",
					"buffer", data);

			ret = gst_amp_demux_do_push (demux, pad, data);

			GST_LOG_OBJECT (demux, "Pushing item %p yielded result %s", data,
					gst_flow_get_name (ret));

			GST_OBJECT_LOCK (demux);
			/* keep track of which pad we pushed and the result value. We need to do
			 * this before we release the refcount on the pad, the PushData is
			 * destroyed when the last ref of the pad goes away. */
			pdata->pushed = TRUE;
			pdata->result = ret;
			gst_object_unref (pad);
		} else {
			/* already pushed, use previous return value */
			ret = pdata->result;
			GST_LOG_OBJECT (demux, "pad already pushed with %s",
					gst_flow_get_name (ret));
		}

		/* before we go combining the return value, check if the pad list is still
		 * the same. It could be possible that the pad we just pushed was removed
		 * and the return value it not valid anymore */
		if (G_UNLIKELY (GST_ELEMENT_CAST (demux)->pads_cookie != cookie)) {
			GST_LOG_OBJECT (demux, "pad list changed");
			/* the list of pads changed, restart iteration. Pads that we already
			 * pushed on and are still in the new list, will not be pushed on
			 * again. */
			goto restart;
		}

		/* stop pushing more buffers when we have a fatal error */
		if (G_UNLIKELY (ret != GST_FLOW_OK && ret != GST_FLOW_NOT_LINKED))
			goto error;

		/* keep all other return values, overwriting the previous one. */
		if (G_LIKELY (ret != GST_FLOW_NOT_LINKED)) {
			GST_LOG_OBJECT (demux, "Replacing ret val %d with %d", cret, ret);
			cret = ret;
		}
		pads = g_list_next (pads);
	}
	GST_OBJECT_UNLOCK (demux);

	gst_mini_object_unref (GST_MINI_OBJECT_CAST (data));

	/* no need to unset gvalue */
	return cret;

	/* ERRORS */
no_pads:
	{
		GST_DEBUG_OBJECT (demux, "there are no pads, return not-linked");
		ret = GST_FLOW_NOT_LINKED;
		goto error;
	}
error:
	{
		GST_DEBUG_OBJECT (demux, "received error %s", gst_flow_get_name (ret));
		GST_OBJECT_UNLOCK (demux);
		gst_mini_object_unref (GST_MINI_OBJECT_CAST (data));
		return ret;
	}
}

static GstFlowReturn
gst_amp_demux_chain (GstPad * pad, GstBuffer * buffer)
{
	GstFlowReturn res;
	GstAmpDemux *demux;

	demux = GST_AMP_DEMUX_CAST (GST_OBJECT_PARENT (pad));

	GstCaps * caps;
	caps = gst_buffer_get_caps (buffer);

	GST_DEBUG_OBJECT (demux, "received buffer %p", buffer);

	res = gst_amp_demux_handle_data (demux, buffer);

	GST_DEBUG_OBJECT (demux, "handled buffer %s", gst_flow_get_name (res));

	return res;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_amp_demux_plugin_init (GstPlugin * ampdemux)
{
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template ampaudiosink' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_amp_demux_debug, "ampdemux",
			0, "amp demux");

	return gst_element_register (ampdemux, "ampdemux", GST_RANK_NONE,
			GST_TYPE_AMP_DEMUX);
}
