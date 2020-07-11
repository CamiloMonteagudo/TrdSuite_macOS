// FillWordAndTypeArray.h: interface for the CFillWordAndTypeArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILLWORDANDTYPEARRAY_H__7BEC3C84_5623_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_FILLWORDANDTYPEARRAY_H__7BEC3C84_5623_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CFillWordAndTypeArray : public CStepInSentence  
{
    private:
    bool m_bWork;
public:
	CFillWordAndTypeArray(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{
        if(GetPtrSentence()->m_WordsInSentence.GetSize() == 0)
            SetWork(true);
        else
            SetWork(false);
        }

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if(GetWork() == false)
            return NULL;
        if((ptrWord->m_Orig.IsEmpty() == FALSE || ptrWord->m_Key.IsEmpty() == FALSE) && ptrWord->m_sTipo.IsEmpty() == FALSE)
            {
            if(ptrWord->m_Orig.IsEmpty() == FALSE)
                {
                GetPtrSentence()->m_WordsKeyInSentence.Add(ptrWord->m_Key);
                GetPtrSentence()->m_WordsInSentence.Add(ptrWord->m_Orig);
                }
            else
                {
                GetPtrSentence()->m_WordsKeyInSentence.Add(ptrWord->m_Key);
                GetPtrSentence()->m_WordsInSentence.Add(ptrWord->m_Key);
                }
            GetPtrSentence()->m_WordsTypeInSentence.Add(ptrWord->m_sTipo);
            }
       
		return ptrNode;
		}
protected:
    void SetWork(bool bWord)
        {m_bWork = bWord;}
    bool GetWork()
        {return m_bWork;}
};

#endif // !defined(AFX_FILLWORDANDTYPEARRAY_H__7BEC3C84_5623_11D3_8926_0060972DBBB5__INCLUDED_)
