// TranslationData.cpp: implementation of the CTranslationData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TranslationData.h"
#include "PeekMsg.h"

IMPLEMENT_SERIAL(CTranslationData,CObject,VERSIONABLE_SCHEMA|1);

///////////////////////////////////////////////////////////////////////////////
//	Carga todos los elementos de la clase
//  con la variable MERCADO activada
///////////////////////////////////////////////////////////////////////////////

bool CTranslationData::Load(BOOL aLoadLangData)
	{
	///////////////////////////////////////////////////////////////////////////////
	// si se ha definido la variable MERCADO
	///////////////////////////////////////////////////////////////////////////////
	//--------------------------------------------------------
	// Carga el diccionario general como Mapped file read-only
	//--------------------------------------------------------

	{
	//COLLECT_INTERVAL("Load-MainDict");
//	CStringA sFileName = MakeName(LF".dic", true, false, LF".dic");

  m_TranslationDict = CDictGen::Get( lngSrc, lngDes );
	if (!m_TranslationDict) return false;
  
  m_TranslationDict->ClearCache(TRUE);
  }
		
	if(LoadSerialize(MakeName(LF"",true,false, EXT_DAT_LOAD)) == false)
		return false;

	if (aLoadLangData)
		{
		if(m_SourceLanguage.Load() == false)
			return false;

		if(m_TargetLanguage.Load(true) == false)
			return false;
		}
		
	return true;
	}

#ifdef ACTIVE_SAVE

bool CTranslationData::Save()
	{
	//if(SaveDictionary(m_TranslationDict,LF".dic",DICC_FILE|DICC_READONLY) == FALSE)
	//	return false;
		
	if(SaveSerialize(MakeName(LF"",true,false, EXT_DAT_SAVE)) == false)
		return false;

	/* estas dos se salvan aparte
	if(m_SourceLanguage.Save() == false)
		return false;

	if(m_TargetLanguage.Save(true) == false)
		return false;*/
		
	return true;
	}

#endif

///////////////////////////////////////////////////////////////////////////////
// Serialize
///////////////////////////////////////////////////////////////////////////////

void CTranslationData::Serialize(CArchive &ar)
	{
	//COLLECT_INTERVAL("CTranslationData::Serialize"); // 0.31 de 1.22
	CStringA sDir = m_SourceLanguage.GetIsoName() + '2' + m_TargetLanguage.GetIsoName();

	if (ar.hasError()) return;
	m_InterchangePattern->Serialize(ar, sDir + "InterchangePattern.txt");
	if (ar.hasError()) return;
	m_TranslationPreffix->Serialize(ar, sDir + "TranslationPreffix.txt");
	if (ar.hasError()) return;
	m_TranslationSuffix->Serialize(ar, sDir + "TranslationSuffix.txt");
	if (ar.hasError()) return;
	m_DataDictionary->Serialize(ar, sDir + "DataDictionary.txt");
	if (ar.hasError()) return;

	m_CmdFindTranslation.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdFindQTranslation.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdFindITranslation.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdFindFTranslation.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdFindVTranslation.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdFindTTranslation.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdCoorConjDict.Serialize(ar);
	if (ar.hasError()) return;
	m_CmdCoorConj.Serialize(ar);
	if (ar.hasError()) return;
  }


///////////////////////////////////////////////////////////////////////////////

const CStringA& CTranslationData::GetInsertWordNumber(CStringA &sWord, int iPos)
	{
	if (m_SourceLanguage.GetIsoName() == "It" && m_TargetLanguage.GetIsoName() == "Es")
		{
		sWord.Empty();
		return sWord;
		}

	CStringA sTempString("INSERT", CStringA::FromInt_(iPos));
	//sTempString.Format("INSERT%i",iPos);
	GetPtrDataDic()->GetKey(sTempString,sWord);

	return sWord;
	}

///////////////////////////////////////////////////////////////////////////////

/*const CStringA& CTranslationData::GetAddWordNumber(CStringA &sWord, int iPos)
	{
	CStringA sTempString("ADD", CStringA::FromInt_(iPos));
	//sTempString.Format("ADD%i",iPos);
	GetPtrDataDic()->GetKey(sTempString,sWord);

	return sWord;
	}*/

///////////////////////////////////////////////////////////////////////////////
