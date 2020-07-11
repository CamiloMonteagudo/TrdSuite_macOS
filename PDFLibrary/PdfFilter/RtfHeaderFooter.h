#ifndef RTFHEADERFOOTER_H
#define RTFHEADERFOOTER_H

#include "RtfElement.h"

//------------------------------------------------------------------------
// RtfHeader
//------------------------------------------------------------------------

class RtfHeader {
public:
	RtfHeader(void);
	GBool isValid(void) { return valid;};
	GBool match(RtfBounds* elem);
	void update(RtfBounds* elem);
	void analyze();

private:
	double yMin;
	int total;					// cantidad total de paginas
	int matches;				// cantidad de elementos que machean
	int unmatches;			// cantidad de elementos que no machean
	GBool valid;				// indica si el header es valido
};

//------------------------------------------------------------------------
// RtfFooter
//------------------------------------------------------------------------

class RtfFooter {
public:
	RtfFooter(void);
	GBool isValid(void) { return valid;};
	GBool match(RtfBounds* elem);
	void update(RtfBounds* elem);
	void analyze();

private:
	double yMax;
	int total;					// cantidad total de paginas
	int matches;				// cantidad de elementos que machean
	int unmatches;			// cantidad de elementos que no machean
	GBool valid;				// indica si el footer es valido
};

#endif