// InsertOrAddWord.h: interface for the CInsertOrAddWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSERTORADDWORD_H__F679792C_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_INSERTORADDWORD_H__F679792C_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"
#include "FindWordSentenceInDict.h"
#include "UtilFun.h"

////////////////////////////////////////////////////////////////////////

class CInsertOrAddWord : public CStepInSentence  
{
private:
	int m_INSERT_SIZE;
	int m_ADD_SIZE;
public:
	CInsertOrAddWord(CTranslationUser *ptrTranslationUser=NULL,
        LPCORACION ptrSentence=NULL,
        POSITION ptrInitialNode = NULL,
		POSITION ptrFinalNode = NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{
		m_INSERT_SIZE=20;
		m_ADD_SIZE=20;
		if(ptrInitialNode != NULL)
			SetInitialNode(ptrInitialNode);
		if(ptrFinalNode != NULL)
			SetFinalNode(ptrFinalNode);
		}

protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("CInsertOrAddWord"); // 0.0
		if ((ptrWord->m_Articulo > 0 && ptrWord->m_Articulo < m_INSERT_SIZE) || ptrWord->m_sInsert.IsEmpty() == FALSE)
			{
			CStringA sTmpString;
      if(ptrWord->m_sInsert.IsEmpty() == FALSE)
				sTmpString = ptrWord->m_sInsert;
      else
			  sTmpString = GetPtrTranslationData()->GetInsertWordNumber(sTmpString,ptrWord->m_Articulo);

			if(sTmpString.IsEmpty() == FALSE)
				{
				CSmartStrArray insertar;   // puede necesitar insertarse más de una palabra Ej: "of the" (de el, de la)

				StringToArray(sTmpString.c_str(), &insertar,";");
				POSITION ptrNewPos = ptrNode;
				for(int j = 0; j < insertar.GetCount(); j++)
					{
					CStringA palabra = insertar[j];
					LPCWORD ptrInsertedWord = (LPCWORD)new CWord(palabra);
                                       
					ASSERT( ptrInsertedWord != NULL);                   
					/*CParseWordsInSentence*/
					//ptrInsertedWord->Llena(CDiWord(DW_WORD));
					ptrInsertedWord->m_Case = C_LOWER;
					ptrNewPos = GetPtrSentence()->m_Ora.InsertBefore(ptrNewPos,ptrInsertedWord);
          CFindWordSentenceInDict FindWordSentenceInDict(GetTranslationUserPtr(),GetPtrSentence());
          FindWordSentenceInDict.FindInDic(ptrInsertedWord);
					}
				}
			}
		if ((ptrWord->m_Adiciona > 0 && ptrWord->m_Adiciona < m_ADD_SIZE) || ptrWord->m_sAdd.IsEmpty() == FALSE)
			{
			CStringA sTmpString;
      if(ptrWord->m_sAdd.IsEmpty() == FALSE)
        sTmpString = ptrWord->m_sAdd;
      else
			  sTmpString = GetPtrTranslationData()->GetInsertWordNumber(sTmpString,ptrWord->m_Adiciona);
			if(sTmpString.IsEmpty() == FALSE)
				{
				CSmartStrArray insertar;   // puede necesitar insertarse más de una palabra Ej: "of the" (de el, de la)

				StringToArray(sTmpString.c_str(), &insertar,";");

				POSITION ptrNewPos = ptrNode;

				for(int j = 0; j < insertar.GetCount(); j++)
					{
					CStringA palabra = insertar[j];
					LPCWORD ptrInsertedWord = (LPCWORD)new CWord(palabra);
                                       
					ASSERT( ptrInsertedWord != NULL);                   
					/*CParseWordsInSentence*/
					//ptrInsertedWord->Llena(CDiWord(DW_WORD));
					ptrInsertedWord->m_Case = C_LOWER;
					ptrNewPos = GetPtrSentence()->m_Ora.InsertAfter(ptrNewPos,ptrInsertedWord);
          CFindWordSentenceInDict FindWordSentenceInDict(GetTranslationUserPtr(),GetPtrSentence());
          FindWordSentenceInDict.FindInDic(ptrInsertedWord);
					}
				}
			}
		return ptrNode;
		}
};

#endif // !defined(AFX_INSERTORADDWORD_H__F679792C_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
