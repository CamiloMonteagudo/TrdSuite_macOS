// FillInsertedWord.h: interface for the CFillInsertedWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLINSERTEDWORD_H__F679792D_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_FILLINSERTEDWORD_H__F679792D_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CFillInsertedWord : public CStepInSentence  
{
public:
	CFillInsertedWord(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if (ptrWord->m_Buscada == FALSE)
            {
			/*CParseWordsInSentence*/
            //ptrWord->Llena(DW_WORD);
            }
		return ptrNode;
		}
};

#endif // !defined(AFX_FILLINSERTEDWORD_H__F679792D_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
