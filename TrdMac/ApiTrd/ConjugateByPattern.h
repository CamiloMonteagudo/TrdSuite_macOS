// ConjugateByPattern.h: interface for the CConjugateByPattern class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONJUGATEBYPATTERN_H__E7A0C503_0E97_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_CONJUGATEBYPATTERN_H__E7A0C503_0E97_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_CNJ_CICLE 5

#include "StepInSentence.h"

class CConjugateByPattern : public CStepInSentenceWithXX  
{
public:
	CConjugateByPattern(CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL):CStepInSentenceWithXX(ptrTranslationUser,ptrSentence)
		{}

	virtual void Process();
};

#endif // !defined(AFX_CONJUGATEBYPATTERN_H__E7A0C503_0E97_11D3_8926_0060972DBBB5__INCLUDED_)
