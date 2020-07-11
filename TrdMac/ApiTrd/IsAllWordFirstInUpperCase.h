// IsAllWordFirstInUpperCase.h: interface for the CIsAllWordFirstInUpperCase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISALLWORDFIRSTINUPPERCASE_H__E9680DE1_FE38_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_ISALLWORDFIRSTINUPPERCASE_H__E9680DE1_FE38_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CIsAllWordFirstInUpperCase : public CStepInSentence  
{
private:
	bool m_bAllUpper;

public:
	CIsAllWordFirstInUpperCase(LPCORACION ptrSentence=NULL):CStepInSentence(NULL,ptrSentence)
		{SetbAllUpper(true);}
		
	virtual bool Test()
		{
		if(GetPtrSentence()->m_bAllFirstUpper == false)
			Process();
		return GetbAllUpper();
		}
protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		if(ptrWord->m_DiWord.Get() == DW_WORD)
			{
			if( ptrWord->m_Case != C_MIXTO && ptrWord->m_Case != C_UPPER && ptrWord->m_Case != C_OTHER)
				{
				CStringA novalid(";PP;OF;TO;RD;RI;NN;FN;CC;");
				if(novalid.Find(ptrWord->m_sTipo) == -1)
					{
					SetbAllUpper(false);
					return NULL;
					}
				}
			}
		return ptrNode;    
		}
		
	virtual bool GetbAllUpper()
		{return m_bAllUpper;}
		
	virtual void SetbAllUpper(bool bAllUpper)
		{m_bAllUpper = bAllUpper;}
};

#endif // !defined(AFX_ISALLWORDFIRSTINUPPERCASE_H__E9680DE1_FE38_11D2_8926_0060972DBBB5__INCLUDED_)
