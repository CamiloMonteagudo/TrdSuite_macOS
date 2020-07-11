#include "reftabla.h"

//IMPLEMENT_SERIAL(CDataConj,CObject,0);
//IMPLEMENT_SERIAL(CRefTabla,CObject,0);

/////////////////////////////////////////////////////////////////////////////
// Implementation of CDataConj
/////////////////////////////////////////////////////////////////////////////

void CDataConj::Serialize(CArchive &ar)
  {
  //CObject::Serialize(ar);
  if(ar.IsStoring())
    {
    /*ar << m_Nombre;
    ar << m_Modo;
    ar << m_Tiempo;
    ar << (DWORD)m_Compuesto;*/
    }
  else
    {
    ar >> m_Nombre;
    ar >> m_Modo;
    ar >> m_Tiempo;

    DWORD comp;
    ar >> comp;
    m_Compuesto = (comp != 0);
    }

  }     

#define CONJ_COUNT  84

/////////////////////////////////////////////////////////////////////////////
// Implementation of CRefTabla
/////////////////////////////////////////////////////////////////////////////
/*BOOL CRefTabla::Load()
  {
  BOOL ret = FALSE;*/
  /*CFile f;

  if( f.Open( CA2CT(m_Name),CFile::modeRead) ) 
    {
    CArchive ar( &f, CArchive::load);
    Serialize(ar);
    ar.Close();
    ret = TRUE;
    }*/      
/*
  if( CONJ_COUNT > m_TabColName.GetSize())
    {
    for( int j=m_TabColName.GetSize(); j < CONJ_COUNT; j++)
      {
      m_TabColName.Add(CString(""));
      m_TabColIndex.AppendL(0);
      }
    }


  return ret;          

  }*/

/////////////////////////////////////////////////////////////////////////////
BOOL CRefTabla::Save()
  {
  BOOL ret = FALSE;
  /*CFile f;

  if( f.Open( CA2CT(m_Name),CFile::modeCreate|CFile::modeWrite)) 
    {
    CArchive ar( &f, CArchive::store);
    Serialize(ar);
    ar.Close();
    ret = TRUE;
    }*/

  /*int index;
  CStringA col;
  for( int j=0; j < CONJ_COUNT; j++)
    {
    index = m_TabColIndex.GetAt(j);
    col = m_TabColName.GetAt(j);
    }*/

  return ret;          
  }

/////////////////////////////////////////////////////////////////////////////
void CRefTabla::Serialize(CArchive &ar)
  {     
  //CObject::Serialize(ar);

  if(ar.IsStoring())
    {
    /*//guardando tabla de referencia
    ar << (WORD) m_TabColName.GetSize();
    int i =0;
    for(i =0; i < m_TabColName.GetSize(); i++)
      {
      ar << m_TabColName[i];
      ar << m_TabColIndex[i];
      } 

    //guardando pronombres reflexivos y personales
    for(i =0; i < 6; i++)
      {
      ar << m_PronomPerson[i];
      ar << m_PronomReflex[i];
      }    

    ar << (WORD) m_DataConjArray.GetSize();
    for(int j =0; j < m_DataConjArray.GetSize(); j++)
      m_DataConjArray[j]->Serialize(ar);

    //guardando terminaciones de verbos reflexivos y sus raíces
    ar << (WORD) m_TerminacReflex.GetSize();
    for(i =0; i < m_TerminacReflex.GetSize(); i++)
      {
      ar << m_TerminacReflex[i];
      ar << m_TerminacVerbal[i];
      }*/
    }
  else
    {
    WORD nOldSize;
    ar >> nOldSize;
    m_TabColName.Preallocate(nOldSize, 15);
    m_TabColIndex.SetSize(nOldSize);
    m_PronomPerson.Preallocate(6, 5);
    m_PronomReflex.Preallocate(6, 5);

    int i =0;
    for(i =0; i < nOldSize; i++)
      {
      CStringA s;
      ar >> s;
      m_TabColName.Add(s);
      ar >> m_TabColIndex[i];
      }    
    m_TabColName.Compress();

    
    for(i =0; i < 6; i++)
      {
      CStringA s;
      ar >> s;
      m_PronomPerson.Add(s);
      ar >> s;
      m_PronomReflex.Add(s);
      }    
    m_PronomPerson.Compress();
    m_PronomReflex.Compress();

    WORD nConjSize;
    ar >> nConjSize;
    m_DataConjArray.SetSize(nConjSize);
    for(int j =0; j < m_DataConjArray.GetSize(); j++)
      {
      m_DataConjArray[j] = new CDataConj();
      m_DataConjArray[j]->Serialize(ar);
      }

    WORD nTerRefSize;
    ar >> nTerRefSize;
    m_TerminacReflex.Preallocate(nTerRefSize, 10);
    m_TerminacVerbal.Preallocate(nTerRefSize, 10);
    for(i =0; i < nTerRefSize; i++)
      {
      CStringA s;
      ar >> s;
      m_TerminacReflex.Add(s);
      ar >> s;
      m_TerminacVerbal.Add(s);
      }

    m_TerminacReflex.Compress();
    m_TerminacVerbal.Compress();
    }



  }

/////////////////////////////////////////////////////////////////////////////
