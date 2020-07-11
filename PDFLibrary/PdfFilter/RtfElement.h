#ifndef RTFELEMENT_H
#define RTFELEMENT_H

#include <math.h>
#include "RtfFont.h"
#include "GList.h"
#include "UnicodeMap.h"
#include "UnicodeTypeTable.h"
#include "GlobalParams.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

typedef enum {
	T_ELEMENT,
	T_WORD,
	T_LINE,
	T_BLOCK,
	T_CELL,
	T_ROW,
	T_COLUMN,
	T_TABLE,
	T_PAGE
} ElementType;

//------------------------------------------------------------------------
// RtfBounds
//------------------------------------------------------------------------

class RtfBounds {
public:
	RtfBounds() { resetBounds(); }

	virtual void resetBounds() {
		xMin = yMin = 1; xMax = yMax = -1;
	}

	GBool empty(void) { return xMin > xMax; };

	void updateBounds(RtfBounds *bound) {
		if (bound->empty())
			return;

		if (empty()) {
			xMin = bound->xMin;
			xMax = bound->xMax;
			yMin = bound->yMin;
			yMax = bound->yMax;
		} else {
			xMin = MIN(xMin, bound->xMin);
			xMax = MAX(xMax, bound->xMax);
			yMin = MIN(yMin, bound->yMin);
			yMax = MAX(yMax, bound->yMax);
		}
	}

  double xMin, xMax;		// bounding box x coordinates
  double yMin, yMax;		// bounding box y coordinates
	
	double getWidth() { return xMax - xMin; }
	double getHeight() { return yMax - yMin; }

};

//------------------------------------------------------------------------
// RtfContext
//------------------------------------------------------------------------

enum RtfAlign {rtfLeft,rtfRight,rtfCenter,rtfJustified};

class RtfContext {
public:
	RtfContext() { 
		font = NULL;
		uMap = NULL;
		align = rtfLeft;
		xMin = 1;
		xMax = -1;
		minRightMargin = 0;
		hyphenatedBlocks = 0;
		lastRowIsPlain = gFalse;
	}
	~RtfContext() {};

	GBool startPage() {
		font = NULL;
		// get the output encoding
		uMap = globalParams->getTextEncoding();
		return (uMap != NULL);
  }

	void endPage() { uMap->decRefCnt(); };

	void setBoundsX(double xMinA, double xMaxA, double marginRight) {
		xMin = xMinA;
		xMax = xMaxA;
		minRightMargin = marginRight - xMax;
	};

public:
	RtfFont *font;
	UnicodeMap *uMap;
	RtfAlign align;			// alineacion del ultimo bloque dumpeado
	double xMin,xMax;		// valores minimos y maximos de todo lo que contienen
											// las paginas, se va actualizando	pagina a pagina
	double minRightMargin;	// valor minimo del margen derecho

	GBool lastRowIsPlain;
	long hyphenatedBlocks;
};

//------------------------------------------------------------------------
// RtfElement
//------------------------------------------------------------------------

class RtfElement:public RtfBounds {
public:
	RtfElement(ElementType typeA, RtfElement *parentA) {
		type = typeA; resetBounds();
		parent = parentA;
	}

	virtual ~RtfElement() {	}

	virtual void resetBounds() {
		RtfBounds::resetBounds();
		minFontSize = 0;
		maxFontSize = 0;
	}

	GBool horizontalMatch(RtfElement *elem) {
		if (fabs(yMin - elem->yMin) < 3.0 && yMin < elem->yMax &&
				fabs(yMax - elem->yMax) < 3.0 && yMax > elem->yMin)
			return gTrue;
		else
			return gFalse;
	}

  virtual void dump(GString *sOutput, RtfContext *context,
		RtfBounds *bounds) {
		if (parent) parent->curY = yMax;			// actualizar el curY del parent
	}

	void updateBounds(RtfElement *elem) {
		RtfBounds::updateBounds(elem);

		if (minFontSize == 0)
			minFontSize = elem->minFontSize;
		else if (elem->minFontSize != 0)
			minFontSize = MIN(minFontSize, elem->minFontSize);

		maxFontSize = MAX(maxFontSize, elem->maxFontSize);
		curY = yMin;
	}

	static int cmpXY(const void *p1, const void *p2) {
		RtfElement *elem1 = *(RtfElement **)p1;
		RtfElement *elem2 = *(RtfElement **)p2;
		double cmp;
		
    cmp = elem1->xMin - elem2->xMin;
		if (cmp == 0)
			cmp = elem1->yMin - elem2->yMin;

		return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
	}

	static int cmpYX(const void *p1, const void *p2) {
		RtfElement *elem1 = *(RtfElement **)p1;
		RtfElement *elem2 = *(RtfElement **)p2;
		double cmp;
		
		cmp = elem1->yMin - elem2->yMin;
		if (cmp == 0)
			cmp = elem1->xMin - elem2->xMin;

		return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
	}

	virtual GBool intersect(RtfElement *elem);

	void printNestLevel(int nestLevel) {
		for(int i=0; i<nestLevel; i++)
			printf("  ");
	}

	virtual void DebugDump(int nestLevel) {
		printNestLevel(nestLevel);
		printf("element: x=%.2f..%.2f y=%.2f..%.2f\n",
			xMin, xMax, yMin, yMax);
	}

	virtual void setParent(RtfElement *elem) {
		parent = elem;
		RtfElement *parent1 = parent;
		while (parent1 && parent1->type != T_PAGE) {
			parent1->updateBounds(this);
			parent1 = parent1->parent;
		}
	};

	// devuelve el padre mas cercano de tipo Atype
	RtfElement *getParentOfType(ElementType Atype) {
    RtfElement *temp;
		for(temp = parent; temp && temp->type != Atype;	temp = temp->parent );
		return temp;
	}

	virtual char getFirstChar() { return 0; };
	virtual char getLastChar() { return 0; };

	ElementType type;
	double curY;					// posicion actual en Y (usado durante el dump)
	double minFontSize;		// minimo tamaño de fuente que tiene este elemento
	double maxFontSize;		// maximo tamaño de fuente que tiene este elemento
	RtfElement *parent;		// objeto que contiene a este
};

//------------------------------------------------------------------------
// GElemList
//------------------------------------------------------------------------

class GElementList:public GList {
public:
	GElementList(GBool bOwnElementsA = gTrue) { bOwnElements = bOwnElementsA;	};
  ~GElementList();

  RtfElement* get(int i) { return (RtfElement*)(GList::get(i)); }

private:
	GBool bOwnElements;
};

#endif