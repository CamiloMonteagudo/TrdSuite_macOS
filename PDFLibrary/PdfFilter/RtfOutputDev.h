//========================================================================
//
// RtfOutputDev.h
//
// Copyright 1997 Derek B. Noonburg
//
// Changed 1999 by G.Ovtcharov
//========================================================================

#ifndef RTFOUTPUTDEV_H
#define RTFOUTPUTDEV_H

#include <aconf.h>
#include <stdio.h>
#include <math.h>
#include "gtypes.h"
#include "OutputDev.h"
#include "GfxFont.h"
#include "GfxState.h"
#include "GList.h"
#include "PDFDoc.h"
#include "ProcOptions.h"
#include "Rtf.h"
#include "RtfElement.h"
#include "RtfHeaderFooter.h"
#include "RtfDumped.h"

class UnicodeMap;

//------------------------------------------------------------------------
// GStringList
//------------------------------------------------------------------------

class GStringList:public GList {
public:

  ~GStringList();
  GString* getString(int i){ return (GString*)(get(i));}
  int getIndexOf(GString* str);
  int addString(GString* str,GBool bForceAdd=gFalse);
};

//------------------------------------------------------------------------
// RtfWord
//------------------------------------------------------------------------

class RtfWord: public RtfElement {
public:

  // Constructor.
  RtfWord(RtfElement *parentA, GfxState *state, int rotA, double x0, double y0,
	   int charPosA, RtfFont *fontA, double fontSize);
	// Constructor usado para las imagenes
  RtfWord(RtfElement *parentA, double x0, double y0, double w,double h, GString *imageA);

  // Destructor.
  ~RtfWord();

  // Add a character to the word.
  void addChar(GfxState *state, double x, double y,
	       double dx, double dy, Unicode u);

  // Merge <word> onto the end of <this>.
  void merge(RtfWord *word);

  // Compares <this> to <word>, returning -1 (<), 0 (=), or +1 (>),
  // based on a primary-axis comparison, e.g., x ordering if rot=0.
  int primaryCmp(RtfWord *word);

  // Return the distance along the primary axis between <this> and
  // <word>.
  double primaryDelta(RtfWord *word);

  static int cmpYX(const void *p1, const void *p2);

  int getLength() { return len; }
  Unicode getChar(int idx) { return text[idx]; }
  GString *getText();
  GString *getFontName() { return font->fontName; }

private:

  int rot;							// rotation, multiple of 90 degrees
												//   (0, 1, 2, or 3)
  double base;					// baseline x or y coordinate
  Unicode *text;				// the text
  double *edge;					// "near" edge x or y coord of each char
												//   (plus one extra entry for the last char)
  int len;							// length of text and edge arrays
  int size;							// size of text and edge arrays
  int charPos;          // character position (within content stream)
  int charLen;          // number of content stream characters in
                        //   this word
  RtfFont *font;				// font information
  GBool spaceAfter;			// set if there is a space between this
												//   word and the next word on the line
	GBool tabBefore;			// indica si hay que poner un tab antes
												// de esta palabra
	GString *image;				// 

  RtfWord *next;				// next word in line

  friend class RtfPool;
  friend class RtfLine;
  friend class RtfBlock;
  friend class RtfRow;
  friend class RtfPage;
};

//------------------------------------------------------------------------
// RtfPool
//------------------------------------------------------------------------

class RtfPool {
public:

  RtfPool();
  ~RtfPool();

  RtfWord *getPool(int baseIdx) { return pool[baseIdx - minBaseIdx]; }
  void setPool(int baseIdx, RtfWord *p) { pool[baseIdx - minBaseIdx] = p; }

  int getBaseIdx(double base);

  void addWord(RtfWord *word);

private:

  int minBaseIdx;				// min baseline bucket index
  int maxBaseIdx;				// max baseline bucket index
  RtfWord **pool;				// array of linked lists, one for each
												//   baseline value (multiple of 4 pts)
  RtfWord *cursor;			// pointer to last-accessed word
  int cursorBaseIdx;		// baseline bucket index of last-accessed word

  friend class RtfBlock;
  friend class RtfPage;
};

//------------------------------------------------------------------------
// RtfLine
//------------------------------------------------------------------------

class RtfLine: public RtfElement {
public:

  RtfLine(RtfElement *parentA, int rotA, double baseA);
  ~RtfLine();

  void addWord(RtfWord *word);

  // Return the distance along the primary axis between <this> and
  // <line>.
  double primaryDelta(RtfLine *line);

  // Compares <this> to <line>, returning -1 (<), 0 (=), or +1 (>),
  // based on a primary-axis comparison, e.g., x ordering if rot=0.
  int primaryCmp(RtfLine *line);

  // Compares <this> to <line>, returning -1 (<), 0 (=), or +1 (>),
  // based on a secondary-axis comparison of the baselines, e.g., y
  // ordering if rot=0.
  int secondaryCmp(RtfLine *line);

  int cmpYX(RtfLine *line);

  static int cmpXY(const void *p1, const void *p2);

  void coalesce(UnicodeMap *uMap);

  void dump(GString *sOutput, RtfContext *context, RtfBounds *bounds);

	void DebugDump(int nestLevel) {
		printNestLevel(nestLevel);
		printf("line: x=%.2f..%.2f y=%.2f..%.2f %s\n",
			xMin, xMax, yMin, yMax, rtext->getCString());
	};

private:

  int rot;								// text rotation
  double base;						// baseline x or y coordinate
  RtfWord *words;					// words in this line
  RtfWord *lastWord;			// last word in this line
  Unicode *text;					// Unicode text of the line, including
													//   spaces between words
  double *edge;						// "near" edge x or y coord of each char
													//   (plus one extra entry for the last char)
  int len;								// number of Unicode chars
  GBool hyphenated;				// set if last char is a hyphen
  RtfLine *next;					// next line in block
  GString *rtext;         // texto de la linea en Rtf
	long nWords;						// cantidad de palabras

  friend class RtfBlock;
  friend class RtfRow;
  friend class RtfPage;
};

//------------------------------------------------------------------------
// RtfBlock
//------------------------------------------------------------------------

class RtfBlock: public RtfElement {
public:

  RtfBlock(RtfElement *parentA, int rotA);
  ~RtfBlock();

  void addWord(RtfWord *word);

  void coalesce(UnicodeMap *uMap);

	virtual char getFirstChar() { return firstChar; };
	virtual char getLastChar() { return lastChar; };

  static int cmpYXPrimaryRot(const void *p1, const void *p2);

  int primaryCmp(RtfBlock *blk);

  double secondaryDelta(RtfBlock *blk);

	// indica si el parrafo podria dividir sus palabras con un "-" al final
	// de sus lineas
	int isHyphenable();

	// indica si la primera linea del parrafo esta indentada
	GBool isFirstLineIndented();

	// obtiene los margenes del parrafo
  double getLeftMargin(double relativeX);
  double getFirstLineLeftMargin(double relativeX);
  double getRightMargin();

  // recalcula los valores de xMin,xMax,yMin,yMax en base a las lineas del bloque
  void recalculateBounds();

	// indica si el contenido del parrafo es una imagen
	GBool isImage();

	// indica si el bloque es un fragmento de codigo
	GBool isCodeSample();

	// indica si el bloque es una enumarecion por empezar con el mismo
	// caracter
	GBool isEnumByFirstChar(char &cFirstBlockChar);

	// inserta un bloque al principio de este
	void insertBlock(RtfBlock *blk);

	// divide el bloque a partir de una linea especifica
	RtfBlock* splitByLine(int index);

	// dividir el bloque por las lineas que empiezen con el caracter c
	// y tengan un posicionamiento propio de las enumeraciones
	void splitByEnumChar(char c);

	// divide todas las lineas del bloque en bloques independientes
	void splitAllLines();

  // divide el bloque
  void splitBlock();

  // genera el texto RTF del bloque y actualiza curFont y yLastMax
	void dump(GString *sOutput, RtfContext *context, RtfBounds *bounds);

	void DebugDump(int nestLevel) {
		printNestLevel(nestLevel);
		if (isImage())
			printf("block: x=%.2f..%.2f y=%.2f..%.2f image\n",
				xMin, xMax, yMin, yMax);
		else {
			printf("block: x=%.2f..%.2f y=%.2f..%.2f lines=%d fontSize=%.2f..%.2f\n",
				xMin, xMax, yMin, yMax, nLines, minFontSize, maxFontSize);
			for (RtfLine *line = lines; line; line = line->next)
				line->DebugDump(nestLevel+1);
		}
	}

private:

	// estas dos solo son validas antes de llamar al coalesce
	int nChars;							// cantidad de caracteres del bloque
	double averageFontSize;	// promedio de los tamaños de fuentes de las palabras

  int rot;								// text rotation
  RtfPool *pool;					// pool of words (used only until lines are built)
  RtfLine *lines;					// linked list of lines
  RtfLine *curLine;				// most recently added line
  int nLines;							// number of lines
  int charCount;					// number of characters in the block
  RtfBlock *next;
	char firstChar,lastChar;	// primer y ultimo caracter del bloque, validos
														// solo despues de llamar al dump()

  friend class RtfLine;
  friend class RtfRow;
	friend class RtfTable;
  friend class RtfPage;
};

//------------------------------------------------------------------------
// RtfCell
//------------------------------------------------------------------------

class RtfCell: public RtfElement {
public:

  RtfCell(RtfElement *parentA);
  ~RtfCell();

	GBool accept(RtfElement *elem) {
		return (elem->xMin < xMax);
	};

  void add(RtfElement *elem);

	RtfElement *getElem(int i) {return elements->get(i); };

	void coalesce(void);

  void dump(GString *sOutput, RtfContext *context, RtfBounds *bounds);

	void DebugDump(int nestLevel) {
		printNestLevel(nestLevel);
		printf("cell: x=%.2f..%.2f y=%.2f..%.2f elements=%d\n",
			xMin, xMax, yMin, yMax, elements->getLength());

		for(int j=0; j<elements->getLength(); j++) {
			RtfElement *elem = (RtfElement *)elements->get(j);
			elem->DebugDump(nestLevel+1);
		}
	}

public:
	GBool plain;							// indica si esta celda se pone sin tabla

private:
	GElementList* elements;		// bloques o subtablas que tiene esta celda

	friend class RtfRow;
	friend class RtfTable;
};

//------------------------------------------------------------------------
// RtfColumn
//------------------------------------------------------------------------

class RtfColumn: public RtfElement {
public:
	RtfColumn(RtfElement *parentA):RtfElement(T_COLUMN,(RtfElement *)parentA) {};

	void DebugDump(int nestLevel) {
		printNestLevel(nestLevel);
		printf("col: x=%.2f..%.2f y=%.2f..%.2f\n",
			xMin, xMax, yMin, yMax);
	}
};

//------------------------------------------------------------------------
// RtfRow
//------------------------------------------------------------------------

class RtfRow: public RtfElement {
public:

  RtfRow(RtfElement *parentA);
  ~RtfRow();

	GBool accept(RtfElement *elem) {
		return (xMin > xMax || elem->yMin < yMax - 2.0); //xxxx
	};

	void add(RtfElement *elem);

	RtfRow* coalesce(void);

	void alignCols(GElementList* cols1);

	void alignBorders(GElementList* cols1);

	GBool canJoinColumns(void);

	void joinColumns(void);

	RtfRow* detectEnumerations(void);

  void dump(GString *sOutput, RtfContext *context, RtfBounds *bounds);

	void DebugDump(int nestLevel) {
		printNestLevel(nestLevel);
		printf("row: x=%.2f..%.2f y=%.2f..%.2f cells=%d\n",
			xMin, xMax, yMin, yMax, cells->getLength());

		for(int j=0; j<cols->getLength(); j++) {
			RtfColumn *col = (RtfColumn *)cols->get(j);
			col->DebugDump(nestLevel+1);
		}

		for(int j=0; j<cells->getLength(); j++) {
			RtfCell *cell = (RtfCell *)cells->get(j);
			cell->DebugDump(nestLevel+1);
		}
	}

private:
	GElementList *elems;								// elementos que contiene esta fila
	GElementList *cells;								// celdas que contiene esta fila
	GElementList *cols;									// columnas

friend class RtfTable;
};

//------------------------------------------------------------------------
// RtfTable
//------------------------------------------------------------------------

class RtfTable: public RtfElement {

public:

  RtfTable(RtfElement *parentA);
  ~RtfTable();

	// el bloque se acepta si esta verticalmente dentro o cerca de la tabla
	GBool accept(RtfElement *blk) {
		return (blk->yMin <= yMax + 6.0);
	};

  void add(RtfElement *blk);

	// copia filas de otra tabla (los bloques se matienen en la tabla original)
	void copyRows(RtfTable* table, int start, int end);

	// intenta unir las columnas de la tabla
	void joinColumns();

	RtfTable* coalesce();

	// alinear columnas y bordes
	void alignCols();
	void alignBorders();

	// intenta unir la tabla con la actual
	GBool join(RtfTable *table);

	// aumentar el ancho de la ultima columna para evitar el wrapping
	void expandLastCol(void);

  void dump(GString *sOutput, RtfContext *context, RtfBounds *bounds);

	void DebugDump(int nestLevel) {
		int j;
		printNestLevel(nestLevel);
		printf("table: x=%.2f..%.2f y=%.2f..%.2f rows=%d absBlocks=%d\n",
			xMin, xMax, yMin, yMax, rows->getLength(), absBlocks->getLength());
		
		if (absBlocks->getLength() > 0) {
			printNestLevel(nestLevel+1);
			printf("absBlocks:\n");
		}
		for(j=0; j<absBlocks->getLength(); j++) {
			RtfBlock *blk = (RtfBlock *)absBlocks->get(j);
			blk->DebugDump(nestLevel+2);
		}

		for(j=0; j<rows->getLength(); j++) {
			RtfRow *row = (RtfRow *)rows->get(j);
			row->DebugDump(nestLevel+1);
		}
	}

	int nestLevel;

private:
	GElementList* blocks;							// listado de bloques antes de hacer el coalesce

	GElementList* rows;								// filas
	GElementList* absBlocks;					// bloques de posicionamiento absoluto
	GBool bFromCopy;

friend class RtfPage;
friend class RtfRow;
};

//------------------------------------------------------------------------
// RtfPage
//------------------------------------------------------------------------

class RtfOutputDev;

class RtfPage: public RtfElement {
public:

  // Constructor.
  RtfPage(RtfOutputDev *outputDev);

  // Destructor.
  ~RtfPage();

  // Start a new page.
  void startPage(GfxState *state);

  // End the current page.
  void endPage();

  // Update the current font.
  void updateFont(GfxState *state);

  // Begin a new word.
  void beginWord(GfxState *state, double x0, double y0);

  // Add a character to the current word.
  void addChar(GfxState *state, double x, double y,
	       double dx, double dy,
	       CharCode c, Unicode *u, int uLen);

  // End the current word, sorting it into the list of words.
  void endWord();

  // Add a word, sorting it into the list of words.
  void addWord(RtfWord *word);

  // Coalesce strings that look like parts of the same line.
  void coalesce();

  // Dump contents of page to a file.
  void dump();

	void detectHeaders(RtfHeader *header, RtfFooter *footer);

	void writeRtf(GString *sRtf);

  // Escribir las definiciones de las fuentes
  void WriteFonts(GString *sOutput);

  // Escribir las definiciones de los colores
  void WriteColorTable(GString *sOutput);

  void addImage(double xt,double yt,double w1,double h1,GString *sImage);

	void setMargins(double marginLeftA, double marginRightA);

	RtfBounds *getBodyBounds();

private:

  void clear();

  double pageWidth, pageHeight;	// width and height of current page
  RtfWord *curWord;					// currently active string
  int charPos;							// next character position (within content
														//   stream)
  RtfFont *curFont;					// current font
  double curFontSize;				// current font size
  int nest;									// current nesting level (for Type 3 fonts)
  int nTinyChars;						// number of "tiny" chars seen so far
  GBool lastCharOverlap;		// set if the last added char overlapped the
														//   previous char

  RtfPool *pools[4];				// a "pool" of RtfWords for each rotation
  RtfBlock **blocks;				// array of blocks, in yx order
  int nBlocks;							// number of blocks
  int nImages;							// numero de imagenes
	int imagesSize;						// tamaño de todas las imagenes unidas
	GElementList *elements;		// lista final de los elementos de la pagina

  int primaryRot;						// primary rotation
  GBool primaryLR;					// primary direction (true means L-to-R,
														//   false means R-to-L)
  RtfWord *rawWords;				// list of words, in raw order (only if
														//   rawOrder is set)
  RtfWord *rawLastWord;			// last word on rawWords list

  GList *fonts;							// all font info objects used on this
														//   page [RtfFont]
  GStringList *rtfFonts;    // listado de strings con los nombres de fuentes
                            // usados
  GStringList *rtfColors;   // listado de los colores usados

	double marginLeft,marginRight;		// margenes de las paginas

	RtfContext *context;

	RtfBounds bodyBounds;			// rectangulo que encierra el texto en todas
														// las paginas

	GList *dumpedPages;

	RtfOutputDev *outputDev;

  friend class RtfLine;
  friend class RtfBlock;
};

//------------------------------------------------------------------------
// RtfOutputDev
//------------------------------------------------------------------------

class RtfOutputDev: public OutputDev {
public:

  RtfOutputDev(GString* sRtfA, PDFDoc *docA, ProcOptions *optionsA);

  // Destructor.
  virtual ~RtfOutputDev();

  // Check if file was successfully created.
  virtual GBool isOk() { return ok; }

  //---- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() { return gTrue; }

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() { return gTrue; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() { return gFalse; }

  // Does this device need non-text content?
  virtual GBool needNonText() { return gFalse; }

  //----- initialization and control

  // Start a page.
  virtual void startPage(int pageNum, GfxState *state);

  // End a page.
  virtual void endPage();

  //----- update text state
  virtual void updateFont(GfxState *state);

  //----- text drawing
  virtual void beginString(GfxState *state, GString *s);
  virtual void endString(GfxState *state);
  virtual void drawChar(GfxState *state, double x, double y,
			double dx, double dy,
			double originX, double originY,
			CharCode c, Unicode *u, int uLen);

  void WriteHeader();

  //----- image drawing
  virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
			     int width, int height, GBool invert,
			     GBool inlineImg);

  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
			 int width, int height, GfxImageColorMap *colorMap,
			 int *maskColors, GBool inlineImg);

public:
	GBool bHeaderFooter;
	RtfHeader *header;
	RtfFooter *footer;

private:
  GString* sRtf;			// string que guarda el contenido del rtf

  RtfPage *text;			// text for the current page
  GBool ok;						// set up ok?
  PDFDoc *doc;
	ProcOptions *options;
};

#endif
