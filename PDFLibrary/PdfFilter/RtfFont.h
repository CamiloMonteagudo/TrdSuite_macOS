#ifndef RTFFONT_H
#define RTFFONT_H

#include "GfxState.h"

//------------------------------------------------------------------------
// RtfFont
//------------------------------------------------------------------------

class RtfFont {
public:

  RtfFont(GfxState *state);
  ~RtfFont();

  GBool matches(RtfFont *font);
	void rtfOpen(GString *s);
	void rtfClose(GString *s);
	double getSize() { return size; };

  static GString* getState_FontName(GfxState *state);
  static GfxRGB getState_FontColor(GfxState *state);
  static double getState_FontSize(GfxState *state);

private:

  GString *fontName;
  int rtfFontIndex;            //indice en el listado de fuentes RTF

  double size;                 //tamaño de la fuente
  double size_0;               //tamaño de la fuente sin ajustar

  GfxRGB color;
  int rtfColorIndex;           //indice en el listado de colores RTF

  GBool isBold;
  GBool isItalic;
	GBool isFixedWidth;

  friend class RtfWord;
  friend class RtfLine;
  friend class RtfBlock;
  friend class RtfPage;
};

#endif