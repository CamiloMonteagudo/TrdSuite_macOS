
#include "RtfDumped.h"
#include "Rtf.h"

// actualizar los headers y footers
void RtfDumpedPage::detectHeaders(RtfHeader *header, RtfFooter *footer) {
	int listLen = elems->getLength();
	if (listLen == 0)
		return;

	// solo se analiza el primer y ultimo elemento
	// para verificar los headers y footers
	RtfDumpedElem *dumped = (RtfDumpedElem*)elems->get(0);
	header->match(dumped);
	dumped = (RtfDumpedElem*)elems->get(listLen-1);
	footer->match(dumped);
}

// actualizar los bounds de los elementos que 
// no son ni headers ni footers
void RtfDumpedPage::calcBounds(RtfHeader *header, RtfFooter *footer) {
	int listLen = elems->getLength();

	for(int j=0; j<listLen; j++) {
		RtfDumpedElem *dumped = (RtfDumpedElem*)elems->get(j);

		if (j == 0 && header->match(dumped))
			;
		else if (j == listLen-1 && footer->match(dumped))
			;
		else {
		// los bounds de la pagina se actualizan con los elementos que no son 
		// ni headers ni footers
			updateBounds(dumped);
		}
	}
}

void RtfDumpedPage::finish(RtfHeader *header, RtfFooter *footer, 
															 RtfBounds *bodyBounds) {
	int listLen = elems->getLength();
	RtfDumpedElem *firstDumped = NULL;
	RtfDumpedElem *lastDumped = NULL;

	for(int j=0; j<listLen; j++) {
		RtfDumpedElem *dumped = (RtfDumpedElem*)elems->get(j);

		if (j == 0 && header->match(dumped))
			;
		else if (j == listLen-1 && footer->match(dumped))
			;
		else {
			lastDumped = dumped;
			if (firstDumped == NULL)
				firstDumped = dumped;
		}
	}

	// guardar el primer y ultimo caracter de la pagina
	if (firstDumped) {
		firstChar = firstDumped->firstChar;

		double fontSize = 9.0;
		if (firstDumped->font)
			fontSize = MAX( fontSize, firstDumped->font->getSize());

		if (firstDumped->firstChar != 0 &&
				fabs(firstDumped->yMin - bodyBounds->yMin) <= fontSize*1.2)
			bContinuedFromLastPage = gTrue;
	}

	if (lastDumped) {
		lastChar = lastDumped->lastChar;

		double fontSize = 9.0;
		if (lastDumped->font)
			fontSize = MAX( fontSize, lastDumped->font->getSize());
		if (lastDumped->lastChar != 0 && lastDumped->lastChar != '.' &&
				bodyBounds->yMax - lastDumped->yMax <= fontSize*1.2)
			bContinueInNextPage = gTrue;
	}
}


void RtfDumpedPage::dump(GString *sRtf, RtfHeader *header, 
												 RtfFooter *footer, RtfBounds *bounds, 
												 double pageHeight, RtfDumpedPage *lastPage) {
	int j,jStart,jEnd;
	RtfDumpedElem *lastDumped = NULL;
	GBool bPendindSection = gFalse;
	GString *sPendingHeader = NULL;
	GString *sPendingFooter = NULL;

	jStart = 0;
	jEnd = elems->getLength()-1;

	// se pone una nueva seccion para que los headers y footers se actualizen
	// en cada pagina y se genera un cambio de pagina de ser necesario
	if (lastPage) {
		if (!lastPage->empty() && lastPage->yMax > pageHeight*0.70) {
			if (lastPage->bContinueInNextPage && bContinuedFromLastPage) {
				// como hay que unir el ultimo parrafo de la pagina anterior con el
				// primero de esta pagina, el cambio de seccion se inserta despues 
				// del primer parrafo
				bPendindSection = gTrue;

				// quitar el cambio de linea que debe existir en el parrafo anterior
				char* p = sRtf->getCString();
				char *p1 = p + MAX(sRtf->getLength() - 15, 0);
				GString *sTemp = new GString();
				*sTemp << RTF_NEW_PARAGRAPH;
				char *p2 = strstr( p1, sTemp->getCString());
				if (p2) {
					sRtf->del( p2-p, sTemp->getLength());
				}
				delete sTemp;
			} else
				*sRtf << RTF_SECT << RTF_NEW_PARAGRAPH;
		} else
			*sRtf << RTF_SECT_BREAK;
	}

	if (jEnd < 0)
		return;

	RtfDumpedElem *dumped = (RtfDumpedElem*)elems->get(0);

	// poner el header
	if (header->isValid()) {
		GString *sHeader = new GString();
		if (header->match(dumped)) {
			*sHeader << RTF_HEADER_START;
			*sHeader << removeFirstSB(dumped->s);
			*sHeader << RTF_HEADER_END;

			// recrear los tag de la fuente
			if (dumped->font)
				dumped->font->rtfOpen(sRtf);

			jStart++;
		} else {
			// poner un header vacio para eliminar el de la seccion anterior
			*sHeader << RTF_HEADER_START << RTF_HEADER_END;
		}

		// si aun no se ha puesto el cambio de seccion, salvar el header para
		// ponerlo despues
		if (bPendindSection) {
			sPendingHeader = sHeader;
			sHeader = NULL;
		} else {
			*sRtf << sHeader;
			delete sHeader;
		}
	}

	dumped = (RtfDumpedElem*)elems->get(jEnd);

	// poner el footer
	if (footer->isValid()) {
		GString *sFooter = new GString();
		if (footer->match(dumped)) {
			*sFooter << RTF_FOOTER_START;
//					tempFont = NULL;
			*sFooter << removeFirstSB(dumped->s);
			*sFooter << RTF_FOOTER_END;
			jEnd--;
		} else {
			// poner un footer vacio para eliminar el de la seccion anterior
			*sFooter << RTF_FOOTER_START << RTF_FOOTER_END;
		}

		// si aun no se ha puesto el cambio de seccion, salvar el footer para
		// ponerlo despues
		if (bPendindSection) {
			sPendingFooter = sFooter;
			sFooter = NULL;
		} else {
			*sRtf << sFooter;
			delete sFooter;
		}
	}

	for(j=jStart; j<=jEnd; j++) {
		RtfDumpedElem *dumped = (RtfDumpedElem*)elems->get(j);

		if (bPendindSection) {
			// eliminar los tags de inicio del parrafo (que estan antes 
			// del primer espacio)
			char *p = strchr( dumped->s->getCString(), ' ');
			if (p)
				dumped->s->del( 0, p - dumped->s->getCString());
		} else if (lastDumped == NULL) { // para el primer bloque
			// reemplazar el sb (space before) con el valor ajustado
			double sb = (dumped->yMin - bounds->yMin) - 2;
			if (sb < 0)	sb = 0;

			adjustFirstSB(dumped->s, sb);

			/*GString *value = new GString();
			// *value << RTF_SB(sb);
			*value << RTF_SB_VIRTUAL(sb);

			replaceFirstTag(dumped->s, "\\sb", value->getCString());

			delete value;*/
		}

		*sRtf << dumped->s;
		lastDumped = dumped;

		// poner la seccion pendiente despues de haber escrito el primer parrafo
		if (bPendindSection) {
			*sRtf << RTF_SECT;

			if (sPendingHeader) {
				*sRtf << sPendingHeader;
				delete sPendingHeader;
				sPendingHeader = NULL;
			}

			if (sPendingFooter) {
				*sRtf << sPendingFooter;
				delete sPendingFooter;
				sPendingFooter = NULL;
			}

			bPendindSection = gFalse;
		}
	} // ciclo para cada elemento

	// al terminar la pagina, cerrar los posibles bold y italic que puedan
	// estar abiertos
	if (lastDumped && lastDumped->font) 
		lastDumped->font->rtfClose(sRtf);

};
