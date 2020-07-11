//////////////////////////////////////////////////////////////////////
// FindPhrasesInSentence.h: interface for the CFindPhrasesInSentence class.
// Busca las frases de la oración que se encuentren en el diccionario
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDPHRASESINSENTENCE_H__002619E2_FC3C_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_FINDPHRASESINSENTENCE_H__002619E2_FC3C_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScanWordsInDictionary.h"


class CFindPhrasesInSentence : public CScanWordsInDictionary  
{
public:

	CFindPhrasesInSentence(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CScanWordsInDictionary(ptrTranslationUser,ptrSentence)
		{}

protected:

    // Pregunta si la frase fue encontrada de forma exacta en el dcc
	virtual bool IsFoundPhraseInDic(const CStringA &/*ptrNewPhrase*/,int ffind)
		{
		bool ret = (ffind == FRS_FULLFOUND)?true:false;
		return ret;
		}

	virtual void FormatPhrase(CStringA &sFormatedPhrase,const CStringA& ptrPhrase)
		{sFormatedPhrase = ptrPhrase;}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord);
	virtual void MakePhrase(LPCWORD w, POSITION ini, POSITION _fin);
	virtual void IfFoundPhrase(LPCWORD ptrTmpWord,POSITION ptrIniNode,POSITION ptrPos,int iPhraseSize,LPCWORD ptrWord);
};


class CFindBracketPhraseInSentence : public CFindPhrasesInSentence  
{
	private: 
	CStrArray m_TiposArray;		// Arreglo con los tipos de las palabras
	CWordArray	m_iTiposArray;		// Arreglo con la posición de los tipos de las palabras
	CStrArray m_CategoryArray;	// Arreglo con las categorias de las palabras
	CWordArray m_iCategoryArray;		// Arreglo con la posición de las categorias de las palabras
	CStringA m_TiposPermitidos;		// Arreglo con los tipos gramaticales permitidos a estar en los comodines de [] de las frases idiomaticas
									// este arreglo debe estar en los datos de la dirección de traducción ya que para cada idioma pueden existir tipos diferentes
									// por ejemplo para el ingles XY (Adj o Adv) puede estar aqui al igual AA ya que no se concuerdan los Adj
	CStringA m_CategoriasPermitidas;
public:

	CFindBracketPhraseInSentence(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CFindPhrasesInSentence(ptrTranslationUser,ptrSentence)
		{
		// OJO llenar los tipos y las categorias permitidas desde disco
		m_TiposPermitidos = ptrTranslationUser->GetTranslationDataPtr()->GetTargetLanguage().GetTiposPermitidosEnComodin();
		m_CategoriasPermitidas = ptrTranslationUser->GetTranslationDataPtr()->GetTargetLanguage().GetSemanticaPermitidaEnComodin();
		ResetCounter();
		}

protected:

	///////////////////////////////////
	// resetea los contadores
	///////////////////////////////////
	virtual void ResetCounter()
		{
		m_TiposArray.RemoveAll();
		m_iTiposArray.RemoveAll();
		m_CategoryArray.RemoveAll();
		m_iCategoryArray.RemoveAll();
		}

	virtual void AddItemToFindArray(CStrArray *aWordsToFind, const CStringA& strTipo, const CStringA& strPermitido, CStrArray *strArray, CWordArray *wArray, char strOpenBrace, char strCloseBrace)
		{
		if(strTipo.IsEmpty() || strPermitido.Find(strTipo) == -1)
			return;

		int index = strArray->Find(strTipo);

		WORD k;
		if(index != -1)
			{
			k = wArray->GetAt(index) + 1;
			wArray->SetAt(index,k);
			}
		else
			{
			k=1;
			strArray->Add(strTipo);
			wArray->Add(k);
			}

		CStringA tmp;
		tmp.Format("%c%s%d%c", strOpenBrace, strTipo.c_str(), k, strCloseBrace);
		aWordsToFind->Add(tmp);
		}

	///////////////////////////////////
	//  llena un CStrArray con las posibles variantes de la palabra a buscar en el diccionario 
	///////////////////////////////////
	virtual void FillWordsToFindArray(CStrArray &aWordsToFind, LPCWORD ptrWord)
		{
		CFindPhrasesInSentence::FillWordsToFindArray(aWordsToFind, ptrWord);

		AddItemToFindArray(&aWordsToFind, ptrWord->m_sTipo, m_TiposPermitidos, &m_TiposArray, &m_iTiposArray, '[', ']');

		if(ptrWord->m_sSemantica.IsEmpty() == FALSE)
			{
			if (ptrWord->m_sSemantica.GetLength() == 2)
				AddItemToFindArray(&aWordsToFind, ptrWord->m_sSemantica, m_CategoriasPermitidas, &m_CategoryArray, &m_iCategoryArray, '<', '>');
			else
				{
				CSmartStrArray CategoryArray;
				StringToArray(ptrWord->m_sSemantica.c_str(), &CategoryArray,";,|");
				for(int i = 0; i < CategoryArray.GetCount(); i++)
					{
					AddItemToFindArray(&aWordsToFind, CategoryArray.GetAt(i), m_CategoriasPermitidas, &m_CategoryArray, &m_iCategoryArray, '<', '>');
					}
				}
			}

		/*TRACE("****************************");
		for(int i = 0; i < aWordsToFind.GetCount(); i++)
			{
			TRACE(aWordsToFind[i].c_str());
			}
		TRACE("****************************");*/
		}
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord);
	virtual void GetComodinList( const CStringA& ptrFrase, CStrArray *ComodinesPresentes,LPCSTR strOpenBrace,LPCSTR strCloseBrace);
	virtual void MakePhrase(LPCWORD w, POSITION ini, POSITION _fin);
	virtual void FillComodinTranslation(CStrArray *ComodinesTranslation, CStrArray &ComodinesPresentes,POSITION ini,POSITION fin);
	virtual void FillCComodinTranslation(CStrArray *ComodinesTranslation, CStrArray &ComodinesPresentes,POSITION ini,POSITION fin);
//	virtual void IfFoundPhrase(LPCWORD ptrTmpWord,POSITION ptrIniNode,POSITION ptrPos,int iPhraseSize,LPCWORD ptrWord);
};


#endif // !defined(AFX_FINDPHRASESINSENTENCE_H__002619E2_FC3C_11D2_8926_0060972DBBB5__INCLUDED_)
