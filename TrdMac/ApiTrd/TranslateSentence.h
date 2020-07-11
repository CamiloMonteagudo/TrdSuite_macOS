// TranslateSentence.h: interface for the CTranslateSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATESENTENCE_H__3A730941_0462_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATESENTENCE_H__3A730941_0462_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Translation.h"
#include "TranslationProcess.h"

class CTranslateSentence : public CTranslation
{
public:
	CTranslateSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CTranslation(ptrTranslationUser,ptrSentence)
		{}
protected:
	virtual void FillStepArray()
		{
		EmptyWordsInSentence();
		KillContractionsInSentence();
		ParseWordsInSentence();

		AddStepInArray(stCaseSentenceProperty);
		AddStepInArray(stFindWords);
		AddStepInArray(stGetTypeOfWord);
		AddStepInArray(stFindPhrases);
		AddStepInArray(stFindPhrases);		// la repeti por el problema que no encuentra las frases con [] dos seguidas
		AddStepInArray(stFindProperNoun);
    AddStepInArray(stTypeWordByWildCard);
		AddStepInArray(stGetTypeOfWord);
		AddStepInArray(stTypeWordByWildCard);
		AddStepInArray(stFindWords);
		AddStepInArray(stFindProperNoun);
		AddStepInArray(stTypeWordByWildCard);
		AddStepInArray(stTranslationWildCard);
//		AddStepInArray(stTranslationWildCard);
		AddStepInArray(stTypeWordByWildCard);
		AddStepInArray(stTranslationWildCard);
//		AddStepInArray(stTranslationWildCard);
		AddStepInArray(stFindWordType);
		AddStepInArray(stTranslationWildCard);
		AddStepInArray(stTranslationWildCard);
		AddStepInArray(stTranslateWords);
		AddStepInArray(stTranslateBe);
		AddStepInArray(stCmdCoorCnj);
		AddStepInArray(stFindMood);
		AddStepInArray(stFindInterchangePattern);
		AddStepInArray(stInsertArticle);
		AddStepInArray(stFillInsertWords);
		AddStepInArray(stFindWords);
		AddStepInArray(stTranslateWords);
		AddStepInArray(stGenderAndNumber);
		AddStepInArray(stTranslatePreffix);
		AddStepInArray(stConjugate);
		AddStepInArray(stEnding);
		AddStepInArray(stTranslateReflexiveVerbs);
		AddStepInArray(stMakeTranslatedSentence);
		AddStepInArray(stDetails);
		}

};


#endif // !defined(AFX_TRANSLATESENTENCE_H__3A730941_0462_11D3_8926_0060972DBBB5__INCLUDED_)
