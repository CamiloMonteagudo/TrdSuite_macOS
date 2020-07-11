// RestoreWordType.h: interface for the CRestoreWordType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESTOREWORDTYPE_H__F6797928_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_RESTOREWORDTYPE_H__F6797928_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CRestoreWordType : public CStepInSentence  
{
public:
	CRestoreWordType(LPCORACION ptrSentence):CStepInSentence(NULL,ptrSentence)
		{}
protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		if(ptrWord->m_StrReserva5.IsEmpty() == FALSE)
			ptrWord->m_sTipo = ptrWord->m_StrReserva5;
        ASSERT(ptrWord->m_sTipo.IsEmpty() == FALSE);
		return ptrNode;
        }
};

#endif // !defined(AFX_RESTOREWORDTYPE_H__F6797928_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
