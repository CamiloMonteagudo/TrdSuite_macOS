// FindRootWordInDic.cpp: implementation of the CFindRootWordInDic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindRootWordInDic.h"

//////////////////////////////////////////////////////////////////////
//  Reduce la palabra
//////////////////////////////////////////////////////////////////////
// Function name	: CFindRootWordInDic::UpdateList
// Description	    : Actualiza la lista de raices para la palabra dada
// Return type		: bool 
//////////////////////////////////////////////////////////////////////

// solo se llama desde UpdateWordData()
bool CFindRootWordInDic::UpdateList()
  {
	//COLLECT_INTERVAL("UpdateList"); // 2.24 9534
  ASSERT(m_ptrWord != NULL);

	CLanguageData *srcLang = GetPtrSourceLanguageData();
	//HREDDATA hDat1;
	//HREDDATA* hDat = &hDat1;
	HREDDATA* hDat = NULL;
	if (!srcLang->m_RedCache.FindAdd(m_ptrWord->m_Key, hDat))
		{
		RWSetGenDict( srcLang->m_hRedW, GetTranslationUserPtr()->GetPtrTranslationDic(), false );

	  *hDat = RWReduce( srcLang->m_hRedW, m_ptrWord->m_Key );
		}

	//TRACE("Reducing: %s", m_ptrWord->m_Key.c_str());
  if(*hDat != NULL)
    {// tipos base VV,VA,BE,HA,SS,AA,AI
		//RWDump(*hDat);
    int iMaxIndex = (int)RWGetCount( *hDat );
    for (int i = 0; i < iMaxIndex; i++)
      {
      STranslationString *Root = (STranslationString *)new STranslationString;

      ASSERT( RWGetType   (*hDat,i).GetLength() == 2  );
      ASSERT( RWGetPreffix(*hDat,i).GetLength() < 50  );
      ASSERT( RWGetRoot   (*hDat,i).GetLength() < 100 );

      strcpy_s( Root->m_sType    ,3   , CS2SZ(RWGetType   (*hDat,i)) );
      strcpy_s( Root->m_sPreffix ,50  , CS2SZ(RWGetPreffix(*hDat,i)) );
      strcpy_s( Root->m_sRoot    ,100 , CS2SZ(RWGetRoot   (*hDat,i)) );

      Root->m_Mode   = (Modo)   RWGetMood  (*hDat,i);
      Root->m_Time   = (Tiempo) RWGetTime  (*hDat,i);
      Root->m_Degree = (Grado)  RWGetDegree(*hDat,i);
      Root->m_Person = (Persona)RWGetPerson(*hDat,i);
      Root->m_Number = (Numero) RWGetNumber(*hDat,i);
      Root->m_Gender = (Genero) RWGetGender(*hDat,i);


      if(strchr(Root->m_sRoot,'|') != NULL)
        m_ptrWord->m_Key = Root->m_sRoot;

      CStringA tmp;
      tmp = RWGetCDirect(*hDat,i);
      if(tmp.IsEmpty() == FALSE)
        SetDirectPronoun(tmp);

      tmp = RWGetCIndirect(*hDat,i);
      if(tmp.IsEmpty() == FALSE)
        SetIndirectPronoun(tmp);

      m_ListOfRoot.Add(Root);
      }

    //RWFreehData( hDat1 );
    return true;
    }
  return false;
  }

//////////////////////////////////////////////////////////////////////
// Function name	: CFindRootWordInDic::UpdateWordData
// Description	    : Actualiza los campos tipo y traducción de la
//                    palabra según la Raiz encontrada
// Return type		: bool 
//////////////////////////////////////////////////////////////////////
// este metodo siempre se llama justo despues del constructor
bool CFindRootWordInDic::UpdateWordData()
  {
	//COLLECT_INTERVAL("UpdateWordData"); // 2.32
  ASSERT(m_ptrWord != NULL);

  if(UpdateList())
    {
    CListOfRoots List(GetTranslationUserPtr(), &m_ListOfRoot, m_ptrWord);
    return List.GetData();
    }
  return false;
  }
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
