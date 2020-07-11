
#include "RtfElement.h"
#include "RtfOutPutDev.h"

//------------------------------------------------------------------------
// RtfElement
//------------------------------------------------------------------------

GBool RtfElement::intersect(RtfElement *elem) {
	GBool bImage1,bImage2;
	double delta;

	bImage1 = (type == T_BLOCK && ((RtfBlock*)this)->isImage());
	bImage2 = (elem->type == T_BLOCK && ((RtfBlock*)elem)->isImage());

	delta = MAX(maxFontSize, elem->maxFontSize);

	// los textos necesitan un delta mayor para superponerse, ya que sino
	// esto ocurriria muy frecuentemente
	if (bImage1 || bImage2)
		delta = delta * 0.5;
	else
		delta = delta * 1.5;

	return !(elem->xMin >= xMax-delta || elem->xMax <= xMin+delta 
				|| elem->yMin >= yMax-delta || elem->yMax <= yMin+delta);
}

//------------------------------------------------------------------------
// GElementList
//------------------------------------------------------------------------

GElementList::~GElementList() { 
	if (bOwnElements) {
		for(int i=0; i<getLength(); i++)
			delete get(i);
	}
}

