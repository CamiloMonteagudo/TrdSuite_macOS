// TranslateWordsInSentence.h: interface for the CTranslateWordsInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATEWORDSINSENTENCE_H__F6797927_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATEWORDSINSENTENCE_H__F6797927_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LengDic.h"
#include "ScanWordsInDictionary.h"
#include "ChangeWordType.h"
#include "RestoreWordType.h"

class CTranslateWordsInSentence : public CScanWordsInDictionary  
{
public:
	CTranslateWordsInSentence(	CTranslationUser* ptrTranslationUser = NULL,	// puntero a los datos para la traducción
								LPCORACION ptrSentence = NULL,					// puntero a la oracion de trabajo
								POSITION ptrInitialNode = NULL,
								POSITION ptrFinalNode = NULL):CScanWordsInDictionary(ptrTranslationUser,ptrSentence,ptrTranslationUser->GetPtrTranslationDic())
		{
		if(ptrInitialNode != NULL)
			SetInitialNode(ptrInitialNode);
		if(ptrFinalNode != NULL)
			SetFinalNode(ptrFinalNode);
		CChangeWordType ChangeWordType(GetTranslationUserPtr(),GetPtrSentence(),"T");
		ChangeWordType.Process();
		}
	virtual ~CTranslateWordsInSentence()
		{
		CRestoreWordType RestoreWordType(GetPtrSentence());
		RestoreWordType.Process();
		}

protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		if(ptrWord->m_Traducida == FALSE && ptrWord->m_Trad.IsEmpty() == TRUE && ptrWord->m_NoBusca == FALSE)
			{
			bool bPosesivo = false;

			if(ptrWord->m_sTipo == "GV")
				{
				ptrWord->m_sTipo = "NP";
				bPosesivo = true;
				}
			else if(ptrWord->m_sTipo == "GW")
				{
				ptrWord->m_sTipo = "SS";
				bPosesivo = true;
				}
			else if(ptrWord->m_sTipo == "GB")
				{
				ptrWord->m_sTipo = "ST";
				bPosesivo = true;
				}

			ptrWord->m_TipoTrd = ptrWord->m_sTipo;
			ptrWord->m_EspTrd = "";

			CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
			l.Execute(ptrNode,ptrWord->m_Data,FALSE,TRUE);
			//ptrWord->m_Trad = m_Data;
			//l.Execute(nodo,ptrWord->m_Trad,FALSE,TRUE);
        
			 // Busca la especialidades según orden dado
			 // si se encontró especialidad
                               
			// Primero busca el señalado en el texto por el usuario
			bool bFindEsp = FindAltMeaning(ptrWord);

			// Busca según el contenido semantico
			if(bFindEsp == false)
				bFindEsp = FindSemanticTranslation(ptrWord);

			if(bFindEsp == false)
				ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Data;

			ptrWord->m_Traducida = TRUE;

			if(bPosesivo == true)
				{
				if(ptrWord->m_sTipo == "NP")            
					ptrWord->m_sTipo = "GV";
				else if(ptrWord->m_sTipo == "SS")            
					ptrWord->m_sTipo = "GW";
				else if(ptrWord->m_sTipo == "ST")            
					ptrWord->m_sTipo = "GB";
				}
			}
			
		return ptrNode;
		}

	virtual bool FindSemanticTranslation(LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("FindSemanticTranslation"); // pequeño
		bool bFindEsp = false;

		for(int k = 0; k < ptrWord->m_AcepArray.GetCount() && bFindEsp == false; k++)
			{
			CWordEsp *esp = (CWordEsp *)ptrWord->m_AcepArray[k];

			ASSERT(esp != NULL);

			for(int i = 0; i < esp->m_Semantica.GetCount() && bFindEsp == false && esp->m_Word.Find("!!") == -1; i++)
				{
				for(POSITION ptrTmpNode = GetHeadPos();  
					  ptrTmpNode != NULL && bFindEsp == false; 
					  GetNextPos(ptrTmpNode))
					{
					ASSERT(ptrTmpNode != NULL);

					LPCWORD ptrTmpWord = GetWordAt(ptrTmpNode);   // Palabra que se analiza 
					ASSERT(ptrTmpWord != NULL);
					if(ptrTmpWord->m_Orig == esp->m_Semantica[i]||ptrTmpWord->m_Origlwr == esp->m_Semantica[i]||ptrTmpWord->m_Key == esp->m_Semantica[i])
						{
						ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Data = esp->m_Word;
						//ptrWord->m_Trad = esp->m_Word;
						ptrWord->m_Genero = esp->m_Genero;
						ptrWord->m_Numero = esp->m_Numero;
						ptrWord->m_Morf   = esp->m_Morf;
						ptrWord->m_Reflexivo   = esp->m_Reflexivo;
						bFindEsp = true;
						}
					}
                      
        for(int j = 0; j < GetPtrSentence()->m_WordsInSentence.GetCount() && j < GetPtrSentence()->m_WordsKeyInSentence.GetCount() && bFindEsp == false; j++)
          {
          CStringA sKey = GetPtrSentence()->m_WordsInSentence.GetAt(j);
          CStringA sOrig = GetPtrSentence()->m_WordsKeyInSentence.GetAt(j);
					if(sOrig == esp->m_Semantica[i] || sKey == esp->m_Semantica[i])
						{
						ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Data = esp->m_Word;
						//ptrWord->m_Trad = esp->m_Word;
						ptrWord->m_Genero = esp->m_Genero;
						ptrWord->m_Numero = esp->m_Numero;
						ptrWord->m_Morf   = esp->m_Morf;
						ptrWord->m_Reflexivo   = esp->m_Reflexivo;
						bFindEsp = true;
						}
          }
				}
			}
			
		return bFindEsp;
		}

	virtual bool FindAltMeaning(LPCWORD ptrWord)
		{
		bool bFindEsp = false;

		int index = ptrWord->m_AltMeaning - 1;
		if(index > -1 && index < ptrWord->m_AcepArray.GetCount())
			{
			CWordEsp *esp = (CWordEsp *)ptrWord->m_AcepArray[index]; 
			ASSERT(esp != NULL);

			if(esp->m_Word.Find("!!") == -1)
				{
				ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Data = esp->m_Word;
				//ptrWord->m_Trad = esp->m_Word;
				ptrWord->m_Genero = esp->m_Genero;
				ptrWord->m_Numero = esp->m_Numero;
				ptrWord->m_Morf   = esp->m_Morf;
				ptrWord->m_Reflexivo   = esp->m_Reflexivo;
				bFindEsp = true;
				ptrWord->m_EspTrd = esp->m_Esp;
				}
			}

		return bFindEsp;
		}
};

#endif // !defined(AFX_TRANSLATEWORDSINSENTENCE_H__F6797927_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
