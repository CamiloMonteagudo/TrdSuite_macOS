// FillInterChangePattern.h: interface for the CFillInterChangePattern class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLINTERCHAGEPATTERN_H__F679792A_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_FILLINTERCHAGEPATTERN_H__F679792A_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CFillInterChangePattern : public CStepInSentence  
{
public:
	CFillInterChangePattern(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}

	virtual CDict *GetDictionary()
		{
		ASSERT(GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic() != NULL);
		return GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic();
		}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		CStringA sData;

		bool bEncontre = (GetDictionary()->GetKey(ptrWord->m_sTipo,sData))?true:false;

		if(bEncontre == true && sData.IsEmpty() == FALSE)
			ptrWord->m_Patron = sData[0];
	#if !defined(MERCADO)
		else
			MsgError(IDS_NOTFOUNDPATRON,ptrWord->m_sTipo);
	#endif
		return ptrNode;
		}
};

#endif // !defined(AFX_FILLINTERCHAGEPATTERN_H__F679792A_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
