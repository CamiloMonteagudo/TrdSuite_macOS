// ScanWordsInDictionary.h: interface for the CScanWordsInDictionary class.
// Busca la palabra en el diccionario general de palabras de 
// no encontrarse la guarda en un diccionario de palabras no encontradas
// con su frecuencia de uso
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANWORDSINDICTIONARY_H__8F71E221_06BC_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_SCANWORDSINDICTIONARY_H__8F71E221_06BC_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CScanWordsInDictionary : public CStepInSentenceWithXX  
{
private:
	bool m_bInsertAllWord;

public:
	///////////////////////////////////
	//   
	///////////////////////////////////
	CScanWordsInDictionary(CTranslationUser *ptrTranslationUser,LPCORACION ptrSentence,CDictMain *ptrDic = NULL,bool bInsertAllWord = false):CStepInSentenceWithXX(ptrTranslationUser,ptrSentence)
		{
		SetDictionary(ptrDic);
		SetInsertAllWord(bInsertAllWord);
		}

	///////////////////////////////////
	//  busca una frase en diccionario 
	///////////////////////////////////
	inline BOOL FindInDic(LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("FindInDic"); // 0.65 37375
		BOOL ret = GetDictionary()->GetKey(ptrWord->m_Key,ptrWord->m_Data);
		//xfx BOOL ret = GetTranslationUserPtr()->GetPtrTranslationDic()->GetKey(ptrWord->m_Key,ptrWord->m_Data);
		if (ret)
			GetDataInDic(ptrWord);

		return ret;
		}

protected:
	void SetInsertAllWord(bool bInsertAllWord)
		{m_bInsertAllWord = bInsertAllWord;}
	bool GetInsertAllWord()
		{return m_bInsertAllWord;}
	POSITION Execute(POSITION ptrNode, LPCWORD ptrWord);
	BOOL FindReducingWord(LPCWORD ptrWord,POSITION nodo);
	BOOL FindIrrgVerb(LPCWORD ptrWord);
	BOOL FindKeySufijo(LPCWORD ptrWord,POSITION nodo);
	BOOL FindKeyPrefijo(LPCWORD ptrWord,POSITION nodo,bool reduce);
	void FindNameGender(LPCWORD ptrWord);
	bool FindTradSufijo(LPCWORD ptrWord,POSITION nodo);
	bool FindTradPrefijo(LPCWORD ptrWord,POSITION nodo);

	///////////////////////////////////
	// Llena estructura Dic con campo 
	// Data de diccionario
	///////////////////////////////////
	void GetDataInDic(LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("GetDataInDic"); // 0.08 24042
		if (ptrWord->m_Data.IsEmpty())
			return;

		//TRACE("Data = %s", ptrWord->m_Data.c_str());
		CStringA sData(ptrWord->m_Data);
		ptrWord->m_Frecuencia = 0;
		GetSubCampo(ptrWord->m_sTipo, sData, 'T', '#');
		GetSubCampo(ptrWord->m_Data, sData, 'E', '#');
		GetSubCampo(ptrWord->m_MinMay, sData, 'C', '#');
		GetSubCampo(ptrWord->m_Frases, sData, 'I', '#');
		GetSubCampo(ptrWord->m_sSemantica, sData, 'M', '#');
		}
};

#endif // !defined(AFX_SCANWORDSINDICTIONARY_H__8F71E221_06BC_11D3_8926_0060972DBBB5__INCLUDED_)
