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

#ifndef __GST_AMPSRC_H__
#define __GST_AMPSRC_H__

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>

#include <unistd.h>

#include <advancedmediaprovider.h>
#include <cdefs_lpb.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_AMPSRC \
	(gst_amp_src_get_type())
#define GST_AMPSRC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AMPSRC,GstAmpSrc))
#define GST_AMPSRC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AMPSRC,GstAmpSrcClass))
#define GST_AMPSRC_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_AMPSRC, GstAmpSrcClass))
#define GST_IS_AMPSRC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AMPSRC))
#define GST_IS_AMPSRC_CLASS(klass) \
(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AMPSRC))
#define GST_AMP_SRC_CAST(obj) ((GstAmpSrc*) obj)

typedef enum
{
	GST_AMP_SRC_NULL,
	GST_AMP_SRC_READY,
	GST_AMP_SRC_STOPPED,
	GST_AMP_SRC_PAUSED,
	GST_AMP_SRC_PLAYING,
	GST_AMP_SRC_SCAN
} GstAmpSrcState;

typedef struct _GstAmpSrc      GstAmpSrc;
typedef struct _GstAmpSrcClass GstAmpSrcClass;

IAdvancedMediaProvider * _amp;

struct _GstAmpSrc
{
	GstBaseSrc element;
	GstPad *srcpad;

	gchar *filename;			/* filename */
	gboolean presentation;
	IDirectFBDisplayLayer   * layer;

	// DirectFB and AMP global variables
	IDirectFB* dfb;	
	IAdvancedMediaProvider *amp;
	IDirectFBEventBuffer *amp_event;

	GstAmpSrcState state;
};

struct _GstAmpSrcClass 
{
	GstBaseSrcClass parent_class;
};

GType gst_amp_src_get_type (void);
gboolean gst_amp_src_plugin_init (GstPlugin * ampsrc);

G_END_DECLS

#endif /* __GST_AMPSRC_H__ */
