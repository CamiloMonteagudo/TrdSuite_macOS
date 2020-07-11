// Translation.h: interface for the CTranslation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATION_H__C3C6AF81_FBBB_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATION_H__C3C6AF81_FBBB_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TranslationProcess.h"
#include "TranslationData.h"
#include "TranslationDataObject.h"
#include "SentenceDataObject.h"

enum enStepInTranslation
	{
	stCaseSentenceProperty,
	stTypeWordByWildCard,
	stGetTypeOfWord,
	stFindWords,
	stFindPhrases,
	stFindProperNoun,
	stTranslationWildCard,
	stFindWordType,
	stTranslateWords,
	stTranslateBe,
	stCmdCoorCnj,
	stFindMood,
	stFindInterchangePattern,
	stInsertArticle,
	stFillInsertWords,
	stGenderAndNumber,
	stTranslatePreffix,
	stConjugate,
	stEnding,
	stTranslateReflexiveVerbs,
	stMakeTranslatedSentence,
	stDetails,
	};



class CTranslation : public CTranslationDataObject,public CSentenceDataObject  , public CMFCObject
{
private:
	CWordArray m_Steps;						// Pasos a realizar durante la traducción de la oración
  CStringA sAllTypeInSentenceTranslation;
  CStringA sAllTypeInSentenceTranslationView;
	DWORD				m_SentenceNumber;		// Numero de la oracion dentro del arreglo de oraciones del texto
	CStringA m_TargetSentence;

public:

	CTranslation(CTranslationUser *ptrTranslationUser = NULL, LPCORACION ptrSentence = NULL):CTranslationDataObject(ptrTranslationUser),CSentenceDataObject(ptrSentence)
		{
		ASSERT(ptrSentence != NULL);
		}
	virtual ~CTranslation()
		{}
	void View(int iStep);
	virtual void Process()
		{
		ClearStepArray();
		FillStepArray();
		ProcessStepArray();
		}

protected:
	void AddStepInArray(int iStep)
		{m_Steps.Add(iStep);}
	void ClearStepArray()
		{m_Steps.RemoveAll();}
	virtual void ProcessStepArray();
	virtual void FillStepArray()
		{}
	virtual void KillContractionsInSentence();
	virtual void ParseWordsInSentence(/*int iFormat = FMT_RTF*/);
	virtual void EmptyWordsInSentence();
	virtual bool TestIsAnyDifferent(CStringA &sStr);
    virtual bool StopTranslation();
	virtual void SetSentenceNumber(DWORD pos)
		{m_SentenceNumber = pos;}
	virtual DWORD GetSentenceNumber()
		{return m_SentenceNumber;}
	virtual void SetTargetSentence(const CStringA& TargetSentence)
		{
		//ASSERT(TargetSentence != NULL);
		m_TargetSentence = TargetSentence;
		}
	virtual CStringA* GetTargetSentence()
		{
		return &m_TargetSentence;
		}
	void FunctionCaseSentenceProperty();
	void FunctionFindWords();
	void FunctionTypeWordByWildCard(bool &bEntreUnaVez,bool &bIsAllGramaticalTypeSimple);
	void FunctionGetTypeOfWord();
	void FunctionFindPhrases();
	void FunctionFindProperNoun();
	void FunctionTranslationWildCard();
	void FunctionFindWordType();
	void FunctionTranslateWords();
	void FunctionTranslateBe();
	void FunctionCmdCoorCnj();
	void FunctionFindMood();
	void FunctionFindInterchangePattern();
	void FunctionInsertArticle();
	void FunctionFillInsertWords();
	void FunctionGenderAndNumber();
	void FunctionTranslatePreffix();
	void FunctionConjugate();
	void FunctionEnding();
	void FunctionTranslateReflexiveVerbs();
	void FunctionMakeTranslatedSentence();
	void FunctionDetails();
};


#endif // !defined(AFX_TRANSLATION_H__C3C6AF81_FBBB_11D2_8926_0060972DBBB5__INCLUDED_)
