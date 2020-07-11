// IsFirstLetterInFirstWordUpperCase.h: interface for the CIsFirstLetterInFirstWordUpperCase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISFIRSTLETTERINFIRSTWORDUPPERCASE_H__48FA6EC3_07C5_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_ISFIRSTLETTERINFIRSTWORDUPPERCASE_H__48FA6EC3_07C5_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIsFirstLetterInFirstWordUpperCase : public CSentenceDataObject
{
public:
	CIsFirstLetterInFirstWordUpperCase(LPCORACION ptrSentence):CSentenceDataObject(ptrSentence)
		{}
		
	virtual bool Test()
		{
		ASSERT(GetPtrSentence()->m_Ora.IsEmpty() == FALSE);
		POSITION ptrNode = GetHeadPos();
		ASSERT(ptrNode != NULL);

		LPCWORD ptrWord = GetWordAt(ptrNode);
		ASSERT(ptrWord != NULL);
		if(ptrWord->m_Case == C_MIXTO)
			return true;
		return false;
		}
};

#endif // !defined(AFX_ISFIRSTLETTERINFIRSTWORDUPPERCASE_H__48FA6EC3_07C5_11D3_8926_0060972DBBB5__INCLUDED_)
