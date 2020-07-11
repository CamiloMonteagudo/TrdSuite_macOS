// EmptyWordsInSentence.h: interface for the CEmptyWordsInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMPTYWORDSINSENTENCE_H__053E7362_0785_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_EMPTYWORDSINSENTENCE_H__053E7362_0785_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CEmptyWordsInSentence : public CStepInSentence  
{
public:
	CEmptyWordsInSentence(LPCORACION ptrSentence = NULL):CStepInSentence(NULL,ptrSentence)
		{}
	virtual ~CEmptyWordsInSentence()
		{GetPtrSentence()->m_Ora.RemoveAll();}


	virtual void Process()
		{
	  if(GetPtrSentence()->m_Ora.IsEmpty() == TRUE)
			return;
		CStepInSentence::Process();
		}
protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		delete ptrWord;
		return ptrNode;
		}
};

#endif // !defined(AFX_EMPTYWORDSINSENTENCE_H__053E7362_0785_11D3_8926_0060972DBBB5__INCLUDED_)
