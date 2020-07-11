// CoordinateWords.h: interface for the CCoordinateWords class.
// Realiza la coordinación de adjetivos y sustantivos
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COORDINATEWORDS_H__F6797931_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_COORDINATEWORDS_H__F6797931_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CCoordinateWords : public CStepInSentence  
{
public:
	CCoordinateWords(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL,POSITION ptrInitialNode=NULL,POSITION ptrFinalNode=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
		{
		if(ptrInitialNode != NULL)
			SetInitialNode(ptrInitialNode);
		if(ptrFinalNode != NULL)
			SetFinalNode(ptrFinalNode);
		}

protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
        if(  ptrWord->m_DiWord.Get() != DW_NOTRADUCIR 
          && ptrWord->m_DiWord.Get() != DW_DOSPUNTOS 
          && ptrWord->m_DiWord.Get() != DW_COMA 
          && ptrWord->m_DiWord.Get() != DW_FININTERROGA
          && ptrWord->m_DiWord.Get() != DW_FINADMIRACION
          && ptrWord->m_DiWord.Get() != DW_ORDINAL
          && ptrWord->m_DiWord.Get() != DW_DECADA_CS
          && ptrWord->m_DiWord.Get() != DW_DECADA_S
          && ptrWord->m_DiWord.Get() != DW_NUMERAL
          && ptrWord->m_DiWord.Get() != DW_DINERO
          && ptrWord->m_DiWord.Get() != DW_MENOS
          && ptrWord->m_DiWord.Get() != DW_MENORQ
          && ptrWord->m_DiWord.Get() != DW_MAYORQ
          && ptrWord->m_DiWord.Get() != DW_LLAVEA
          && ptrWord->m_DiWord.Get() != DW_LLAVEC
          && ptrWord->m_DiWord.Get() != DW_CORCHETEA
          && ptrWord->m_DiWord.Get() != DW_CORCHETEC
          && ptrWord->m_DiWord.Get() != DW_PARENTA
          && ptrWord->m_DiWord.Get() != DW_PARENTC
          && ptrWord->m_DiWord.Get() != DW_COMILLAD
          && ptrWord->m_DiWord.Get() != DW_COMILLAS
          && ptrWord->m_DiWord.Get() != DW_PORCIENTO
          && ptrWord->m_DiWord.Get() != DW_NUMERO
          && ptrWord->m_DiWord.Get() != DW_CUALQUIERA
          && ptrWord->m_DiWord.Get() != DW_FILENAME
          && ptrWord->m_DiWord.Get() != DW_RAYA)
			{
			MakeConcordance(ptrWord);
			}
		return ptrNode;
        }
public:

	virtual void MakeConcordance(LPCWORD ptrWord)
		{   
		CStringA sTranslation(ptrWord->m_Trad);
        CStringA salvapalabra;

		sTranslation.Replace("**", "*");

//		if(ptrWord->m_sTipo == "AN" || ptrWord->m_sTipo == "NN" || ptrWord->m_sTipo == "MV")
//            {
//		    ptrWord->m_Trad = sTranslation;
//            ptrWord->m_Concordada = TRUE;
//            return;
//            }

		if((ptrWord->m_sTipo == "FS" || ptrWord->m_sTipo == "FO") && ptrWord->m_Trad.FindOneOf(" *") == -1)
      {
		  ptrWord->m_Trad = sTranslation;
      ptrWord->m_Concordada = TRUE;
      return;
      }

		//if(ptrWord->m_sTipo == "NP" && sTranslation.Find('*') != -1)
		//	sTranslation.Replace("*", "");
		if(ptrWord->m_sTipo == "NP")
			sTranslation.Remove('*');

		   // OJO hay que preguntar si es adj o sust en español, francés e Italiano o sust en inglés
		if((  (GetPtrTargetLanguageData()->GetFemaleType().Find(ptrWord->m_sTipo) != -1 && ptrWord->m_Genero.Get() != MASCULINO)
			||(GetPtrTargetLanguageData()->GetPluralType().Find(ptrWord->m_sTipo) != -1 && ptrWord->m_Numero.Get() == PLURAL)
            ||(GetPtrTargetLanguageData()->GetGradeType().Find(ptrWord->m_sTipo) != -1  && ptrWord->m_Grade.Get() != POSITIVO)
            ||(GetPtrTargetLanguageData()->GetTypeAdjDeclination().Find(ptrWord->m_sTipo) != -1  && ptrWord->m_Declination.Get() != NODECLINE)
            ||(GetPtrTargetLanguageData()->GetTypeNounDeclination().Find(ptrWord->m_sTipo) != -1 && ptrWord->m_Declination.Get() != NODECLINE))
			 && sTranslation.FindOneOf(" *") == -1)
			sTranslation += '*';
		for(;;)
			{
			CStringA sPalabra,sInicio,sFinal;
			int fin = sTranslation.Find('*');
        
			if( fin != -1)
				{        
				CStringA tmp(sTranslation,fin);
				int ini = tmp.ReverseFind(' ');
				sPalabra = (ini == -1)?sTranslation.Left(fin):sTranslation.Mid(ini+1,fin-(ini+1));
                
				if(ini > 0)
					sInicio = sTranslation.Left(ini+1);
				if((sTranslation.GetLength()-(fin+1)) > 0)    
					sFinal = sTranslation.Right(sTranslation.GetLength()-(fin+1));
				}    
			else
				break; 
      CStringA sTempPalabra(sPalabra);
      CStringF sTargetLanguage(GetPtrTranslationData()->GetTargetLanguage().GetIsoName());
      sTempPalabra.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)sTempPalabra );

			if(ptrWord->m_Grade.Get() != POSITIVO && ptrWord->m_Concordada == FALSE)
				{
				if(GetPtrTargetLanguageData()->GetGradeType().Find(ptrWord->m_sTipo) != -1) // caso de concordar en Grado
					sPalabra = GetPtrTargetLanguageData()->PutAdjectiveInGrade(sPalabra,ptrWord->m_Grade.Get());
				}
			if(ptrWord->m_Genero.Get() != MASCULINO && ptrWord->m_Concordada == FALSE)
				{
				if(GetPtrTargetLanguageData()->GetFemaleType().Find(ptrWord->m_sTipo) != -1  || GetPtrTargetLanguageData()->GetFemaleTypeSiAsterisco().Find(ptrWord->m_sTipo) != -1/*|| (ptrWord->m_Trad.Find("*") != -1 || ptrWord->m_Trad.Find("~") || ptrWord->m_Trad.Find("^") != -1)*/) // caso de concordar en género femenino
					sPalabra = GetPtrTargetLanguageData()->PutWordInGender(sPalabra,ptrWord->m_Genero.Get());
				}
			if(ptrWord->m_Numero.Get() == PLURAL && ptrWord->m_Concordada == FALSE)                          
				{
				if(((GetPtrTargetLanguageData()->GetPluralType().Find(ptrWord->m_sTipo) != -1 || GetPtrTargetLanguageData()->GetPluralTypeSiAsterisco().Find(ptrWord->m_sTipo) != -1/*|| (ptrWord->m_Trad.Find("*") != -1 || ptrWord->m_Trad.Find("~") || ptrWord->m_Trad.Find("^") != -1)*/)&& sTargetLanguage.Find("De") == -1) || (GetPtrTargetLanguageData()->GetPluralType().Find(ptrWord->m_sTipo) != -1 && sPalabra != sTempPalabra && sTargetLanguage.Find("De") != -1)) // caso de concordar en número plural
					sPalabra = GetPtrTargetLanguageData()->PutWordInNumber(sPalabra,ptrWord->m_Numero.Get());
				}        

			if(ptrWord->m_Declination.Get() != NODECLINE && ptrWord->m_Concordada == FALSE && (sTargetLanguage.Find(LF"De") != -1))                          
				{
				if(GetPtrTargetLanguageData()->GetTypeAdjDeclination().Find(ptrWord->m_sTipo) != -1 || (sPalabra == sTempPalabra && GetPtrTargetLanguageData()->GetTypeNounDeclination().Find(ptrWord->m_sTipo) != -1)) // caso de concordar en declinacion de adjetivos
					sPalabra = GetPtrTargetLanguageData()->PutAdjectiveInDeclination(sPalabra,ptrWord->m_Genero.Get(), ptrWord->m_Numero.Get(),ptrWord->m_Declination.Get(),ptrWord->m_ArticleType.Get());
				}        
			if(ptrWord->m_Declination.Get() != NODECLINE && ptrWord->m_Concordada == FALSE && (sTargetLanguage.Find(LF"De") != -1))
				{
				if(GetPtrTargetLanguageData()->GetTypeNounDeclination().Find(ptrWord->m_sTipo) != -1 && (sPalabra != sTempPalabra)) // caso de concordar en declinacion de sustantivos
					sPalabra = GetPtrTargetLanguageData()->PutNounInDeclination(sPalabra,ptrWord->m_Genero.Get(), ptrWord->m_Numero.Get(),ptrWord->m_Declination.Get());
				}        

			sTranslation = sInicio + sPalabra + sFinal;
			}

		for(;;)
			{
			CStringA sPalabra,sInicio,sFinal;
			int fin = sTranslation.Find('^');
        
			if( fin != -1)
				{        
				CStringA tmp(sTranslation,fin);
				int ini = tmp.ReverseFind(' ');
				sPalabra = (ini == -1)?sTranslation.Left(fin):sTranslation.Mid(ini+1,fin-(ini+1));
                
				if(ini > 0)
					sInicio = sTranslation.Left(ini+1);
				if((sTranslation.GetLength()-(fin+1)) > 0)    
					sFinal = sTranslation.Right(sTranslation.GetLength()-(fin+1));
				}    
			else
				break; 
      CStringA sTempPalabra(sPalabra);
      CStringF sTargetLanguage(GetPtrTranslationData()->GetTargetLanguage().GetIsoName());

      sTempPalabra.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)sTempPalabra );

			if(ptrWord->m_Genero.Get() != MASCULINO && ptrWord->m_Concordada == FALSE)
				{
				if(GetPtrTargetLanguageData()->GetFemaleType().Find(ptrWord->m_sTipo) != -1) // caso de concordar en género femenino
					sPalabra = GetPtrTargetLanguageData()->PutWordInGender(sPalabra,ptrWord->m_Genero.Get());
				}

			sTranslation = sInicio + sPalabra + sFinal;
			}

		for(;;)
			{
			CStringA sPalabra,sInicio,sFinal;
			int fin = sTranslation.Find('~');
        
			if( fin != -1)
				{        
				CStringA tmp(sTranslation,fin);
				int ini = tmp.ReverseFind(' ');
				sPalabra = (ini == -1)?sTranslation.Left(fin):sTranslation.Mid(ini+1,fin-(ini+1));
                
				if(ini > 0)
					sInicio = sTranslation.Left(ini+1);
				if((sTranslation.GetLength()-(fin+1)) > 0)    
					sFinal = sTranslation.Right(sTranslation.GetLength()-(fin+1));
				}    
			else
				break; 
     CStringA sTempPalabra(sPalabra);
     CStringF sTargetLanguage(GetPtrTranslationData()->GetTargetLanguage().GetIsoName());
     sTempPalabra.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)sTempPalabra );

			if(ptrWord->m_Numero.Get() == PLURAL && ptrWord->m_Concordada == FALSE)                          
				{
				if((GetPtrTargetLanguageData()->GetPluralType().Find(ptrWord->m_sTipo) != -1 && sTargetLanguage.Find(LF"De") == -1) || (GetPtrTargetLanguageData()->GetPluralType().Find(ptrWord->m_sTipo) != -1 && sPalabra != sTempPalabra && sTargetLanguage.Find("De") != -1)) // caso de concordar en número plural
					sPalabra = GetPtrTargetLanguageData()->PutWordInNumber(sPalabra,ptrWord->m_Numero.Get());
				}        
			sTranslation = sInicio + sPalabra + sFinal;
			}
		ptrWord->m_Trad = sTranslation;
    ptrWord->m_Concordada = TRUE;
		}

};

#endif // !defined(AFX_COORDINATEWORDS_H__F6797931_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
