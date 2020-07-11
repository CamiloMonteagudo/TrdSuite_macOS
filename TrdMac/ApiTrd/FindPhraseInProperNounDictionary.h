// FindPhraseInProperNounDictionary.h: interface for the CFindPhraseInProperNounDictionary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDPHRASEINPROPERNOUNDICTIONARY_H__B1FC7DE1_FCF4_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_FINDPHRASEINPROPERNOUNDICTIONARY_H__B1FC7DE1_FCF4_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FindPhrasesInSentence.h"

class CFindPhraseInProperNounDictionary : public CFindPhrasesInSentence  
	{
public:
	CFindPhraseInProperNounDictionary(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CFindPhrasesInSentence(ptrTranslationUser,ptrSentence)
		{}

protected:

	virtual int GetStringArraySize()
		{return 1;}
		
	virtual CDict *GetDictionary()
		{
		ASSERT(GetPtrSourceLanguageData()->GetPtrProperNounDic() != NULL);
		return GetPtrSourceLanguageData()->GetPtrProperNounDic();
		}

	virtual void FillWordsToFindArray(CStrArray &aWordsToFind, LPCWORD ptrWord)
		{
		aWordsToFind.RemoveAll();
    aWordsToFind.SetSize(GetStringArraySize());
    aWordsToFind.SetAt(0,ptrWord->m_Orig);
		}

	virtual void IfFoundPhrase(
			LPCWORD ptrTmpWord,	// Nodo temporal donde se almacena la frase
			POSITION ptrIniNode,// Nodo de inicio de la frase
			POSITION ptrPos,    // Nodo final de la frase
			int /*iPhraseSize*/,	// Tamaño de la frase
			LPCWORD ptrWord)	// Palabra de trabajo
		{
		ptrTmpWord->m_Trad = ptrTmpWord->m_Key;
		ptrTmpWord->m_Buscada = TRUE;
		ptrTmpWord->m_Traducida = TRUE;

		if(ptrWord->m_sTipo == "GW")
			ptrTmpWord->m_sTipo = "GW";
		else
			ptrTmpWord->m_sTipo = "NP";
			
		MakePhrase(ptrTmpWord, ptrIniNode, ptrPos);
		}

	};

#endif // !defined(AFX_FINDPHRASEINPROPERNOUNDICTIONARY_H__B1FC7DE1_FCF4_11D2_8926_0060972DBBB5__INCLUDED_)
