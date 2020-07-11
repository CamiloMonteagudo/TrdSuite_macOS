// CmdCom.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "UtilFun.h"
#include "COra.h"
#include "CmdDcc.h"
#include "Cmd.h"
#include "CmdItKey.h"
//#include "afxtempl.h"


IMPLEMENT_SERIAL(CDccComodin,CObject,1);
IMPLEMENT_SERIAL(CDccCmdFrase,CObject,1);

//IMPLEMENT_SERIAL(CCmdItem,CObject,1);
//IMPLEMENT_SERIAL(CItemArray,CObArray,1);
//IMPLEMENT_SERIAL(CCompiledCmd,CObject,1);
//IMPLEMENT_SERIAL(CCompiledCmdArray,CObArray,1);
IMPLEMENT_SERIAL(CCompiledRuleTable,CObject,1);

//IMPLEMENT_SERIAL(CItemKeyWord,CObject,1);
//IMPLEMENT_SERIAL(CActionPhrase,CObject,1);
//IMPLEMENT_SERIAL(CMatchItem,CObject,1);
//IMPLEMENT_SERIAL(CStringWordArrayEntry,CObject,1);
//IMPLEMENT_SERIAL(CKeyWordEntry,CObject,1);

//IMPLEMENT_SERIAL(CObOrderArray,CObject,1);
//IMPLEMENT_SERIAL(CObOrderStrArray,CObject,1);
//IMPLEMENT_SERIAL(CFixedAndOrderStrArray,CObject,1);

/////////////////////////////////////////////////////////////////////////////

void CDccComodin::Serialize(CArchive &ar)
  {
	//if(ar.IsLoading())
	//	m_RuleTable.Inicialize();

	m_RuleTable.Serialize(ar);
	}

/////////////////////////////////////////////////////////////////////////////
// Serializa el diccionario de comodines de Coordinación y conjugación de verbos
/////////////////////////////////////////////////////////////////////////////

void CDccCmdFrase::Serialize(CArchive &ar)
  {
  CDccComodin::Serialize(ar);
	if(ar.IsStoring())
		{
		/*WORD tmp = (WORD) m_bSalirAlPrimero;
		ar << tmp;*/
		}
	else
		{
		WORD tmp = (WORD) m_bSalirAlPrimero;
		ar >> tmp;
		m_bSalirAlPrimero = (bool)((tmp)?true:false);
		}
	}

