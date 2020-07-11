// PutMoodAndTimeInGramType.h: interface for the CPutMoodAndTimeInGramType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUTMOODANDTIMEINGRAMTYPE_H__7460C8E4_351D_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_PUTMOODANDTIMEINGRAMTYPE_H__7460C8E4_351D_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CPutMoodAndTimeInGramType : public CStepInSentence  
{
public:
	CPutMoodAndTimeInGramType(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
        {}

protected:
	POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if(ptrWord->m_sTipo == "VV" && ptrWord->m_Modo == mGERUNDIO)
            ptrWord->m_sTipo = "VG";
        if(ptrWord->m_sTipo == "VT" && ptrWord->m_Modo == mGERUNDIO)
            ptrWord->m_sTipo = "GT";
        if(ptrWord->m_sTipo == "VI" && ptrWord->m_Modo == mGERUNDIO)
            ptrWord->m_sTipo = "GI";
        if(ptrWord->m_sTipo == "VG" || ptrWord->m_sTipo == "GT" || ptrWord->m_sTipo == "GI")
            ptrWord->m_Modo = mGERUNDIO;
/*********
        if(ptrWord->m_sTipo == "VV" && ptrWord->m_Tiempo == mPASADO)
            ptrWord->m_sTipo = "VG";
        if(ptrWord->m_sTipo == "VT" && ptrWord->m_Modo == mGERUNDIO)
            ptrWord->m_sTipo = "GT";
        if(ptrWord->m_sTipo == "VI" && ptrWord->m_Modo == mGERUNDIO)
            ptrWord->m_sTipo = "GI";
************/
		return ptrNode;
		}
};

#endif // !defined(AFX_PUTMOODANDTIMEINGRAMTYPE_H__7460C8E4_351D_11D3_8926_0060972DBBB5__INCLUDED_)
