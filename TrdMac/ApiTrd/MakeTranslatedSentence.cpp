//////////////////////////////////////////////////////////////////////
// FindPhrasesInSentence.cpp: implementation of the CScanWordsInDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MakeTranslatedSentence.h"
#include "TranslateSentence.h"
#include "ExeWordAndTypeCmd.h"


POSITION CExeWordAndTypeCmd::m_arrWords[300];

/////////////////////////////////////////////////////////////////////////////
// Crea la oración traducida
//////////////////////////////////////////////////////////////////////

void CMakeTranslatedSentence::Process()
	{
	ASSERT(GetWordList().IsEmpty() == FALSE);
	GetPtrSentence()->m_Dest.Empty();
	BOOL first = FALSE;
	CStringA LastWord;
	int iEntreQue = GetPtrSentence()->m_EntreQue;

	if ( iEntreQue > 0)
		{
		COracion tmp(GetPtrSentence()->m_EntreParentesis,FMT_RTF);
			{
			CTranslateSentence objTranslateSentence(GetTranslationUserPtr(),&tmp);
			objTranslateSentence.Process();
			}
		GetPtrSentence()->m_Dest += (iEntreQue == 1)?"(":(iEntreQue == 2)?"\"":(iEntreQue == 3)?"\'":(iEntreQue == 4)?"[":"";
		GetPtrSentence()->m_Dest += tmp.m_Dest;
		GetPtrSentence()->m_Dest += (iEntreQue == 1)?")":(iEntreQue == 2)?"\"":(iEntreQue == 3)?"\'":(iEntreQue == 4)?"]":"";
		}
	
	for(POSITION nodo = GetHeadPos();  
		  nodo != NULL;)
		{
		ASSERT(nodo != NULL);
            
		//if(nodo != GetWordList().GetHeadPosition())
		//    m_Dest += ' ';
        
		LPCWORD w = GetWordAt(nodo);   // Palabra que se analiza 
		ASSERT(w != NULL);

		GetPtrSentence()->m_Dest += w->m_RtfFijoIni;
//		if(w->m_StrIniBlank.GetLength() > 1)
//			GetPtrSentence()->m_Dest += w->m_StrIniBlank;
		if (w->m_Delete == TRUE || w->m_Trad.IsEmpty() == TRUE)
			{
			GetPtrSentence()->m_Dest += w->m_RtfFijoFin;
			GetWordList().GetNext(nodo);
			continue;
			}
    

		if(first == FALSE)  // Borra artículo si la palabra no lo lleva al inicio de oración
			{  
			POSITION nodo2 = nodo;
			GetWordList().GetNext(nodo2);
			if(nodo2 != NULL)
				{
				LPCWORD wtmp = GetWordAt(nodo2);   // próxima palabra a la que se analiza 
				ASSERT(wtmp != NULL);
				if(wtmp->m_Articulo == 1 && wtmp->m_ArtAtFirst == TRUE) // si se le inserto artículo y no lo lleva al inicio de oración
					{
					w->m_Delete = TRUE;
					GetPtrSentence()->m_Dest += w->m_RtfFijoFin;
					GetWordList().GetNext(nodo);
					if(GetPtrSentence()->m_Dest.GetAt(GetPtrSentence()->m_Dest.GetLength()-1) != ' ')
						GetPtrSentence()->m_Dest += ' ';
					continue;
					}
				}
			} 

//#ifndef _DEBUG
		if(w->m_Trad == LastWord && LastWord.IsEmpty() == FALSE)
			{
			GetWordList().GetNext(nodo);
			continue;
			}
		else
			LastWord = w->m_Trad;
//#endif

		if(GetPtrSentence()->m_AllUpper == TRUE)      
			{
			w->m_Trad.MakeUpper();
      //_strupr( (LPSTR)(LPCSTR)w->m_Trad );
			}
		else
			{    
			if(first == FALSE)        
				{  
				if(GetPtrSentence()->m_FirstWordUpper == TRUE)
					FirstToUpper(w->m_Trad);
				first = TRUE;   
				}            
			if(GetPtrTargetLanguageData()->GetTypeWithFirstLetterInUpperCase().Find(w->m_sTipo) != -1 && (w->m_DiWord.Get() != DW_NOTRADUCIR || w->m_DiWord.Get() != DW_NOMBRE_PROPIO)&& w->m_Trad.Find(' ') == -1 && w->m_Trad.Find('-') == -1 && w->m_Key.IsEmpty() == FALSE)
				FirstToUpper(w->m_Trad);
			else if(w->m_Case == C_UPPER && w->m_Origlwr.GetLength()>1 && w->m_Origlwr.Find(" ") == -1)
        {
				w->m_Trad.MakeUpper();
        //_strupr( (LPSTR)(LPCSTR)w->m_Trad );
        }
			}    
		if(w->m_EntreComillas == TRUE)  // pone palabra entre comillas
			GetPtrSentence()->m_Dest+="\"";
		GetPtrSentence()->m_Dest += w->m_Trad;

		if(w->m_EntreComillas == TRUE)  
			GetPtrSentence()->m_Dest+="\"";

		// para el idioma español pone los signos de ¿¡ al inicio de la oración
		if(w->m_Trad == "?" && GetPtrTargetLanguageData()->GetIsoName() == LF"Es")
			GetPtrSentence()->m_Dest = "¿" + GetPtrSentence()->m_Dest;
		else if(w->m_Trad == "!" && GetPtrTargetLanguageData()->GetIsoName() == LF"Es")
			GetPtrSentence()->m_Dest = "¡" + GetPtrSentence()->m_Dest;
    
		GetPtrSentence()->m_Dest += w->m_RtfFijoFin;

		if ( w->m_EntreQue > 0)
			{
			COracion tmp(w->m_EntreParentesis,FMT_RTF);

			CTranslateSentence objTranslateSentence(GetTranslationUserPtr(),&tmp);
			objTranslateSentence.Process();

			GetPtrSentence()->m_Dest += (w->m_EntreQue == 1)?" (":(w->m_EntreQue == 2)?" \"":(w->m_EntreQue == 3)?" \'":(w->m_EntreQue == 4)?" [":"";
			GetPtrSentence()->m_Dest += tmp.m_Dest;
			GetPtrSentence()->m_Dest += (w->m_EntreQue == 1)?")":(w->m_EntreQue == 2)?"\"":(w->m_EntreQue == 3)?"\'":(w->m_EntreQue == 4)?"]":"";
			}

		GetWordList().GetNext(nodo);

		if(nodo != NULL)     // para saltar la próxima palabra borrada
			{   
			LPCWORD w1 = NULL;   
			do
				{
				w1 = GetWordAt(nodo);   // Palabra que se analiza 
				ASSERT(w1 != NULL);
				if(w1->m_Delete == TRUE)
					GetWordList().GetNext(nodo);
				}while(nodo != NULL && w1->m_Delete == TRUE);
			}            
    
		if(nodo != NULL)
			{
			LPCWORD w1 = GetWordAt(nodo);   // Palabra que se analiza 
			ASSERT(w1 != NULL);
    
			if(w1->m_Delete == TRUE)
				continue;
			if(GetPtrTargetLanguageData()->GetTypeWithSpaceBeforeWord().Find(w1->m_sTipo) != -1)
				continue;
			else if(GetPtrTargetLanguageData()->GetTypeWithSpaceAfterWord().Find(w->m_sTipo) != -1 
				 || GetPtrTargetLanguageData()->GetTypeWithSpaceAfterWord().Find(w1->m_sTipo) != -1)
				continue;
			else 
				{
				if(GetPtrSentence()->m_Dest.GetAt(GetPtrSentence()->m_Dest.GetLength()-1) != ' ')
					GetPtrSentence()->m_Dest += ' ';
				}
			}
		}
	}
	
//////////////////////////////////////////////////////////////////////
//  End of file
//////////////////////////////////////////////////////////////////////

