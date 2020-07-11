// ChangeWordType.h: interface for the CChangeWordType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEWORDTYPE_H__A20F9081_FCA8_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_CHANGEWORDTYPE_H__A20F9081_FCA8_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CChangeWordType : public CStepInSentence  
{
protected:
	CStringA m_sPreffix;

public:
	CChangeWordType(CTranslationUser* ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL,const CStringA& ptrPreffix=CANull):CStepInSentence(ptrTranslationUser, ptrSentence)
		{
		m_sPreffix = ptrPreffix;
		}

protected:

	virtual void IfExecuteDictLanguage(LPCWORD ptrWord, const CStringA& ptrDataType, bool bReflexive)
		{
		ptrWord->m_sTipo = ptrDataType;
		ptrWord->m_TradTipo = TRUE;
		ptrWord->m_Reflexivo = bReflexive;
		}
	virtual CDict *GetDictionary()
		{
		ASSERT(GetPtrTranslationData() != NULL);
		return GetPtrTranslationData()->GetSourceLanguage().GetPtrTypeByDictLanguageDic();
		}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		bool bEncontre = false;
		CStringA sDataType;
		CStringA sKey = m_sPreffix + ptrWord->m_sTipo;

		ptrWord->m_StrReserva5 = ptrWord->m_sTipo;

		bEncontre = (GetDictionary()->GetKey(sKey,sDataType))?true:false;

		if(bEncontre == false)
			return ptrNode;

		bool bReflexive = (ptrWord->m_Reflexivo)?true:false;

		if(sDataType.IsEmpty() == FALSE)
			{
			CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
                
			l.Execute(ptrNode,sDataType);

			IfExecuteDictLanguage(ptrWord,sDataType,bReflexive);
			}

		ASSERT(ptrWord->m_sTipo.IsEmpty() == FALSE);
		return ptrNode;
		}
};

#endif // !defined(AFX_CHANGEWORDTYPE_H__A20F9081_FCA8_11D2_8926_0060972DBBB5__INCLUDED_)
