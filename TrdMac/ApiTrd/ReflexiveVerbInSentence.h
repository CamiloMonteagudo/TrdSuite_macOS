// ReflexiveVerbInSentence.h: interface for the CReflexiveVerbInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFLEXIVEVERBINSENTENCE_H__E7A0C502_0E97_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_REFLEXIVEVERBINSENTENCE_H__E7A0C502_0E97_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CReflexiveVerbInSentence : public CStepInSentence  
{
public:
	CReflexiveVerbInSentence(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
    void PutReflexivePronoun(LPCWORD ptrWord);

protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord);

};

#endif // !defined(AFX_REFLEXIVEVERBINSENTENCE_H__E7A0C502_0E97_11D3_8926_0060972DBBB5__INCLUDED_)
