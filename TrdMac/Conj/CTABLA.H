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
#ifndef _CTABLA_H_
#define _CTABLA_H_

#include "WinUtil.h"
#include "Dict.h"
#include "UtilFun.h"

class CTabla
  { 
  //DECLARE_SERIAL(CTabla);

  public:
    int FindCol(const CStringA& name);
    int FindFila(const CStringA& name);
    CPointerArray<CSmartStrArray> m_Array;
    CSmartStrArray m_NameFila;
    CSmartStrArray m_NameCol;
    CStringA m_Name;        
    BOOL m_NeedSave;

  public:    

    CTabla(const CStringA& name = CANull)
      {                         
      m_Name = name;
      }

    void SetName(const CStringA& name )
      {
      m_Name = name;
      }

    ~CTabla();
    CString GetCelda(int fila, int col);
    LPCSTR GetCeldaSZ(int fila, int col);
    void SetCelda(int fila, int col, const CString& s);
    void InsFila(int fila, const CString& aNameFila);
    void InsCol(int col, const CString& aNameCol, const CString& s);
    void AddFila(const CString& aNameFila);
    void AddCol(const CString& aNameCol, const CString& s);
    void DelFila(int fila);
    void DelCol(int col);
    void HideData(CString& aData);
    void GetNameFila(CStringA& aName, int fila);
    void GetNameCol(CStringA& aName, int col);
    BOOL LoadTxt(const CStringF& filename);
    BOOL SaveTxt(const CStringF& filename);
    BOOL Load();
    BOOL Save();
    BOOL LoadFromDict(CDict *dict, CSmartStrArray *namecol);
    //BOOL SaveToDict(CDict *dict);
    //void SaveHideArray(CArchive &ar,CStrArray *array);
    void LoadHideArray(CArchive &ar,CSmartStrArray *array);
    virtual void Serialize(CArchive &ar);
  };
#endif    
