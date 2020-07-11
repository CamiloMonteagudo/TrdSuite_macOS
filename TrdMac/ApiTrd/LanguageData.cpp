// LanguageData.cpp: implementation of the CLanguageData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RootLanguageData.h"
#include "LanguageData.h"
#include "PeekMsg.h"

IMPLEMENT_SERIAL(CRootLanguageData,CObject,VERSIONABLE_SCHEMA|1);
IMPLEMENT_SERIAL(CLanguageData,CObject,VERSIONABLE_SCHEMA|1);

#if defined(MERCADO)
    #define NP_DIC_FLAG     DICC_FILE|DICC_READONLY
#else
//    #define NP_DIC_FLAG     DICC_FILE|DICC_READONLY
    #define NP_DIC_FLAG     DICC_FILE
#endif

//////////////////////////////////////////////////////////////////////
// Serialize
//////////////////////////////////////////////////////////////////////

void CLanguageData::Serialize(CArchive &ar)
  {
	COLLECT_INTERVAL("CLanguageData::Serialize"); // 0.14 de 1.22

	if(ar.IsLoading())
		{
		m_Contractions     = new CSGrepArray;	// RE para eliminar detalles en trad  
		m_Details		   = new CSGrepArray;	// RE para descontracciones  
		m_ProperNounGender = new CSGrepArray;	// RE para detectar el género de nombres propios que no se encuentren en el diccionario
		m_SplitInWords	   = new CSGrepArray;	// RE para dividir la oración en palabras
		}

	m_EspecialWordTranslation->Serialize(ar, GetIsoName() + "EspecialWord.txt");
	if (ar.hasError()) return;
	m_DataDictionary->Serialize(ar, GetIsoName() + "DataDictionary.txt");
	if (ar.hasError()) return;
	m_Details->Serialize(ar);
	if (ar.hasError()) return;
	m_ProperNounGender->Serialize(ar);
	if (ar.hasError()) return;

#ifndef ACTIVE_SAVE
	if(m_Target == false || ar.IsStoring())
#endif
		{ // Si la opcion de salvar esta activada entonces hay que entrar aqui siempre
		m_CmdFindGrFunction.Serialize(ar);
		if (ar.hasError()) return;
		m_CmdFindQGrFunction.Serialize(ar);
		if (ar.hasError()) return;
		m_CmdFindIGrFunction.Serialize(ar);
		if (ar.hasError()) return;
		m_CmdFindTGrFunction.Serialize(ar);
		if (ar.hasError()) return;
		m_CmdFindProperNoun.Serialize(ar);
		if (ar.hasError()) return;

		m_TypeByDictLanguage->Serialize(ar, GetIsoName() + "TypeByDictLanguage.txt");
		if (ar.hasError()) return;
		m_TypeAndPattern->Serialize(ar, GetIsoName() + "TypeAndPattern.txt");
		if (ar.hasError()) return;
		m_PreffixReduction->Serialize(ar, GetIsoName() + "PreffixReduction.txt");
		if (ar.hasError()) return;
		m_SuffixReduction->Serialize(ar, GetIsoName() + "SuffixReduction.txt");
		if (ar.hasError()) return;
		m_WordReduction->Serialize(ar, GetIsoName() + "WordReduction.txt");
		if (ar.hasError()) return;
		m_Contractions->Serialize(ar);
		if (ar.hasError()) return;
		m_SplitInWords->Serialize(ar);
		if (ar.hasError()) return;
		}
  }
///////////////////////////////////////////////////////////////////////////////
//	Carga todos los elementos de la clase
//  con la variable MERCADO activada
///////////////////////////////////////////////////////////////////////////////

bool CLanguageData::Load(bool target)
	{
	bool ret = true;
	
	SetTarget(target);

	// carga como mapped file el diccionario de nombres propios
	PEEKMESSAGE();
	if(LoadDictionary(m_ProperNounDic,LF"PNoun")==false)
		return false;
	///////////////////////////////////////////////////////////////////////////////
	// si se ha definido la variable MERCADO
	///////////////////////////////////////////////////////////////////////////////
	ret = LoadSerialize(MakeName(LF"Data",true,false,EXT_DAT_LOAD));

	m_VerbalType                      = GetStringInDataDic("VERBO");
	m_AdjectiveType                   = GetStringInDataDic("ADJETIVO");
	m_AdverbialType                   = GetStringInDataDic("ADVERBIO");
	m_ArticleType                     = GetStringInDataDic("ARTICULO");
	m_TypeWithSpaceBeforeWord         = GetStringInDataDic("ESPACIO1");
	m_TypeWithSpaceAfterWord          = GetStringInDataDic("ESPACIO2");
	m_GerundType                      = GetStringInDataDic("GERUNDIO");
	m_TypeWithFirstLetterInUpperCase  = GetStringInDataDic("MAYUSCULA");
	m_ParticipleType                  = GetStringInDataDic("PARTICIPIO");
	m_PastType                        = GetStringInDataDic("PASADO");
	m_PastOrParticipleType            = GetStringInDataDic("PPARTICIPIO");
	m_PosesiveType                    = GetStringInDataDic("POSESIVO");
	m_PrepositionType                 = GetStringInDataDic("PREPOSICION");
	m_ProNounType                     = GetStringInDataDic("PRONOMBRE");
	m_NounType                        = GetStringInDataDic("SUSTANTIVO");
	m_GradeType                       = GetStringInDataDic("GRADE");
	m_FemaleType                      = GetStringInDataDic("TIPOFEMENINO");
	m_PluralType                      = GetStringInDataDic("TIPOPLURAL");
	m_TypeThatNotNeedReduction        = GetStringInDataDic("TIPOSINREDUCCION");
	m_TypeNounDeclination             = GetStringInDataDic("TIPOSSDECLINACION");
	m_TypeAdjDeclination              = GetStringInDataDic("TIPOAADECLINACION");
	m_FemaleTypeSiAsterisco           = GetStringInDataDic("FEMENINOSIASTERISCO");
	m_PluralTypeSiAsterisco           = GetStringInDataDic("PLURALSIASTERISCO");
	m_TiposPermitidosEnComodin        = GetStringInDataDic("TIPOSENCOMODIN");
	m_SemanticaPermitidaEnComodin     = GetStringInDataDic("SEMANTICAENCOMODIN");

	return ret;
	}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#ifdef ACTIVE_SAVE
bool CLanguageData::Save(bool target)
	{
	bool ret = true;
	
	SetTarget(target);

	// carga como mapped file el diccionario de nombres propios
	//if(SaveDictionary(m_ProperNounDic,LF"PNoun",NP_DIC_FLAG)==false)
	//	return false;

	///////////////////////////////////////////////////////////////////////////////
	// si se ha definido la variable MERCADO
	///////////////////////////////////////////////////////////////////////////////
	ret = SaveSerialize(MakeName(LF"Data",true,false,EXT_DAT_SAVE));

	return ret;
	}
#endif