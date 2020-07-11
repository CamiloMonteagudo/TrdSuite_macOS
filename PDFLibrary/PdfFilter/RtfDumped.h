#ifndef RTFDUMPED_H
#define RTFDUMPED_H

#include "RtfElement.h"
#include "RtfHeaderFooter.h"

class RtfDumpedElem: public RtfBounds {
public:

	RtfDumpedElem(RtfElement *elem, GString *sA, RtfFont *fontA):RtfBounds() {
		updateBounds(elem);
		s = sA;
		font = fontA;
		firstChar = elem->getFirstChar();
		lastChar = elem->getLastChar();
	}

	~RtfDumpedElem() { if (s) delete s; }

	GString *s;
	RtfFont *font;
	char firstChar, lastChar;			// primer y ultimo caracter del elemento
};

class RtfDumpedPage: public RtfBounds {
public:
	RtfDumpedPage():RtfBounds() {	
		elems = new GList();
		firstChar = 0; 
		lastChar = 0;
		bContinueInNextPage = gFalse;
		bContinuedFromLastPage = gFalse;
	}

	~RtfDumpedPage() { deleteGList( elems, RtfDumpedElem); }

	void add( RtfDumpedElem *dumped) { elems->append( dumped); };

	void detectHeaders(RtfHeader *header, RtfFooter *footer);

	void calcBounds(RtfHeader *header, RtfFooter *footer);

	void finish(RtfHeader *header, RtfFooter *footer, RtfBounds *bodyBounds);

	void dump(GString *sRtf, RtfHeader *header, RtfFooter *footer, 
		RtfBounds *bounds, double pageHeight, RtfDumpedPage *lastPage);

	GList *elems;
	char firstChar, lastChar;			// primer y ultimo caracter de la pagina
	GBool bContinueInNextPage;		// indica si esta pagina puede continuar
																// en la proxima
	GBool bContinuedFromLastPage; // indica si esta pagina puede ser una 
																// continuacion de la anterior
};

#endif