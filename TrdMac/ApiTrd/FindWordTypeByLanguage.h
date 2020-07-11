// FindWordTypeByLanguage.h: interface for the CFindWordTypeByLanguage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDWORDTYPEBYLANGUAGE_H__F6797926_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_FINDWORDTYPEBYLANGUAGE_H__F6797926_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CFindWordTypeByLanguage : public CStepInSentenceWithXX  
{
public:
	CFindWordTypeByLanguage(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentenceWithXX(ptrTranslationUser,ptrSentence)
		{}

protected:

	virtual CDict *GetDictionary()
		{
		ASSERT(GetPtrSourceLanguageData()->GetPtrTypeByDictLanguageDic() != NULL);
		return GetPtrSourceLanguageData()->GetPtrTypeByDictLanguageDic();
		}

	virtual CDict *GetPatternDictionary()
		{
		ASSERT(GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic() != NULL);
		return GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic();
		}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
    for(int i=0; i < 4 && ptrWord->m_Traducida == FALSE;i++)
    //int i = 1; // OJO quitar el for para probar comodines
      {
      //CStringA sKeyType;	// Tipo gramatical a buscar con P(del pase delante)
      //sKeyType.Format("P%i",i);
      CStringA sKeyType("P", CStringA::FromInt_(i));	// Tipo gramatical a buscar con P(del pase delante)
			CStringA sData;		  // Cadena de lenguaje de diccionario para el tipo dado

      sKeyType += ptrWord->m_sTipo;

      BOOL bEncontre = GetDictionary()->GetKey(sKeyType,sData);

      if(bEncontre == TRUE && sData.IsEmpty() == FALSE)
        {
        // ejecuta el lenguaje de diccionario sobre lo encontrado en el tipo dado
        CLengDic l(GetTranslationUserPtr(),GetPtrSentence());       
            
        l.Execute(ptrNode,sData);

				ASSERT(sData.GetLength() == 2);

        ptrWord->m_sTipo = sData;
        ptrWord->m_TradTipo = TRUE;
        }
      }   

    CStringA sPattern;	// Patrón de intercambio de palabras en la oración

		// Busca el tipo encontrado en el diccionario de patrones de intercambio

    BOOL bEncontre = GetPatternDictionary()->GetKey(ptrWord->m_sTipo,sPattern);

    if(bEncontre == TRUE && sPattern.IsEmpty() == FALSE)
      ptrWord->m_Patron = sPattern[0];

#if !defined(MERCADO)
    else
      MsgError(IDS_NOTFOUNDPATRON,ptrWord->m_sTipo);
#endif
#ifdef _DEBUG
        ASSERT(ptrWord->m_sTipo.IsEmpty() == FALSE);
#else
		if(ptrWord->m_sTipo.IsEmpty() == TRUE)
			ptrWord->m_sTipo = "SS";
#endif
		return ptrNode;
		}
};

#endif // !defined(AFX_FINDWORDTYPEBYLANGUAGE_H__F6797926_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
