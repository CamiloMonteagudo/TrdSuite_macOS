// MakeTranslatedSentence.h: interface for the CMakeTranslatedSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAKETRANSLATEDSENTENCE_H__AF8D0AC3_0848_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_MAKETRANSLATEDSENTENCE_H__AF8D0AC3_0848_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CMakeTranslatedSentence : public CStepInSentence  
{
public:
	CMakeTranslatedSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL, int /*iFormat*/=FMT_RTF):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
	
	virtual void Process();
};


#endif // !defined(AFX_MAKETRANSLATEDSENTENCE_H__AF8D0AC3_0848_11D3_8926_0060972DBBB5__INCLUDED_)
