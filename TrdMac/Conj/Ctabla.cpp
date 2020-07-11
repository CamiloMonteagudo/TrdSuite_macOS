////////////////////////////////////////////////////////////////////////////
//   Class CTabla                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         23/8/94                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa class para el trabajo con tablas                        // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "CTabla.h"
#include "WinUtil.h"

////////////////////////////////////////////////////////////////////////////

CTabla::~CTabla()
  {
  for(int i = 0; i < m_Array.GetSize(); i++)
    {
    CSmartStrArray *a = m_Array[i];
    if( a!= NULL )
      {
      delete a;
      a = NULL;
      }
    }
  m_Array.RemoveAll();    
  }

////////////////////////////////////////////////////////////////////////////

CString CTabla::GetCelda(int fila, int col)
  {
	//COLLECT_INTERVAL("CTabla::GetCelda"); // 0.08 238846
  ASSERT(fila < m_Array.GetSize());

  //if(fila >= m_Array.GetSize())
  //  return CString("");
  
  CSmartStrArray *array = m_Array[fila];
  ASSERT(array != NULL);
  ASSERT(col < array->GetCount());
  return array->GetAt(col);
  }

////////////////////////////////////////////////////////////////////////////

LPCSTR CTabla::GetCeldaSZ(int fila, int col)
  {
	//COLLECT_INTERVAL("CTabla::GetCeldaSZ"); // 0.04 159346
  ASSERT(fila < m_Array.GetSize());

  //if(fila >= m_Array.GetSize())
  //  return CString("");
  
  CSmartStrArray *array = m_Array[fila];
  ASSERT(array != NULL);
  ASSERT(col < array->GetCount());
  return array->GetAtSZ(col);
  }

////////////////////////////////////////////////////////////////////////////

void CTabla::LoadHideArray(CArchive &ar, CSmartStrArray *array)
  {
  WORD l;
  ar >> l;              
  array->Preallocate(l, 25);
  for(int j = 0; j < l; j++)
    {     
    CStringA s;
    ar >> s;
    HideData(s);
    array->Add(s);
    }
  
  array->Compress();
  }

////////////////////////////////////////////////////////////////////////////

void CTabla::Serialize(CArchive &ar)
  {     
  //CObject::Serialize(ar);

  if(ar.IsStoring())
    {
    /*ar << (WORD) m_Array.Count();
    for(int i =0; i < m_Array.Count(); i++)
      {
      CStrArray *array = (CStrArray *)m_Array.GetAt(i);
      ASSERT(array != NULL);
      SaveHideArray(ar,array);
      }*/    
    }
  else
    {
    WORD nOldSize;
    ar >> nOldSize;
    m_Array.SetSize(nOldSize);

    for (int i = 0; i < m_Array.GetSize(); i++)
      {
      CSmartStrArray *array = new CSmartStrArray;
      LoadHideArray(ar,array);
      m_Array.SetAt(i,array);
      }  
    }
  if(ar.IsStoring())
    {     
    //SaveHideArray(ar,&m_NameFila);
    //SaveHideArray(ar,&m_NameCol);
    }
  else
    {    
    LoadHideArray(ar,&m_NameFila);
    LoadHideArray(ar,&m_NameCol);
    }
  }   

/////////////////////////////////////////////////////////////////// 

void CTabla::HideData(CString& aData)  // Hide data
  {
  for(int i = 0; i < aData.Length(); i++)
    {
    char c = aData.GetAt(i);
    c = 255 - c;
    aData.SetAt(i, c);
    }
  }

////////////////////////////////////////////////////////////////////////////

void CTabla::GetNameFila(CStringA& aName, int fila)
  {
  ASSERT(fila < m_NameFila.GetCount());
  aName = m_NameFila[fila];
  }

////////////////////////////////////////////////////////////////////////////

void CTabla::GetNameCol(CStringA& aName, int col)
  {
  ASSERT(col < m_NameCol.GetCount());
  aName = m_NameCol[col];
  }

////////////////////////////////////////////////////////////////////////////

int CTabla::FindFila(const CStringA& name)
  {
	return m_NameFila.Find(name);
  }

int CTabla::FindCol(const CStringA& name)
  {
	return m_NameCol.Find(name);
  }
