////////////////////////////////////////////////////////////////////////////
//   Class COracion                                                       //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa class para el trabajo con la oración                    // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <ctype.h>
#include "UtilFun.h"
#include "ApiConj.h"
#include "COra.h"
#include "LengDic.h"
#include "PeekMsg.h"
#include "ViewFlag.h"
#include "Translation.h"
#include "ExpandCmdPhrases.h"
#include "EmptyWordsInSentence.h"

                       
IMPLEMENT_SERIAL(COracion,CObject,0);

////////////////////////////////////////////////////////////////////////////
//  Destructor de la oración
////////////////////////////////////////////////////////////////////////////

COracion::~COracion()
  {
	CEmptyWordsInSentence objEmptyWordsInSentence(this);

	objEmptyWordsInSentence.Process();
  m_WordsInSentence.RemoveAll();
  m_WordsTypeInSentence.RemoveAll();
  }

/////////////////////////////////////////////////////////////////////////////

#ifdef ACTIVEVIEW
const char *StringArrayStepInTranslation[]=
	{
	"stCaseSentenceProperty",
	"stTypeWordByWildCard",
	"stGetTypeOfWord",
	"stFindWords",
	"stFindPhrases",
	"stFindProperNoun",
	"stTranslationWildCard",
	"stFindWordType",
	"stTranslateWords",
	"stTranslateBe",
	"stCmdCoorCnj",
	"stFindMood",
	"stFindInterchangePattern",
	"stInsertArticle",
	"stFillInsertWords",
	"stGenderAndNumber",
	"stTranslatePreffix",
	"stConjugate",
	"stEnding",
	"stTranslateReflexiveVerbs",
	"stMakeTranslatedSentence",
	"stDetails"
	};

void COracion::View(int iStep)
	{
	CStringA sText;
	
	for(POSITION pos = m_Ora.GetHeadPosition(); pos != NULL; m_Ora.GetNext(pos))
		{
		if(pos != NULL)
			{
			LPCWORD w = (LPCWORD)m_Ora.GetAt(pos);
			if (w)
				{
				sText += "<"
					+ w->m_Key
					+ '|' + w->m_Orig
					+ '|' + w->m_Trad
					+ '|' + w->m_sTipo
					+ '|' + w->m_Data
					+ "> ";
				}
			ASSERT(w != NULL);
			}
		}
	
	if ((unsigned)iStep >= COUNT_OF(StringArrayStepInTranslation))
		TRACE("Step %d = %s", iStep, CS2SZ(sText))
	else
		TRACE("Step %s = %s", StringArrayStepInTranslation[iStep], CS2SZ(sText));
	//TRACE("    Dest = %s", m_Dest.c_str());
	}
#endif

/////////////////////////////////////////////////////////////////////////////

BOOL COracion::PoneXX()
    {
    CWord *Wini = new CWord("XXXX");
    CWord *Wfin = new CWord("XXXX");
             
    if(Wini == NULL || Wfin == NULL)
        {
#if !defined(MERCADO)
        AfxMessageBox(IDS_NOT_MEMORY,MB_OK|MB_ICONSTOP);
#endif
        return FALSE;                    
        }
     else
        {                   
		Wini->m_Origlwr = Wfin->m_Origlwr = Wini->m_Key = Wfin->m_Key = "xxxx";
        Wini->m_sTipo = Wfin->m_sTipo = "XX";
        Wini->m_sTipo = Wfin->m_sTipo = "XX";
		Wini->m_Buscada = TRUE;
		Wfin->m_Buscada = TRUE;
		Wini->m_Traducida = TRUE;
		Wfin->m_Traducida = TRUE;
		Wini->m_NoBusca = TRUE;
		Wfin->m_NoBusca = TRUE;
        m_Ora.AddHead(Wini);
        m_Ora.AddTail(Wfin);
        }   

    ASSERT(Wini != NULL);                              
    ASSERT(Wfin != NULL);
    return TRUE;
    }

/////////////////////////////////////////////////////////////////////////////

void COracion::QuitaXX()
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);

    POSITION nodo = m_Ora.GetHeadPosition(); 
	CWord *w  = (CWord *)m_Ora.GetAt(nodo);
	ASSERT(w != NULL);
	if(w->m_sTipo == "XX" && (w->m_Orig == "XXXX" || w->m_Origlwr == "xxxx"))
		delete m_Ora.RemoveHead();
	nodo = m_Ora.GetTailPosition(); 
	w  = (CWord *)m_Ora.GetAt(nodo);
	ASSERT(w != NULL);
	if(w->m_sTipo == "XX"&& (w->m_Orig == "XXXX"|| w->m_Origlwr == "xxxx"))
	    delete m_Ora.RemoveTail();
    }

/////////////////////////////////////////////////////////////////////////////

void COracion::ExpandeFrases()
    {
	CExpandCmdPhrases objExpandCmdPhrases(this);

	objExpandCmdPhrases.Process();
    }

/////////////////////////////////////////////////////////////////////////////
// Borra los artículos que no son necesarios
/////////////////////////////////////////////////////////////////////////////

void COracion::DeleteArticulo()     
    {                        
/*******
    BOOL first = FALSE;
    ASSERT(m_Ora.IsEmpty() == FALSE);
    
    for(POSITION nodo = m_Ora.GetHeadPosition();  
          nodo != NULL; 
          m_Ora.GetNext(nodo))
        {  
        POSITION nodo2 = nodo;
        ASSERT(nodo != NULL);
        
        m_Ora.GetNext(nodo2);
        CWord *w = (CWord *)m_Ora.GetAt(nodo);   // Palabra que se analiza 
        ASSERT(w != NULL);    
        
        if(w->m_sTipo == "FS" && w->m_sTipo2.IsEmpty() == false)
            w->m_sTipo = w->m_sTipo2;
        
        // pone mayúsculas la palabra insertada si próxima es mayúsculas
        
        //if(Translate->m_Idioma->TipoArt.Find(w->m_sTipo) != -1)     
        //    {
        //    if(nodo2 != NULL)
        //        {
        //        CWord *w2 = (CWord *)m_Ora.GetAt(nodo2);   // Palabra próxima a la que se analiza 
        //        ASSERT(w2 != NULL);
        //        if(w2->m_Articulo != 0 && w2->m_Case == C_UPPER)
        //            ToUpper(w->m_Trad);
        //        }
        //    }   

        
        if(nodo2 != NULL)
            {
            CWord *w2 = (CWord *)m_Ora.GetAt(nodo2);   // Palabra próxima a la que se analiza 
            ASSERT(w2 != NULL);


            if(first == FALSE)
                {
                if(w2->m_Articulo == 1 && w2->m_ArtAtFirst == TRUE && Translate->m_Idioma->TipoArt.Find(w->m_sTipo) != -1) // si se le inserto artículo y no lo lleva al inicio de oración y la primera palabra es artículo
                    w->m_Delete = TRUE;
                first = TRUE;   
                }   
                
            if(Translate->m_Idioma->TipoArt.Find(w2->m_sTipo) != -1 && Translate->m_Idioma->TipoPreposicion.Find(w->m_sTipo) != -1) 
                {                                    
                m_Ora.GetNext(nodo2);
                if(nodo2 != NULL)
                    {
                    CWord *w3 = (CWord *)m_Ora.GetAt(nodo2);   // Palabra próxima a la que se analiza 
                    ASSERT(w3 != NULL);                           
                    if(w3->m_Articulo == 1 && w3->m_LastPrepNoArt == TRUE)
                        w2->m_Delete = TRUE;
                    }
                } 
                
                
            }
        if(w->m_Prep.IsEmpty() == FALSE)
            {   
            CWord *wl1 = GetXNode(nodo,1,FALSE);
            CWord *wl2 = GetXNode(nodo,2,FALSE);

            if(wl2 != NULL && wl1 != NULL && Translate->m_Idioma->TipoPreposicion.Find(wl2->m_sTipo) != -1 && wl1->m_sTipo == "RD")
                {
                CStringA coma(",");
                CStringA wherefind = coma + w->m_Prep + coma;       // pone la cadena en que se busca entre comas
                CStringA wfind = coma + wl2->m_Origlwr + coma;  // pone la palabra entre comas
                if(wherefind.Find(wfind) != -1)
                    wl1->m_Delete = TRUE;
                }
            }   
            
        PEEKMESSAGE();
        }
*********/
    }

/////////////////////////////////////////////////////////////////////////////
//  Adiciona una palabra a la oración detrás del ptr dado con la m_Key y 
//  el tipo dado
/////////////////////////////////////////////////////////////////////////////

void COracion::InsertWordAfter(POSITION ptrnodo, const CStringA& key, const CStringA& tipo)
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    CWord *iw = (CWord *)new CWord(key);
                               
    ASSERT( iw != NULL);                   
//    iw->Llena(CDiWord(DW_WORD));
//	iw->FindWordInDic((void *)this,ptrnodo);
	iw->m_Buscada = TRUE;
	if(tipo.GetLength() > 1)
		{
		iw->m_sTipo = tipo;
		CStringA tmp;
		/******* OJOOOOOOOOOOOOOOOOO
		BOOL encontre = Translate->m_Idioma->DPatron->FindKey(iw->m_sTipo,tmp);
		if(encontre == TRUE && tmp.IsEmpty() == FALSE)
			iw->m_Patron = tmp[0];
		*******/
		}
    m_Ora.InsertAfter(ptrnodo, iw);
    }

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
