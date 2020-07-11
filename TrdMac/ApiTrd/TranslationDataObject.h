// TranslationDataObject.h: interface for the CTranslationDataObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATIONDATAOBJECT_H__F3AB8342_0A0E_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATIONDATAOBJECT_H__F3AB8342_0A0E_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TranslationData.h"
#include "TranslationProcess.h"

class CTranslationDataObject
{
private:
	CTranslationUser   *m_ptrTranslationUser;	// puntero a los datos para realizar la traducción
	virtual void SetPtrTranslationUser(CTranslationUser *ptrTranslationUser)
		{m_ptrTranslationUser = ptrTranslationUser;}

public:
	CTranslationDataObject(CTranslationUser* ptrTranslationUser = NULL)// puntero a los datos para realizar la traducción
		{SetPtrTranslationUser(ptrTranslationUser);}

	virtual ~CTranslationDataObject()
		{}

protected:
	inline CTranslationUser *GetTranslationUserPtr()
		{return m_ptrTranslationUser;}
	inline CTranslationData *GetPtrTranslationData()
		{return m_ptrTranslationUser->GetTranslationDataPtr();}
	inline CLanguageData *GetPtrSourceLanguageData()
		{return &m_ptrTranslationUser->GetTranslationDataPtr()->GetSourceLanguage();}
	inline CLanguageData *GetPtrTargetLanguageData()
		{return &m_ptrTranslationUser->GetTranslationDataPtr()->GetTargetLanguage();}
};

#endif // !defined(AFX_TRANSLATIONDATAOBJECT_H__F3AB8342_0A0E_11D3_8926_0060972DBBB5__INCLUDED_)
