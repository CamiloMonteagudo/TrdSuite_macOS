
#include "Rtf.h"

void getFirstSB(GString* s, char* &sbStart, char* &sbEnd);

void EncodeRtfChars(GString *s, char c) {
	switch (c) {
	case '\\': s->append("\\\\");  break;
	case '{': s->append("\\{");  break;
	case '}': s->append("\\}");  break;
	default: s->append( &c, 1);
	}
}

char* IntToHex8(int x) {
	static char HexTable[256][3];
	static GBool bInicializado = gFalse;
	char HexValues[] = "0123456789ABCDEF";
	
	if (!bInicializado)	{
		bInicializado = gTrue;
		
		for(int i=0; i<256; i++) {
			HexTable[i][0] = HexValues[i >> 4];
			HexTable[i][1] = HexValues[i & 15];
			HexTable[i][2] = 0;
		}
	}
	
	return HexTable[x & 255];
}

void getFirstSB(GString* s, char* &sbStart, char* &sbEnd)	{
	sbStart = strstr(s->getCString(), SB_VIRTUAL_START);
	sbEnd = strstr(s->getCString(), SB_VIRTUAL_END);
	}

GString* removeFirstSB(GString* s, long* delPos) {
	char *sbStart, *sbEnd;

	getFirstSB(s, sbStart, sbEnd);

	if (delPos)
		if (sbStart)
			*delPos = sbStart - s->getCString();
		else
			*delPos = -1;

	if (sbStart && sbEnd)
		{
		sbEnd += strlen(SB_VIRTUAL_END);
		s->del(sbStart - s->getCString(), sbEnd - sbStart);
		}

/*	char *sb = strstr(s->getCString(), "\\sb");
	if (sb) {
		char *sbEnd = sb+3;
		// avanzar hasta encontrar un \ o un espacio
		for(; *sbEnd != '\\' && *sbEnd != 32; sbEnd++);
		s->del(sb-s->getCString(), sbEnd-sb);
	}*/

	return s;
}

GString* adjustFirstSB(GString* s, double newValue) {
	long delPos;

	removeFirstSB(s, &delPos);

	if (delPos != -1)	{
		GString s1;
		s1 << RTF_SB_VIRTUAL(newValue);
		s->insert(delPos, &s1);
	}

	return s;
}

GString* replaceFirstTag(GString* s, char* tag, char* value) {
	char *sTagStart = strstr(s->getCString(), tag);

	//char *sbStart = strstr(s->getCString(), SB_VIRTUAL_START);
	//char *sbEnd = strstr(s->getCString(), SB_VIRTUAL_END);
	if (sTagStart) {
		int iTagStart = sTagStart - s->getCString();
		char *sTagEnd = sTagStart + strlen(tag);

		// avanzar hasta encontrar un \ o un espacio
		for(int valueLen=0;
			*sTagEnd != '\\' && *sTagEnd != 32;
			sTagEnd++,valueLen++);

		s->del( iTagStart, sTagEnd - sTagStart);
		s->insert( iTagStart, value);
	}

	return s;
}

GString SpaceBeforeWithNewLines(double points) {
	long twips = PointsToTwips(points);
	long nLines = twips / 275;	// fs24 = 275 twips
	long extra = twips % 275;
	GString s;

	if (twips == 0)
		return &s;

	s << SB_VIRTUAL_START;

	if (nLines > 0)	{
		s << "\\fs24"  << "\\sb" << extra;
		for(int i=0; i<nLines-1; i++) {
			s << RTF_LINE;
		}
		s << "\\par\\pard}";
	}
	else	// si no hay ninguna linea, el sb se pone fuera de las llaves
		// porque sino no hace efecto
		s << "}\\sb" << extra;

	s << SB_VIRTUAL_END;
	return &s;
}

