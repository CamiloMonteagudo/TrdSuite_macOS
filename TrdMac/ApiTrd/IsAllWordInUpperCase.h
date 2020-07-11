// IsAllWordInUpperCase.h: interface for the CIsAllWordInUpperCase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISALLWORDINUPPERCASE_H__48FA6EC2_07C5_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_ISALLWORDINUPPERCASE_H__48FA6EC2_07C5_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IsAllWordFirstInUpperCase.h"

class CIsAllWordInUpperCase : public CIsAllWordFirstInUpperCase
{
private:
	bool m_bEntre;
public:
	CIsAllWordInUpperCase(LPCORACION ptrSentence=NULL):CIsAllWordFirstInUpperCase(ptrSentence)
		{}
		
	virtual bool Test()
		{
		SetbAllUpper(true);
		SetbEntre(false);
		Process();
		return (GetbEntre() == true && GetbAllUpper() == true)?true:false;
		}
		
protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		if(ptrWord->m_DiWord.Get() == DW_WORD)
			{
			if( ptrWord->m_Case != C_UPPER)
				SetbAllUpper(false);
			SetbEntre(true);
			}
		return ptrNode;    
		}
		
	virtual bool GetbEntre()
		{return m_bEntre;}
		
	virtual void SetbEntre(bool bEntre)
		{m_bEntre = bEntre;}
};

#endif // !defined(AFX_ISALLWORDINUPPERCASE_H__48FA6EC2_07C5_11D3_8926_0060972DBBB5__INCLUDED_)
