#ifndef _IMG_LIB_H
#define _IMG_LIB_H

#include "gtypes.h"
#include "gstring.h"

int	CreatePNG(int width,int height,int bpp,Guchar** rows,GString* sImage, GBool dumpBinaryImg);

int flipJPEG(GString *srcImage, GString* dstImage);

#endif
