// IsInInterrogativeSentence.h: interface for the CIsInInterrogativeSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISININTERROGATIVESENTENCE_H__C40D6466_0F62_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_ISININTERROGATIVESENTENCE_H__C40D6466_0F62_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CIsInInterrogativeSentence : public CSentenceDataObject
{
public:
	CIsInInterrogativeSentence(LPCORACION ptrSentence):CSentenceDataObject(ptrSentence)
		{}
	virtual bool Test()
		{
		ASSERT(GetPtrSentence()->m_Ora.IsEmpty() == FALSE);
		POSITION ptrNode = GetTailPos();
		ASSERT(ptrNode != NULL);

		LPCWORD ptrWord = GetWordAt(ptrNode);
		ASSERT(ptrWord != NULL);
		if(ptrWord->m_Orig == "?" || ptrWord->m_sTipo == "GS")
			return true;
		return false;
		}
};

#endif // !defined(AFX_ISININTERROGATIVESENTENCE_H__C40D6466_0F62_11D3_8926_0060972DBBB5__INCLUDED_)
