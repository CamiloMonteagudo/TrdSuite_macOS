// ScanWordsInDictionary.cpp: implementation of the CScanWordsInDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanWordsInDictionary.h"
#include "FindRootWordInDic.h"
#include "ApiRed.h"
#include "LengDic.h"

/////////////////////////////////////////////////////////////////////////////
// Este metodo nunca se ejecuta pues es redefinido en sus clases derivadas
/////////////////////////////////////////////////////////////////////////////
POSITION CScanWordsInDictionary::Execute(POSITION ptrNode, LPCWORD ptrWord)
	{
	ASSERT(GetDictionary() != NULL);
  if (GetDictionary() != NULL && ptrWord->m_NoBusca == FALSE)
    {
		BOOL bFind = FindReducingWord(ptrWord,ptrNode);

    if ( bFind == FALSE || GetInsertAllWord() == true)
      {
			CStringA sKey;

      if(GetDictionary()->IsKey(ptrWord->m_Orig) == TRUE)
				sKey = ptrWord->m_Orig;
      else if(GetDictionary()->IsKey(ptrWord->m_Origlwr) == TRUE)
				sKey = ptrWord->m_Origlwr;
			else
				{
				if(ptrWord->m_Orig.IsEmpty() == FALSE && ptrWord->m_Orig.Find(' ') == -1)
					sKey = ptrWord->m_Orig;
				else if(ptrWord->m_Origlwr.IsEmpty() == FALSE && ptrWord->m_Origlwr.Find(' ') == -1)
					sKey = ptrWord->m_Origlwr;
				else
					sKey = ptrWord->m_Key;
				}

			UpdateFrecuency(sKey); // hace falta?
      }
    }

	return ptrNode;
	}

/////////////////////////////////////////////////////////////////////////////
// Busca palabra en diccionario reduciendo a la raiz
/////////////////////////////////////////////////////////////////////////////

BOOL CScanWordsInDictionary::FindReducingWord(LPCWORD ptrWord,POSITION nodo)
    {
    BOOL found = FALSE;    // si se encontro la palabra en dicc

    // búsqueda de cualquier palabra
    found = FindInDic(ptrWord);
        
    if(found == FALSE)
			{
      CFindRootWordInDic find(GetTranslationUserPtr(),ptrWord);
      found = find.UpdateWordData();
      }

    if(found == FALSE)
			{
			ptrWord->m_Key = ptrWord->m_Orig;
      found = FindInDic(ptrWord);
			}

    if(found == FALSE)     // prueba primero si es un verbo irregular
      found = FindIrrgVerb(ptrWord);
            
    if(found == FALSE)
      found = FindKeySufijo(ptrWord,nodo);

    if(found == FALSE)     
      found = FindKeyPrefijo(ptrWord,nodo,false);

    return found;
    }
/////////////////////////////////////////////////////////////////////////////

BOOL CScanWordsInDictionary::FindIrrgVerb(LPCWORD ptrWord) 
    {      
		//COLLECT_INTERVAL("FindIrrgVerb"); // 0.02
    ASSERT(GetPtrSourceLanguageData()->GetPtrWordReductionDic() != NULL);
    
    BOOL ret = GetPtrSourceLanguageData()->GetPtrWordReductionDic()->GetKey(ptrWord->m_Key,ptrWord->m_Data);

    if (ret == TRUE)
      {
      GetDataInDic(ptrWord);
      ptrWord->m_Key = ptrWord->m_Data.Right(ptrWord->m_Data.GetLength()-1);
      ptrWord->m_Key = ptrWord->m_Key.Left(ptrWord->m_Key.GetLength()-1);
      CStringA tipo(ptrWord->m_sTipo);
      ret = FindInDic(ptrWord);
			if(tipo.IsEmpty()==FALSE)
				ptrWord->m_sTipo = tipo;
      }
    return ret;
    }

/////////////////////////////////////////////////////////////////////////////

BOOL CScanWordsInDictionary::FindKeySufijo(LPCWORD ptrWord,POSITION nodo) 
    {      
		//COLLECT_INTERVAL("FindKeySufijo"); // 0.08
    ASSERT(GetPtrSourceLanguageData()->GetPtrSuffixReductionDic() != NULL);
    CStringA key(ptrWord->m_Key);
    CStringA findkey(ptrWord->m_Key);
    CStringA tipo;       
    BOOL ret = FALSE;
        
    for(;;)    
        {
        ret = GetPtrSourceLanguageData()->GetPtrSuffixReductionDic()->FindSuffix(findkey, ptrWord->m_Data);
    
        if (ret == TRUE)
            {
			//m_sKeySufijo = findkey;
            GetDataInDic(ptrWord);
            //if(m_sTipo.GetLength())
            tipo = ptrWord->m_sTipo;
            CLengDic l(GetTranslationUserPtr(),GetPtrSentence()); // se ejecuta lenguaje para sufijo
            
            l.Execute(nodo,ptrWord->m_Data);
            int coma = ptrWord->m_Key.Find(',');
            if( coma == -1) 
                {
                ret = FindInDic(ptrWord);
                //break;
                }
            else
                {
                CStringA raiz(ptrWord->m_Key.Left(coma));
				ptrWord->m_sKeySufijo = findkey.Right(findkey.GetLength()-raiz.GetLength());
                CStringA resto(ptrWord->m_Key.Right(ptrWord->m_Key.GetLength()-coma-1));
                
                ret = FALSE;
                for(;ret == FALSE;)
                    {          
                    coma = resto.Find(',');
                    if( coma == -1) 
                        {
                        ptrWord->m_Key = raiz + resto;
                        ret = FindInDic(ptrWord);
                        break;
                        }
                    else
                        {
                        CStringA sufijo(resto.Left(coma));
                        resto = resto.Right(resto.GetLength()-coma-1);
                        ptrWord->m_Key = raiz + sufijo;
                        ret = FindInDic(ptrWord);
                        }    
                    }
                    
                    
                }    
            if(ret == FALSE)
                {          
                if(findkey.GetLength() > 1)
                    {
                    ptrWord->m_Key = key;
                    findkey = findkey.Right(findkey.GetLength()-1);
                    continue;
                    }
                else
                    break;  
                }
			/*****************
            if(m_sTipo2.IsEmpty() == FALSE && ret == TRUE)
                {         
                CStringA tip(m_sTipo); // salva el tipo real de la palabra intermedia
                
                m_sTipo = m_sTipo2;
                CLengDic l(ptrTranslationData,(LPCORACION)ptrOra);
                l.Execute(nodo,m_Data);
                m_Trad = m_Data;

                CSGrep setsufijo("@@",m_Trad);    // compile a regexp to sgrep
                if(m_sSufijo.IsEmpty() == FALSE)
                    {
                    setsufijo.Replace(m_sSufijo,TRUE);
                    m_Data = m_sSufijo;
                    for(;;)
                        {
                        int ini = m_Data.Find('['); 
                        int fin = m_Data.Find(']'); 
                        
                        if( ini != -1 && fin != -1)
                            {                      
                            const char *tmp = m_Data;
                            CStringA left(tmp,ini);
                            CStringA right(tmp+fin+1,m_Data.GetLength()-(fin+1));
                            CStringA corchete = m_Data.Mid(ini+1,fin-(ini+1));
                            PutSufijo(left,corchete,right);
                            m_Data = left;
                            }
                        else
                            break;    
                        }
                    }
                m_sTipo2 = tip; 
                }
			********/
            if(tipo.GetLength() && ret == TRUE)
                ptrWord->m_sTipo = tipo;
            break;    
            }            
        else
            break;    
        }    
    if(ret == FALSE)
      {           
      ptrWord->m_Key = key;
			ptrWord->m_Trad = ptrWord->m_Orig;
			ptrWord->m_sSufijo.Empty();
			ptrWord->m_sKeySufijo.Empty();
			ptrWord->m_sTipo.Empty();
			ptrWord->m_Data.Empty();
      }    
	  return ret;
    }

/////////////////////////////////////////////////////////////////////////////

BOOL CScanWordsInDictionary::FindKeyPrefijo(LPCWORD ptrWord,POSITION nodo,bool reduce) 
    {      
		//COLLECT_INTERVAL("FindKeyPrefijo"); // 0.03
    ASSERT(GetPtrSourceLanguageData()->GetPtrPreffixReductionDic() != NULL);
    CStringA key(ptrWord->m_Key);
    CStringA tipo;       
    
    BOOL ret = (GetPtrSourceLanguageData()->GetPtrPreffixReductionDic()->FindPreffix(ptrWord->m_Key,ptrWord->m_Data))?true:false;

    if (ret == TRUE)
      {
			ptrWord->m_sKeyPrefijo = ptrWord->m_Key;
      GetDataInDic(ptrWord);
      if(ptrWord->m_sTipo.GetLength())
				tipo = ptrWord->m_sTipo;
      CLengDic l(GetTranslationUserPtr(),GetPtrSentence()); // se ejecuta lenguaje para prefijo
        
      l.Execute(nodo,ptrWord->m_Data);
      ret = FindInDic(ptrWord);
      if(tipo.GetLength())
        ptrWord->m_sTipo = tipo;
      if(ret == FALSE)
        {
        ret = FindIrrgVerb(ptrWord);
        if(ret == FALSE)
          ret = FindKeySufijo(ptrWord,nodo);
        if(ret == FALSE && (ptrWord->m_Case != C_UPPER || ptrWord->m_Abreviatura == TRUE)&& reduce == true)
          {
          ptrWord->m_Trad = ptrWord->m_Key;
          ret = FindTradSufijo(ptrWord,nodo);
          if(ret)
            {
            FindTradPrefijo(ptrWord,nodo);
					
            //ASSERT(Translate->m_Idioma->RE_Traduce != NULL);
            //Translate->m_Idioma->RE_Traduce->Replace(m_Trad,TRUE);
            }
          }   
        }

			if(ret)
				ptrWord->m_sKeyPrefijo = ptrWord->m_sKeyPrefijo.Left(ptrWord->m_sKeyPrefijo.GetLength()-ptrWord->m_Key.GetLength());
      }            
		else
      ptrWord->m_Key = key;

    if(ret == FALSE)
      {           
      ptrWord->m_Key = key;
			ptrWord->m_Trad = ptrWord->m_Orig;
			ptrWord->m_sPrefijo.Empty();
			ptrWord->m_sKeyPrefijo.Empty();
			ptrWord->m_sTipo.Empty();
			ptrWord->m_Data.Empty();
      }    

    return ret;
    }
    
/////////////////////////////////////////////////////////////////////////////

bool CScanWordsInDictionary::FindTradSufijo(LPCWORD ptrWord,POSITION nodo) 
    {      
		COLLECT_INTERVAL("FindTradSufijo");
    ASSERT(GetPtrTranslationData()->GetPtrTranslationSuffixDic() != NULL);
    CStringA key(ptrWord->m_Key);
    CStringA tipo;       
    
    bool ret = (GetPtrTranslationData()->GetPtrTranslationSuffixDic()->FindSuffix(ptrWord->m_Key,ptrWord->m_Data))?true:false;

    if (ret == true)
        {
        GetDataInDic(ptrWord);          
        ptrWord->m_Traducida = TRUE;
        if(ptrWord->m_sTipo.GetLength())
            tipo = ptrWord->m_sTipo;
        CLengDic l(GetTranslationUserPtr(),GetPtrSentence()); // se ejecuta lenguaje para sufijo
        
        l.Execute(nodo,ptrWord->m_Data);
        if(tipo.GetLength())
            ptrWord->m_sTipo = tipo; 
        }            
    return ret;
    }

/////////////////////////////////////////////////////////////////////////////
#if 0
bool CScanWordsInDictionary::FindTipoPorSufijo(LPCWORD ptrWord) 
    {      
    ASSERT(GetPtrTranslationData()->GetPtrTranslationPreffixDic() != NULL);

    bool ret = (GetPtrTranslationData()->GetPtrTranslationPreffixDic()->FindSuffix(m_Key,m_Data))?true:false;

    if (ret == true)
        {
        GetDataInDic(ptrWord);
        ptrWord->m_Traducida = TRUE;
        }       
    return ret;
    }
#endif
/////////////////////////////////////////////////////////////////////////////

bool CScanWordsInDictionary::FindTradPrefijo(LPCWORD ptrWord,POSITION nodo) 
    {      
    ASSERT(GetPtrTranslationData()->GetPtrTranslationPreffixDic() != NULL);
    CStringA key(ptrWord->m_Key);
    CStringA tipo;       
    
    bool ret = (GetPtrTranslationData()->GetPtrTranslationPreffixDic()->FindPreffix(ptrWord->m_Key,ptrWord->m_Data))?true:false;

    if (ret == true)
        {
        GetDataInDic(ptrWord);
        ptrWord->m_Traducida = TRUE;
        if(ptrWord->m_sTipo.GetLength())
            tipo = ptrWord->m_sTipo;
        CLengDic l(GetTranslationUserPtr(),GetPtrSentence()); // se ejecuta lenguaje para sufijo
        
        l.Execute(nodo,ptrWord->m_Data);
        if(tipo.GetLength())
            ptrWord->m_sTipo = tipo;
        }            
    if(ret == false)
        {           
        ptrWord->m_Key = key;
        }    
    return ret;
    }
    
/////////////////////////////////////////////////////////////////////////////

void CScanWordsInDictionary::FindNameGender(LPCWORD ptrWord)
    {
    ASSERT(GetPtrSourceLanguageData()->GetPtrProperNounGender() != NULL);
    int i = GetPtrSourceLanguageData()->GetPtrProperNounGender()->Match(ptrWord->m_Trad);
    if(i > 0)
      ptrWord->m_Genero = FEMENINO;
    }

/////////////////////////////////////////////////////////////////////////////



