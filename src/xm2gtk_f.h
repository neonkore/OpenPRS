
#define XmStringFree(s) (FREE(s))

char *XmStringCreate(char *s);

char *XmStringCreateSimple(char *s);

#define XmStringCreateLtoR(s,cs) XmStringCreate(s)

#define XtMalloc(s) g_malloc(s)
#define XtRealloc(s,t) g_realloc(s,t)
#define XtFree(s) g_free(s)

char *XmStringConcat(char *s1, char *s2);
char *XmStringSeparatorCreate();

void XtDestroyWidget(void *x);

void XClearWindow(void *ignore_display, void *window);

void XBell(void *x, int y);

gboolean XmToggleButtonGetState(Widget w);

void XFlush(void *y);

gboolean XtIsSensitive(void *s);


void XmStringExtent(cairo_t *cr, XmString s, int *w, int *h);

guint XmStringHeight(cairo_t *cr, XmString s);

guint XmStringWidth(cairo_t *cr, XmString s);

Region XCreateRegion(void);

void XUnionRectWithRegion(XRectangle *rect, Region reg, Region res);

void XDestroyRegion(Region region);

gboolean XPointInRegion(GdkRegion *region, int x, int y);

void XClearArea(void *canvas,  void *window, int x, int y, unsigned width, unsigned height, int exposures);

#define XtDisplay(s) (s)


int XDrawRectangle(void *i1, void *i2, cairo_t *cr, int x, int y, unsigned int width, unsigned int height);

int XDrawLine(void *i1, void *i2, cairo_t *cr, int x1, int y1, int x2, int y2);

int XDrawLines(void *i1, void *i2, cairo_t *cr, XPoint *points, int npoints,void *unused);


int XFillPolygon(void *i1, void *i2, cairo_t *cr, XPoint *points, int npoints, void *shape, void * mode);
#define XtManageChild(s) 

void XmStringDraw(void* d, void *w, void *i1, XmString string, cairo_t *cr, gint x, Position y,  
		  Dimension i2, void * i3, void * i4, XRectangle *i5);



