// V.h :   header file of virtual page graphic functions
//
// Author: Yann Chou
//

#ifndef _V_H_
#define _V_H_

// VPages maintenance functions
int   allocvpage        (int pageno, int width, int height);
int   duplicvpage       (int oldpage, int px, int py, int newpage, int width, int height);
int   assignvpage       (int pageno, char *ptrHead, int bufWidth, int width, int height);
int   makevpage         (int pageno, int px1, int py1, int px2, int py2, char *buffer);
void  freeallvpages     (void);
void  freevpage         (int pageno);
int   switchactvpage    (int pageno);
void  vpagecpy          (int srcpage, int px1, int py1, int px2, int py2, int descpage, int destx, int desty);

// VPages information functions
char* getvpagebuffptr  (int pageno);
int   getvpagebuffwidth(int pageno);
int   getvpagewidth    (int pageno);
int   getvpageheight   (int pageno);
void  getvpagedim      (int pageno, int *width, int *height);

// Draw functions
void  vputimage         (int px, int py, char *buffer);
void  vputimageflip     (int px, int py, char *buffer);
void  vputimagemirror   (int px, int py, char *buffer);
void  vputimagereverse  (int px, int py, char *buffer);
void  vputimagetrans    (int px, int py, char *buffer);
void  vputzpimage       (int px, int py, char *buffer);
void  vgetimage         (int px1, int py1, int px2, int py2, char *buffer);
void  vfillbox          (int px1, int py1, int px2, int py2, int color);
void  vdrawline         (int px1, int py1, int px2, int py2, int color);
void  vdrawrectangle    (int px1, int py1, int px2, int py2, int color);
void  vputpixel         (int px, int py, int color);
int   vgetpixel         (int px, int py);

// Draw with alpha blending functions (only work in 16-bits mode)
void  vputimagedarken   (int px, int py, char *buffer);
void  vputzpimagedarken (int px, int py, char *buffer);
void  vfillboxdarken    (int px1, int py1, int px2, int py2, int color);
 
void  vputimagelighten  (int px, int py, char *buffer);
void  vputzpimagelighten(int px, int py, char *buffer);
void  vfillboxlighten   (int px1, int py1, int px2, int py2, int color);

void  vputimageblend    (int px, int py, char *buffer, int sf, int df);
void  vputzpimageblend  (int px, int py, char *buffer, int sf, int df);
void  vfillboxblend     (int px1, int py1, int px2, int py2, int color, int df, int sf);

void  vputimagealpha    (int px, int py, char *buffer, char *alpha);
 
void  vputapimage       (int px, int py, char *buffer);
void  vputapimagemirror (int px, int py, char *buffer);
void  vputzaimage       (int px, int py, char *buffer);

// Draw color mask functions (only work in 16-bits mode)
void  vputcolormask16   (int px, int py, int color, unsigned char *mask);
void  vputcolormask256  (int px, int py, int color, unsigned char *mask);
void  vputblackmask256  (int px, int py, unsigned char *mask);

// Draw 256c pics funstions (only work in 16-bits mode)
void  vputimage8        (int px, int py, char *buffer);
void  vputzpimage8      (int px, int py, char *buffer);

void  vputimage8blend   (int px, int py, char *buffer, int sf, int df);
void  vputzpimage8blend (int px, int py, char *buffer, int sf, int df);

void  vputimage8darken  (int px, int py, char *buffer);
void  vputzpimage8darken(int px, int py, char *buffer);

void  vputimage8lighten (int px, int py, char *buffer);
void  vputzpimage8lighten(int px, int py, char *buffer);

void  vputimage8alpha	(int px, int py, char *buffer, char *alpha);

void  vputapimage8      (int px, int py, char *buffer);
void  vputapimage8mirror(int px, int py, char *buffer);
void  vputzaimage8      (int px, int py, char *buffer);


// Definations of pixel format (belows only work in 16-bits mode)
#define  PIXELFMT_UNDEF 0       // undefined or failed
#define  PIXELFMT_8    -1       // 8 bits palette indexed
#define  PIXELFMT_555   2       // 16bits with [R5:G5:B5]
#define  PIXELFMT_565   4       // 16bits with [R5:G6:B5]
#define  PIXELFMT_888   8       // 24bits with [R8:G8:B8]
#define  PIXELFMT_BGR   128     // not supported yet


// Set pixel format or bit mask (two funtions will do the same thing)
int   setpixelformat	(int format);				        // also return format
int   setpixelbitmask   (int rmask, int gmask, int bmask);	// also return format
int   getpixelformat	(void);

// Pack the RGB values into a 16-bits color word
#define       _RGB555(r,g,b)    (((b) >> 3) | (((g) >> 3) << 5) | (((r) >> 3) << 10))
#define       _RGB565(r,g,b)    (((b) >> 3) | (((g) >> 2) << 5) | (((r) >> 3) << 11))
#define       _RGB888(r,g,b)    ( (b)       |  ((g)       << 8) |  ((r)       << 16))

unsigned long _RGB      (int r, int g, int b);

// Pixel format converting functions
void  adjustimage555to565  (char *buffer);
void  adjustimage565to555  (char *buffer);
void  adjustzpimage555to565(char *buffer);
void  adjustzpimage565to555(char *buffer);

void  adjustpalette565to555(char *buffer);
void  adjustpalette555to565(char *buffer);

#endif  // _V_H_
