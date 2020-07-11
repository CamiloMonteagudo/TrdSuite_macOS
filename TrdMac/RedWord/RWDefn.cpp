// RWDefn.cpp: implementation of the CRWDicRef class.
//
//////////////////////////////////////////////////////////////////////

#include "RootLanguageData.h"
#include "RWDefn.h"


//IMPLEMENT_SERIAL(CRootLanguageData,CObject,VERSIONABLE_SCHEMA|1);
//IMPLEMENT_SERIAL(CRWDicRef,CRootLanguageData,VERSIONABLE_SCHEMA|1);

#if defined(MERCADO)
    #define DIC_FLAG     DICC_MEM|DICC_READONLY
#else
    #define DIC_FLAG     DICC_MEM

#endif

//////////////////////////////////////////////////////////////////////
// Serialize
//////////////////////////////////////////////////////////////////////

void CRWDicRef::Serialize(CArchive &ar)
	{
	m_SAIDic->Serialize(ar, NULL);
	m_VIDic->Serialize(ar, NULL);
	m_SufVDic->Serialize(ar, NULL);
	m_SufSADic->Serialize(ar, NULL);
	m_PrefDic->Serialize(ar, NULL);
	}

///////////////////////////////////////////////////////////////////////////////
//	Carga todos los elementos de la clase
//  con la variable MERCADO activada
///////////////////////////////////////////////////////////////////////////////

bool CRWDicRef::Load()
	{
	return LoadSerialize(MakeName(LF"RWData",false,true, EXT_RED_LOAD));
	}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
