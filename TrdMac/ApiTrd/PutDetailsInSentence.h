// PutDetailsInSentence.h: interface for the CPutDetailsInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUTDETAILSINSENTENCE_H__053E7364_0785_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_PUTDETAILSINSENTENCE_H__053E7364_0785_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CPutDetailsInSentence : public CStepInSentence  
{
public:
	CPutDetailsInSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
	virtual void Process()
		{
		ASSERT(GetPtrSentence()->m_Ora.IsEmpty() != TRUE);
		GetPtrTranslationData()->GetTargetLanguage().GetPtrDetails()->Replace(GetPtrSentence()->m_Dest);
//		if(strlen(GetTranslationUserPtr()->GetNotTranslationInitialMark()) > 0)
//			GetPtrSentence()->m_Dest.Replace(GetTranslationUserPtr()->GetNotTranslationInitialMark(),"");
//		if(strlen(GetTranslationUserPtr()->GetNotTranslationFinalMark()) > 0)
//			GetPtrSentence()->m_Dest.Replace(GetTranslationUserPtr()->GetNotTranslationFinalMark(),"");
		GetPtrSentence()->m_Dest = GetPtrSentence()->m_RtfIni + GetPtrSentence()->m_Dest + GetPtrSentence()->m_RtfFin;
		}
};

///////////////////////////////////////////////////////////////////////
// detiene la busqueda de comodines si la oración ya esta traducida
///////////////////////////////////////////////////////////////////////
class CStopTranslationInTranslateSentence : public CStepInSentence  
{
    bool m_bTraducida;
public:
	CStopTranslationInTranslateSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{m_bTraducida = false;}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if(GetPtrSentence()->GetLength() == 1 && ptrWord->m_sTipo == "FO" && ptrWord->m_Dest.IsEmpty() == FALSE)
            {
            m_bTraducida = true;
            }
        return ptrNode;
        }
    bool GetTranslatedFlag()
        {
        if(m_bTraducida == false)
            Process();
        return m_bTraducida;
        }
    };

///////////////////////////////////////////////////////////////////////
// Busca si todas las palabras de la oracion son de tipo simple
///////////////////////////////////////////////////////////////////////

class CIsAllGramaticalTypeInSentenceSimple : public CStepInSentence  
{
    bool bIsAllGramaticalTypeSimple;
public:
	CIsAllGramaticalTypeInSentenceSimple(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{
        bIsAllGramaticalTypeSimple = true;
        }

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        CStringA sSimpleType("XX,FO,GS,GP,GD,RD,RI,AA,AI,AF,AP,AE,SS,VT,VI,VR,FS,FO,FP,FI,FT,FL,FM,PP,TO,BE,VA,PT,PI,GT,GI,BN,BG,TI,NP,CC,DD,OO,OD,OI,OF,PV,PB,OC");

        if(sSimpleType.Find(ptrWord->m_sTipo) == -1)
            {
            bIsAllGramaticalTypeSimple = false;
            ptrNode = NULL;
            }
        return ptrNode;
        }
    bool GetIsAllGramaticalTypeSimpleFlag()
        {
        Process();
        return bIsAllGramaticalTypeSimple;
        }
};

//////////////////////////////////////////////////////////////////////////////
// Pone los valores de genero número y persona si la oración ya esta traducida
//////////////////////////////////////////////////////////////////////////////

class CPutGramaticalAccidentInTranslateSentence : public CStepInSentence  
{
public:
	CPutGramaticalAccidentInTranslateSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}
protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if(ptrWord->m_sTipo == "FO")
            {
            GetPtrSentence()->m_Genero = ptrWord->m_Genero;
            GetPtrSentence()->m_Numero = ptrWord->m_Numero;
            GetPtrSentence()->m_Persona = ptrWord->m_Persona;
            GetPtrSentence()->m_Modo = ptrWord->m_Modo;
            }
        return ptrNode;
        }
};

#endif // !defined(AFX_PUTDETAILSINSENTENCE_H__053E7364_0785_11D3_8926_0060972DBBB5__INCLUDED_)
