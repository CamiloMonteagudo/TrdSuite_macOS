// KillContractionsInSentence.h: interface for the CKillContractionsInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KILLCONTRACTIONSINSENTENCE_H__053E7361_0785_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_KILLCONTRACTIONSINSENTENCE_H__053E7361_0785_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CKillContractionsInSentence : public CStepInSentence  
{
public:
	CKillContractionsInSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
	virtual void Process()
		{
		//TRACE("Quitando contracciones");

		while(GetPtrSentence()->m_Orig.GetLength() > 0 && GetPtrSentence()->m_Orig.GetAt(0) == '×')
			{
			GetPtrSentence()->m_RtfIni += '×';
			GetPtrSentence()->m_Orig = GetPtrSentence()->m_Orig.Right(GetPtrSentence()->m_Orig.GetLength()-1);
			}
		while(GetPtrSentence()->m_Orig.GetLength() > 0 && GetPtrSentence()->m_Orig.GetAt(GetPtrSentence()->m_Orig.GetLength()-1) == '×')
			{
			GetPtrSentence()->m_RtfFin += '×';
			GetPtrSentence()->m_Orig = GetPtrSentence()->m_Orig.Left(GetPtrSentence()->m_Orig.GetLength()-1);
			}

		int Ini = GetPtrSentence()->m_Orig.Find(GetTranslationUserPtr()->GetNotTranslationInitialMark());
		int Fin = GetPtrSentence()->m_Orig.Find(GetTranslationUserPtr()->GetNotTranslationFinalMark());

		if(Ini >= 0 && Fin > 0)
			{
			CStringA Parte1 = GetPtrSentence()->m_Orig.Left(Ini);
			CStringA Parte2 = GetPtrSentence()->m_Orig.Mid(Ini,(Fin-Ini)+1);
			CStringA Parte3 = GetPtrSentence()->m_Orig.Right((GetPtrSentence()->m_Orig.GetLength()-Fin-1));
			GetPtrTranslationData()->GetSourceLanguage().GetPtrContractions()->Replace(Parte1);
			GetPtrTranslationData()->GetSourceLanguage().GetPtrContractions()->Replace(Parte3);
			GetPtrSentence()->m_Orig = Parte1 + Parte2 + Parte3;
			}
		else if(Ini >= 0 && Fin == -1)
			{
			CStringA Parte1 = GetPtrSentence()->m_Orig.Left(Ini);
			CStringA Parte2 = GetPtrSentence()->m_Orig.Mid(Ini,(GetPtrSentence()->m_Orig.GetLength()-Ini)+1);
			GetPtrTranslationData()->GetSourceLanguage().GetPtrContractions()->Replace(Parte1);
			GetPtrSentence()->m_Orig = Parte1 + Parte2;
			}
		else
			{
			GetPtrTranslationData()->GetSourceLanguage().GetPtrContractions()->Replace(GetPtrSentence()->m_Orig);
			}
		}
};

#endif // !defined(AFX_KILLCONTRACTIONSINSENTENCE_H__053E7361_0785_11D3_8926_0060972DBBB5__INCLUDED_)
