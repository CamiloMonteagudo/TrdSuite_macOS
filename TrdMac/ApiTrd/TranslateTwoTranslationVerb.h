// TranslateTwoTranslationVerb.h: interface for the CTranslateTwoTranslationVerb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATETWOTRANSLATIONVERB_H__F6797929_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATETWOTRANSLATIONVERB_H__F6797929_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CTranslateTwoTranslationVerb : public CStepInSentence  
{
public:
	CTranslateTwoTranslationVerb(CTranslationUser* ptrTranslationUser = NULL,	// puntero a los datos para la traducción
								LPCORACION ptrSentence = NULL,					// puntero a la oracion de trabajo
								POSITION ptrInitialNode = NULL,
								POSITION ptrFinalNode = NULL):CStepInSentence(ptrTranslationUser,ptrSentence,ptrInitialNode,ptrFinalNode)
		{}
protected:

	virtual CDict *GetDictionary()
		{
		ASSERT(GetPtrTargetLanguageData()->GetPtrEspecialWordTranslationDic() != NULL);
		return GetPtrTargetLanguageData()->GetPtrEspecialWordTranslationDic();
		}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("CTranslateTwoTranslationVerb");
		CStringA sTmpString(ptrWord->m_Trad);

		sTmpString.Replace("*","");

		if(GetDictionary()->IsKey(sTmpString) == TRUE)
			GetPtrSentence()->IsLastNotDDSetByType(ptrNode,"BE","estar",0,"DD,DN,DF");
    else
      {
      int pos = sTmpString.Find(' ');
      if( pos != -1)
        {
        sTmpString = sTmpString.Left(pos);
				if(GetDictionary()->IsKey(sTmpString) == TRUE)
					GetPtrSentence()->IsLastNotDDSetByType(ptrNode,"BE","estar",0,"DD,DN,DF");
				}
      }

    ASSERT(ptrWord->m_sTipo.IsEmpty() == FALSE);

		return ptrNode;
		}
};

#endif // !defined(AFX_TRANSLATETWOTRANSLATIONVERB_H__F6797929_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
