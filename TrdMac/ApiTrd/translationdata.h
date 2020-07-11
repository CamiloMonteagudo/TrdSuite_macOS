// TranslationData.h: interface for the CTranslationData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATIONDATA_H__659264E1_FF5D_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATIONDATA_H__659264E1_FF5D_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UtilFun.h"
#include "ApiConj.h"
#include "ViewFlag.h"
#include "LanguageName.h"
#include "LanguageData.h"
#include "DictMain.h"

class CTranslationData : public CRootLanguageData
{
private:
	CDictMain		 *m_TranslationDict;			  // Diccionario principal de traducción
	CDictLoad		 *m_InterchangePattern;		  // Patrones de intercambio
	CDict   		 *m_TranslationPreffix;		  // Diccionario para la traducción de prefijos
	CDict   		 *m_TranslationSuffix;		  // Diccionario para la traducción de sufijos
	CDict   		 *m_DataDictionary;			    // Diccionario que contiene cadenas con datos especificos de esta dirección de traducción
	CDccComodin		m_CmdFindTranslation;		  // Trabajo con comodines para traducir frases hechas
	CDccComodin		m_CmdFindQTranslation;		// Trabajo con comodines para traducir frases hechas
	CDccComodin		m_CmdFindITranslation;		// Trabajo con comodines para traducir frases hechas
	CDccComodin		m_CmdFindFTranslation;		// Trabajo con comodines para traducir frases hechas
	CDccComodin		m_CmdFindVTranslation;		// Trabajo con comodines para traducir frases hechas
	CDccComodin		m_CmdFindTTranslation;		// Trabajo con comodines para traducir frases hechas
	CDccCmdFrase	m_CmdCoorConj;				    // Trabajo con comodines para coordinar adj y sust y conjugar verbos
	CDccComodin		m_CmdCoorConjDict;			  // Trabajo con comodines para coordinar adj y sust y conjugar verbos
	CLanguageData	m_SourceLanguage;			    // Lenguaje de partida en la traducción
	CLanguageData	m_TargetLanguage;			    // Lenguaje de llegada en la traducción
//	CSGrepArray	   *m_OrdinalNumber;			// RE para traducir numeros ordinales

#if !defined(MERCADO)
	CViewFlag		m_ViewFlag;
#endif

public:

	DECLARE_SERIAL(CTranslationData);

	CTranslationData()
		{
		m_TranslationDict 		= NULL;
		m_InterchangePattern 	= new CDictLoad("DirData_InterchangePattern");
		m_TranslationPreffix 	= new CDict("DirData_TranslationPreffix");
		m_TranslationSuffix 	= new CDict("DirData_TranslationSuffix");
		m_DataDictionary 			= new CDict("DirData_DataDictionary");
		}

	~CTranslationData()
		{
		delete m_InterchangePattern;
		delete m_TranslationPreffix;
		delete m_TranslationSuffix;		 
		delete m_DataDictionary;
		};

	inline CDictMain *GetPtrTranslationDic()
		{return m_TranslationDict;}

	inline CDictLoad *GetPtrInterchangePatternDic()
		{return m_InterchangePattern;}

	inline CDict *GetPtrTranslationPreffixDic()
		{return m_TranslationPreffix;}

	inline CDict *GetPtrTranslationSuffixDic()
		{return m_TranslationSuffix;}

	inline CLanguageData &GetSourceLanguage()
		{return m_SourceLanguage;}

	inline CLanguageData &GetTargetLanguage()
		{return m_TargetLanguage;}

	inline CDccComodin *GetCmdFindTranslationDic()
		{return &m_CmdFindTranslation;}

	inline CDccComodin *GetCmdFindQTranslationDic()
		{return &m_CmdFindQTranslation;}

	inline CDccComodin *GetCmdFindITranslationDic()
		{return &m_CmdFindITranslation;}

	inline CDccComodin *GetCmdFindFTranslationDic()
		{return &m_CmdFindFTranslation;}

	inline CDccComodin *GetCmdFindVTranslationDic()
		{return &m_CmdFindVTranslation;}

	inline CDccComodin *GetCmdFindTTranslationDic()
		{return &m_CmdFindTTranslation;}

	inline CDccComodin *GetCmdCoorConjDic()
		{return &m_CmdCoorConjDict;	}

	inline CDccCmdFrase *GetCmdCoorConj()
		{return &m_CmdCoorConj;	}

	bool Load(BOOL aLoadLangData = TRUE);

#ifdef ACTIVE_SAVE
	bool Save();
#endif

	const CStringA& GetInsertWordNumber(CStringA &sWord, int iPos);
	//const CStringA& GetAddWordNumber(CStringA &sWord, int iPos);



#if !defined(MERCADO)
	CViewFlag *GetPtrViewFlag()
		{return &m_ViewFlag;}
#endif


protected:
	inline CDict *GetPtrDataDic()
		{return m_DataDictionary;}
	virtual void Serialize(CArchive &ar);
};

#endif // !defined(AFX_TRANSLATIONDATA_H__659264E1_FF5D_11D2_8926_0060972DBBB5__INCLUDED_)
