
#include <math.h>
#include "RtfFont.h"
#include "GfxFont.h"
#include "Rtf.h"

//------------------------------------------------------------------------
// RtfFont
//------------------------------------------------------------------------

GString* RtfFont::getState_FontName(GfxState *state) {
  GfxFont *gfxFont;
  GString *fontName;
	
  gfxFont = state->getFont();
  fontName = (gfxFont && gfxFont->getName())
		? gfxFont->getName()->copy()
		: (GString *)NULL;
		/*  fontName = (gfxFont && gfxFont->getOrigName())
		? gfxFont->getOrigName()->copy()
	: (GString *)NULL;*/
	
  if (fontName == NULL)
    fontName = new GString(DEFAULT_FONT);
	
  return fontName;
}


GfxRGB RtfFont::getState_FontColor(GfxState *state) {
  GfxRGB color;
	
  if ((state->getRender() & 3) == 1) {
    state->getStrokeRGB(&color);
  } else {
    state->getFillRGB(&color);
  }
	
  return color;
}

// obtener el tamaño de la fuente y ajustarlo
double RtfFont::getState_FontSize(GfxState *state) {
  double w;
  double *fm;
  int code, mCode, letterCode, anyCode;
  double size;
  const char *name;
  GfxFont *gfxFont;
	
  size = state->getTransformedFontSize();
	
  gfxFont = state->getFont();
  if (gfxFont && gfxFont->getType() == fontType3) {
    // This is a hack which makes it possible to deal with some Type 3
    // fonts.  The problem is that it's impossible to know what the
    // base coordinate system used in the font is without actually
    // rendering the font.  This code tries to guess by looking at the
    // width of the character 'm' (which breaks if the font is a
    // subset that doesn't contain 'm').
    mCode = letterCode = anyCode = -1;
    for (code = 0; code < 256; ++code) {
      name = ((Gfx8BitFont *)gfxFont)->getCharName(code);
      if (name && name[0] == 'm' && name[1] == '\0') {
				mCode = code;
      }
      if (letterCode < 0 && name && name[1] == '\0' &&
				((name[0] >= 'A' && name[0] <= 'Z') ||
				(name[0] >= 'a' && name[0] <= 'z')) &&
				((Gfx8BitFont *)gfxFont)->getWidth(code) > 0) {
				letterCode = code;
      }
      if (anyCode < 0 && name &&
				((Gfx8BitFont *)gfxFont)->getWidth(code) > 0) {
				anyCode = code;
      }
    }
    if (mCode >= 0 &&
			(w = ((Gfx8BitFont *)gfxFont)->getWidth(mCode)) > 0) {
      // 0.6 is a generic average 'm' width -- yes, this is a hack
      size *= w / 0.6;
    } else if (letterCode >= 0 &&
			(w = ((Gfx8BitFont *)gfxFont)->getWidth(letterCode)) > 0) {
      // even more of a hack: 0.5 is a generic letter width
      size *= w / 0.5;
    } else if (anyCode >= 0 &&
			(w = ((Gfx8BitFont *)gfxFont)->getWidth(anyCode)) > 0) {
      // better than nothing: 0.5 is a generic character width
      size *= w / 0.5;
    }
    fm = gfxFont->getFontMatrix();
    if (fm[0] != 0) {
      size *= fabs(fm[3] / fm[0]);
    }
  }
	
  return size;
}

RtfFont::RtfFont(GfxState *state) {
  GfxFont *gfxFont;
	
  gfxFont = state->getFont();
	
  fontName = getState_FontName( state);
  size_0 = state->getTransformedFontSize();
  size = getState_FontSize( state);
  color = getState_FontColor( state);
	
  GString* s = fontName->copy()->lowerCase();
  isBold = (strstr(s->getCString(),"bold") != NULL);
  
  isItalic = (strstr(s->getCString(),"italic") ||
		strstr(s->getCString(),"oblique"));

	isFixedWidth = (gfxFont && gfxFont->isFixedWidth());

  delete s;
}

RtfFont::~RtfFont() {
  if (fontName) {
    delete fontName;
  }
}

GBool RtfFont::matches(RtfFont *font) {
  GBool ret = (font->size == size) &&
		(font->color.r == color.r) && (font->color.g == color.g) && (font->color.b == color.b) &&
		(fontName->cmp( font->fontName) == 0) &&
		true;
	
  return ret;
}

void RtfFont::rtfOpen(GString *s) {
	*s << RTF_FONT(rtfFontIndex);
	*s << RTF_FONT_SIZE(size);
	*s << RTF_COLOR(rtfColorIndex);
	if (isBold)
		*s << RTF_BOLD_START;
	if (isItalic)
		*s << RTF_ITALIC_START;
}

void RtfFont::rtfClose(GString *s) {
	if (isBold)
		*s << RTF_BOLD_END;
	if (isItalic)
		*s << RTF_ITALIC_END;
}
