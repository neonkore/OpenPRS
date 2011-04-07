
#define XmStringFree(s) (FREE(s))

static 
char *XmStringCreate(char *s)
{
  char *res;
  NEWSTR(s,res);
  return res;
}

#define XmStringCreateLtoR(s,cs) XmStringCreate(s)

static 
char *XmStringConcat(char *s1, char *s2)
{
  char *res;
  int len = strlen(s1)+strlen(s2);

  res = (char *)MALLOC(len+1);
  BCOPY(s1, res, strlen(s1));	      
  BCOPY(s2, res+strlen(s1), strlen(s2)+1);
  
  return res;
}
static
char *XmStringSeparatorCreate()
{
  char *res;
  NEWSTR("\n",res);
  return res;
}

static
void XClearWindow(void *x, void *y)
{
  fprintf(stderr,"XClearWindow need to be defined\n");
}

static
void XmStringExtent(cairo_t *cr, XmString s, int *w, int *h)
{
  cairo_text_extents_t extents;
 
  cairo_text_extents(cr, s, &extents);
  *w = extents.width;
  *h = extents.height;
}

static
Region XCreateRegion(void)
{
  return gdk_region_new();
}

static
void XUnionRectWithRegion(XRectangle *rect, Region reg, Region res)
{
  if (reg == res) {
    gdk_region_union_with_rect(reg,rect);
  } else {
    fprintf(stderr,"XUnionRectWithRegion is not properly used... ");
  }
}

static
void XDestroyRegion(Region region)
{
  gdk_region_destroy(region);
}

static
void XClearArea(void *canvas,  void *window, int x, int y, unsigned width, unsigned height, int exposures)
{
 fprintf(stderr,"XClearArea need to be defined\n");
}

#define XtDisplay(s) (s)


static
int XDrawRectangle(void *i1, void *i2, cairo_t *cr, int x, int y, unsigned int width, unsigned int height)
{
  cairo_rectangle(cr, x, y, width, height);
  cairo_stroke(cr);
  return 1;
}

static
int XDrawLine(void *i1, void *i2, cairo_t *cr, int x1, int y1, int x2, int y2)
{
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);
  cairo_stroke(cr);
  return 1;
}

static
int XDrawLines(void *i1, void *i2, cairo_t *cr, XPoint *points, int npoints,void *unused)
{
  int i = 0;
  if (npoints > 0) {
    cairo_move_to(cr, points[i].x, points[i].y);
    for (i = 1; i < npoints; i++) {
      cairo_line_to(cr, points[i].x, points[i].y);
    }
    cairo_stroke(cr);
  }
}


static
int XFillPolygon(void *i1, void *i2, cairo_t *cr, XPoint *points, int npoints, void *shape, void * mode)
{
  gint i;
  for ( i = 0; i < npoints; i++ ) {
    cairo_line_to(cr, points[i].x, points[i].y);
  }
  
  cairo_close_path(cr);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);
}

static
void XmStringDraw(void* d, void *w, void *i1, XmString string, cairo_t *cr, gint x, Position y,  
		  Dimension i2, void * i3, void * i4, XRectangle *i5)
{
  cairo_text_extents_t extents;
  int h;
  
  cairo_text_extents(cr, string, &extents);

  cairo_move_to(cr, x, y+extents.height);
  cairo_show_text(cr, string);  
}


