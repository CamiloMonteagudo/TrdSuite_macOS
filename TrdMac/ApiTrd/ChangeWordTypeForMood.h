// ChangeWordTypeForMood.h: interface for the CChangeWordTypeForMood class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEWORDTYPEFORMOOD_H__F679792B_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_CHANGEWORDTYPEFORMOOD_H__F679792B_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChangeWordType.h"

class CChangeWordTypeForMood : public CChangeWordType  
{
public:
	CChangeWordTypeForMood(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL,LPCSTR ptrPreffix=NULL):CChangeWordType(ptrTranslationUser,ptrSentence, ptrPreffix)
		{}

	virtual void IfExecuteDictLanguage(LPCWORD /*ptrWord*/, const CStringA& /*ptrDataType*/, bool /*bReflexive*/)
		{
		}
};

#endif // !defined(AFX_CHANGEWORDTYPEFORMOOD_H__F679792B_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
