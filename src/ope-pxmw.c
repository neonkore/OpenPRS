/*                               -*- Mode: C -*- 
 * ope-pxmw.c -- 
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include "ope-pxmw_f.h"
#include "lang.h"

/*
 * xwd.c MIT Project Athena, X Window system window raster image dumper.
 *
 * This program will create an x window dump (xwd) file containing the
 * contents of a user supplied pixmap.  The file may the be used for
 * output on any printer.
 *
 *  Original Author:    Tony Della Fera, DEC
 *                      17-Jun-85
 *
 *  Modification: Nancie P. Matson, ENSCO
 *                14-Aug-1991
 *                Stripped down code to use for printing the map software.
 *
 *                4-Mar-1991
 *                Changed the code to work with a Pixmap for SANDBOX.
 *
 * 		  Francois Felix Ingrand (FFI 9/2/92)
 *		  Ghanged the code to adapt it to OPRS.
 */

WritePixmap(Widget  parent, char *file_name, int width, int height, Pixmap pixmap)
{
     FILE *out_file;

     /* Open the output file */
     if (!(out_file = fopen(file_name, "w"))) {
	  fprintf(stderr,LG_STR("Can't open output file as specified.\n",
				"Can't open output file as specified.\n"));
	  return -1;
     }

     /* Dump it! */
     Pixmap_Dump(parent, pixmap, width, height, out_file);

     fclose(out_file);
    
     return 1;
}


/*
 * Pixmap_Dump: dump a pixmap to a file which must already be open for
 *              writing.
 */

#include "X11/XWDFile.h"

int format = ZPixmap;

Pixmap_Dump(Widget widget, Pixmap pixmap, int width, int height, FILE   *out)
{
     Window        window = XtWindow(widget);
     Display       *display = XtDisplay(widget);
     unsigned long swaptest = 1;
     unsigned      buffer_size;
     int           win_name_size;
     int           header_size;
     int           ncolors, i;
     char          *win_name;
     int           absx, absy, x, y;
     int           dwidth, dheight;
     int           bw;
     Window        dummywin;
     int           flag;
     int           screen;
     XColor        *colors;
     XImage        *image;
     XWDFileHeader header;
     XWindowAttributes win_info;

     /* Get the Screen value */ 
     screen = DefaultScreen (display);

     /*
      * Get the parameters of the window being dumped.
      */

     if(!XGetWindowAttributes(display, window, &win_info))
     {
	  printf(LG_STR("Can't get target window attributes.",
			"Can't get target window attributes."));
	  exit(-2);
     }

     /* handle any frame window */
     flag = XTranslateCoordinates (display, window,
				   RootWindow (display, screen), 0, 0,
				   &absx, &absy, &dummywin);
     bw = 0;

     absx -= win_info.border_width;
     absy -= win_info.border_width;
     bw = win_info.border_width;
 
     win_info.width = width;
     win_info.height = height;

     win_name = "xwdump";

     /* sizeof(char) is included for the null string terminator. */
     win_name_size = strlen(win_name) + sizeof(char);
 
     /*
      * Snarf the pixmap with XGetImage.
      */

     image = XGetImage (display, pixmap, 0, 0, width, height,
			AllPlanes, format);
     if (!image) 
     {
	  fprintf (stderr, LG_STR("pixmapPrint:  unable to get image at %dx%d+0+0\n",
				  "pixmapPrint:  unable to get image at %dx%d+0+0\n"),
		   width, height);
	  exit (1);
     }

     /* Determine the pixmap size */
     buffer_size = Image_Size(image);

     ncolors = Get_XColors(display, &win_info, &colors, widget);

     XFlush(display);

     /* Calculate header size */
     header_size = sizeof(header) + win_name_size;

     /* Write out header information */
     header.header_size = (CARD32) header_size;
     header.file_version = (CARD32) XWD_FILE_VERSION;
     header.pixmap_format = (CARD32) format;
     header.pixmap_depth = (CARD32) image->depth;
     header.pixmap_width = (CARD32) image->width;
     header.pixmap_height = (CARD32) image->height;
     header.xoffset = (CARD32) image->xoffset;
     header.byte_order = (CARD32) image->byte_order;
     header.bitmap_unit = (CARD32) image->bitmap_unit;
     header.bitmap_bit_order = (CARD32) image->bitmap_bit_order;
     header.bitmap_pad = (CARD32) image->bitmap_pad;
     header.bits_per_pixel = (CARD32) image->bits_per_pixel;
     header.bytes_per_line = (CARD32) image->bytes_per_line;
     header.visual_class = (CARD32) win_info.visual->class;
     header.red_mask = (CARD32) win_info.visual->red_mask;
     header.green_mask = (CARD32) win_info.visual->green_mask;
     header.blue_mask = (CARD32) win_info.visual->blue_mask;
     header.bits_per_rgb = (CARD32) win_info.visual->bits_per_rgb;
     header.colormap_entries = (CARD32) win_info.visual->map_entries;
     header.ncolors = ncolors;
     header.window_width = (CARD32) win_info.width;
     header.window_height = (CARD32) win_info.height;
     header.window_x = absx;
     header.window_y = absy;
     header.window_bdrwidth = (CARD32) win_info.border_width;

     if (*(char *) &swaptest) {
	  _swaplong((char *) &header, sizeof(header));
	  for (i = 0; i < ncolors; i++) {
	       _swaplong((char *) &colors[i].pixel, sizeof(long));
	       _swapshort((char *) &colors[i].red, 3 * sizeof(short));
	  }
     }

     (void) fwrite((char *)&header, sizeof(header), 1, out);
     (void) fwrite(win_name, win_name_size, 1, out);

     /* Write out the color maps, if any */
     (void) fwrite((char *) colors, sizeof(XColor), ncolors, out);


     /*
      *  This copying of the bit stream (data) to a file is to be replaced
      *  by an Xlib call which hasn't been written yet.  It is not clear
      *  what other functions of xwd will be taken over by this (as yet)
      *  non-existant X function.
      */
     (void) fwrite(image->data, (int) buffer_size, 1, out);

     /* Free the color buffer */
     if(ncolors > 0) free(colors);

     /* Free image */
     XDestroyImage(image);
}


/*
 * Determine the pixmap size.
 */

int Image_Size(image)
     XImage *image;
{
    if (format != ZPixmap)
      return(image->bytes_per_line * image->height * image->depth);

    return(image->bytes_per_line * image->height);
}

#define lowbit(x) ((x) & (~(x) + 1))

/*
 * Get the XColors of all pixels in image - returns # of colors
 */
int Get_XColors(Display *dpy, XWindowAttributes *win_info, XColor **colors)
{
    int i, ncolors;

    if (!win_info->colormap)
	return(0);

    if (win_info->visual->class == TrueColor)
	return(0);    /* colormap is not needed */

    ncolors = win_info->visual->map_entries;
    if (!(*colors = (XColor *) malloc (sizeof(XColor) * ncolors))) {
      fprintf(stderr,LG_STR("Out of memory!",
			    "Out of memory!"));
      return -1;
 }

    if (win_info->visual->class == DirectColor) {
	Pixel red, green, blue, red1, green1, blue1;

	red = green = blue = 0;
	red1 = lowbit(win_info->visual->red_mask);
	green1 = lowbit(win_info->visual->green_mask);
	blue1 = lowbit(win_info->visual->blue_mask);
	for (i=0; i<ncolors; i++) {
	  (*colors)[i].pixel = red|green|blue;
	  (*colors)[i].pad = 0;
	  red += red1;
	  if (red > win_info->visual->red_mask)
	    red = 0;
	  green += green1;
	  if (green > win_info->visual->green_mask)
	    green = 0;
	  blue += blue1;
	  if (blue > win_info->visual->blue_mask)
	    blue = 0;
	}
    } else {
	for (i=0; i<ncolors; i++) {
	  (*colors)[i].pixel = i;
	  (*colors)[i].pad = 0;
	}
    }

    XQueryColors(dpy, win_info->colormap, *colors, ncolors);
    
    return(ncolors);
}

_swapshort (bp, n)
    register char *bp;
    register unsigned n;
{
    register char c;
    register char *ep = bp + n;

    while (bp < ep) {
	c = *bp;
	*bp = *(bp + 1);
	bp++;
	*bp++ = c;
    }
}

_swaplong (bp, n)
    register char *bp;
    register unsigned n;
{
    register char c;
    register char *ep = bp + n;
    register char *sp;

    while (bp < ep) {
	sp = bp + 3;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	sp = bp + 1;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	bp += 2;
    }
}
