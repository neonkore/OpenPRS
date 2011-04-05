
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
  NEWSTR("",res);
  return res;
}

static
void XClearWindow(void *x, void *y)
{
  fprintf(stderr,"XClearWindow need to be defined");
}

/* void XmStringExtent(void *fl, XmString s, int *w, int *h) */
/* { */
/*   fprintf(stderr,"replace with cairo_text_extents"); */
/* } */

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


#define XtDisplay(s) (s)
