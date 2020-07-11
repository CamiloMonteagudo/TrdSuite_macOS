// TranslatePreffix.h: interface for the CTranslatePreffix class.
// Traduce los prefijos de la palabra que lo contengan y forma 
// la traducción de la palabra con el prefijo traducido
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATEPREFFIX_H__F679792F_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATEPREFFIX_H__F679792F_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TranslateWordsInSentence.h"

class CTranslatePreffix : public CTranslateWordsInSentence  
	{
public:
	CTranslatePreffix(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CTranslateWordsInSentence(ptrTranslationUser,ptrSentence)
		{}

protected:

	virtual CDict *GetPreffixDictionary()
		{
		ASSERT(GetPtrTranslationData()->GetPtrTranslationSuffixDic() != NULL);
		return GetPtrTranslationData()->GetPtrTranslationSuffixDic();
		}

	virtual CDict *GetSuffixDictionary()
		{
		ASSERT(GetPtrTranslationData()->GetPtrTranslationPreffixDic() != NULL);
		return GetPtrTranslationData()->GetPtrTranslationPreffixDic();
		}

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
    if(ptrWord->m_sTipo == "FS" && ptrWord->m_sTipo2.IsEmpty() == false)
      ptrWord->m_sTipo = ptrWord->m_sTipo2;

    if(ptrWord->m_sKeySufijo.IsEmpty() == FALSE) // si la palabra tiene sufijo de reducción
      {
			CStringA sufijo("$"),data(ptrWord->m_Data),tipo(ptrWord->m_sTipo),trad(ptrWord->m_Trad);
			sufijo += ptrWord->m_sKeySufijo;
			sufijo += "$";
		  BOOL ret = GetSuffixDictionary()->FindSuffix(sufijo, ptrWord->m_Data);

      if (ret == TRUE)
        {         
        CStringA tip(ptrWord->m_sTipo); // salva el tipo real de la palabra intermedia
				GetDataInDic(ptrWord);
                
        ptrWord->m_sTipo = ptrWord->m_sTipo2;
        CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
        l.Execute(ptrNode,ptrWord->m_Data);
        ptrWord->m_Trad = trad;
				ptrWord->m_Data = data;

        CSGrep setsufijo("@@",ptrWord->m_Trad);    // compile a regexp to sgrep
        if(ptrWord->m_sSufijo.IsEmpty() == FALSE)
          {
          setsufijo.Replace(ptrWord->m_sSufijo,TRUE);
          ptrWord->m_Data = ptrWord->m_sSufijo;
          for(;;)
            {
            int ini = ptrWord->m_Data.Find('['); 
            int fin = ptrWord->m_Data.Find(']'); 
                        
            if( ini != -1 && fin != -1)
              {                      
              LPCSTR tmp = CS2SZ(ptrWord->m_Data);

              CStringA left(tmp,ini);
              CStringA right(tmp+fin+1,ptrWord->m_Data.GetLength()-(fin+1));
              CStringA corchete = ptrWord->m_Data.Mid(ini+1,fin-(ini+1));
              PutSufijo(left,corchete,right);
              ptrWord->m_Data = left;
              }
            else
              break;    
            }
          }
        ptrWord->m_sTipo2 = ptrWord->m_sTipo; 
        ptrWord->m_sTipo  = tip; 
				ptrWord->m_Traducida = FALSE;

				CTranslateWordsInSentence::Execute(ptrNode,ptrWord); 
				}
			}

		if(ptrWord->m_sKeyPrefijo.IsEmpty() == FALSE)		// si la palabra tiene prefijo de reducción
			{
			CStringA prefijo("$"),data(ptrWord->m_Data),tipo(ptrWord->m_sTipo),trad(ptrWord->m_Trad);
			prefijo += ptrWord->m_sKeyPrefijo;
			prefijo += "$";
			BOOL ret = GetPreffixDictionary()->FindPreffix(prefijo,ptrWord->m_Data);

			if(ret == TRUE)
				{
				CStringA tip(ptrWord->m_sTipo); // salva el tipo real de la palabra intermedia
				//ptrWord->m_Data = data;
				GetDataInDic(ptrWord);
								 
				ptrWord->m_sTipo = ptrWord->m_sTipo2;
				CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
				l.Execute(ptrNode,ptrWord->m_Data);
				ptrWord->m_Trad = trad;
				ptrWord->m_Data = data;

				if(ptrWord->m_sPrefijo.IsEmpty() == FALSE && ptrWord->m_Traducida == TRUE)
					{
					CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
					l.Execute(ptrNode,ptrWord->m_sPrefijo);         
					
					CSGrep setprefijo("@@",ptrWord->m_Trad);    // compile a sgrep
					setprefijo.Replace(ptrWord->m_sPrefijo);
					ptrWord->m_Trad = ptrWord->m_sPrefijo;
					}
				ptrWord->m_sTipo2 = ptrWord->m_sTipo; 
				ptrWord->m_sTipo  = tip; 
				}
			}
		return ptrNode;
		}
	};

#endif // !defined(AFX_TRANSLATEPREFFIX_H__F679792F_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
