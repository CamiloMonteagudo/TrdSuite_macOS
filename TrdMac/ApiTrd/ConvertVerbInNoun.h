// ConvertVerbInNoun.h: interface for the CConvertVerbInNoun class.
// Si la palabra que le sigue a un articulo es un verbo lo fuerza 
// a que sea sustantivo
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTVERBINNOUN_H__A20F9084_FCA8_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_CONVERTVERBINNOUN_H__A20F9084_FCA8_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CConvertVerbInNoun : public CStepInSentenceWithXX  
{
protected:

    CStringA m_FuerzaSust;
    CStringA m_TipoNoSust;

public:
	CConvertVerbInNoun(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentenceWithXX(ptrTranslationUser,ptrSentence)
		{
		m_FuerzaSust = "RI;RD;AI;AY;";
		m_TipoNoSust = "VV;VT;VI;BN;BG";
		}

protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		if(ptrWord->m_sTipo != "XX")
			{
			POSITION ptrPreviousNode = ptrNode;

			GetPrevPos(ptrPreviousNode);

			if(ptrPreviousNode != NULL)
				{
				LPCWORD ptrPreviousWord = GetWordAt(ptrPreviousNode);

				if(ptrPreviousWord != NULL)

				if(m_FuerzaSust.Find(ptrPreviousWord->m_sTipo) != -1 && m_TipoNoSust.Find(ptrWord->m_sTipo) != -1)
					{
					ptrWord->m_sTipo = "NP";
					ptrWord->m_Trad = ptrWord->m_Orig;
					ptrWord->m_Buscada = TRUE;
					ptrWord->m_Traducida = TRUE;
					}
				}
			}
		return ptrNode;
		}
	};

#endif // !defined(AFX_CONVERTVERBINNOUN_H__A20F9084_FCA8_11D2_8926_0060972DBBB5__INCLUDED_)
