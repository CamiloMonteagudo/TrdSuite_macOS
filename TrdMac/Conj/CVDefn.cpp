// CVDefn.cpp: implementation of the CDicRef class.
//
//////////////////////////////////////////////////////////////////////

#include "RootLanguageData.h"
#include "CVDefn.h"

//IMPLEMENT_SERIAL(CRootLanguageData,CObject,VERSIONABLE_SCHEMA|1);
//IMPLEMENT_SERIAL(CDicRef,CRootLanguageData,VERSIONABLE_SCHEMA|1);

#if defined(MERCADO)
    #define DIC_FLAG     DICC_MEM|DICC_READONLY
#else
    #define DIC_FLAG     DICC_MEM

///////////////////////////////////////////////////////////////////
// Función interna para salvar diccionarios
///////////////////////////////////////////////////////////////////
/*static void SaveDict(CDict &Dic)
	{
	Dic.SetModifiedFlag(TRUE);
	Dic.Save();
	}*/
#endif

//////////////////////////////////////////////////////////////////////
// Serialize
//////////////////////////////////////////////////////////////////////

void CDicRef::Serialize(CArchive &ar)
	{
	m_PIDic->Serialize(ar);
	m_SufDic->Serialize(ar);
	m_AuxVerbs->Serialize(ar);
	m_TablaV->Serialize(ar);
	m_TablaPatron->Serialize(ar);
	m_TablaRef->Serialize(ar);
	m_FIDic->Serialize(ar);
	m_SufFDic->Serialize(ar);
	m_PluralIDic->Serialize(ar);
	m_SufPluralDic->Serialize(ar);
	m_AdjSupIDic->Serialize(ar);
	m_AdjCompIDic->Serialize(ar);
	m_AdjSupSufDic->Serialize(ar);
	m_AdjCompSufDic->Serialize(ar);
	m_PrefVerbDic->Serialize(ar);
	m_DecNounIDic->Serialize(ar);
	m_DecNounSufDic->Serialize(ar);
	m_DecAdjIDic->Serialize(ar);
	m_DecAdjSufDic->Serialize(ar);
	}


///////////////////////////////////////////////////////////////////////////////
//	Carga todos los elementos de la clase
//  con la variable MERCADO activada
///////////////////////////////////////////////////////////////////////////////

bool CDicRef::Load()
	{
	///////////////////////////////////////////////////////////////////////////////
	// si se ha definido la variable MERCADO
	///////////////////////////////////////////////////////////////////////////////
#if defined(MERCADO)
	return LoadSerialize(MakeName(LF"CVData",false,true, EXT_CNJ_LOAD));
#else
	///////////////////////////////////////////////////////////////////////////////
	// si no se ha definido la variable MERCADO
	///////////////////////////////////////////////////////////////////////////////
	//---------------------------------------------------------------
	// Carga el diccionario de verbos irregulares
	//---------------------------------------------------------------
	if(LoadDictionary(m_PIDic,"CIgW",DIC_FLAG,true)==false)
		return false;
	//4-------------------------------------------------
	// Carga el diccionario de sufijos verbales
	//-------------------------------------------------
	if(LoadDictionary(m_SufDic,"CSuf",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------
	// Carga el diccionario de verbos auxiliares para tiempos compuestos
	//----------------------------------------------
	if(LoadDictionary(m_AuxVerbs,"CAuxV",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------
	// Carga la tabla de Conjugaciones
	//----------------------------------------------
	if(LoadTabla(m_TablaV,"CVerb")==false)
		return false;
	//----------------------------------------------
	// Carga la tabla de patrones de negación y prefijos
	//----------------------------------------------
	if(LoadTabla(m_TablaPatron,"CPatN")==false)
		return false;
	//----------------------------------------------
	// Carga la tabla de referencias
	//----------------------------------------------
	CStringA TblName;

	m_TablaRef->SetName(MakeName(TblName,"CRefT",false,true,EXT_CNJ_LOAD));
	if( m_TablaRef->Load() == FALSE )
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de femeninos irregulares
	//----------------------------------------------------------
	if(LoadDictionary(m_FIDic,"CIFm",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de sufijos de femeninos
	//----------------------------------------------------------
	if(LoadDictionary(m_SufFDic,"CSFm",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de plurales irregulares
	//----------------------------------------------------------
	if(LoadDictionary(m_PluralIDic,"CIPl",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de sufijos de plurales
	//----------------------------------------------------------
	if(LoadDictionary(m_SufPluralDic,"CSPl",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Superlativos Irregulares
	//----------------------------------------------------------
	if(LoadDictionary(m_AdjSupIDic,"CISup",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Sufijos de Superlativos
	//----------------------------------------------------------
	if(LoadDictionary(m_AdjSupSufDic,"CSSup",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Comparativos Irregulares
	//----------------------------------------------------------
	if(LoadDictionary(m_AdjCompIDic,"CIComp",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Sufijos de Comparativos
	//----------------------------------------------------------
	if(LoadDictionary(m_AdjCompSufDic,"CSComp",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Verbos con Prefijos separables y no separables
	//----------------------------------------------------------
	if(LoadDictionary(m_PrefVerbDic,"CPrefV",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Declinaciones Irregulares de Sustantivos
	//----------------------------------------------------------
	if(LoadDictionary(m_DecNounIDic,"CIDecN",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Sufijos de Declinaciones de Sustantivos
	//----------------------------------------------------------
	if(LoadDictionary(m_DecNounSufDic,"CSDecN",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Declinaciones Irregulares de Adjetivos
	//----------------------------------------------------------
	if(LoadDictionary(m_DecAdjIDic,"CIDecA",DIC_FLAG,true)==false)
		return false;
	//----------------------------------------------------------
	// Carga el diccionario de Sufijos de Declinaciones de Adjetivos
	//----------------------------------------------------------
	if(LoadDictionary(m_DecAdjSufDic,"CSDecA",DIC_FLAG,true)==false)
		return false;
	return true;
#endif
	}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
