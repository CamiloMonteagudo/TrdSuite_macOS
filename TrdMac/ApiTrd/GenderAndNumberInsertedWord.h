// GenderAndNumberInsertedWord.h: interface for the CGenderAndNumberInsertedWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENDERANDNUMBERINSERTEDWORD_H__F679792E_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_GENDERANDNUMBERINSERTEDWORD_H__F679792E_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CGenderAndNumberInsertedWord : public CStepInSentence  
{
public:
	CGenderAndNumberInsertedWord(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}

protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if (GetPtrSourceLanguageData()->GetArticleType().Find(ptrWord->m_sTipo) != -1)
          {      
            POSITION ptrNextNode = ptrNode;
            GetNextPos(ptrNextNode);                
            if(ptrNextNode != NULL)    // Palabra posterior a la que se analiza 
                {
                LPCWORD ptrNextWord = GetWordAt(ptrNextNode);
                
                ptrWord->m_Genero = ptrNextWord->m_Genero;
                ptrWord->m_Numero = ptrNextWord->m_Numero;
                }
            }
		return ptrNode;
		}
};

#endif // !defined(AFX_GENDERANDNUMBERINSERTEDWORD_H__F679792E_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
