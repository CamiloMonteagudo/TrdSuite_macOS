//========================================================================
//
// RtfOutputDev.cpp
//
//========================================================================


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <math.h>
#include "GString.h"
#include "GList.h"
#include "Error.h"
#include "GfxState.h"
#include "GlobalParams.h"
#include "Errors.h"
#include "UnicodeMap.h"
#include "UnicodeTypeTable.h"
#include "RtfOutputDev.h"
#include "Rtf.h"
#include "ImgLib.h"

//------------------------------------------------------------------------
// parameters
//------------------------------------------------------------------------

// Each bucket in a text pool includes baselines within a range of
// this many points.
#define textPoolStep 4

// Inter-character space width which will cause addChar to start a new
// word.
#define minWordBreakSpace 0.1

// Negative inter-character space width, i.e., overlap, which will
// cause addChar to start a new word.
#define minDupBreakOverlap 0.3 //original=0.2

// Max distance between baselines of two lines within a block, as a
// fraction of the font size.
#define maxLineSpacingDelta 1.4 //original=1.5

// Max difference in primary font sizes on two lines in the same
// block.  Delta1 is used when examining new lines above and below the
// current block; delta2 is used when examining text that overlaps the
// current block; delta3 is used when examining text to the left and
// right of the current block.
#define maxBlockFontSizeDelta1 0.07 //0.1 //original=0.05
#define maxBlockFontSizeDelta2 0.6
#define maxBlockFontSizeDelta3 0.2

// maxima diferencia horizontal entre el inicio de dos lineas consecutivas 
// de un bloque si la primera termina en un punto y la segunda empieza en 
// mayusculas o con un numero para dividir el bloque, expresado como fraccion
// del tamaño de fuente
#define maxBlockSplit1 0.1

// Max difference in font sizes inside a word.
#define maxWordFontSizeDelta 0.05

// Maximum distance between baselines of two words on the same line,
// e.g., distance between subscript or superscript and the primary
// baseline, as a fraction of the font size.
#define maxIntraLineDelta 0.5

// Minimum inter-word spacing, as a fraction of the font size.  (Only
// used for raw ordering.)
#define minWordSpacing 0.15

// Maximum inter-word spacing, as a fraction of the font size.
#define maxWordSpacing 15.5 //original=1.5

// Maximum horizontal spacing which will allow a word to be pulled
// into a block.
#define minColSpacing1 0.7 //original=0.3

// Minimum spacing between columns, as a fraction of the font size.
#define minColSpacing2 1.0

// Maximum vertical spacing between blocks within a flow, as a
// multiple of the font size.
#define maxBlockSpacing 2.5

// Minimum spacing between characters within a word, as a fraction of
// the font size.
#define minCharSpacing -200.0 //original=-0.2

// Maximum spacing between characters within a word, as a fraction of
// the font size, when there is no obvious extra-wide character
// spacing.
#define maxCharSpacing 0.03

// When extra-wide character spacing is detected, the inter-character
// space threshold is set to the minimum inter-character space
// multiplied by this constant.
#define maxWideCharSpacingMul 1.3

// Max difference in primary,secondary coordinates (as a fraction of
// the font size) allowed for duplicated text (fake boldface, drop
// shadows) which is to be discarded.
#define dupMaxPriDelta 0.1
#define dupMaxSecDelta 0.2

// Maxima diferencia horizontal que puede haber entre dos lineas de un bloque
// para no separar una de las dos lineas en otro bloque
#define maxBlockHorzDelta 10.0

// Si la distancia vertical entre dos tablas consecutivas es menor que este valor
// las tablas se dibujan como si estuvieran unidas
#define minInterTableHeight (70.0/20.0)

//------------------------------------------------------------------------
// GStringList
//------------------------------------------------------------------------

GStringList::~GStringList() { 
  for(int i=0; i<getLength(); i++)
    delete getString(i);
}

int GStringList::getIndexOf(GString* str) {
  for(int i=0; i<getLength(); i++)
    if (getString(i)->cmp(str) == 0)
      return i;
		return -1;
};

int GStringList::addString(GString* str,GBool bForceAdd) {
  int index = -1;

	if (bForceAdd == gFalse)
		index = getIndexOf(str);

  if (index == -1)
  {
    append(str);
    index = getLength()-1;
  }
  else //Si la cadena no se inserto, destruirla
    delete str;
	
  return index;
};

//------------------------------------------------------------------------
// RtfWord
//------------------------------------------------------------------------

RtfWord::RtfWord(RtfElement *parentA,GfxState *state, int rotA, double x0, double y0,
								 int charPosA, RtfFont *fontA, double fontSizeA):
					RtfElement(T_WORD, parentA) {
  GfxFont *gfxFont;
  double x, y, ascent, descent;
	
  rot = rotA;
  charPos = charPosA;
  charLen = 0;
  font = fontA;
  minFontSize = maxFontSize = fontSizeA;
  state->transform(x0, y0, &x, &y);
  if ((gfxFont = state->getFont())) {
    ascent = gfxFont->getAscent() * maxFontSize;
    descent = gfxFont->getDescent() * maxFontSize;
  } else {
    // this means that the PDF file draws text without a current font,
    // which should never happen
    ascent = 0.95 * maxFontSize;
    descent = -0.35 * maxFontSize;
  }
  switch (rot) {
  case 0:
    yMin = y - ascent;
    yMax = y - descent;
    if (yMin == yMax) {
      // this is a sanity check for a case that shouldn't happen -- but
      // if it does happen, we want to avoid dividing by zero later
      yMin = y;
      yMax = y + 1;
    }
    base = y;
    break;
  case 1:
    xMin = x + descent;
    xMax = x + ascent;
    if (xMin == xMax) {
      // this is a sanity check for a case that shouldn't happen -- but
      // if it does happen, we want to avoid dividing by zero later
      xMin = x;
      xMax = x + 1;
    }
    base = x;
    break;
  case 2:
    yMin = y + descent;
    yMax = y + ascent;
    if (yMin == yMax) {
      // this is a sanity check for a case that shouldn't happen -- but
      // if it does happen, we want to avoid dividing by zero later
      yMin = y;
      yMax = y + 1;
    }
    base = y;
    break;
  case 3:
    xMin = x - ascent;
    xMax = x - descent;
    if (xMin == xMax) {
      // this is a sanity check for a case that shouldn't happen -- but
      // if it does happen, we want to avoid dividing by zero later
      xMin = x;
      xMax = x + 1;
    }
    base = x;
    break;
  }
  text = NULL;
  edge = NULL;
  len = size = 0;
  spaceAfter = gFalse;
	tabBefore = gFalse;
  next = NULL;
	image = NULL;
}

// Constructor para las imagenes
RtfWord::RtfWord(RtfElement *parentA, double x0, double y0, double w, double h, 
								 GString *imageA):RtfElement(T_WORD, parentA) {
  text = NULL;
  edge = NULL;
  len = size = 0;
  spaceAfter = gFalse;
	tabBefore = gFalse;
  next = NULL;
  rot = 0;
  charPos = 0;
  charLen = 0;
  font = NULL;

	image = imageA;
	xMin = x0;
	yMin = y0;
	xMax = x0 + w;
	yMax = y0 + h;
	base = yMax;
}

RtfWord::~RtfWord() {
  gfree(text);
  gfree(edge);
  if (image)
		delete image;
}

void RtfWord::addChar(GfxState *state, double x, double y,
											double dx, double dy, Unicode u) {
  if (len == size) {
    size += 16;
    text = (Unicode *)grealloc(text, size * sizeof(Unicode));
    edge = (double *)grealloc(edge, (size + 1) * sizeof(double));
  }
  text[len] = u;
  switch (rot) {
  case 0:
    if (len == 0) {
      xMin = x;
    }
    edge[len] = x;
    xMax = edge[len+1] = x + dx;
    break;
  case 1:
    if (len == 0) {
      yMin = y;
    }
    edge[len] = y;
    yMax = edge[len+1] = y + dy;
    break;
  case 2:
    if (len == 0) {
      xMax = x;
    }
    edge[len] = x;
    xMin = edge[len+1] = x + dx;
    break;
  case 3:
    if (len == 0) {
      yMax = y;
    }
    edge[len] = y;
    yMin = edge[len+1] = y + dy;
    break;
  }
  ++len;
}

void RtfWord::merge(RtfWord *word) {
  int i;
	
  if (word->xMin < xMin) {
    xMin = word->xMin;
  }
  if (word->yMin < yMin) {
    yMin = word->yMin;
  }
  if (word->xMax > xMax) {
    xMax = word->xMax;
  }
  if (word->yMax > yMax) {
    yMax = word->yMax;
  }
  if (len + word->len > size) {
    size = len + word->len;
    text = (Unicode *)grealloc(text, size * sizeof(Unicode));
    edge = (double *)grealloc(edge, (size + 1) * sizeof(double));
  }
  for (i = 0; i < word->len; ++i) {
    text[len + i] = word->text[i];
    edge[len + i] = word->edge[i];
  }
  edge[len + word->len] = word->edge[word->len];
  len += word->len;
  charLen += word->charLen;
}

inline int RtfWord::primaryCmp(RtfWord *word) {
  double cmp;
	
  cmp = 0; // make gcc happy
  switch (rot) {
  case 0:
    cmp = xMin - word->xMin;
    break;
  case 1:
    cmp = yMin - word->yMin;
    break;
  case 2:
    cmp = word->xMax - xMax;
    break;
  case 3:
    cmp = word->yMax - yMax;
    break;
  }
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

double RtfWord::primaryDelta(RtfWord *word) {
  double delta;
	
  delta = 0; // make gcc happy
  switch (rot) {
  case 0:
    delta = word->xMin - xMax;
    break;
  case 1:
    delta = word->yMin - yMax;
    break;
  case 2:
    delta = xMin - word->xMax;
    break;
  case 3:
    delta = yMin - word->yMax;
    break;
  }
  return delta;
}

int RtfWord::cmpYX(const void *p1, const void *p2) {
  RtfWord *word1 = *(RtfWord **)p1;
  RtfWord *word2 = *(RtfWord **)p2;
  double cmp;
	
  cmp = word1->yMin - word2->yMin;
  if (cmp == 0) {
    cmp = word1->xMin - word2->xMin;
  }
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

//------------------------------------------------------------------------
// RtfPool
//------------------------------------------------------------------------

RtfPool::RtfPool() {
  minBaseIdx = 0;
  maxBaseIdx = -1;
  pool = NULL;
  cursor = NULL;
  cursorBaseIdx = -1;
}

RtfPool::~RtfPool() {
  int baseIdx;
  RtfWord *word, *word2;
	
  for (baseIdx = minBaseIdx; baseIdx <= maxBaseIdx; ++baseIdx) {
    for (word = pool[baseIdx - minBaseIdx]; word; word = word2) {
      word2 = word->next;
      delete word;
    }
  }
  gfree(pool);
}

int RtfPool::getBaseIdx(double base) {
  int baseIdx;
	
  baseIdx = (int)(base / textPoolStep);
  if (baseIdx < minBaseIdx) {
    return minBaseIdx;
  }
  if (baseIdx > maxBaseIdx) {
    return maxBaseIdx;
  }
  return baseIdx;
}

void RtfPool::addWord(RtfWord *word) {
  RtfWord **newPool;
  int wordBaseIdx, newMinBaseIdx, newMaxBaseIdx, baseIdx;
  RtfWord *w0, *w1;
	
  // expand the array if needed
  wordBaseIdx = (int)(word->base / textPoolStep);
  if (minBaseIdx > maxBaseIdx) {
    minBaseIdx = wordBaseIdx - 128;
    maxBaseIdx = wordBaseIdx + 128;
    pool = (RtfWord **)gmalloc((maxBaseIdx - minBaseIdx + 1) *
			sizeof(RtfWord *));
    for (baseIdx = minBaseIdx; baseIdx <= maxBaseIdx; ++baseIdx) {
      pool[baseIdx - minBaseIdx] = NULL;
    }
  } else if (wordBaseIdx < minBaseIdx) {
    newMinBaseIdx = wordBaseIdx - 128;
    newPool = (RtfWord **)gmalloc((maxBaseIdx - newMinBaseIdx + 1) *
			sizeof(RtfWord *));
    for (baseIdx = newMinBaseIdx; baseIdx < minBaseIdx; ++baseIdx) {
      newPool[baseIdx - newMinBaseIdx] = NULL;
    }
    memcpy(&newPool[minBaseIdx - newMinBaseIdx], pool,
			(maxBaseIdx - minBaseIdx + 1) * sizeof(RtfWord *));
    gfree(pool);
    pool = newPool;
    minBaseIdx = newMinBaseIdx;
  } else if (wordBaseIdx > maxBaseIdx) {
    newMaxBaseIdx = wordBaseIdx + 128;
    pool = (RtfWord **)grealloc(pool, (newMaxBaseIdx - minBaseIdx + 1) *
			sizeof(RtfWord *));
    for (baseIdx = maxBaseIdx + 1; baseIdx <= newMaxBaseIdx; ++baseIdx) {
      pool[baseIdx - minBaseIdx] = NULL;
    }
    maxBaseIdx = newMaxBaseIdx;
  }
	
  // insert the new word
  if (cursor && wordBaseIdx == cursorBaseIdx &&
		word->primaryCmp(cursor) > 0) {
    w0 = cursor;
    w1 = cursor->next;
  } else {
    w0 = NULL;
    w1 = pool[wordBaseIdx - minBaseIdx];
  }
  for (; w1 && word->primaryCmp(w1) > 0; w0 = w1, w1 = w1->next) ;
  word->next = w1;
  if (w0) {
    w0->next = word;
  } else {
    pool[wordBaseIdx - minBaseIdx] = word;
  }
  cursor = word;
  cursorBaseIdx = wordBaseIdx;
}

//------------------------------------------------------------------------
// RtfLine
//------------------------------------------------------------------------

RtfLine::RtfLine(RtfElement *parentA, int rotA, double baseA):RtfElement(T_LINE, parentA) {
  rot = rotA;
  xMin = yMin = 0;
  xMax = yMax = -1;
  base = baseA;
  words = lastWord = NULL;
  text = NULL;
  edge = NULL;
  len = 0;
  hyphenated = gFalse;
  next = NULL;
  rtext = NULL;
	nWords = 0;
}

RtfLine::~RtfLine() {
  RtfWord *word;
	
  while (words) {
    word = words;
    words = words->next;
    delete word;
  }
  gfree(text);
  gfree(edge);
  if (rtext) delete rtext;
}

void RtfLine::addWord(RtfWord *word) {
  if (lastWord) {
    lastWord->next = word;
  } else {
    words = word;
  }
  lastWord = word;
	
	nWords++;
	updateBounds(word);
}

double RtfLine::primaryDelta(RtfLine *line) {
  double delta;
	
  delta = 0; // make gcc happy
  switch (rot) {
  case 0:
    delta = line->xMin - xMax;
    break;
  case 1:
    delta = line->yMin - yMax;
    break;
  case 2:
    delta = xMin - line->xMax;
    break;
  case 3:
    delta = yMin - line->yMax;
    break;
  }
  return delta;
}

int RtfLine::primaryCmp(RtfLine *line) {
  double cmp;
	
  cmp = 0; // make gcc happy
  switch (rot) {
  case 0:
    cmp = xMin - line->xMin;
    break;
  case 1:
    cmp = yMin - line->yMin;
    break;
  case 2:
    cmp = line->xMax - xMax;
    break;
  case 3:
    cmp = line->yMax - yMax;
    break;
  }
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

int RtfLine::secondaryCmp(RtfLine *line) {
  double cmp;
	
  cmp = (rot == 0 || rot == 3) ? base - line->base : line->base - base;
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

int RtfLine::cmpYX(RtfLine *line) {
  int cmp;
	
  if ((cmp = secondaryCmp(line))) {
    return cmp;
  }
  return primaryCmp(line);
}

int RtfLine::cmpXY(const void *p1, const void *p2) {
  RtfLine *line1 = *(RtfLine **)p1;
  RtfLine *line2 = *(RtfLine **)p2;
  int cmp;
	
  if ((cmp = line1->primaryCmp(line2))) {
    return cmp;
  }
  return line1->secondaryCmp(line2);
}

void RtfLine::coalesce(UnicodeMap *uMap) {
  RtfWord *word0, *word1;
  double space/*, delta, minSpace*/;
  GBool isUnicode;
  char buf[8];
  int i, j;
	
  if (words->next) {
		
    // compute the inter-word space threshold
		/*    ignorar el chequeo de maxWideCharSpacingMul
    if (words->len > 1 || words->next->len > 1) {
		minSpace = 0;
    } else {
		minSpace = words->primaryDelta(words->next);
		for (word0 = words->next, word1 = word0->next;
	   word1 && minSpace > 0;
		 word0 = word1, word1 = word0->next) {
		 if (word1->len > 1) {
		 minSpace = 0;
		 }
		 delta = word0->primaryDelta(word1);
		 if (delta < minSpace) {
		 minSpace = delta;
		 }
		 }
		 }
    if (minSpace <= 0) {*/
		space = maxCharSpacing * words->maxFontSize;
		/*    } else {
		space = maxWideCharSpacingMul * minSpace;
	}*/
		
    // merge words
    word0 = words;
    word1 = words->next;
    while (word1) {
      if (word0->primaryDelta(word1) >= space) {
				word0->spaceAfter = gTrue;
				word0 = word1;
				word1 = word1->next;
      } else if (word0->font != NULL && word0->font == word1->font &&
				// word0->font != NULL es para que no una las imagenes
				fabs(word0->maxFontSize - word1->maxFontSize) <
				maxWordFontSizeDelta * words->maxFontSize &&
				word1->charPos == word0->charPos + word0->charLen) {
				word0->merge(word1);
				word0->next = word1->next;
				delete word1;
				word1 = word0->next;
      } else {
				word0 = word1;
				word1 = word1->next;
      }
    }
  }
	
  // build the line text
  isUnicode = uMap ? uMap->isUnicode() : gFalse;
  len = 0;
  for (word1 = words; word1; word1 = word1->next) {
    len += word1->len;
    if (word1->spaceAfter) {
      ++len;
    }
  }

  text = (Unicode *)gmalloc(len * sizeof(Unicode));
  edge = (double *)gmalloc((len + 1) * sizeof(double));
  rtext = new GString();
  i = 0;

  for (word1 = words; word1; word1 = word1->next) {
		if (word1->image == NULL) {
			for (j = 0; j < word1->len; ++j) {
				text[i] = word1->text[j];
				edge[i] = word1->edge[j];
				
				int n;
				n = uMap->mapUnicode(text[i], buf, sizeof(buf));
				rtext->append(buf, n);
				++i;
			}
			edge[i] = word1->edge[word1->len];
			if (word1->spaceAfter) {
				text[i] = (Unicode)0x0020;
				*rtext << SPACE;
				++i;
			}
		}
  }
	
  // check for hyphen at end of line
  //~ need to check for other chars used as hyphens
  hyphenated = (text != NULL) && (text[len - 1] == (Unicode)'-');
}

void RtfLine::dump(GString *sOutput, RtfContext *context, RtfBounds *bounds) {
  RtfWord *word1,*lastWord;
  char buf[8];
	
	double startBaseline = words->base;
	double curBaseline = startBaseline;

  for (word1 = words,lastWord = NULL; word1; word1 = word1->next) {
		// actualizar la fuente
		if (context->font == NULL && word1->font != NULL) {
			// es el inicio de una pagina
			context->font = word1->font;
			context->font->rtfOpen( sOutput);
    }
    else if (context->font != word1->font && word1->font != NULL) {
			RtfFont *newFont = word1->font;
			
			if (context->font->rtfFontIndex != newFont->rtfFontIndex)
				*sOutput << RTF_FONT(newFont->rtfFontIndex);
			
			if (context->font->size != newFont->size)
				*sOutput << RTF_FONT_SIZE(newFont->size);
			
			if (context->font->rtfColorIndex != newFont->rtfColorIndex)
				*sOutput << RTF_COLOR(newFont->rtfColorIndex);
			
			if (!context->font->isBold && newFont->isBold)
				*sOutput << RTF_BOLD_START;
			if (context->font->isBold && !newFont->isBold)
				*sOutput << RTF_BOLD_END;
			
			if (!context->font->isItalic && newFont->isItalic)
				*sOutput << RTF_ITALIC_START;
			if (context->font->isItalic && !newFont->isItalic)
				*sOutput << RTF_ITALIC_END;
			
			context->font = newFont;
    }

		// actualizar la linea de base
		if (fabs(curBaseline - word1->base) > 1.0) {
			curBaseline = word1->base;
			*sOutput << RTF_UP(startBaseline - curBaseline);
		}

		if (word1->image)
			*sOutput << word1->image;
		else {
			int wordLen = word1->len;
			// no tomar en cuenta el "-" al final de las lineas
			if (hyphenated && word1->next == NULL && next != NULL &&
				context->hyphenatedBlocks >= 0)
				wordLen--;
			
			if (word1->tabBefore && lastWord != NULL)
				if (context->align == rtfRight)
					// con alineacion derecha el tab se da relativo al inicio de la linea
					*sOutput << RTF_TAB(word1->xMin - xMin);
				else
					*sOutput << RTF_TAB(word1->xMin - bounds->xMin);

			for (int j = 0; j < wordLen; ++j) {
				int n;
				n = context->uMap->mapUnicode(word1->text[j], buf, sizeof(buf));
				
				// si no hay definida una conversion de Unicode a Latin
				// (ver latin1UnicodeMapRanges en UnicodeMapTables.h)
				// usar el mismo codigo Unicode
				if (n == 0)
					*sOutput << RTF_UNICODE(word1->text[j]);
				else {
					for (int k = 0; k < n; ++k)
						EncodeRtfChars( sOutput, buf[k]);
				}
			}

			// añadir un espacio si la palabra lo lleva o es la ultima palabra
			// de la linea y no termina con un guion y tiene mas lineas debajo
			if (word1->spaceAfter || (word1->next == NULL && !hyphenated && 
				next != NULL))
				*sOutput << SPACE;
		}

		lastWord = word1;
  }

	if (fabs(startBaseline - curBaseline) > 1.0) 
		*sOutput << RTF_UP_NORMAL;
}

//------------------------------------------------------------------------
// RtfBlock
//------------------------------------------------------------------------

RtfBlock::RtfBlock(RtfElement *parentA, int rotA):RtfElement(T_BLOCK, parentA) {
  rot = rotA;
  xMin = yMin = 0;
  xMax = yMax = -1;
  pool = new RtfPool();
  lines = NULL;
  curLine = NULL;
  next = NULL;
	averageFontSize = 0.0;
	nChars = 0;
	lastChar = firstChar = 0;
}

RtfBlock::~RtfBlock() {
  RtfLine *line;
	
  delete pool;
	
  while (lines) {
    line = lines;
    lines = lines->next;
    delete line;
  }
}

void RtfBlock::addWord(RtfWord *word) {
  pool->addWord(word);
	updateBounds(word);

	// calcular el tamaño de fuente usado como promedio
	if (word->font) {
		int wordLen = word->getLength();
		if (averageFontSize == 0)
			averageFontSize = word->font->size;
		else
			averageFontSize = (averageFontSize*nChars + 
				word->font->size*wordLen)/(nChars + wordLen);
		nChars += wordLen;
	}
}

void RtfBlock::coalesce(UnicodeMap *uMap) {
  RtfWord *word0, *word1, *word2, *bestWord0, *bestWord1, *lastWord;
  RtfLine *line, *line0, *line1;
  int poolMinBaseIdx, startBaseIdx, minBaseIdx, maxBaseIdx;
  int baseIdx, bestWordBaseIdx, idx0, idx1;
  double minBase, maxBase;
  double fontSize, delta, priDelta, secDelta;
  GBool found;
	
  // discard duplicated text (fake boldface, drop shadows)
  for (idx0 = pool->minBaseIdx; idx0 <= pool->maxBaseIdx; ++idx0) {
    word0 = pool->getPool(idx0);
    while (word0) {
      priDelta = dupMaxPriDelta * word0->maxFontSize;
      secDelta = dupMaxSecDelta * word0->maxFontSize;
      if (rot == 0 || rot == 3) {
				maxBaseIdx = pool->getBaseIdx(word0->base + secDelta);
      } else {
				maxBaseIdx = pool->getBaseIdx(word0->base - secDelta);
      }
      found = gFalse;
      word1 = word2 = NULL; // make gcc happy
      for (idx1 = idx0; idx1 <= maxBaseIdx; ++idx1) {
				if (idx1 == idx0) {
					word1 = word0;
					word2 = word0->next;
				} else {
					word1 = NULL;
					word2 = pool->getPool(idx1);
				}
				for (; word2; word1 = word2, word2 = word2->next) {
					if (word2->len == word0->len &&
						!memcmp(word2->text, word0->text,
						word0->len * sizeof(Unicode))) {
						switch (rot) {
						case 0:
						case 2:
							found = fabs(word0->xMin - word2->xMin) < priDelta &&
								fabs(word0->xMax - word2->xMax) < priDelta &&
								fabs(word0->yMin - word2->yMin) < secDelta &&
								fabs(word0->yMax - word2->yMax) < secDelta;
							break;
						case 1:
						case 3:
							found = fabs(word0->xMin - word2->xMin) < secDelta &&
								fabs(word0->xMax - word2->xMax) < secDelta &&
								fabs(word0->yMin - word2->yMin) < priDelta &&
								fabs(word0->yMax - word2->yMax) < priDelta;
							break;
						}
					}
					if (found) {
						break;
					}
				}
				if (found) {
					break;
				}
      }
      if (found) {
				if (word1) {
					word1->next = word2->next;
				} else {
					pool->setPool(idx1, word2->next);
				}
				delete word2;
      } else {
				word0 = word0->next;
      }
    }
  }
	
  // build the lines
  curLine = NULL;
  poolMinBaseIdx = pool->minBaseIdx;
  charCount = 0;
  nLines = 0;
  while (1) {
		
    // find the first non-empty line in the pool
    for (;
		poolMinBaseIdx <= pool->maxBaseIdx && !pool->getPool(poolMinBaseIdx);
		++poolMinBaseIdx) ;
    if (poolMinBaseIdx > pool->maxBaseIdx) {
      break;
    }
		
    // look for the left-most word in the first four lines of the
    // pool -- this avoids starting with a superscript word
    startBaseIdx = poolMinBaseIdx;
    for (baseIdx = poolMinBaseIdx + 1;
		baseIdx < poolMinBaseIdx + 4 && baseIdx <= pool->maxBaseIdx;
		++baseIdx) {
      if (!pool->getPool(baseIdx)) {
				continue;
      }
      if (pool->getPool(baseIdx)->primaryCmp(pool->getPool(startBaseIdx))
				< 0) {
				startBaseIdx = baseIdx;
      }
    }
		
    // create a new line
    word0 = pool->getPool(startBaseIdx);
    pool->setPool(startBaseIdx, word0->next);
    word0->next = NULL;
    line = new RtfLine(this, word0->rot, word0->base);
    line->addWord(word0);
    lastWord = word0;
		
    // compute the search range
    fontSize = word0->maxFontSize;
    minBase = word0->base - maxIntraLineDelta * fontSize;
    maxBase = word0->base + maxIntraLineDelta * fontSize;
    minBaseIdx = pool->getBaseIdx(minBase);
    maxBaseIdx = pool->getBaseIdx(maxBase);
		
    // find the rest of the words in this line
    while (1) {
			
      // find the left-most word whose baseline is in the range for
      // this line
      bestWordBaseIdx = 0;
      bestWord0 = bestWord1 = NULL;
      for (baseIdx = minBaseIdx; baseIdx <= maxBaseIdx; ++baseIdx) {
				for (word0 = NULL, word1 = pool->getPool(baseIdx);
				word1;
				word0 = word1, word1 = word1->next) {
					if (word1->base >= minBase &&
						word1->base <= maxBase &&
						(delta = lastWord->primaryDelta(word1)) >=
						minCharSpacing * fontSize) {
						if (delta < maxWordSpacing * fontSize &&
							(!bestWord1 || word1->primaryCmp(bestWord1) < 0)) {
							bestWordBaseIdx = baseIdx;
							bestWord0 = word0;
							bestWord1 = word1;
						}
						break;
					}
				}
      }
      if (!bestWord1) {
				break;
      }
			
      // remove it from the pool, and add it to the line
      if (bestWord0) {
				bestWord0->next = bestWord1->next;
      } else {
				pool->setPool(bestWordBaseIdx, bestWord1->next);
      }
      bestWord1->next = NULL;
      line->addWord(bestWord1);
      lastWord = bestWord1;
    }
		
    // add the line
    if (curLine && line->cmpYX(curLine) > 0) {
      line0 = curLine;
      line1 = curLine->next;
    } else {
      line0 = NULL;
      line1 = lines;
    }
    for (;
		line1 && line->cmpYX(line1) > 0;
		line0 = line1, line1 = line1->next) ;
    if (line0) {
      line0->next = line;
    } else {
      lines = line;
    }
    line->next = line1;
    curLine = line;
    line->coalesce(uMap);
    charCount += line->len;
    ++nLines;
  }
	
	/*  for (line = lines; line; line = line->next) {
	line->coalesce(uMap);
	charCount += line->len;
	lastLine = line;
  }*/
}

int RtfBlock::cmpYXPrimaryRot(const void *p1, const void *p2) {
  RtfBlock *blk1 = *(RtfBlock **)p1;
  RtfBlock *blk2 = *(RtfBlock **)p2;
  double cmp;
	
  cmp = 0; // make gcc happy
  switch (0 /*TODO: blk1->page->primaryRot*/) {
  case 0:
		if ((cmp = blk1->yMin - blk2->yMin) == 0) {
			cmp = blk1->xMin - blk2->xMin;
		}
/*		if (blk1->yMax <= blk2->yMin || blk1->yMin >= blk2->yMax) {
			if ((cmp = blk1->yMin - blk2->yMin) == 0) {
				cmp = blk1->xMin - blk2->xMin;
			}
		} else {
			cmp = blk1->xMin - blk2->xMin;
		}*/
    break;
  case 1:
    if ((cmp = blk2->xMax - blk1->xMax) == 0) {
      cmp = blk1->yMin - blk2->yMin;
    }
    break;
  case 2:
    if ((cmp = blk2->yMin - blk1->yMin) == 0) {
      cmp = blk2->xMax - blk1->xMax;
    }
    break;
  case 3:
    if ((cmp = blk1->xMax - blk2->xMax) == 0) {
      cmp = blk2->yMax - blk1->yMax;
    }
    break;
  }
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

int RtfBlock::primaryCmp(RtfBlock *blk) {
  double cmp;
	
  cmp = 0; // make gcc happy
  switch (rot) {
  case 0:
    cmp = xMin - blk->xMin;
    break;
  case 1:
    cmp = yMin - blk->yMin;
    break;
  case 2:
    cmp = blk->xMax - xMax;
    break;
  case 3:
    cmp = blk->yMax - yMax;
    break;
  }
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

double RtfBlock::secondaryDelta(RtfBlock *blk) {
  double delta;
	
  delta = 0; // make gcc happy
  switch (rot) {
  case 0:
    delta = blk->yMin - yMax;
    break;
  case 1:
    delta = xMin - blk->xMax;
    break;
  case 2:
    delta = yMin - blk->yMax;
    break;
  case 3:
    delta = blk->xMin - xMax;
    break;
  }
  return delta;
}

// indica si el parrafo podria dividir sus palabras con un "-" al final
// de sus lineas.
// el criterio se da en base al espacio entre el extremo derecho de cada
// linea y el extremo derecho del parrafo.
// valor de retorno:
// -1 : no
//  0 : indeterminado
//  1 : si
int RtfBlock::isHyphenable() {
  if (nLines < 3) return 0;

	// desde la primera hasta la penultima linea
	for(RtfLine *line=lines; line->next; line=line->next) {
		if (xMax - line->xMax > maxFontSize*0.5)
			return -1;
	}

	return 1;
}

GBool RtfBlock::isFirstLineIndented() {
  if (nLines >= 2) {
    if (lines->xMin - xMin < lines->maxFontSize*1.0)
			return gFalse;

		if (lines->next->xMin - xMin > lines->maxFontSize*1.0)
			return gFalse;
//    for(RtfLine *line=lines->next; line; line=line->next)
//			if (line->xMin - xMin > line->maxFontSize*1.0)
//				return gFalse;
		
		return gTrue;
	} else
		return gFalse;
}

// retornar el menor margen de las lineas sin contar la primera
double RtfBlock::getLeftMargin(double relativeX) {
  if (lines && lines->next) {
    double xMinA = lines->next->xMin;
		
    for(RtfLine *line=lines->next; line; line=line->next)
			xMinA = MIN(xMinA, line->xMin);
			
		return xMinA - relativeX;
  }
  else
    return xMin - relativeX;
}

double RtfBlock::getFirstLineLeftMargin(double relativeX) {
	if (lines)
		return lines->xMin - relativeX;
	else
		return xMin - relativeX;
}

double RtfBlock::getRightMargin() {
	return xMax;
}

GBool RtfBlock::isImage() {
	return (lines != NULL && lines->next == NULL && lines->words->image != NULL
		&& lines->words->next == NULL);
}

// indica si el bloque es un fragmento de codigo. Estos casos tienen
// un tratamiento especial ya que las lineas no se pueden unir en un solo
// flujo como en los parrafos normales.
// Se detectan tomando en cuenta si tiene una fuente de ahcho fijo y 
// el area que ocupan las lineas respecto al area del bloque.
GBool RtfBlock::isCodeSample() {
  RtfLine *line;
  RtfWord *word;
	GBool isFixedWidth = gTrue;

	if (nLines<3) // al menos 3 lineas
		return gFalse;

	// detectar si todas las palabras tienen una fuente de ancho fijo
	// y calcular el area ocupada por las lineas
	double areaLines = 0.0;
	int nWords = 0;
  for (line = lines; line; line = line->next) {
		if (line->next)	// no tomar en cuenta la ultima linea
			areaLines += line->xMax - line->xMin;

		for (word = line->words; word; word = word->next) {
			nWords++;
			if (word->font && word->font->isFixedWidth == gFalse)
				isFixedWidth = gFalse;
		}
  }

//TODO?:	casos de tablas no funcionan bien
	// si son pocas palabras el area no es un buen indicador
	if (nWords < 6)
		return gFalse;

	// si la fuente no es de ancho fijo, la diferencia de area debe ser mayor
	double areaBlock = (xMax - xMin) * (nLines-1);
	if (areaLines < areaBlock*(0.62 + 0.2*isFixedWidth))
		return gTrue;
	else
		return gFalse;

}

GBool RtfBlock::isEnumByFirstChar(char &cFirstBlockChar) {
	// cantidad de veces que una linea comienza con el primer caracter
	// del bloque
	long equalFirstCharMatchs;
  RtfLine *line = lines;
	
	cFirstBlockChar = 0;
  // no tomar en cuenta los bloques con una sola linea
  if (line->next == NULL) 
		return gFalse;
	
	cFirstBlockChar = line->rtext->getChar(0);
	equalFirstCharMatchs = 0;

	// si la primera linea no esta cerca del borde izquierdo del bloque
	// entonces no hacer chequeo de enumeraciones (por que empiezen por
	// el mismo caracter)
	if (fabs(line->xMin - xMin) > maxFontSize*0.1)
		return gFalse;
		
  for (; line; line = line->next) {
		// detectar si el primer caracter de las lineas no es el mismo
		// pero solo tomando en cuenta las lineas pegadas al borde izquierdo
		if (fabs((line->xMin - xMin)) <= maxFontSize*0.1) {
			if (line->rtext->getChar(0) == cFirstBlockChar)
				equalFirstCharMatchs++;
			else
				return gFalse;
		}
  }
		
	if (equalFirstCharMatchs > 2)
		return gTrue;
	return gFalse;
}

void RtfBlock::recalculateBounds() {
  RtfLine *line;
	
	resetBounds();

  nLines = 0;
  for (line = lines; line; line = line->next,nLines++) { 
		updateBounds(line);
		line->parent = this;
  }
}

// inserta un bloque al principio de este
// supone que blk solo tiene una linea
void RtfBlock::insertBlock(RtfBlock *blk){
	RtfWord *lastWord,*tempWord;

	// buscar la ultima palabra y actualizar los bounds y el parent
	for(tempWord = blk->lines->words; tempWord; tempWord = tempWord->next) {
		tempWord->setParent(lines);
		lastWord = tempWord;
	}

	lastWord->next = lines->words;
	lines->words = blk->lines->words;
	lastWord->next->tabBefore = gTrue;

	// quitar las palabras del bloque insertado
	blk->lines->words = NULL;
}

void RtfBlock::splitByEnumChar(char c) {
	RtfLine *line, *nextLine, *lastLine;
	RtfBlock *curblk, *newblk;
	lastLine = lines;
	
	curblk = this;
	double blkxMin = curblk->xMin;
	double blk_maxFontSize = curblk->maxFontSize;

	// comenzar por la segunda linea
	nextLine = NULL;
	for(line = lastLine->next; line; line = nextLine)	{
		if (line->rtext->getChar(0) == c &&
			fabs(line->xMin - blkxMin) <= blk_maxFontSize*0.1) {
			lastLine->next = NULL;
			newblk = new RtfBlock(parent, rot);
			newblk->lines = line;
			nextLine = line->next;
			lastLine = line;
			
			newblk->next = curblk->next;
			curblk->next = newblk;
			curblk->recalculateBounds();
			curblk->splitBlock();
			curblk = newblk;
		} else {
			nextLine = line->next;
			lastLine = line;
		}
	}
	
	if (curblk != this) {
    curblk->recalculateBounds();
		curblk->splitBlock();
	}
}

RtfBlock* RtfBlock::splitByLine(int index) {
	int i;
	RtfLine *lastLine = lines;
	RtfLine *line = lines->next;
	RtfBlock *newBlk = NULL;

	// se empieza a buscar a partir de la segunda linea
	for(i=1; (i != index) && line; i++) {
		lastLine = line;
		line = line->next;
	}

	if (line == NULL)
		return NULL;

	lastLine->next = NULL;
	recalculateBounds();

	newBlk = new RtfBlock(parent, rot);
	newBlk->lines = line;
	newBlk->lines->parent = newBlk;
	newBlk->recalculateBounds();

	return newBlk;
}

void RtfBlock::splitAllLines() {
	RtfLine *line, *nextLine, *lastLine;
	RtfBlock *curblk, *newblk;
	lastLine = lines;
	
	curblk = this;

	// comenzar por la segunda linea
	nextLine = NULL;
	for(line = lastLine->next; line; line = nextLine)	{
		lastLine->next = NULL;
		newblk = new RtfBlock(parent, rot);
		newblk->lines = line;
		nextLine = line->next;
		line->next = NULL;
		lastLine = line;
		
		newblk->next = curblk->next;
		curblk->next = newblk;
		curblk->recalculateBounds();
		curblk = newblk;
	}
	
  if (curblk != this)
    curblk->recalculateBounds();
}

// El proceso de creacion de bloques consiste en reunir palabras
// y lineas basandose solo en su cercania. Esta funcion analiza cada
// linea de un bloque y en base a distintos factores divide el bloque
// extrayendo lineas a nuevos bloques
void RtfBlock::splitBlock() {
  RtfBlock *newblk = NULL, *curblk = this;
  int i,j;
	char cFirstBlockChar;
	
  //return;
	if (isCodeSample()) {
		splitAllLines();
		return;
	}

	if (isEnumByFirstChar(cFirstBlockChar)) {
		splitByEnumChar(cFirstBlockChar);
		return;
	}

  do {
    newblk = NULL;
    RtfLine *line = curblk->lines;
    RtfLine *lastLine = NULL;
		
    // no tomar en cuenta los bloques con una sola linea
    if (line->next == NULL) break;
		
    GBool bTodasMayus = gTrue; //indica si todas las lineas comienzan con mayusculas
    GBool bTieneMinus = gFalse; //indica si alguna letra esta en minuscula
		
		cFirstBlockChar = line->rtext->getChar(0);

		GBool bFirstLineIndented = curblk->isFirstLineIndented();

		GBool bBloqueComienzaNum = (cFirstBlockChar >= '0' && cFirstBlockChar <= '9');
		
		// guardar el 1er caracter del bloque, si este es de una enumeracion
		char *BloqueComienzaEnum = NULL;
		if (cFirstBlockChar != 0)
			BloqueComienzaEnum = (char *)strchr("·-", cFirstBlockChar);
		
    for (i=1; line; line = line->next, i++) {
			GBool bEsIndice,bEsEnumeracion;
			
			char* sLine = line->rtext->getCString();

			GBool bComienzaMayus = (sLine[0] >= 'A' && sLine[0] <= 'Z');
			if (bComienzaMayus == gFalse)
				bTodasMayus = gFalse;
			
			// detectar si la linea tiene minusculas
			for(j=0; j<line->rtext->getLength(); j++)
				if (sLine[j] >= 'a' && sLine[j] <= 'z')	{
					bTieneMinus = gTrue;
					break;
				}
				
			GBool bComienzaNum = (sLine[0] >= '0' && sLine[0] <= '9');
			
			// si la linea contiene estas cadenas entonces es un indice
			bEsIndice = strstr(sLine, ". . . .") != NULL || strstr(sLine, "....") != NULL;
			
			// chequeo de enumeraciones
			bEsEnumeracion = (sLine[0] == '·') || (bBloqueComienzaNum && bComienzaNum) ||
				(BloqueComienzaEnum && *BloqueComienzaEnum == sLine[0]);
			
			if (
				(bEsIndice) ||
				(bEsEnumeracion && i>1) ||
				// lineas que comienzan con mayuscula o numero y la anterior termina con un punto
				((bComienzaMayus || bComienzaNum) && lastLine != NULL &&
				lastLine->rtext->getChar(lastLine->rtext->getLength()-1) == '.' && 
				!bFirstLineIndented /*&&
				fabs(line->xMin - lastLine->xMin) <= line->maxFontSize*maxBlockSplit1*/) ||
				// si la posicion de inicio en x de la linea esta desplazada hacia adentro
				// respecto a la anterior y no es ni la 1ra o 2da linea y tiene mas de 3
				// palabras
				(lastLine && (line->xMin - lastLine->xMin) > curblk->maxFontSize*2 && 
				i>2 && line->nWords > 3) ||
				// si la primera palabra de esta linea cabe en el final de la linea
				// anterior
				(lastLine && lastLine->xMax + line->words->getWidth() + 
				curblk->maxFontSize*2 < curblk->xMax) ||
				// si la linea anterior termina con una imagen y esta es mas grande que
				// el doble de la linea actual
				(lastLine && lastLine->lastWord->image && 
				lastLine->getHeight() > line->getHeight()*2) ||
				gFalse)	{
				// dividir el bloque por la linea actual
				// si esta es la primera linea, el nuevo bloque contiene
				// las lineas siguientes a esta.
				// si no es la primera linea, entonces el nuevo bloque contiene
				// esta linea y las siguientes.
				
				newblk = new RtfBlock(parent, rot);
				
				if (lastLine == NULL) {
					newblk->lines = line->next;
					line->next = NULL;
				}
				else {
					newblk->lines = line;
					lastLine->next = NULL;
				}
				
				newblk->next = curblk->next;
				curblk->next = newblk;
				curblk->recalculateBounds();
				curblk = newblk;
				curblk->recalculateBounds();
				break;
			}
			
			lastLine = line;
    }
		
    // si no se ha creado un nuevo bloque y todas las lineas 
		// del bloque comienzan en mayusculas
    // y tienen letras minusculas
    // dividir cada linea del bloque en bloques independientes
		int curLineCount = i-1;
		if (newblk == NULL && bTodasMayus && bTieneMinus && curLineCount>3) {
			curblk->recalculateBounds();
			curblk->splitAllLines();
			return;
    }

	// repetir el ciclo mientras se creen nuevos bloques
  } while (newblk != NULL);
}

// genera el texto RTF del bloque
void RtfBlock::dump(GString *sOutput, RtfContext *context, RtfBounds *bounds) {
  double leftMargin,firstLineLeftMargin;
  double rightMargin;
	double spaceBefore;
  RtfAlign align = rtfLeft;

	context->hyphenatedBlocks += isHyphenable();

	// buscar la celda padre de este bloque
	RtfCell *parentCell = NULL;
	for(RtfElement *parent1 = this; parent1; parent1 = parent1->parent) {
		if (parent1->type == T_CELL && ((RtfCell*)parent1)->plain == gFalse) {
			parentCell = (RtfCell*)parent1;
			break;
		}
	}

	rightMargin = bounds->xMax - xMax;
	leftMargin = getLeftMargin(bounds->xMin);
	// el margen de la primera linea se da respecto al margen izquierdo
	firstLineLeftMargin = getFirstLineLeftMargin(bounds->xMin) - leftMargin;

	// espacio vertical antes del parrafo
	if (bounds != this) { // no se hace para los bloques de posicionamiento absoluto
		spaceBefore = (yMin - parent->curY) - 2;
		if (spaceBefore < 0) 
			spaceBefore = 0;
	} else spaceBefore = 0;

	// si el bloque pertenece a una celda y es estrecho, ampliar los margenes 
	if (parentCell) {
		GBool bIsImage = isImage();

		if (getWidth() <= maxFontSize*2.5) {
			rightMargin -= maxFontSize*2.5;
			if (rightMargin < 0) {
				leftMargin += rightMargin;
				rightMargin = 0;
				if (leftMargin < 0)
					leftMargin = 0;
			}
		} else {
			// dentro de una celda estos son los margenes minimos a menos que
			// sea una imagen
			if (!bIsImage) {
				rightMargin = MAX(rightMargin, 2.0);
				leftMargin = MAX(leftMargin, 2.0);
			}
		}
		// si no es una imagen y es el primer bloque, el spaceBefore tiene un
		// valor minimo
		if (!bIsImage && parentCell->getElem(0) == this) {
			spaceBefore = MAX(spaceBefore, 2.0);
		}
	}

	if (bounds->xMin == 0 && rightMargin < bounds->getWidth()/6 && xMin > (bounds->xMax/3) && nLines == 1) {
		leftMargin = MIN(leftMargin, 10);
		align = rtfRight;
	}	else if (nLines == 1)
		align = rtfLeft;
	else
		align = rtfJustified;

	*sOutput << RTF_LI(leftMargin);
	*sOutput << RTF_FI(firstLineLeftMargin);

	// poner el margen derecho al minimo si el parrafo tiene una sola linea 
	// (para que pueda crecer) y la alineacion no es derecha (porque entonces
	// crece hacia la izquierda)
	if (nLines <= 1 && align != rtfRight)
		if (parentCell == NULL)
			rightMargin = context->minRightMargin;
		else
			rightMargin = 2.0;

	*sOutput << RTF_RI(rightMargin);
	
	//Poner la alineacion horizontal
	switch (align) {
		case rtfRight     : *sOutput << RTF_QR; break;
		case rtfLeft      : *sOutput << RTF_QL; break;
		case rtfCenter    : *sOutput << RTF_QC; break;
		case rtfJustified : *sOutput << RTF_QJ; break;
	}
	
	*sOutput << RTF_SB_VIRTUAL(spaceBefore) << SPACE;

	context->align = align;

	for (RtfLine *line = lines; line; line = line->next) {
		line->dump( sOutput, context, bounds);
		
		// hallar el ultimo caracter del bloque
		if (line->next == NULL) {
      RtfWord *word1;
			for( word1 = line->words; word1->next; word1 = word1->next );

			if( word1->len > 0 && word1->image == NULL) {
				int n;
			  char buf[8];

				n = context->uMap->mapUnicode(word1->text[word1->len-1], buf, sizeof(buf));
				if (n > 0)
					lastChar = buf[n-1];
			}
		}
	}

	context->align = rtfLeft;

	// hallar el primer caracter del bloque
	if (lines) {
		RtfWord *word1 = lines->words;

		if (word1->len > 0 && word1->image == NULL) {
			int n;
			char buf[8];

			n = context->uMap->mapUnicode(word1->text[0], buf, sizeof(buf));
			if (n > 0)
				firstChar = buf[0];
		}
	}

	if (bounds != this && parent && parent->type != T_CELL)
//	if (bounds != this && parentCell == NULL)
		*sOutput << RTF_NEW_PARAGRAPH;
	else
		*sOutput << NEW_LINE;

	if (bounds)
		RtfElement::dump(sOutput, context, bounds);
}

//------------------------------------------------------------------------
// RtfCell
//------------------------------------------------------------------------

RtfCell::RtfCell(RtfElement *parentA):RtfElement(T_CELL,parentA) {
	elements = new GElementList();
	plain = gFalse;
}

RtfCell::~RtfCell() {
	delete elements;
}

void RtfCell::add(RtfElement *elem) {
	elem->setParent(this);
	elements->append( elem);
}

void RtfCell::coalesce(void) {
	elements->sort( cmpYX);

	// si hay superposicion horizontal de dos o mas elementos, crear una tabla
	// que los contenga

	// solo se admiten 3 niveles de anidamiento de las tablas
	// para evitar hipoteticos ciclos infinitos
	RtfTable *parentTable = (RtfTable *)getParentOfType(T_TABLE);
	if (parentTable && parentTable->nestLevel > 2)
		return;

	RtfElement *lastElem = elements->get(0);
	RtfTable *table = NULL;
	int iTableStart = 0;

	for(int i=1; i<elements->getLength(); i++) {
		RtfElement *elem = elements->get(i);

		if ((table && table->accept(elem)) || (lastElem->yMax > elem->yMin + 6.0)) {
			if (table == NULL) {
				table = new RtfTable(this);
				table->add(lastElem);
				table->add(elem);
				iTableStart = i-1;
				elements->del(i);
				i--;
			} else {
				if (table->accept(elem)) {
					table->add(elem);
					elements->del(i);
					i--;
				} else { // la tabla ya se termino de formar, añadirla a los elementos
					elements->del(iTableStart);
					elements->insert( iTableStart, table);
					RtfTable *newTable = table->coalesce();
					while (newTable != NULL) {
						elements->insert( ++iTableStart, newTable);
						newTable = newTable->coalesce();
						i++;
					}
					table = NULL;
				}
			}
		}
		lastElem = elem;
	}

	if (table) {
		elements->del(iTableStart);
		elements->insert( iTableStart, table);
		RtfTable *newTable = table->coalesce();
		while (newTable != NULL) {
			elements->insert( ++iTableStart, newTable);
			newTable = newTable->coalesce();
		}
	}
}

void RtfCell::dump(GString *sOutput, RtfContext *context, RtfBounds *bounds) {
	curY = parent->curY;

	RtfElement *elem = NULL;
	for(int i=0; i<elements->getLength(); i++) {
		if (i>0 && context->lastRowIsPlain == gFalse) {
			*sOutput << RTF_PAR;
		}

		context->lastRowIsPlain = gFalse;
		elem = elements->get(i);
		elem->dump(sOutput, context, bounds);
	}

	if (!plain) {
		RtfTable *table = (RtfTable *)getParentOfType(T_TABLE);
		if (table && table->nestLevel > 1)
			*sOutput << RTF_NEST_CELL;
		else
			*sOutput << RTF_CELL;
	}
}

//------------------------------------------------------------------------
// RtfRow
//------------------------------------------------------------------------

RtfRow::RtfRow(RtfElement *parentA):RtfElement(T_ROW, parentA) {
	elems = new GElementList(gFalse);
	cells = new GElementList();
	cols = new GElementList();
}

RtfRow::~RtfRow() {
	delete elems;
	delete cells;
	delete cols;
}

void RtfRow::add(RtfElement *elem) {
	elems->append( elem);
	elem->setParent(this);
}

RtfRow *RtfRow::coalesce(void) {
	int i,j,nElems = elems->getLength();
	RtfColumn *col = NULL;

	if (nElems == 0) 
		return NULL;

	// ordenar los elementos por la x ascendentemente
	elems->sort( cmpXY);

	// crear las celdas y llenarlas con los elementos
	RtfCell *cell;
	RtfElement *elem;
	for(i=0; i<nElems; i++) {
		elem = (RtfElement *)elems->get(i);
		GBool bAdded = gFalse;

		for(j=0; j<cells->getLength(); j++) {
			cell = (RtfCell *)cells->get(j);

			if (cell->accept(elem)) {
				cell->add(elem);
				bAdded = gTrue;
				break;
			}
		}

		// si el elemento no cabe en ninguna celda crear una nueva
		if (!bAdded) {
			cell = new RtfCell(this);
			cell->add( elem);
			cells->append( cell);
		}
	}

	// coalesce the cells
	for(j=0; j<cells->getLength(); j++) {
		cell = (RtfCell *)cells->get(j);
		cell->coalesce();
	}

	RtfRow *newRow = detectEnumerations();

	// crear las columnas iniciales (una para cada celda)
	for(j=0; j<cells->getLength(); j++) {
		cell = (RtfCell *)cells->get(j);

		col = new RtfColumn(this);
		col->updateBounds(cell);
		cols->append( col);
	}

	return newRow;
}

// alinear las columnas de esta fila con las
// columnas de otra fila
void RtfRow::alignCols(GElementList* cols1) {
	int i,j;
	RtfColumn *col,*col1,*lastCol,*nextCol;

	for(i=0,col=NULL; i<cols->getLength(); i++) {
		lastCol = col;
		col = (RtfColumn *)cols->get(i);
		nextCol = (i+1 < cols->getLength() ? (RtfColumn*)cols->get(i+1) : NULL);

		for(j=0; j<cols1->getLength(); j++) {
			col1 = (RtfColumn *)cols1->get(j);

			if (col->xMin > col1->xMin && col->xMin < col1->xMax && 
					(lastCol == NULL || lastCol->xMax < col1->xMin))
				col->xMin = col1->xMin;

			if (col->xMax > col1->xMin && col->xMax < col1->xMax && 
					(nextCol == NULL || nextCol->xMin > col1->xMax))
				col->xMax = col1->xMax;
		}
	}
}

// alinear los bordes de esta fila con las
// columnas de otra fila
void RtfRow::alignBorders(GElementList* cols1) {
	int j;
	RtfColumn *lastCol,*col1;

	lastCol = (RtfColumn *)cols->get(cols->getLength()-1);

	for(j=0; j<cols1->getLength(); j++) {
		col1 = (RtfColumn *)cols1->get(j);

		if (lastCol->xMax == col1->xMin)
			break;
		if (lastCol->xMax < col1->xMin) {
			lastCol->xMax = col1->xMin;
			break;
		}
	}
}

GBool RtfRow::canJoinColumns(void) {
//	return gFalse;

	// la fila debe tener dos celdas
	if (cells->getLength() > 2)
		return gFalse;

	// caso en que tenga una sola celda
	if (cells->getLength() < 2)
		return gTrue;

	// la primera celda debe tener solo un elemento
	RtfCell *cell = (RtfCell *)cells->get(0);
	if (cell->elements->getLength() != 1)
		return gFalse;

	// la primera celda no debe ser una imagen y debe tener una sola linea
	// y una sola palabra
	RtfBlock *blk = (RtfBlock *)cell->elements->get(0);
	if (blk->type != T_BLOCK || blk->isImage() || blk->lines->next != NULL /*|| 
		blk->lines->words->next != NULL*/)
		return gFalse;

	// el primer elemento de la segunda celda debe ser un bloque
	RtfCell *cell2 = (RtfCell *)cells->get(1);
	if (cell2->elements->get(0)->type != T_BLOCK)
		return gFalse;

	return gTrue;
}

// une las columnas de una fila, supone que solo se llama
// si canJoinColumns retorna gTrue
void RtfRow::joinColumns(void) {

	if (cells->getLength() < 2)
		return;

	RtfCell *cell1 = (RtfCell *)cells->get(0);
	RtfCell *cell2 = (RtfCell *)cells->get(1);

	RtfBlock *blk1 = (RtfBlock *)cell1->elements->get(0);
	RtfBlock *blk2 = (RtfBlock *)cell2->elements->get(0);

	// unir las dos celdas
	blk2->insertBlock(blk1);

	delete cell1;
	cells->del(0);

	// eliminar la primera columna y actualizar la segunda
	cols->get(1)->resetBounds();
	cols->get(1)->updateBounds(cell2);
	delete cols->get(0);
	cols->del(0);
}

//Detectar posibles enumeraciones que aun queden pendientes
// el algoritmo es el siguiente:
// para cada fila
// si tiene dos celdas
// si todos los bloques de la primera celda tienen una sola linea y 
//		una sola palabra y estan alineados
// para cada bloque b2[i] de la segunda celda
//		para cada bloque b1[j] de la primera celda
//			si b1[j] apunta a la primera linea de b2[i] y el proximo bloque b1[j+1] apunta a otra linea de b2[i]
//				dividir b2[i] empezando por esa otra linea

RtfRow* RtfRow::detectEnumerations(void) {
	int i,j,k;

	if (cells->getLength() != 2)
		return NULL;

	RtfCell *cell1 = (RtfCell *)cells->get(0);
	RtfCell *cell2 = (RtfCell *)cells->get(1);

	if (cell1->elements->getLength() <= 1)
		return NULL;

	if (cell2->elements->getLength() != 1)
		return NULL;

	for(i=0; i<cell1->elements->getLength(); i++) {
		RtfBlock *blk = (RtfBlock *)cell1->elements->get(i);

		if (blk->type != T_BLOCK)
			return NULL;

		if (blk->nLines != 1)
			return NULL;

		if (blk->lines->next != NULL)
			return NULL;
		//TODO: alineacion
	}

	RtfRow *newRow = NULL;

	for(i=0; i<cell2->elements->getLength(); i++) {
		RtfBlock *blk2 = (RtfBlock *)cell2->elements->get(i);

		if (blk2->type != T_BLOCK)
			continue;

		for(j=0; j<cell1->elements->getLength(); j++) {
			RtfBlock *blk1 = (RtfBlock *)cell1->elements->get(j);

			if (blk1->horizontalMatch(blk2->lines)) {
				if (j+1 >= cell1->elements->getLength())
					continue;

				RtfBlock *blk1_1 = (RtfBlock *)cell1->elements->get(j+1);
				RtfLine *line = blk2->lines->next;
				RtfBlock *newBlk = NULL;

				for(k=1; line; k++,line=line->next) {
					if (blk1_1->horizontalMatch(line)) {
						newBlk = blk2->splitByLine(k);

						newRow = new RtfRow(parent);

						// añadir el bloque blk[j+1] y los que le siguen a este
						for(j=j+1; j<cell1->elements->getLength(); ) {
							newRow->add( (RtfBlock *)cell1->elements->get(j));
							cell1->elements->del(j);
						}

						newRow->add(newBlk);

						cell1->resetBounds();
						cell2->resetBounds();
						resetBounds();
						blk1->setParent(cell1);
						blk2->setParent(cell2);
						break;
					}
				}

				if (newBlk)
					break;
			}
		}
	}

	return newRow;
}

void RtfRow::dump(GString *sOutput, RtfContext *context, RtfBounds *bounds) {
	int i,nextRowIndex;
	double interRowMargin;
	
	RtfTable *table = (RtfTable *)parent;

	// calcular el espacio entre esta fila y la siguiente
	nextRowIndex = table->rows->indexOf(this) + 1;
	if (nextRowIndex < table->rows->getLength()) {
		interRowMargin = ((RtfRow*)table->rows->get(nextRowIndex))->yMin - yMax;
	}	else {
		// si es la ultima fila, se toma el yMax de la tabla
		// esto hace falta en el caso de un bloque de pos. absoluta
		// que sobresalga de la tabla
		interRowMargin = table->yMax - yMax;
	}

	if (cells->getLength() != 1) {
		GString* sRowDef = new GString();
		*sRowDef << RTF_ROW_HEADER(interRowMargin);
		
		// poner la x donde empieza cada columna, las coordenadas en la x
		// se dan relativas a la celda que la contiene
		RtfElement *parentCell = getParentOfType(T_CELL);
		double leftBound = (parentCell) ? parentCell->xMin : 0.0;

		double* colArray = new double[cols->getLength()+1];
		for(i=0; i<cols->getLength(); i++) {
			RtfColumn *col = (RtfColumn *)cols->get(i);

			if (i == 0)
				*sRowDef << RTF_TABLE_LEFT(col->xMin - leftBound);
			else
				*sRowDef << RTF_CELL_POS(col->xMin - leftBound);
			colArray[i] = col->xMin;

			if (i+1 == cols->getLength()) {
				*sRowDef << RTF_CELL_POS(col->xMax - leftBound);
				colArray[i+1] = col->xMax;
			}
		}
		
		// si la tabla esta anidada, la definicion de columnas solo se pone 
		// final, sino, al principio y al final
		if (table->nestLevel == 1)
			*sOutput << sRowDef;

		if (table->nestLevel > 1)
			*sOutput << RTF_TABLE_PAR_NEST(table->nestLevel);
		else
			*sOutput << RTF_TABLE_PAR;

		for(i=0; i<cells->getLength(); i++) {
			RtfElement *cell = cells->get(i);
			RtfBounds cellBounds;

			cellBounds.xMin = colArray[i];
			cellBounds.xMax = colArray[i+1];
			cellBounds.yMin = cell->yMin;
			curY = yMin;

			cell->dump(sOutput, context, &cellBounds);
		}

		delete []colArray;

		if (table->nestLevel > 1)
			*sOutput << RTF_NEST_ROW(sRowDef);
		else
			*sOutput << sRowDef << RTF_ROW;
			//*sOutput << RTF_ROW;

		delete sRowDef;
	} else { // cuando hay una sola columna se pone sin tabla
		curY = parent->curY;
		RtfCell *cell = (RtfCell *)cells->get(0);
		cell->plain = gTrue;

		// poner el espacio despues de la fila
		if (interRowMargin > 0)
			*sOutput << RTF_SA(interRowMargin);

		cell->dump(sOutput, context, bounds);
//		*sOutput << RTF_NEW_PARAGRAPH;
		*sOutput << RTF_PAR;
		context->lastRowIsPlain = gTrue;
	}
}

//------------------------------------------------------------------------
// RtfTable
//------------------------------------------------------------------------

RtfTable::RtfTable(RtfElement *parentA):RtfElement(T_TABLE, parentA) {
	blocks = new GElementList(gFalse);
	rows = new GElementList();
	absBlocks = new GElementList();
	bFromCopy = gFalse;

	// buscar el nivel de anidamiento de la tabla
	RtfTable *temp = (RtfTable*)getParentOfType(T_TABLE);
	if (temp) 
		nestLevel = temp->nestLevel + 1;
	else
		nestLevel = 1;
}

RtfTable::~RtfTable() {
	delete blocks;
	delete rows;
	delete absBlocks;
}

void RtfTable::add(RtfElement *blk) {
	blk->setParent(this);
	blocks->append(blk);
}

void RtfTable::copyRows(RtfTable* table, int start, int end) {
	bFromCopy = gTrue;
	for(int i=start; i <= MIN( end, table->rows->getLength()-1); i++) {
		RtfRow *row = (RtfRow *)table->rows->get(i);
		rows->append( row);
		row->setParent( this);
	}
}

RtfTable* RtfTable::coalesce() {
	int i,j;
	RtfRow *row;
	RtfTable *newTable = NULL;

	// si la tabla fue creada por una copia, no hay que hacer esta parte
	if (bFromCopy == gFalse) {

		if (blocks->getLength() == 0)
			return newTable;

		// detectar los bloques que por solapamiento necesitan
		// posicionamiento absoluto
		for (i = 0; i<blocks->getLength(); i++) {
			RtfBlock *blk1 = (RtfBlock *)blocks->get(i);

			for (j = i+1; j<blocks->getLength(); j++) {
				RtfBlock *blk2 = (RtfBlock *)blocks->get(j);

				if (blk1->intersect(blk2)) {
					absBlocks->append(blk2);
					blocks->del(j);
					j--;
				}
			}
		}
		row = NULL;
		
		// crear las filas
		for (i = 0; i<blocks->getLength(); i++) {
			RtfBlock *blk = (RtfBlock *)blocks->get(i);

			if (row == NULL || row->accept(blk) == gFalse) {
				row = new RtfRow(this);
				rows->append(row);
			}
			row->add(blk);
		}

		for(i=0; i<rows->getLength(); i++) {
			row = (RtfRow *)rows->get(i);
			RtfRow *newRow = row->coalesce();
			if (newRow)
				rows->insert(i+1, newRow);
		}
	}

	// alinear las columnas de cada fila
	alignCols();

	// crear celdas vacias en las filas si es necesario para
	// que todas las filas tengan las mismas columnas
/*	RtfCell *newCell;

	for(i=0; i<rows->getLength(); i++) {
		row = (RtfRow *)rows->get(i);

		for(j=0,k=0; j<cols->getLength(); j++) {
			col = (RtfColumn *)cols->get(j);

			if (k >= row->cells->getLength()) {
				newCell = new RtfCell();
				// poner las coordenadas en base a la columna y la fila
				newCell->xMin = col->xMin;
				newCell->xMax = col->xMax;
				newCell->yMin = row->yMin;
				newCell->yMax = row->yMax;

				row->insertCell(-1, newCell);
				k++;
			} else {
				cell = (RtfCell *)row->cells->get(k);

				if (col->xMax < cell->xMin) {
					newCell = new RtfCell();
					// poner las coordenadas en base a la columna y la fila
					newCell->xMin = col->xMin;
					newCell->xMax = col->xMax;
					newCell->yMin = row->yMin;
					newCell->yMax = row->yMax;

					row->insertCell(k, newCell);
				} else {
					j += cell->span;
				}
				k++;
			}
		}
	}*/

	return newTable;
}

// intenta unir las columnas de la tabla
void RtfTable::joinColumns() {
	int i,j,k;
	//return;

	// si la tabla tiene dos columnas y la primera es de una sola linea
	// entonces se puede poner sin tabla
	GBool canJoinColumns = gFalse;
	for(i=0; i<rows->getLength(); i++) {
		RtfRow* row = (RtfRow*)rows->get(i);
		if (row->canJoinColumns())
			canJoinColumns = gTrue;
		else {
			canJoinColumns = gFalse;
			break;
		}
	}

	if (canJoinColumns == gTrue) {
		for(i=0; i<rows->getLength(); i++) {
			RtfRow* row = (RtfRow*)rows->get(i);
			row->joinColumns();
		}
	}

	// realizar el mismo proceso con las posibles subtablas que tengan
	// las celdas
	for(i=0; i<rows->getLength(); i++) {
		RtfRow* row = (RtfRow*)rows->get(i);

		for(j=0; j<row->cells->getLength(); j++) {
			RtfCell* cell = (RtfCell*)row->cells->get(j);

			for(k=0; k<cell->elements->getLength(); k++) {
				RtfElement* elem = cell->elements->get(k);

				if (elem->type == T_TABLE)
					((RtfTable*)elem)->joinColumns();
			}
		}
	}
}
void RtfTable::alignCols() {
	RtfRow *row1,*row2;
	int i,j;

	for(i=0; i<rows->getLength(); i++) {
		row1 = (RtfRow *)rows->get(i);

		for(j=0; j<rows->getLength(); j++) {
			if (i == j)
				continue;

			row2 = (RtfRow *)rows->get(j);
			row1->alignCols(row2->cols);
		}
	}
}

void RtfTable::alignBorders() {
	RtfRow *row1,*row2;
	int i,j;

	for(i=0; i<rows->getLength(); i++) {
		row1 = (RtfRow *)rows->get(i);

		for(j=0; j<rows->getLength(); j++) {
			if (i == j)
				continue;

			row2 = (RtfRow *)rows->get(j);
			row1->alignBorders(row2->cols);
		}
	}
}

GBool RtfTable::join(RtfTable *table) {
	if (yMax + MAX(9.0, maxFontSize*1.4) < table->yMin)
		return gFalse;

	// determinar si las columnas concuerdan lo suficiente como para unir las tablas

	// comparar la ultima fila de esta tabla con la primera de la que se 
	// quiere unir
	RtfRow *lastRow,*firstRow;

	lastRow = (RtfRow *)rows->getLast();
	firstRow = (RtfRow *)table->rows->get(0);

	if (lastRow->cols->getLength() != firstRow->cols->getLength())
		return gFalse;

	int i;
	for(i=0; i<lastRow->cols->getLength(); i++) {
		RtfColumn *col1 = (RtfColumn *)lastRow->cols->get(i);
		RtfColumn *col2 = (RtfColumn *)firstRow->cols->get(i);
		if (col1->xMin >= col2->xMax || col1->xMax <= col2->xMin)
			return gFalse;
	}

	// copiar y borrar las filas de table
	for(i=0; i<table->rows->getLength(); i++) {
		RtfRow *row= (RtfRow *)table->rows->get(i);
		row->setParent(this);
		rows->append( row);
	}
	table->rows->clear();

	// copiar y borrar los bloques de pos. absoluta
	for(i=0; i<table->absBlocks->getLength(); i++) {
		RtfBlock *blk= (RtfBlock *)table->absBlocks->get(i);
		blk->setParent(this);
		absBlocks->append( blk);
	}
	table->absBlocks->clear();

	//actualizar las columnas
	alignCols();

	return gTrue;
}

// aumentar el ancho de la ultima columna para evitar el wrapping
void RtfTable::expandLastCol(void) {
	RtfColumn *col = NULL;
	//RtfColumn *lastCol = NULL;
	int i;
	double maxValue = -1;
	// valor con que se actualizara la ultima columna
	double lastColxMinUpdate = 10000;
	GBool bExpandNeeded = gFalse;

	// obtener el valor maximo de la x de la penultima columna (maxValue),
	// para saber hasta donde se puede expandir la ultima columna
	for(i=0; i<rows->getLength(); i++) {
		RtfRow* row = (RtfRow*)rows->get(i);

		int colLen = row->cols->getLength();
		if (colLen < 2)
			continue;

		RtfColumn *col = (RtfColumn*) row->cols->get(colLen-2);
		maxValue = MAX( maxValue, col->xMax);

		// verificar si la expansion hace falta
		col = (RtfColumn*)row->cols->get(colLen-1);
		RtfCell *cell = (RtfCell*)row->cells->get(colLen-1);

		if (col->xMin < maxValue)
			continue;

		if (col->getWidth() < cell->maxFontSize*2.5) {
			// celdas pequeñas que necesitan expandirse
			bExpandNeeded = gTrue;
			double temp = MAX( maxValue, col->xMin - cell->maxFontSize*1.8);
			lastColxMinUpdate = MIN( lastColxMinUpdate, temp);
		}
	}

	if (maxValue < 0 || bExpandNeeded == gFalse)
		return;

	// realizar la expansion
	for(i=0; i<rows->getLength(); i++) {
		RtfRow* row = (RtfRow*)rows->get(i);

		int colLen = row->cols->getLength();
		if (colLen < 2)
			continue;

		col = (RtfColumn*)row->cols->get(colLen-1);
		if (col->xMin > lastColxMinUpdate)
			col->xMin = lastColxMinUpdate;
	}
}

void RtfTable::dump(GString *sOutput, RtfContext *context, RtfBounds *bounds) {
	int i;
	RtfBlock *blk;

	// alinear los bordes de la tabla, no se llama en el coalesce
	// porque hay que hacerlo cuando la tabla ya no va a cambiar 
	// sus columnas
	alignBorders();

	// para ajustar la posicion vertical de la tabla, se inserta un parrafo
	// con sb para poner la distancia respecto al ultimo bloque.
	if (((RtfRow*)rows->get(0))->cells->getLength() != 1) {
		if (yMin - parent->curY > minInterTableHeight) {
			if (nestLevel > 1)
				*sOutput << RTF_TABLE_TOP_NESTED(yMin - parent->curY - minInterTableHeight);
			else
				*sOutput << RTF_TABLE_TOP(yMin - parent->curY - minInterTableHeight);
		}
	} else {
		curY = parent->curY;
	}
	
	// poner los bloques de posicion absoluta
	for(i=0; i<absBlocks->getLength(); i++) {
		blk = (RtfBlock*)absBlocks->get(i);
		double width = blk->xMax - blk->xMin;
		*sOutput << RTF_FRAME(blk->getLeftMargin(0),blk->yMin - yMin, width);
		
		blk->dump( sOutput, context, blk);
		*sOutput << RTF_NEW_PARAGRAPH;
	}
	
	// si hay una sola columna no se hace una tabla y entonces el container
	// es la pagina
/*	GBool oneColumn = gTrue;
	for(i=0; i<rows->getLength(); i++) {
		RtfRow* row = (RtfRow*)rows->get(i);
		if (row->cells->getLength() != 1) {
			oneColumn = gFalse;
			break;
		}
	}
	if (oneColumn) {
		for(i=0; i<rows->getLength(); i++) {
			RtfRow* row = (RtfRow*)rows->get(i);
			row->raw = gTrue;
			row->dump(sOutput, curFont, uMap, bounds);
		}
	} else {*/
		// poner las filas
		for(i=0; i<rows->getLength(); i++) {
			RtfRow* row = (RtfRow*)rows->get(i);

			if (i > 0) 
				curY = row->yMin;

			row->dump(sOutput, context, bounds);
		}

	// actualizar curY del parent
	RtfElement::dump(sOutput, context, bounds);
}

//------------------------------------------------------------------------
// RtfPage
//------------------------------------------------------------------------

RtfPage::RtfPage(RtfOutputDev *outputDevA):RtfElement(T_PAGE, NULL) {
  int rot;
	
	outputDev = outputDevA;
  curWord = NULL;
  charPos = 0;
  curFont = NULL;
  curFontSize = 0;
  nest = 0;
  nTinyChars = 0;
  nImages = 0;
	imagesSize = 0;
  lastCharOverlap = gFalse;
	dumpedPages = new GList();
	
  for (rot = 0; rot < 4; ++rot) {
		pools[rot] = new RtfPool();
  }
	
  blocks = NULL;
  rawWords = NULL;
  rawLastWord = NULL;
  fonts = new GList();
  nBlocks = 0;
	elements = NULL;
	context = new RtfContext();

  rtfFonts = new GStringList();
  rtfColors = new GStringList();
}

RtfPage::~RtfPage() {
  int rot;
	
  clear();
	
  for (rot = 0; rot < 4; ++rot) {
		delete pools[rot];
  }
	
	delete context;

  deleteGList(fonts, RtfFont);
	//  delete fonts;
  if (rtfFonts) delete rtfFonts;
  if (rtfColors) delete rtfColors;
	if (dumpedPages) deleteGList( dumpedPages, RtfDumpedPage);
}

void RtfPage::startPage(GfxState *state) {
  clear();
	elements = new GElementList();

  if (state) {
    pageWidth = state->getPageWidth();
    pageHeight = state->getPageHeight();
  } else {
    pageWidth = pageHeight = 0;
  }
}

void RtfPage::endPage() {
  if (curWord) {
    endWord();
  }
}

void RtfPage::clear() {
  int rot;
	
  if (curWord) {
    delete curWord;
    curWord = NULL;
  }
	
  //for(int i=0; i<nBlocks; i++)
	//	delete blocks[i];
	
  for (rot = 0; rot < 4; ++rot) {
		delete pools[rot];
  }
	
  gfree(blocks);
	
	//  deleteGList(fonts, RtfFont);
	
  curWord = NULL;
  charPos = 0;
  curFont = NULL;
  curFontSize = 0;
  nest = 0;
  nTinyChars = 0;
  nBlocks = 0;
	
  for (rot = 0; rot < 4; ++rot) {
		pools[rot] = new RtfPool();
  }
	
	if (elements) {
		delete elements;
		elements = NULL;
	}

  blocks = NULL;
  nImages = 0;
	imagesSize = 0;
  rawWords = NULL;
  rawLastWord = NULL;
	//  fonts = new GList();
}

void RtfPage::updateFont(GfxState *state) {
  int i;
	
  // crear una fuente para el estado actual y ver si existe
  // entre las que ya se han creado
  RtfFont *stateFont = new RtfFont(state);
  curFont = NULL;
  for (i = 0; i < fonts->getLength(); ++i) {
    curFont = (RtfFont *)fonts->get(i);
    if (curFont->matches(stateFont)) {
      break;
    }
    curFont = NULL;
  }
	
  if (curFont == NULL) {
    curFont = stateFont;
    fonts->append(curFont);
		
    //adicionar el nombre de la fuente al listado de fuentes rtf y actualizar su indice
    
    GString *sRtfFontName;
    //convertir el nombre al usado por los RTF
    sRtfFontName = curFont->fontName->copy();
    //eliminar lo que esta despues del "-" pues son modificadores
    char *hyphen;
    hyphen = strchr( sRtfFontName->getCString(), '-');
    if (hyphen) {
      *hyphen = 0x0;
      GString *s = new GString(sRtfFontName->getCString());
      delete sRtfFontName;
      sRtfFontName = s;
    }
		
    //quitar el tag que pueden tener los fonts-subsets
    if (sRtfFontName->getChar(6) == '+')
			sRtfFontName->del(0,6+1);
		
    if (sRtfFontName->cmp("Times") == 0) {
      delete sRtfFontName;
      sRtfFontName = new GString(TIMES_NEW_ROMAN);
    }
    else if (sRtfFontName->cmp("Courier") == 0) {
      delete sRtfFontName;
      sRtfFontName = new GString(COURIER_NEW);
    }
		
    int rtfFontIndex = rtfFonts->addString(sRtfFontName);
    curFont->rtfFontIndex = rtfFontIndex;
		
    //adicionar el color de la fuente al listado de colores rtf y actualizar su indice
		
    GString *sColor = new GString();
    *sColor << RTF_COLOR_DEF(curFont->color);
    int rtfColorIndex = rtfColors->addString( sColor);
		
    curFont->rtfColorIndex = rtfColorIndex;
  }
  else
		delete stateFont;
	
  curFontSize = curFont->size;
}

void RtfPage::beginWord(GfxState *state, double x0, double y0) {
  double *txtm, *ctm, *fontm;
  double m[4], m2[4];
  int rot;
	
  // This check is needed because Type 3 characters can contain
  // text-drawing operations (when RtfPage is being used via
  // {X,Win}SplashOutputDev rather than RtfOutputDev).
  if (curWord) {
    ++nest;
    return;
  }
	
  // compute the rotation
  txtm = state->getTextMat();
  ctm = state->getCTM();
  m[0] = txtm[0] * ctm[0] + txtm[1] * ctm[2];
  m[1] = txtm[0] * ctm[1] + txtm[1] * ctm[3];
  m[2] = txtm[2] * ctm[0] + txtm[3] * ctm[2];
  m[3] = txtm[2] * ctm[1] + txtm[3] * ctm[3];
  if (state->getFont()->getType() == fontType3) {
    fontm = state->getFont()->getFontMatrix();
    m2[0] = fontm[0] * m[0] + fontm[1] * m[2];
    m2[1] = fontm[0] * m[1] + fontm[1] * m[3];
    m2[2] = fontm[2] * m[0] + fontm[3] * m[2];
    m2[3] = fontm[2] * m[1] + fontm[3] * m[3];
    m[0] = m2[0];
    m[1] = m2[1];
    m[2] = m2[2];
    m[3] = m2[3];
  }
  if (fabs(m[0] * m[3]) > fabs(m[1] * m[2])) {
    rot = (m[3] < 0) ? 0 : 2;
  } else {
    rot = (m[2] > 0) ? 1 : 3;
  }
	
  // si cambió el color, actualizar la fuente
  GfxRGB colorA = RtfFont::getState_FontColor(state);
  if ((colorA.r != curFont->color.r) || (colorA.g != curFont->color.g) || 
		(colorA.b != curFont->color.b)) {
		updateFont(state);
  }
	
  curWord = new RtfWord(this, state, rot, x0, y0, charPos, curFont, curFontSize);
}

void RtfPage::addChar(GfxState *state, double x, double y,
											double dx, double dy,
											CharCode c, Unicode *u, int uLen) {
  double x1, y1, w1, h1, dx2, dy2, base, sp;
  int i;
	
	// descartar los caracteres sin ancho
	if (dx <= 0 && dy <= 0)
		return;
	//	dx = 2.0;

  // if the previous char was a space, addChar will have called
  // endWord, so we need to start a new word
  if (!curWord) {
    beginWord(state, x, y);
  }
	
  // throw away chars that aren't inside the page bounds
  state->transform(x, y, &x1, &y1);
  if (x1 < 0 || x1 > pageWidth ||
		y1 < 0 || y1 > pageHeight) {
    return;
  }
	
  // subtract char and word spacing from the dx,dy values
  sp = state->getCharSpace();
  if (c == (CharCode)0x20) {
    sp += state->getWordSpace();
  }
  state->textTransformDelta(sp * state->getHorizScaling(), 0, &dx2, &dy2);
  dx -= dx2;
  dy -= dy2;
  state->transformDelta(dx, dy, &w1, &h1);
	
  // check the tiny chars limit
  if (!globalParams->getTextKeepTinyChars() &&
		fabs(w1) < 3 && fabs(h1) < 3) {
    if (++nTinyChars > 50000) {
      return;
    }
  }
	
  // break words at space character
  if (uLen == 1 && u[0] == (Unicode)0x20) {
    ++curWord->charLen;
    ++charPos;
    endWord();
    return;
  }
	
  // start a new word if:
  // (1) this character's baseline doesn't match the current word's
  //     baseline, or
  // (2) there is space between the end of the current word and this
  //     character, or
  // (3) this character overlaps the previous one (duplicated text), or
  // (4) the previous character was an overlap (we want each duplicated
  //     characters to be in a word by itself)
  // (5) hay un cambio de fuente
  base = sp = 0; // make gcc happy
	//  if (curWord->len > 0) {
	switch (curWord->rot) {
	case 0:
		base = y1;
		sp = x1 - curWord->xMax;
		break;
	case 1:
		base = x1;
		sp = y1 - curWord->yMax;
		break;
	case 2:
		base = y1;
		sp = curWord->xMin - x1;
		break;
	case 3:
		base = x1;
		sp = curWord->yMin - y1;
		break;
	}
	if (fabs(base - curWord->base) > 0.5 ||
		(fabs(curWord->maxFontSize - curFontSize) > 0.5) ||
		(curWord->len > 0 && (
		sp > minWordBreakSpace * curWord->maxFontSize ||
		sp < -minDupBreakOverlap * curWord->maxFontSize)
		) ||
		(curWord != NULL && curWord->font != curFont) ||
		//lastCharOverlap
		gFalse) {
		lastCharOverlap = gTrue;
		endWord();
		beginWord(state, x, y);
	} else {
		lastCharOverlap = gFalse;
	}
	/*  } else {
	lastCharOverlap = gFalse;
}*/
	
  // page rotation and/or transform matrices can cause text to be
  // drawn in reverse order -- in this case, swap the begin/end
  // coordinates and break text into individual chars
  if ((curWord->rot == 0 && w1 < 0) ||
		(curWord->rot == 1 && h1 < 0) ||
		(curWord->rot == 2 && w1 > 0) ||
		(curWord->rot == 3 && h1 > 0)) {
    endWord();
    beginWord(state, x + dx, y + dy);
    x1 += w1;
    y1 += h1;
    w1 = -w1;
    h1 = -h1;
  }
	
  // si no hay codigo unicode disponible, usar el codigo ascii
  if (uLen == 0 && c>32) {
    uLen = 1;
    u[0] = c;
  }
	
  // add the characters to the current word
  if (uLen != 0) {
    w1 /= uLen;
    h1 /= uLen;
  }
  for (i = 0; i < uLen; ++i) {
    curWord->addChar(state, x1 + i*w1, y1 + i*h1, w1, h1, u[i]);
  }
  ++curWord->charLen;
  ++charPos;
}

void RtfPage::endWord() {
  // This check is needed because Type 3 characters can contain
  // text-drawing operations (when RtfPage is being used via
  // {X,Win}SplashOutputDev rather than RtfOutputDev).
  if (nest > 0) {
    --nest;
    return;
  }
	
  if (curWord) {
    addWord(curWord);
    curWord = NULL;
  }
}

void RtfPage::addWord(RtfWord *word) {
  // throw away zero-length words -- they don't have valid xMin/xMax
  // values, and they're useless anyway
  if (word->len == 0 && word->image == NULL) {
    delete word;
    return;
  }

	updateBounds(word);

	pools[word->rot]->addWord(word);
}

// crear una palabra para la imagen
void RtfPage::addImage(double x,double y,double w,double h,GString *sImage)
{
  RtfWord *newWord = new RtfWord(this, x, y, w, h, sImage);
	
  nImages++;
	imagesSize += sImage->getLength();

	addWord( newWord);
}

void RtfPage::coalesce() {
  UnicodeMap *uMap;
  RtfPool *pool;
  RtfWord *word0, *word1, *word2;
  RtfLine *line;
  RtfBlock *blkList, *blk, *lastBlk;
  int rot, poolMinBaseIdx, baseIdx, startBaseIdx;
  double minBase, maxBase, newMinBase, newMaxBase;
	double colSpace1, colSpace2, lineSpace, intraLineSpace;
  GBool found;
  int count[4];
  int lrCount;
  int i, j, n;
	
  uMap = globalParams->getTextEncoding();
  blkList = NULL;
  lastBlk = NULL;
  nBlocks = 0;
  primaryRot = -1;
	
#if 1 && _DEBUG // for debugging
  printf("*** initial words ***\n");
  for (rot = 0; rot < 4; ++rot) {
    pool = pools[rot];
    for (baseIdx = pool->minBaseIdx; baseIdx <= pool->maxBaseIdx; ++baseIdx) {
      for (word0 = pool->getPool(baseIdx); word0; word0 = word0->next) {
				printf("    word: x=%.2f..%.2f y=%.2f..%.2f base=%.2f fontSize=%.2f '",
					word0->xMin, word0->xMax, word0->yMin, word0->yMax,
					word0->base, word0->maxFontSize);
				if (word0->image)
					printf("image");
				else {
					for (i = 0; i < word0->len; ++i) {
						fputc(word0->text[i] & 0xff, stdout);
					}
					printf("' fontName='%s", word0->font->fontName->getCString());
				}
				printf("'\n");
      }
    }
  }
  printf("\n");
#endif
	
  //----- assemble the blocks
	
  //~ add an outer loop for writing mode (vertical text)
	
  // build blocks for each rotation value
  for (rot = 0; rot < 4; ++rot) {
    pool = pools[rot];
    poolMinBaseIdx = pool->minBaseIdx;
    count[rot] = 0;
		
    // add blocks until no more words are left
    while (1) {
			
      // find the first non-empty line in the pool
      for (;
			poolMinBaseIdx <= pool->maxBaseIdx &&
				!pool->getPool(poolMinBaseIdx);
			++poolMinBaseIdx) ;
      if (poolMinBaseIdx > pool->maxBaseIdx) {
				break;
      }
			
      // look for the left-most word in the first four lines of the
      // pool -- this avoids starting with a superscript word
      startBaseIdx = poolMinBaseIdx;
      for (baseIdx = poolMinBaseIdx + 1;
			baseIdx < poolMinBaseIdx + 4 && baseIdx <= pool->maxBaseIdx;
			++baseIdx) {
				if (!pool->getPool(baseIdx)) {
					continue;
				}
				if (pool->getPool(baseIdx)->primaryCmp(pool->getPool(startBaseIdx))
					< 0) {
					startBaseIdx = baseIdx;
				}
      }
			
      // create a new block
      word0 = pool->getPool(startBaseIdx);
      pool->setPool(startBaseIdx, word0->next);
      word0->next = NULL;
      blk = new RtfBlock(this, rot);
      blk->addWord(word0);
			word1 = word0;
			
			// si la palabra es una imagen, buscar palabras adyacentes para
			// obtener un tamaño de fuente
			// TODO: verificar si no hace falta
			/*if (word0->font == NULL) {
				for (baseIdx = pool->getBaseIdx(word0->yMin);
					baseIdx <= pool->getBaseIdx(word0->yMax);
					baseIdx++) {
					for(word1 = pool->getPool(baseIdx); 
					word1 && word1->font == NULL; 
					word1 = word1->next);

					if (word1 && word1->font != NULL)
						break;
				}
			}
			if (word1 == NULL) word1 = word0;*/

      minBase = maxBase = word1->base;
      //fontSize = maxFontSize = minFontSize = word1->maxFontSize;
      colSpace1 = minColSpacing1 * blk->maxFontSize;
      colSpace2 = minColSpacing2 * blk->maxFontSize;
      lineSpace = maxLineSpacingDelta * blk->maxFontSize;
      intraLineSpace = maxIntraLineDelta * blk->maxFontSize;

      // add words to the block
      do {
				found = gFalse;
				
				// look for words on the line above the current top edge of
				// the block
				newMinBase = minBase;
				for (baseIdx = pool->getBaseIdx(minBase);
					baseIdx >= pool->getBaseIdx(minBase - lineSpace);
					--baseIdx) {
					word0 = NULL;
					word1 = pool->getPool(baseIdx);
					while (word1) {
						if (word1->base < minBase &&
							word1->base >= minBase - lineSpace &&
							((rot == 0 || rot == 2)
							? (word1->xMin < blk->xMax && word1->xMax > blk->xMin)
							: (word1->yMin < blk->yMax && word1->yMax > blk->yMin)) &&
//							(fabs(word1->maxFontSize - fontSize) < maxBlockFontSizeDelta1 * fontSize ||
							((word1->maxFontSize >= blk->maxFontSize*(1-maxBlockFontSizeDelta1) &&
							word1->maxFontSize <= blk->maxFontSize*(1+maxBlockFontSizeDelta1)) ||
							word1->font == NULL)) {
							word2 = word1;
							if (word0) {
								word0->next = word1->next;
							} else {
								pool->setPool(baseIdx, word1->next);
							}
							word1 = word1->next;
							word2->next = NULL;
							blk->addWord(word2);
							lineSpace = maxLineSpacingDelta * blk->maxFontSize;
							intraLineSpace = maxIntraLineDelta * blk->maxFontSize;
							found = gTrue;
							newMinBase = word2->base;
						} else {
							word0 = word1;
							word1 = word1->next;
						}
					}
				}
				minBase = newMinBase;
				
				// look for words on the line below the current bottom edge of
				// the block
				newMaxBase = maxBase;
				for (baseIdx = pool->getBaseIdx(maxBase);
						baseIdx <= pool->getBaseIdx(maxBase + lineSpace);
						++baseIdx) {
					word0 = NULL;
					word1 = pool->getPool(baseIdx);
					while (word1) {
						if (word1->base > maxBase &&
							word1->base <= maxBase + lineSpace &&
							((rot == 0 || rot == 2)
							? (word1->xMin < blk->xMax && word1->xMax > blk->xMin)
							: (word1->yMin < blk->yMax && word1->yMax > blk->yMin)) &&
//							fabs(word1->fontSize - fontSize) < maxBlockFontSizeDelta1 * fontSize) 
							((word1->maxFontSize >= blk->averageFontSize*(1-maxBlockFontSizeDelta1) &&
							word1->maxFontSize <= blk->averageFontSize*(1+maxBlockFontSizeDelta1)) ||
//							((word1->maxFontSize >= blk->maxFontSize*(1-maxBlockFontSizeDelta1) &&
//							word1->maxFontSize <= blk->maxFontSize*(1+maxBlockFontSizeDelta1)) ||
							word1->font == NULL)) {
							word2 = word1;
							if (word0) {
								word0->next = word1->next;
							} else {
								pool->setPool(baseIdx, word1->next);
							}
							word1 = word1->next;
							word2->next = NULL;
							blk->addWord(word2);
							lineSpace = maxLineSpacingDelta * blk->maxFontSize;
							intraLineSpace = maxIntraLineDelta * blk->maxFontSize;
							found = gTrue;
							newMaxBase = word2->base;
						} else {
							word0 = word1;
							word1 = word1->next;
						}
					}
				}
				maxBase = newMaxBase;
				
				// look for words that are on lines already in the block, and
				// that overlap the block horizontally
				for (baseIdx = pool->getBaseIdx(minBase - intraLineSpace);
				baseIdx <= pool->getBaseIdx(maxBase + intraLineSpace);
				++baseIdx) {
					word0 = NULL;
					word1 = pool->getPool(baseIdx);
					while (word1) {
						double maxColSpace1 = MAX(colSpace1,minColSpacing1 * word1->maxFontSize);
						if (word1->base >= minBase - intraLineSpace &&
							word1->base <= maxBase + intraLineSpace &&
							((rot == 0 || rot == 2)
							? (word1->xMin < blk->xMax + maxColSpace1 &&
							word1->xMax > blk->xMin - maxColSpace1)
							: (word1->yMin < blk->yMax + maxColSpace1 &&
							word1->yMax > blk->yMin - maxColSpace1)) &&
							word1->yMin < blk->yMax && word1->yMax > blk->yMin /*&&
							(fabs(word1->fontSize - fontSize) <	maxBlockFontSizeDelta2 * fontSize ||
							word1->font == NULL)*/) {
							word2 = word1;
							if (word0) {
								word0->next = word1->next;
							} else {
								pool->setPool(baseIdx, word1->next);
							}
							word1 = word1->next;
							word2->next = NULL;
							blk->addWord(word2);
							lineSpace = maxLineSpacingDelta * blk->maxFontSize;
							intraLineSpace = maxIntraLineDelta * blk->maxFontSize;
							found = gTrue;
						} else {
							word0 = word1;
							word1 = word1->next;
						}
					}
				}
				
				// only check for outlying words (the next two chunks of code)
				// if we didn't find anything else
				if (found) {
					continue;
				}
				
				// scan down the left side of the block, looking for words
				// that are near (but not overlapping) the block; if there are
				// three or fewer, add them to the block
				n = 0;
				for (baseIdx = pool->getBaseIdx(minBase - intraLineSpace);
				baseIdx <= pool->getBaseIdx(maxBase + intraLineSpace);
				++baseIdx) {
					word1 = pool->getPool(baseIdx);
					while (word1) {
						double maxColSpace2 = MAX(colSpace2,minColSpacing2 * word1->maxFontSize);
						if (word1->base >= minBase - intraLineSpace &&
							word1->base <= maxBase + intraLineSpace &&
							((rot == 0 || rot == 2)
							? (word1->xMax <= blk->xMin &&
							word1->xMax > blk->xMin - maxColSpace2)
							: (word1->yMax <= blk->yMin &&
							word1->yMax > blk->yMin - maxColSpace2)) &&
							fabs(word1->maxFontSize - blk->maxFontSize) <
							maxBlockFontSizeDelta3 * blk->maxFontSize) {
							++n;
							break;
						}
						word1 = word1->next;
					}
				}
				if (n > 0 && n <= 3) {
					for (baseIdx = pool->getBaseIdx(minBase - intraLineSpace);
					baseIdx <= pool->getBaseIdx(maxBase + intraLineSpace);
					++baseIdx) {
						word0 = NULL;
						word1 = pool->getPool(baseIdx);
						while (word1) {
							double maxColSpace2 = MAX(colSpace2,minColSpacing2 * word1->maxFontSize);
							if (word1->base >= minBase - intraLineSpace &&
								word1->base <= maxBase + intraLineSpace &&
								((rot == 0 || rot == 2)
								? (word1->xMax <= blk->xMin &&
								word1->xMax > blk->xMin - maxColSpace2)
								: (word1->yMax <= blk->yMin &&
								word1->yMax > blk->yMin - maxColSpace2)) &&
								fabs(word1->maxFontSize - blk->maxFontSize) <
								maxBlockFontSizeDelta3 * blk->maxFontSize) {
								word2 = word1;
								if (word0) {
									word0->next = word1->next;
								} else {
									pool->setPool(baseIdx, word1->next);
								}
								word1 = word1->next;
								word2->next = NULL;
								blk->addWord(word2);
								if (word2->base < minBase) {
									minBase = word2->base;
								} else if (word2->base > maxBase) {
									maxBase = word2->base;
								}
								found = gTrue;
								break;
							} else {
								word0 = word1;
								word1 = word1->next;
							}
						}
					}
				}
				
				// scan down the right side of the block, looking for words
				// that are near (but not overlapping) the block; if there are
				// three or fewer, add them to the block
				n = 0;
				for (baseIdx = pool->getBaseIdx(minBase - intraLineSpace);
				baseIdx <= pool->getBaseIdx(maxBase + intraLineSpace);
				++baseIdx) {
					word1 = pool->getPool(baseIdx);
					while (word1) {
						double maxColSpace2 = MAX(colSpace2,minColSpacing2 * word1->maxFontSize);
						if (word1->base >= minBase - intraLineSpace &&
							word1->base <= maxBase + intraLineSpace &&
							((rot == 0 || rot == 2)
							? (word1->xMin >= blk->xMax &&
							word1->xMin < blk->xMax + maxColSpace2)
							: (word1->yMin >= blk->yMax &&
							word1->yMin < blk->yMax + maxColSpace2)) &&
							fabs(word1->maxFontSize - blk->maxFontSize) <
							maxBlockFontSizeDelta3 * blk->maxFontSize) {
							++n;
							break;
						}
						word1 = word1->next;
					}
				}
				if (n > 0 && n <= 3) {
					for (baseIdx = pool->getBaseIdx(minBase - intraLineSpace);
					baseIdx <= pool->getBaseIdx(maxBase + intraLineSpace);
					++baseIdx) {
						word0 = NULL;
						word1 = pool->getPool(baseIdx);
						while (word1) {
							double maxColSpace2 = MAX(colSpace2,minColSpacing2 * word1->maxFontSize);
							if (word1->base >= minBase - intraLineSpace &&
								word1->base <= maxBase + intraLineSpace &&
								((rot == 0 || rot == 2)
								? (word1->xMin >= blk->xMax &&
								word1->xMin < blk->xMax + maxColSpace2)
								: (word1->yMin >= blk->yMax &&
								word1->yMin < blk->yMax + maxColSpace2)) &&
								fabs(word1->maxFontSize - blk->maxFontSize) <
								maxBlockFontSizeDelta3 * blk->maxFontSize) {
								word2 = word1;
								if (word0) {
									word0->next = word1->next;
								} else {
									pool->setPool(baseIdx, word1->next);
								}
								word1 = word1->next;
								word2->next = NULL;
								blk->addWord(word2);
								if (word2->base < minBase) {
									minBase = word2->base;
								} else if (word2->base > maxBase) {
									maxBase = word2->base;
								}
								found = gTrue;
								break;
							} else {
								word0 = word1;
								word1 = word1->next;
							}
						}
					}
				}
				
      } while (found);
			
      //~ need to compute the primary writing mode (horiz/vert) in
      //~ addition to primary rotation
			
      // coalesce the block, and add it to the list
      blk->coalesce(uMap);
			
      if (lastBlk) {
				lastBlk->next = blk;
      } else {
				blkList = blk;
      }
      lastBlk = blk;
      count[rot] += blk->charCount;
      if (primaryRot < 0 || count[rot] > count[primaryRot]) {
				primaryRot = rot;
      }
      ++nBlocks;
			
      //dividir el bloque y actualizar variables
      blk->splitBlock();
      for(;lastBlk->next; lastBlk = lastBlk->next,++nBlocks);
    }
  }
	
	if (primaryRot == -1)
		primaryRot = 0;
	
#if 0 && _DEBUG // for debugging 
  printf("*** rotation ***\n");
  for (rot = 0; rot < 4; ++rot) {
    printf("  %d: %6d\n", rot, count[rot]);
  }
  printf("  primary rot = %d\n", primaryRot);
  printf("\n");
#endif
	
#if 1 && _DEBUG // for debugging
  printf("*** blocks ***\n");
  for (blk = blkList; blk; blk = blk->next) {
    printf("block: rot=%d x=%.2f..%.2f y=%.2f..%.2f\n",
			blk->rot, blk->xMin, blk->xMax, blk->yMin, blk->yMax);
    for (line = blk->lines; line; line = line->next) {
      printf("  line: x=%.2f..%.2f y=%.2f..%.2f base=%.2f\n",
				line->xMin, line->xMax, line->yMin, line->yMax, line->base);
      for (word0 = line->words; word0; word0 = word0->next) {
				printf("    word: x=%.2f..%.2f y=%.2f..%.2f base=%.2f fontSize=%.2f space=%d: '",
					word0->xMin, word0->xMax, word0->yMin, word0->yMax,
					word0->base, word0->maxFontSize, word0->spaceAfter);
				if (word0->image)
					printf("image");
				else {
					for (i = 0; i < word0->len; ++i) {
						fputc(word0->text[i] & 0xff, stdout);
					}
					printf("' fontName='%s", word0->font->fontName->getCString());
				}
				printf("'\n");
			}
    }
  }
  printf("\n");
#endif
	
  // determine the primary direction
  lrCount = 0;
  for (blk = blkList; blk; blk = blk->next) {
    for (line = blk->lines; line; line = line->next) {
      for (word0 = line->words; word0; word0 = word0->next) {
				for (i = 0; i < word0->len; ++i) {
					if (unicodeTypeL(word0->text[i])) {
						++lrCount;
					} else if (unicodeTypeR(word0->text[i])) {
						--lrCount;
					}
				}
      }
    }
  }
  primaryLR = lrCount >= 0;
	
#if 0 && _DEBUG // for debugging
  printf("*** direction ***\n");
  printf("lrCount = %d\n", lrCount);
  printf("primaryLR = %d\n", primaryLR);
#endif
	
  //----- reading order sort
	
  // sort blocks into yx order (in preparation for reading order sort)
  blocks = (RtfBlock **)gmalloc(nBlocks * sizeof(RtfBlock *));
  for (blk = blkList, i = 0; blk; blk = blk->next, ++i) {
    blocks[i] = blk;
  }
  qsort(blocks, nBlocks, sizeof(RtfBlock *), &RtfBlock::cmpYXPrimaryRot);
	
#if 1 && _DEBUG // for debugging
  printf("*** blocks, after yx sort ***\n");
  for (i = 0; i < nBlocks; ++i) {
    blk = blocks[i];
    printf("block: rot=%d x=%.2f..%.2f y=%.2f..%.2f\n",
			blk->rot, blk->xMin, blk->xMax, blk->yMin, blk->yMax);
    for (line = blk->lines; line; line = line->next) {
      printf("  line:\n");
      for (word0 = line->words; word0; word0 = word0->next) {
				printf("    word: x=%.2f..%.2f y=%.2f..%.2f base=%.2f fontSize=%.2f space=%d: '",
					word0->xMin, word0->xMax, word0->yMin, word0->yMax,
					word0->base, word0->maxFontSize, word0->spaceAfter);
				for (j = 0; j < word0->len; ++j) {
					fputc(word0->text[j] & 0xff, stdout);
				}
				printf("'\n");
      }
    }
  }
  printf("\n");
#endif
	
	// crear los elementos

  RtfBlock *nextBlk;
	RtfTable *lastTable = NULL;
  for (i = 0; i < nBlocks; ++i) {
    blk = blocks[i];
    nextBlk = (i+1 < nBlocks) ? blocks[i+1] : NULL;
		
    if (nextBlk == NULL || nextBlk->yMin - blk->yMax > -4.0 ) {
      // si el proximo bloque esta debajo del actual
			// poner el bloque actual como un parrafo normal
			elements->append( blk);
			lastTable = NULL;
    }
    else {
      // El bloque actual tiene al menos otro al lado, por lo tanto hay que
      // hacer una tabla.
			// determinar cuales bloques van a estar en la tabla
			RtfTable *table = new RtfTable(this);
			table->add( blk);				

			for (j = i+1; j<nBlocks; j++) {
				blk = blocks[j];
				
				// si este bloque esta por debajo de los anteriores, parar el procesamiento
				if (table->accept( blk))
					table->add( blk);
				else
					break;
			}

			RtfTable* newTable = table;
			RtfTable* tempTable = NULL;
			do {
				tempTable = newTable;
				newTable = tempTable->coalesce();

				// intentar mezclar dos tablas consecutivas
				if (tempTable == table) {
					if (lastTable != NULL && lastTable->join(table) == gTrue) {
						delete table;
					} else {
						elements->append( table);
						lastTable = table;
					}
				} else {
					elements->append( tempTable);
					lastTable = tempTable;
				}
			} while (newTable != NULL);


			// continuar con el proximo bloque
			i = j-1;
		}
	}

	// realizar determinadas operaciones sobre las tablas
  for (i = 0; i<elements->getLength(); ++i) {
		if (elements->get(i)->type == T_TABLE) {
			RtfTable *table = (RtfTable*)elements->get(i);

			table->joinColumns();
			table->expandLastCol();
		}
	}

#if 1 && _DEBUG // for debugging
  printf("*** elements ***\n");
  for (i = 0; i < elements->getLength(); ++i) {
		RtfElement *elem = elements->get(i);
		elem->DebugDump(0);
	}
  printf("\n");
#endif

  if (uMap) {
    uMap->decRefCnt();
  }
}

void RtfPage::dump() {
  int i;
	
  //return;
	if (!context->startPage())
		return;

	context->setBoundsX(xMin, xMax, marginRight);

	curY = 0;
	RtfBounds marginBounds;
	marginBounds.xMin = 0;
	marginBounds.xMax = marginRight;
	marginBounds.yMin = 0;
	marginBounds.yMax = 10000;

	RtfDumpedPage *pageDump = new RtfDumpedPage();
	
	// reservar un espacio inicial igual a la suma del tamaño de todas las imagenes
	// mas una cantidad fija
	//sOutput->reservSize(imagesSize+1000);

	int elemLen = elements->getLength();
	for (i = 0; i < elemLen; ++i) {
		RtfElement *elem = elements->get(i);

		GString *s = new GString();
		elem->dump(s, context, &marginBounds);

		// esto se hace para que el ultimo elemento empieze con una fuente
		// limpia, lo cual es necesario si se usa como footer
		if (i == elemLen-2) {
			context->font->rtfClose(s);
			context->font = NULL;
		}

		RtfDumpedElem *dumped = new RtfDumpedElem(elem, s, context->font);
		pageDump->add( dumped);
	}

	dumpedPages->append( pageDump);
	
	context->endPage();
}

void RtfPage::detectHeaders(RtfHeader *header, RtfFooter *footer) {
	int i;

	// determinar la cantidad de exitos
	// de los headers y footers
	for(i=0; i<dumpedPages->getLength(); i++) {
		RtfDumpedPage *page = (RtfDumpedPage*)dumpedPages->get(i);
		page->detectHeaders( header, footer);
	}

	// determinar la validez de los headers y footers
	header->analyze();
	footer->analyze();

	// obtener los bounds de las paginas
	for(i=0; i<dumpedPages->getLength(); i++) {
		RtfDumpedPage *page = (RtfDumpedPage*)dumpedPages->get(i);
		page->calcBounds( header, footer);

		// actualizar los bounds
		bodyBounds.updateBounds(page);
	}

	// obtener los bounds de las paginas
	for(i=0; i<dumpedPages->getLength(); i++) {
		RtfDumpedPage *page = (RtfDumpedPage*)dumpedPages->get(i);
		page->finish( header, footer, &bodyBounds);
	}
}

void RtfPage::writeRtf(GString *sRtf) {
	int i;

	RtfDumpedPage *lastPage = NULL;
	for(i=0; i<dumpedPages->getLength(); i++) {
		RtfDumpedPage *page = (RtfDumpedPage*)dumpedPages->get(i);
		page->dump( sRtf, outputDev->header, outputDev->footer, &bodyBounds, 
			pageHeight, lastPage);
		lastPage = page;
	}

/*TOFIX		totalLen += sRtfPages->getString(i)->getLength();

	sRtf->reservSize(totalLen);

	for(i=0; i<sRtfPages->getLength(); i++)
		*sRtf << sRtfPages->getString(i);
		*/
}

// Escribir las definiciones de las fuentes
void RtfPage::WriteFonts(GString *sOutput){
  *sOutput << "{\\fonttbl";
	
  for(int i=0; i<rtfFonts->getLength(); i++) {
    GString *s = rtfFonts->getString(i);		

		if (strcasecmp(s->getCString(), "ZapfDingbats") == 0)
			*sOutput << RTF_FONT_DEF(i,"ITC Zapf DingBats")
		else
			*sOutput << RTF_FONT_DEF(i,s);
  }
	
  *sOutput << "}" << NEW_LINE;
}

// Escribir las definiciones de los colores
void RtfPage::WriteColorTable(GString *sOutput) {
  *sOutput << "{\\colortbl;";
	
  for(int i=0; i<rtfColors->getLength(); i++)
		*sOutput << rtfColors->getString(i)->getCString();
	
  *sOutput << "}" << NEW_LINE;
}

void RtfPage::setMargins(double marginLeftA, double marginRightA) {
	marginLeft = marginLeftA;
	marginRight = marginRightA;
}

RtfBounds* RtfPage::getBodyBounds() {
	return &bodyBounds;
}

//------------------------------------------------------------------------
// RtfOutputDev
//------------------------------------------------------------------------

RtfOutputDev::RtfOutputDev(GString* sRtfA, PDFDoc *docA, ProcOptions *optionsA)
{
  sRtf = sRtfA;
  ok = gTrue;
  doc = docA;
	options = optionsA;
//	bHeaderFooter = gFalse;
	bHeaderFooter = gTrue;

	header = new RtfHeader();
	footer = new RtfFooter();
  // set up text object
  text = new RtfPage(this);

	//obtener los margenes a partir de la primera pagina
  PDFRectangle *MediaBox, *CropBox;
//  PDFRectangle *Box, *TrimBox, *ArtBox;
	
  //MediaBox es el tamaño real de la pagina
  //CropBox es el tamaño de la parte visible (las coordenadas son
	// relativas al CropBox)
  MediaBox = doc->getCatalog()->getPage(1)->getMediaBox();
  CropBox = doc->getCatalog()->getPage(1)->getCropBox();
/*  Box = doc->getCatalog()->getPage(1)->getBox();
  TrimBox = doc->getCatalog()->getPage(1)->getTrimBox();
  ArtBox = doc->getCatalog()->getPage(1)->getArtBox();*/
	
	double marginLeft = CropBox->x1;
	double marginRight = CropBox->x2 - CropBox->x1;
	text->setMargins(marginLeft, marginRight);

}

RtfOutputDev::~RtfOutputDev() {
  if (text)
    delete text;

	if (header)
		delete header;

	if (footer)
		delete footer;
}

void RtfOutputDev::startPage(int pageNum, GfxState *state) {
#ifdef _DEBUG
  printf("Page %d \n",pageNum);
#endif
	
  text->startPage(state);

	options->updateProgress(pageNum, doc->getNumPages());
}

void RtfOutputDev::endPage() {
  text->endPage();
  text->coalesce();
	text->dump();

	// extraer las posiciones de los headers y footers
	if (!text->empty()) {
		header->update( text);
		footer->update( text);
	}

	text->resetBounds();
}

void RtfOutputDev::updateFont(GfxState *state) {
  text->updateFont(state);
}

void RtfOutputDev::beginString(GfxState *state, GString *s) {
}

void RtfOutputDev::endString(GfxState *state) {
}

void RtfOutputDev::drawChar(GfxState *state, double x, double y,
														double dx, double dy,
														double originX, double originY,
														CharCode c, Unicode *u, int uLen) {
  text->addChar(state, x, y, dx, dy, c, u, uLen);
}

void RtfOutputDev::WriteHeader() {
	//int totalLen = 0;

  if (!isOk())
		return;
	
	if (bHeaderFooter) {
		text->detectHeaders(header, footer);
	}

  text->writeRtf(sRtf);

  GString sHeader;
	
  sHeader << RTF_HEADER;
  text->WriteFonts(&sHeader);
  text->WriteColorTable(&sHeader);
	
  PDFRectangle *MediaBox, *CropBox;
	
  //MediaBox es el tamaño real de la pagina
  //CropBox es el tamaño de la parte visible
  MediaBox = doc->getCatalog()->getPage(1)->getMediaBox();
  CropBox = doc->getCatalog()->getPage(1)->getCropBox();
	
  //escribir el tamaño de la pagina
  sHeader << RTF_PAPER_WIDTH(MediaBox->x2 - MediaBox->x1);
  sHeader << RTF_PAPER_HEIGHT(MediaBox->y2 - MediaBox->y1);
	
  //escribir los margenes
//  double margt = int(CropBox->y1 - MediaBox->y1);
//  double margb = int(MediaBox->y2 - CropBox->y2);
  double margt = CropBox->y1 + text->getBodyBounds()->yMin - MediaBox->y1;
  double margb = MediaBox->y2 - (CropBox->y1 + text->getBodyBounds()->yMax);

//  if (margt <= 0) margt = MIN_MARG_TOP;
//  if (margb <= 0) margb = MIN_MARG_BOTTOM;
  if (margt <= 0) margt = 0;
  if (margb <= 0) margb = 0;
	//TODO: tomar en cuenta o no?, si se toma en cuenta hay que ajustar el sb
  margt = MIN( margt, MAX_MARG_TOP);
  margb = MIN( margb, MAX_MARG_BOTTOM);
	
	double marginLeft = CropBox->x1 - MediaBox->x1;
	double marginRight = MediaBox->x2 - CropBox->x2;
  sHeader << RTF_MARGIN_LEFT(marginLeft);
  sHeader << RTF_MARGIN_RIGHT(marginRight);
  sHeader << RTF_MARGIN_TOP(margt) << RTF_MARGIN_BOTTOM(margb);
	
  sHeader << RTF_HEADER2;
  *sRtf << RTF_END;
	
  sRtf->insert(0, &sHeader);
	
  return;
}

void RtfOutputDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
																 int width, int height, GBool invert,
																 GBool inlineImg) {
  if (options->bImages == gFalse)
		return;
  
	GString *sImage;
  int x0, y0;			// top left corner of image
  int w0, h0, w1, h1;		// size of image
  double xt, yt, wt, ht;
  GBool rotate, xFlip, yFlip;
	int x,y;
	
  // get image position and size
  state->transform(0, 0, &xt, &yt);
  state->transformDelta(1, 1, &wt, &ht);
  if (wt > 0) {
    x0 = Round(xt);
    w0 = Round(wt);
  } else {
    x0 = Round(xt + wt);
    w0 = Round(-wt);
  }
  if (ht > 0) {
    y0 = Round(yt);
    h0 = Round(ht);
  } else {
    y0 = Round(yt + ht);
    h0 = Round(-ht);
  }
  state->transformDelta(1, 0, &xt, &yt);
  rotate = fabs(xt) < fabs(yt);
  if (rotate == gFalse) {
    w1 = w0;
    h1 = h0;
    xFlip = wt < 0;
    yFlip = ht > 0;
  } else {
    w1 = h0;
    h1 = w0;
    xFlip = ht < 0;
    yFlip = wt < 0;
  }

	// para no permitir ancho o alto nulo
  if (w1 == 0) w1 = 1;
  if (h1 == 0) h1 = 1;

  // initialize stream
  str->reset();
	// ancho en bytes de cada fila
	int widthB = (width + 7) / 8;
		
	if (options->bPNG) { // salvar la imagen en formato PNG
		
		// cargar la imagen en un formato de 1 bpp
		Guchar* pImage = new Guchar[width*height];
		Guchar** rows = new Guchar*[height];
		Guchar* pImg = pImage;

		int start = (yFlip) ? height-1 : 0;
		int sign = (yFlip) ? -1 : 1;
		for (y = 0; y < height; ++y) {
			rows[start + y*sign] = pImg;
			for (x = 0; x < widthB; ++x) 
				*(pImg++) = str->getChar();
		}

		// en los png solo se puede especificar el tamaño modificando la escala
		int wZoom = Round(fabs((rotate) ?ht :wt)/(width*0.75) * 100);
		int hZoom = Round(fabs((rotate) ?wt :ht)/(height*0.75) * 100);

		sImage = new GString();
		if (options->bBinary == gFalse)
			*sImage << RTF_PNG_HEADER(wZoom,hZoom);

		// convertirla a png
		CreatePNG( width, height, 1, rows, sImage, options->bBinary);

		if (options->bBinary == gTrue) {
			GString sHeader;
			sHeader << RTF_PNG_HEADER(wZoom,hZoom);
			sHeader << RTF_BIN(sImage->getLength());
			sImage->insert( 0, &sHeader);
		}

		delete[] pImage;
		delete[] rows;
		
		*sImage << RTF_PNG_END;
	} else { //salvar la imagen en formato WMF (1 bpp)
		// ancho en bytes de cada fila tomando en cuenta la alineacion a 4 bytes
		int widthBPad4 = (widthB + 3) & ~3;
		
		sImage = new GString((widthBPad4*height + 500)*2);

		*sImage << RTF_WMF_HEADER(width,height,widthBPad4,w1,h1,1,2);
		*sImage << RTF_WMF_PALETTE(0,0,0) << RTF_WMF_PALETTE(255,255,255);
		int dataStart = sImage->getLength();

		// reservar la memoria que ocupa una linea mas 2 por el cambio de linea
		GString* sLine = new GString((widthBPad4 + 2)*2);
	
		for (y = 0; y < height; ++y) {
			sLine->clear();
			
			for (x = 0; x < widthB; ++x) 
				*sLine << HEX8(str->getChar());
			
			// completar para que cada fila comienze en un multiplo de 4
			for (x = widthB; x < widthBPad4; ++x) 
				*sLine << "00";
			
			*sLine << NEW_LINE;
			
			if (yFlip) // al parecer los WMF se guardan invertidos verticalmente
				*sImage << sLine;
			else
				sImage->insert(dataStart, sLine);
		}
		
		str->close();
		delete sLine;

		*sImage << RTF_WMF_END;
	}

	text->addImage(x0, y0, w1, h1, sImage);
}

void RtfOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
														 int width, int height,
														 GfxImageColorMap *colorMap,
														 int *maskColors, GBool inlineImg) {
	
  if (options->bImages == gFalse)
		return;
	
  ImageStream *imgStr;
	GString *sImage;
  Guchar *p;
  GfxRGB rgb;
  int x0, y0;						// top left corner of image
  int w0, h0, w1, h1;		// size of image
  double xt, yt, wt, ht;
  GBool rotate, xFlip, yFlip;
  int x, y;
	int c;
	
  // get image position and size
  state->transform(0, 0, &xt, &yt);
  state->transformDelta(1, 1, &wt, &ht);
  if (wt > 0) {
    x0 = Round(xt);
    w0 = Round(wt);
  } else {
    x0 = Round(xt + wt);
    w0 = Round(-wt);
  }
  if (ht > 0) {
    y0 = Round(yt);
    h0 = Round(ht);
  } else {
    y0 = Round(yt + ht);
    h0 = Round(-ht);
  }
  state->transformDelta(1, 0, &xt, &yt);
  rotate = fabs(xt) < fabs(yt);
  if (rotate == gFalse) {
    w1 = w0;
    h1 = h0;
    xFlip = wt < 0;
    yFlip = ht > 0;
  } else {
    w1 = h0;
    h1 = w0;
    xFlip = ht < 0;
    yFlip = wt < 0;
  }

	// para no permitir ancho o alto nulo
  if (w1 == 0) w1 = 1;
  if (h1 == 0) h1 = 1;

	if (options->bJPG && str->getKind() == strDCT &&
      colorMap->getNumPixelComps() == 3 &&
      !inlineImg) {	//salvar la imagen en formato JPG

		// en los jpg solo se puede especificar el tamaño modificando la escala
		int wZoom = Round(fabs((rotate) ?ht :wt)/(width*0.75) * 100);
		int hZoom = Round(fabs((rotate) ?wt :ht)/(height*0.75) * 100);

    // initialize stream
    str = ((DCTStream *)str)->getRawStream();
    str->reset();

		//TODO: reservar el tamaño del jpg
		GString *sBinImage = new GString(8000);

		// cargar toda la imagen en un buffer
    while ((c = str->getChar()) != EOF) {
			sBinImage->append(c);
		}
    str->close();

		// invertir la imagen
		if (yFlip) {
			GString* dstImage = new GString();
			/*int res =*/ flipJPEG(sBinImage, dstImage);
			// salir si ocurrio un error (esto no debe pasar)
//			if (res != 0) 
//				return;
			delete sBinImage;
			sBinImage = dstImage;
		}

    // convertir la imagen a hexadecimal si es necesario e insertar el encabezamiento
		if (options->bBinary == gTrue) {
			sImage = sBinImage;
			sBinImage = NULL;

			GString sHeader;
			sHeader << RTF_JPG_HEADER(wZoom,hZoom);
			sHeader << RTF_BIN(sImage->getLength());

			sImage->insert( 0, &sHeader);
		} else {
			char* p = sBinImage->getCString();
			int len = sBinImage->getLength();

			sImage = new GString(len*2 + 1000);
			*sImage << RTF_JPG_HEADER(wZoom,hZoom);

			int column = 0;
			for(int i=0; i<len; i++)
			{
				*sImage << HEX8(*(p++));
				if (column++ == 80) {
					column = 0;
					*sImage << NEW_LINE;
				}
			}
			delete sBinImage;
			sBinImage = NULL;
		}
		
		*sImage << RTF_JPG_END;

	} else if (options->bPNG) { // salvar la imagen en formato PNG

		// initialize stream
		imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(),
			colorMap->getBits());
		imgStr->reset();
		
		// llevar la imagen a un formato de 24 bpp
		Guchar* pImage = new Guchar[width*height*3];
		Guchar** rows = new Guchar*[height];
		Guchar* pImg = pImage;

		// for each line...
		int start = (yFlip) ? height-1 : 0;
		int sign = (yFlip) ? -1 : 1;
		for (y = 0; y < height; ++y) 
		{
			rows[start + y*sign] = pImg;
			p = imgStr->getLine();
			for (x = 0; x < width; ++x) 
			{
				// esta parte podria optimizarse mas pero como hay que tener en cuenta 
				// los tipos de Color Spaces, bpp y Decode Arrays seria algo
				// complicado
				colorMap->getRGB(p, &rgb);
				*(pImg++) = Round(rgb.r*255);
				*(pImg++) = Round(rgb.g*255);
				*(pImg++) = Round(rgb.b*255);

				p += colorMap->getNumPixelComps();
			}
		}
		
		delete imgStr;

		// en los png solo se puede especificar el tamaño modificando la escala
		int wZoom = Round(fabs((rotate) ?ht :wt)/(width*0.75) * 100);
		int hZoom = Round(fabs((rotate) ?wt :ht)/(height*0.75) * 100);

		sImage = new GString();
		if (options->bBinary == gFalse)
			*sImage << RTF_PNG_HEADER(wZoom,hZoom);

		// convertirla a png
		CreatePNG( width, height, 24, rows, sImage, options->bBinary);

		if (options->bBinary == gTrue) {
			GString sHeader;
			sHeader << RTF_PNG_HEADER(wZoom,hZoom);
			sHeader << RTF_BIN(sImage->getLength());
			sImage->insert( 0, &sHeader);
		}

		delete[] pImage;
		delete[] rows;
		
		*sImage << RTF_PNG_END;
	} else {	//salvar la imagen en formato WMF (24 bpp)

		// initialize stream
		imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(),
			colorMap->getBits());
		imgStr->reset();
		
		// ancho en bytes de cada fila (tomando en cuenta el padding)
		int widthBPad4 = (width*3 + 3) & ~3;

		sImage = new GString((widthBPad4*height + 500)*2);
		*sImage << RTF_WMF_HEADER(width,height,widthBPad4,w1,h1,24,0);
		int dataStart = sImage->getLength();

		// reservar la memoria que ocupa una linea mas 2 por el cambio de linea
		GString* sLine = new GString((widthBPad4 + 2)*2);

		// for each line...
		for (y = 0; y < height; ++y) 
		{
			// write the line
			sLine->clear();
			
			p = imgStr->getLine();
			for (x = 0; x < width; ++x) 
			{
				colorMap->getRGB(p, &rgb);
				*sLine << RTF_WMF_COLOR(rgb);
				p += colorMap->getNumPixelComps();
			}
			
			// completar para que cada fila comienze en un multiplo de 4
			for (x = width*3; x < widthBPad4; ++x) 
				*sLine << "00";
			
			*sLine << NEW_LINE;
			
			if (yFlip) // al parecer los WMF se guardan invertidos verticalmente
				*sImage << sLine;
			else
				sImage->insert(dataStart, sLine);
		}
		
		delete imgStr;
		delete sLine;
		
		*sImage << RTF_WMF_END;
	}

  text->addImage(x0, y0, w1, h1, sImage);
}
