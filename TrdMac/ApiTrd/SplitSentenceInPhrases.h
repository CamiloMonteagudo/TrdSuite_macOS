// SplitSentenceInPhrases.h: interface for the CSplitSentenceInPhrases class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITSENTENCEINPHRASES_H__E7A0C504_0E97_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_SPLITSENTENCEINPHRASES_H__E7A0C504_0E97_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CSplitSentenceInPhrases : public CStepInSentence  
{
public:

	CSplitSentenceInPhrases(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}

	virtual void Process();

};

class CGetAllTypeInSentence : public CStepInSentence  
{
private:
    CStringA m_sType;
public:

	CGetAllTypeInSentence(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
    CStringA GetTypeList()
        {
        Process();
        return m_sType;
        }
protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		ASSERT(ptrNode != NULL);
		ASSERT(ptrWord != NULL);
        m_sType += ptrWord->m_sTipo;
        m_sType += " ";
		return ptrNode;
		}


};

#endif // !defined(AFX_SPLITSENTENCEINPHRASES_H__E7A0C504_0E97_11D3_8926_0060972DBBB5__INCLUDED_)
