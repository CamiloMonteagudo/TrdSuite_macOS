// SentenceDataObject.h: interface for the CSentenceDataObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENTENCEDATAOBJECT_H__F3AB8343_0A0E_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_SENTENCEDATAOBJECT_H__F3AB8343_0A0E_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "COra.h"

class CSentenceDataObject  
{
private:
	LPCORACION	m_ptrSentence;	// puntero a la oración que se procesa
public:
	CSentenceDataObject(LPCORACION ptrSentence = NULL)		// puntero a la oracion de trabajo
		{
		//ASSERT(ptrSentence != NULL);
		m_ptrSentence = NULL;
		if(ptrSentence != NULL)
			SetPtrSentence(ptrSentence);
		//ASSERT(GetPtrSentence() != NULL);
		}
protected:

	inline LPCWORD GetWordAt(POSITION ptrNode)
		{return (ptrNode != NULL)?(LPCWORD)GetWordList().GetAt(ptrNode):NULL;}

	inline void GetNextPos(POSITION &ptrNode)
		{
		if(ptrNode != NULL)
			GetWordList().GetNext(ptrNode);
		}
	inline void GetPrevPos(POSITION &ptrNode)
		{
		if(ptrNode != NULL)
			GetWordList().GetPrev(ptrNode);
		}
	inline POSITION GetHeadPos()
		{return GetWordList().GetHeadPosition();}
	inline POSITION GetTailPos()
		{return GetWordList().GetTailPosition();}
	inline CObList &GetWordList()
		{return GetPtrSentence()->m_Ora;}
	inline LPCORACION GetPtrSentence()
		{
		ASSERT(m_ptrSentence != NULL);
		return m_ptrSentence;
		}
	inline void SetPtrSentence(LPCORACION ptrSentence)
		{
		ASSERT(ptrSentence != NULL);
		m_ptrSentence = ptrSentence;
		}
};

#endif // !defined(AFX_SENTENCEDATAOBJECT_H__F3AB8343_0A0E_11D3_8926_0060972DBBB5__INCLUDED_)
