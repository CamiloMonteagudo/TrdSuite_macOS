// ListOfRoots.h: interface for the CListOfRoots class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTOFROOTS_H__0F86D044_256E_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_LISTOFROOTS_H__0F86D044_256E_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FindRootWordInDic.h"

struct STranslationString
  {
  char    m_sType[3];
  Grado   m_Degree;
  Genero  m_Gender;
  Numero  m_Number;
  Persona m_Person;
  Tiempo  m_Time;
  int     m_Mode;
  Numero  m_NounNumber;  // número del sustantivo
  char    m_sRoot[100];
  char    m_sPreffix[50];
  };


class CListOfRoots : public CTranslationDataObject, public CPtrArray
  {
  protected:
    CArrayValue<STranslationString*> *m_ptrArray;
    int m_iSizeItem;
    LPCWORD m_ptrWord;

  public:
    CListOfRoots(CTranslationUser *ptrTranslationUser,CArrayValue<STranslationString*> *ptrArrayItem, LPCWORD ptrWord):CTranslationDataObject(ptrTranslationUser)
      {
      ASSERT(ptrArrayItem != NULL);
      ASSERT(ptrWord != NULL);
      m_ptrArray = ptrArrayItem;
      m_ptrWord = ptrWord;
      CompactList();
      m_iSizeItem = m_ptrArray->GetSize();
      }
    virtual ~CListOfRoots()
      {}
    bool GetData();                            // Llena el campo tipo y traducción para la palabra 

  protected:
    bool GetTranslation();                     // 
    int CompareItem();
    CArrayValue<STranslationString*> *GetPtrArray()  // arreglo de reducciones para la palabra de trabajo
      {return m_ptrArray;}
    STranslationString *GetArrayPos(int size); // arreglo estático segun size
    STranslationString *GetArrayItem(int iPos)  // arreglo de reducciones para la palabra de trabajo
      {return m_ptrArray->GetAt(iPos);}
    void DeleteArrayItemAt(int iPos)  // arreglo de reducciones para la palabra de trabajo
      {
      if(iPos < m_ptrArray->GetSize())
        {
        STranslationString *ptr = GetArrayItem(iPos);
        m_ptrArray->RemoveAt(iPos);
        delete ptr;
        }
      }

    int GetRootCount(CStrArray *ptrArray);   // cantidad de raices diferentes a la que se reduce la palabra

    CDictMain *GetMainDict()                   // obtine el diccionario general de palabras
      {
      return GetTranslationUserPtr()->GetPtrTranslationDic();
      }

    void ConvertVerbs();
    void FindVerbConvertion(bool bTransitive);
    void CompactList();
    void CompactItem(int cant,STranslationString *Reduce, int i);
  };

#endif // !defined(AFX_LISTOFROOTS_H__0F86D044_256E_11D3_8926_0060972DBBB5__INCLUDED_)
