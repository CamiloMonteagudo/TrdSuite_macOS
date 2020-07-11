// GetWordType.h: interface for the CGetWordType class.
// Busca el tipo de la palabra en el caso de que esta contenga lenguaje de
// diccionario lo ejecuta para llegar a un tipo de dos letras
// si el tipo tiene marcas para la especialidad determina el tipo de dos
// letras según la especialidad principald del texto que se traduce
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETWORDTYPE_H__002619E1_FC3C_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_GETWORDTYPE_H__002619E1_FC3C_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CGetWordType : public CStepInSentence
{
public:
	CGetWordType(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{}

protected:

	virtual CDict *GetDictionary()
		{
		ASSERT(GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic() != NULL);
		return GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic();
		}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		CDict* dict = GetDictionary();
    CStringA sType(ptrWord->m_sTipo);
    if(sType.GetLength() == 2)
      {
			#ifdef _DEBUG
			CStringA tmp;
			BOOL encontre = dict->IsKey(sType);
			if(encontre == FALSE)
				LOG_ERROR("Error en tipo en diccionario: %s", CS2SZ(ptrWord->m_Key));
			#endif
      }
    if(sType.GetLength() != 2)
      {
			if(sType.GetLength() > 2 && (sType[0] == '(' || sType[0] == '\"'))
				{
				CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
                
				l.Execute(ptrNode,sType);
				ptrWord->m_sTipo = sType;
				CStringA tmp;
				BOOL encontre = dict->GetKey(sType,tmp);
				if(encontre == TRUE)
					ptrWord->m_sTipo = sType;
			#ifdef _DEBUG
				else    
					LOG_ERROR("Error en tipo en diccionario: %s", CS2SZ(ptrWord->m_Key));
			#endif
				}
			else
				ptrWord->m_sTipo = ptrWord->m_sTipo.Left(2);
      }
    ASSERT(ptrWord->m_sTipo.IsEmpty() == FALSE);
		return ptrNode;
		}
};

#endif // !defined(AFX_GETWORDTYPE_H__002619E1_FC3C_11D2_8926_0060972DBBB5__INCLUDED_)
