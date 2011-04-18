/* GStreamer AVI GAB2 subtitle parser
 * Copyright (C) <2007> Thijs Vermeir <thijsvermeir@gmail.com>
 * Copyright (C) <2007> Tim-Philipp Müller <tim centricular net>
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
 * SECTION:element-avisubtitle
 *
 * <refsect2>
 * <para>
 * Parses the subtitle stream from an avi file.
 * </para>
 * <title>Example launch line</title>
 * <para>
 * <programlisting>
 * gst-launch filesrc location=subtitle.avi ! avidemux name=demux ! queue ! avisubtitle ! subparse ! textoverlay name=overlay ! ffmpegcolorspace ! autovideosink demux. ! queue ! decodebin ! overlay.
 * </programlisting>
 * This plays an avi file with a video and subtitle stream.
 * </para>
 * </refsect2>
 *
 * Last reviewed on 2008-02-01
 */

/* example of a subtitle chunk in an avi file
 * 00000000: 47 41 42 32 00 02 00 10 00 00 00 45 00 6e 00 67  GAB2.......E.n.g
 * 00000010: 00 6c 00 69 00 73 00 68 00 00 00 04 00 8e 00 00  .l.i.s.h........
 * 00000020: 00 ef bb bf 31 0d 0a 30 30 3a 30 30 3a 30 30 2c  ....1..00:00:00,
 * 00000030: 31 30 30 20 2d 2d 3e 20 30 30 3a 30 30 3a 30 32  100 --> 00:00:02
 * 00000040: 2c 30 30 30 0d 0a 3c 62 3e 41 6e 20 55 54 46 38  ,000..<b>An UTF8
 * 00000050: 20 53 75 62 74 69 74 6c 65 20 77 69 74 68 20 42   Subtitle with B
 * 00000060: 4f 4d 3c 2f 62 3e 0d 0a 0d 0a 32 0d 0a 30 30 3a  OM</b>....2..00:
 * 00000070: 30 30 3a 30 32 2c 31 30 30 20 2d 2d 3e 20 30 30  00:02,100 --> 00
 * 00000080: 3a 30 30 3a 30 34 2c 30 30 30 0d 0a 53 6f 6d 65  :00:04,000..Some
 * 00000090: 74 68 69 6e 67 20 6e 6f 6e 41 53 43 49 49 20 2d  thing nonASCII -
 * 000000a0: 20 c2 b5 c3 b6 c3 a4 c3 bc c3 9f 0d 0a 0d 0a      ..............
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "gstavisubtitle.h"

GST_DEBUG_CATEGORY_STATIC (avisubtitle_debug);
#define GST_CAT_DEFAULT avisubtitle_debug

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-subtitle-avi")
    );

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-subtitle")
    );

static void gst_avi_subtitle_title_tag (GstAviSubtitle * sub, gchar * title);
static GstFlowReturn gst_avi_subtitle_chain (GstPad * pad, GstBuffer * buffer);
static GstStateChangeReturn gst_avi_subtitle_change_state (GstElement * element,
    GstStateChange transition);
static gboolean gst_avi_subtitle_send_event (GstElement * element,
    GstEvent * event);

GST_BOILERPLATE (GstAviSubtitle, gst_avi_subtitle, GstElement,
    GST_TYPE_ELEMENT);

#define IS_BOM_UTF8(data)     ((GST_READ_UINT32_BE(data) >> 8) == 0xEFBBBF)
#define IS_BOM_UTF16_BE(data) (GST_READ_UINT16_BE(data) == 0xFEFF)
#define IS_BOM_UTF16_LE(data) (GST_READ_UINT16_LE(data) == 0xFEFF)
#define IS_BOM_UTF32_BE(data) (GST_READ_UINT32_BE(data) == 0xFEFF)
#define IS_BOM_UTF32_LE(data) (GST_READ_UINT32_LE(data) == 0xFEFF)

static GstBuffer *
gst_avi_subtitle_extract_file (GstAviSubtitle * sub, GstBuffer * buffer,
    guint offset, guint len)
{
  const gchar *input_enc = NULL;
  GstBuffer *ret = NULL;
  gchar *data;

  data = (gchar *) GST_BUFFER_DATA (buffer) + offset;

  if (len >= (3 + 1) && IS_BOM_UTF8 (data) &&
      g_utf8_validate (data + 3, len - 3, NULL)) {
    ret = gst_buffer_create_sub (buffer, offset + 3, len - 3);
  } else if (len >= 2 && IS_BOM_UTF16_BE (data)) {
    input_enc = "UTF-16BE";
    data += 2;
    len -= 2;
  } else if (len >= 2 && IS_BOM_UTF16_LE (data)) {
    input_enc = "UTF-16LE";
    data += 2;
    len -= 2;
  } else if (len >= 4 && IS_BOM_UTF32_BE (data)) {
    input_enc = "UTF-32BE";
    data += 4;
    len -= 4;
  } else if (len >= 4 && IS_BOM_UTF32_LE (data)) {
    input_enc = "UTF-32LE";
    data += 4;
    len -= 4;
  } else if (g_utf8_validate (data, len, NULL)) {
    /* not specified, check if it's UTF-8 */
    ret = gst_buffer_create_sub (buffer, offset, len);
  } else {
    /* we could fall back to gst_tag_freeform_to_utf8() here */
    GST_WARNING_OBJECT (sub, "unspecified encoding, and not UTF-8");
    return NULL;
  }

  g_return_val_if_fail (ret != NULL || input_enc != NULL, NULL);

  if (input_enc) {
    GError *err = NULL;
    gchar *utf8;

    GST_DEBUG_OBJECT (sub, "converting subtitles from %s to UTF-8", input_enc);
    utf8 = g_convert (data, len, "UTF-8", input_enc, NULL, NULL, &err);

    if (err != NULL) {
      GST_WARNING_OBJECT (sub, "conversion to UTF-8 failed : %s", err->message);
      g_error_free (err);
      return NULL;
    }

    ret = gst_buffer_new ();
    GST_BUFFER_DATA (ret) = (guint8 *) utf8;
    GST_BUFFER_MALLOCDATA (ret) = (guint8 *) utf8;
    GST_BUFFER_SIZE (ret) = strlen (utf8);
    GST_BUFFER_OFFSET (ret) = 0;
  }

  GST_BUFFER_CAPS (ret) = gst_caps_new_simple ("application/x-subtitle", NULL);
  return ret;
}

/**
 * gst_avi_subtitle_title_tag:
 * @sub: subtitle element
 * @title: the title of this subtitle stream
 *
 * Send an event to the srcpad of the @sub element with the title
 * of the subtitle stream as a GST_TAG_TITLE
 */
static void
gst_avi_subtitle_title_tag (GstAviSubtitle * sub, gchar * title)
{
  GstTagList *temp_list = gst_tag_list_new ();

  gst_tag_list_add (temp_list, GST_TAG_MERGE_APPEND, GST_TAG_TITLE, title,
      NULL);
  gst_pad_push_event (sub->src, gst_event_new_tag (temp_list));
}

static GstFlowReturn
gst_avi_subtitle_parse_gab2_chunk (GstAviSubtitle * sub, GstBuffer * buf)
{
  const guint8 *data;
  gchar *name_utf8;
  guint name_length;
  guint file_length;
  guint size;

  data = GST_BUFFER_DATA (buf);
  size = GST_BUFFER_SIZE (buf);

  /* check the magic word "GAB2\0", and the next word must be 2 */
  if (size < 12 || memcmp (data, "GAB2\0\2\0", 5 + 2) != 0)
    goto wrong_magic_word;

  /* read 'name' of subtitle */
  name_length = GST_READ_UINT32_LE (data + 5 + 2);
  GST_LOG_OBJECT (sub, "length of name: %u", name_length);
  if (size <= 17 + name_length)
    goto wrong_name_length;

  name_utf8 = g_convert ((gchar *) data + 11, name_length, "UTF-8", "UTF-16LE",
      NULL, NULL, NULL);

  if (name_utf8) {
    GST_LOG_OBJECT (sub, "subtitle name: %s", name_utf8);
    gst_avi_subtitle_title_tag (sub, name_utf8);
    g_free (name_utf8);
  }

  /* next word must be 4 */
  if (GST_READ_UINT16_LE (data + 11 + name_length) != 0x4)
    goto wrong_fixed_word_2;

  file_length = GST_READ_UINT32_LE (data + 13 + name_length);
  GST_LOG_OBJECT (sub, "length srt/ssa file: %u", file_length);

  if (size < (17 + name_length + file_length))
    goto wrong_total_length;

  /* store this, so we can send it again after a seek; note that we shouldn't
   * assume all the remaining data in the chunk is subtitle data, there may
   * be padding at the end for some reason, so only parse file_length bytes */
  sub->subfile =
      gst_avi_subtitle_extract_file (sub, buf, 17 + name_length, file_length);

  if (sub->subfile == NULL)
    goto extract_failed;

  return GST_FLOW_OK;

  /* ERRORS */
wrong_magic_word:
  {
    GST_ELEMENT_ERROR (sub, STREAM, DECODE, (NULL), ("Wrong magic word"));
    return GST_FLOW_ERROR;
  }
wrong_name_length:
  {
    GST_ELEMENT_ERROR (sub, STREAM, DECODE, (NULL),
        ("name doesn't fit in buffer (%d < %d)", size, 17 + name_length));
    return GST_FLOW_ERROR;
  }
wrong_fixed_word_2:
  {
    GST_ELEMENT_ERROR (sub, STREAM, DECODE, (NULL),
        ("wrong fixed word: expected %u, got %u", 4,
            GST_READ_UINT16_LE (data + 11 + name_length)));
    return GST_FLOW_ERROR;
  }
wrong_total_length:
  {
    GST_ELEMENT_ERROR (sub, STREAM, DECODE, (NULL),
        ("buffer size is wrong: need %d bytes, have %d bytes",
            17 + name_length + file_length, size));
    return GST_FLOW_ERROR;
  }
extract_failed:
  {
    GST_ELEMENT_ERROR (sub, STREAM, DECODE, (NULL),
        ("could not extract subtitles"));
    return GST_FLOW_ERROR;
  }
}

static GstFlowReturn
gst_avi_subtitle_chain (GstPad * pad, GstBuffer * buffer)
{
  GstAviSubtitle *sub = GST_AVI_SUBTITLE (GST_PAD_PARENT (pad));
  GstFlowReturn ret;

  if (sub->subfile != NULL) {
    GST_WARNING_OBJECT (sub, "Got more buffers than expected, dropping");
    ret = GST_FLOW_UNEXPECTED;
    goto done;
  }

  /* we expect exactly one buffer with the whole srt/ssa file in it */
  ret = gst_avi_subtitle_parse_gab2_chunk (sub, buffer);
  if (ret != GST_FLOW_OK)
    goto done;

  /* now push the subtitle data downstream */
  ret = gst_pad_push (sub->src, gst_buffer_ref (sub->subfile));

done:

  gst_buffer_unref (buffer);
  return ret;
}

static gboolean
gst_avi_subtitle_send_event (GstElement * element, GstEvent * event)
{
  GstAviSubtitle *avisubtitle = GST_AVI_SUBTITLE (element);
  gboolean ret = FALSE;

  if (GST_EVENT_TYPE (event) == GST_EVENT_SEEK) {
    if (avisubtitle->subfile) {
      if (gst_pad_push (avisubtitle->src,
              gst_buffer_ref (avisubtitle->subfile)) == GST_FLOW_OK)
        ret = TRUE;
    }
  }
  gst_event_unref (event);
  return ret;
}

static void
gst_avi_subtitle_base_init (gpointer klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (avisubtitle_debug, "avisubtitle", 0,
      "parse avi subtitle stream");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_template));

  gst_element_class_set_details_simple (element_class,
      "Avi subtitle parser", "Codec/Parser/Subtitle",
      "Parse avi subtitle stream", "Thijs Vermeir <thijsvermeir@gmail.com>");
}

static void
gst_avi_subtitle_class_init (GstAviSubtitleClass * klass)
{
  GstElementClass *gstelement_class = (GstElementClass *) klass;

  gstelement_class->change_state =
      GST_DEBUG_FUNCPTR (gst_avi_subtitle_change_state);
  gstelement_class->send_event =
      GST_DEBUG_FUNCPTR (gst_avi_subtitle_send_event);
}

static void
gst_avi_subtitle_init (GstAviSubtitle * self, GstAviSubtitleClass * klass)
{
  self->src = gst_pad_new_from_static_template (&src_template, "src");
  gst_element_add_pad (GST_ELEMENT (self), self->src);

  self->sink = gst_pad_new_from_static_template (&sink_template, "sink");
  gst_pad_set_chain_function (self->sink,
      GST_DEBUG_FUNCPTR (gst_avi_subtitle_chain));
  gst_element_add_pad (GST_ELEMENT (self), self->sink);

  self->subfile = NULL;
}

static GstStateChangeReturn
gst_avi_subtitle_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret;
  GstAviSubtitle *sub = GST_AVI_SUBTITLE (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
    case GST_STATE_CHANGE_READY_TO_PAUSED:
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      if (sub->subfile) {
        gst_buffer_unref (sub->subfile);
        sub->subfile = NULL;
      }
      break;
    default:
      break;
  }

  return ret;
}
