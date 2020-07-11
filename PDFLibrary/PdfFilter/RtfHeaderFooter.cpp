
#include "RtfHeaderFooter.h"

//------------------------------------------------------------------------
// RtfHeader
//------------------------------------------------------------------------

#define DELTA_YMIN 10.0
#define MAX_HEADER_HEIGHT 15.0

RtfHeader::RtfHeader(void) {
	yMin = -1.0;
	total = 0;
	matches = 0;
	unmatches = 0;
	valid = gTrue;
}

// indica si elem se puede considerar un header
GBool RtfHeader::match(RtfBounds* elem) {
	if (valid == gFalse)
		return gFalse;

	total++;

	if (yMin < 0 && elem->getHeight() <= MAX_HEADER_HEIGHT) {
		matches++;
		return gTrue;
	}

	if (elem->yMin <= yMin + DELTA_YMIN) {
		if (elem->getHeight() > MAX_HEADER_HEIGHT) {
			unmatches++;
			return gFalse;
		}
		matches++;
		return gTrue;
	}

	return gFalse;
}

// actualiza las coordenadas del header con elem (atualizar yMin)
void RtfHeader::update(RtfBounds* elem) {
	if (yMin < 0)
		yMin = elem->yMin;
	else
		yMin = MIN(yMin, elem->yMin);
}

// analiza la cantidad de exitos (matches) que tuvo el header para
// determinar si puede considerarse un header valido
void RtfHeader::analyze() {
	if (matches < 4)
		valid = gFalse;		// deben haber al menos 4 exitos
	else if (unmatches*2.0 > matches)
		valid = gFalse;		// la cantidad de exitos debe ser mayor que el doble
											// de los fracasos
	else if (matches < total/2)
		valid = gFalse;		// existir al menos en la mitad de las paginas
}

//------------------------------------------------------------------------
// RtfFooter
//------------------------------------------------------------------------

#define DELTA_YMAX 10.0
#define MAX_FOOTER_HEIGHT 15.0

RtfFooter::RtfFooter(void) {
	yMax = -1.0;
	total = 0;
	matches = 0;
	unmatches = 0;
	valid = gTrue;
}

// indica si elem se puede considerar un footer
GBool RtfFooter::match(RtfBounds* elem) {
	if (valid == gFalse)
		return gFalse;

	total++;
	if (yMax < 0 && elem->getHeight() <= MAX_FOOTER_HEIGHT) {
		matches++;
		return gTrue;
	}

	if (elem->yMax >= yMax - DELTA_YMAX) {
		if (elem->getHeight() > MAX_FOOTER_HEIGHT) {
			unmatches++;
			return gFalse;
		}
		matches++;
		return gTrue;
	}

	return gFalse;
}

// actualiza las coordenadas del footer con elem (atualizar yMax)
void RtfFooter::update(RtfBounds* elem) {
	if (yMax < 0)
		yMax = elem->yMax;
	else
		yMax = MAX(yMax, elem->yMax);
}

// analiza la cantidad de exitos (matches) que tuvo el header para
// determinar si puede considerarse un header valido
void RtfFooter::analyze() {
	if (matches < 4)
		valid = gFalse;		// deben haber al menos 4 exitos
	else if (unmatches*2.0 > matches)
		valid = gFalse;		// la cantidad de exitos debe ser mayor que el doble
											// de los fracasos
	else if (matches < total/2)
		valid = gFalse;		// existir al menos en la mitad de las paginas
}
