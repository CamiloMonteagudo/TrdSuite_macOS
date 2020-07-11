// CmdCom.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "UtilFun.h"
#include "CWord.h"
#include "CmdItData.h"

/////////////////////////////////////////////////////////////////////////////

bool CComodinItemData::Execute(CActionPhraseArray *ptrAsignaValores) // asigna los valores de Action, Execute o Translation segun el comodin casado
    {
		if(GetPtrWord() == NULL)
			return false;
    ASSERT(GetPtrWord()->m_sTipo.IsEmpty() == FALSE);
    for(int i = 0; i < ptrAsignaValores->GetSize(); i++)
			{
#ifdef LOAD_FROM_DCX
			const CActionPhrase *ptrVal = GetRuleTable()->m_Actions.GetAtPtr(ptrAsignaValores->GetAt(i));
#else
			const CActionPhrase *ptrVal = &ptrAsignaValores->GetAt(i);
#endif
      if(Proccess(ptrVal) == false)
				return false;
      }
		return true;
    }
/////////////////////////////////////////////////////////////////////////////
// Procesa un comando del tipo %@......%
/////////////////////////////////////////////////////////////////////////////

bool CComodinItemData::Proccess(const CActionPhrase *ptrFrase)
  {
	if(GetPtrWord() == NULL || ptrFrase == NULL)
		return false;
	LPCWORD ptrWorkingWord = NULL;			// palabra de trabajo
	WORD pos = ptrFrase->GetPos();

	if( pos	!= NOT_FOUND_IN_ARRAY)
		{
		int kkk = GetPtrWord()->m_ComodinArray.GetSize();
		if(kkk == 0 || pos == 0)
			ptrWorkingWord = GetPtrWord();
		else if(pos > 0 && pos <= kkk)
			{
			int kk = pos-1;
			ptrWorkingWord = (LPCWORD)GetPtrWord()->m_ComodinArray[kk];
			ASSERT(ptrWorkingWord != NULL);
			#if defined(_DEBUG)
				for(int k =0; k < GetPtrWord()->m_ComodinArray.GetSize(); k++)
					{
					LPCWORD ww = (LPCWORD)GetPtrWord()->m_ComodinArray[k];
					bool f = (ww != NULL);
					f = f;
					}
			#endif
			}
		else
			ptrWorkingWord = NULL;
		ASSERT(ptrWorkingWord != NULL);
		}
	if(ptrWorkingWord != NULL)
		{
		WORD wProperty_AModo = ptrFrase->GetProperty(WP_AModo);
		WORD wProperty_Modo = ptrFrase->GetProperty(WP_Modo);
		if(wProperty_AModo != NOT_FOUND_IN_ARRAY)
			{
			// si la expresión es verdadera le asigno el valor de lo contrario permanece igual
			ptrWorkingWord->m_Modo = (ptrWorkingWord->m_Modo == ((Modo)wProperty_Modo))?((Modo)wProperty_AModo):((ptrWorkingWord->m_Modo != mSUBJUNTIVO_IMPERATIVO&&ptrWorkingWord->m_Modo != mSUBJUNTIVO_IMPERATIVO_INDICATIVO &&ptrWorkingWord->m_Modo != mIMPERATIVO_PARTICIPIO_INDICATIVO && ptrWorkingWord->m_Modo != mSUBJUNTIVO_IMPERATIVO_INDICATIVO_PARTICIPIO && ptrWorkingWord->m_Modo != mPARTICIPIO_INDICATIVO && ptrWorkingWord->m_Modo != mIMPERATIVO_INDICATIVO && ptrWorkingWord->m_Modo != mIMPERATIVO_INFINITIVO && ptrWorkingWord->m_Modo != mSUBJUNTIVO_INDICATIVO)? ptrWorkingWord->m_Modo:(Modo)wProperty_AModo);
			ptrWorkingWord->m_Reserva2 = 1;
			}
		else if(wProperty_Modo != NOT_FOUND_IN_ARRAY)
			{
			ptrWorkingWord->m_Modo = (Modo)wProperty_Modo;
			ptrWorkingWord->m_Reserva2 = 1;
			}

		/*for(int i = 1; (i + 1) < ptrFrase->GetSize(); i+= 2)
			{
			WORD wproperty = ptrFrase->GetAt(i+1);
			// especial para el modo que lleva el if de ?:
			switch(ptrFrase->GetAt(i))*/
		for(CActionPhrase::Iterator iter = ptrFrase->begin(); iter.isValid(); iter.next())
			{
			WORD wproperty = iter.getValue();

			// especial para el modo que lleva el if de ?:
			switch(iter.getProperty())
				{
				case WP_NGenero: // Pone el Genero
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Genero = ws->m_Genero;
					break;
					}
				case WP_Genero:
					{
					ptrWorkingWord->m_Genero = (Genero)wproperty;
					break;
					}
				case WP_NNumero:  // Pone el Numero
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						{
						ptrWorkingWord->m_Numero = ws->m_Numero;
						ptrWorkingWord->m_Plural = FALSE;
						}
					break;
					}
				case WP_Numero:
					{
					ptrWorkingWord->m_Numero = (Numero)wproperty;
					break;
					}
				case WP_NPersona:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Persona = ws->m_Persona;
					break;
					}
				case WP_Persona:
					{
					ptrWorkingWord->m_Persona = (Persona)wproperty;
					break;
					}
				case WP_NGrado:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Grade = ws->m_Grade;
					break;
					}
				case WP_Grado:
					{
					ptrWorkingWord->m_Grade = (Grado)wproperty;
					break;
					}
				case WP_NTiempo:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Tiempo = ws->m_Tiempo;
					break;
					}
				case WP_Tiempo:
					{
					ptrWorkingWord->m_Tiempo = (Tiempo)wproperty;
					break;
					}
				case WP_NModo:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						{
						ptrWorkingWord->m_Modo = ws->m_Modo;
						ptrWorkingWord->m_Reserva2 = 1;
						}
					break;
					}
				case WP_NCaso:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Case = ws->m_Case;
					break;
					}
				case WP_Caso:
					{
					ptrWorkingWord->m_Case = (Caso)wproperty;
					break;
					}
				case WP_NDeclination:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Declination = ws->m_Declination;
					break;
					}
				case WP_Declination:
					{
					ptrWorkingWord->m_Declination = (Declination)wproperty;
					break;
					}
				case WP_NArticleType:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_ArticleType = ws->m_ArticleType;
					break;
					}
				case WP_ArticleType:
					{
					ptrWorkingWord->m_ArticleType = (ArticleType)wproperty;
					break;
					}
				case WP_ToPhrase:
					{
					ptrWorkingWord->m_NoEnComodin = (wproperty != 0);
					break;
					}
				case WP_NNegative:
					{
					ASSERT(wproperty-1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Negado = ws->m_Negado;
					break;
					}
				case WP_Negative:
					{
					ptrWorkingWord->m_Negado = (wproperty != 0);
					break;
					}
				case WP_Delete:
					{
					ptrWorkingWord->m_Delete = (wproperty != 0);
					break;
					}
				case WP_Complex:
					{
					ptrWorkingWord->m_Compuesto = (wproperty != 0);
					break;
					}
				case WP_Translate:
					{
					if(wproperty == FALSE)
						{
						ptrWorkingWord->m_StrSaveTrad = (ptrWorkingWord->m_StrSaveTrad.IsEmpty()==TRUE)?ptrWorkingWord->m_Trad:ptrWorkingWord->m_StrSaveTrad;

						if(ptrWorkingWord->m_Orig.IsEmpty()==FALSE)
							{
							ptrWorkingWord->m_Trad = ptrWorkingWord->m_Orig;
							ptrWorkingWord->m_Traducida = TRUE;
							}
						else if(ptrWorkingWord->m_Origlwr.IsEmpty()==FALSE)
							{
							ptrWorkingWord->m_Trad = ptrWorkingWord->m_Origlwr;
							ptrWorkingWord->m_Traducida = TRUE;
							}
						else if(ptrWorkingWord->m_Key.IsEmpty()==FALSE)
							{
							ptrWorkingWord->m_Trad = ptrWorkingWord->m_Key;
							ptrWorkingWord->m_Traducida = TRUE;
							}
						}
					else
						{
						ptrWorkingWord->m_Traducida = FALSE;
						ptrWorkingWord->m_Trad = ptrWorkingWord->m_StrSaveTrad;
						}
					break;
					}
				case WP_NReflexive:
					{
					ASSERT(wproperty -1 < GetPtrWord()->m_ComodinArray.GetSize());
					LPCWORD ws = (LPCWORD)GetPtrWord()->m_ComodinArray[wproperty-1];
					ASSERT( ws != NULL);
					if(ws != NULL)
						ptrWorkingWord->m_Reflexivo = ws->m_Reflexivo;
					break;
					}
				case WP_Reflexive:
					{
					ptrWorkingWord->m_Reflexivo = (wproperty != 0);
					break;
					}
				case WP_FindInDic:
					{
					if(wproperty == TRUE)
						{
						ptrWorkingWord->m_Traducida = FALSE;
						ptrWorkingWord->m_Buscada = FALSE;
						ptrWorkingWord->m_NoBusca = FALSE;
						}
					}
				case WP_GrammarTypeArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aGrammarType.m_FixedArray.GetCount());
					ptrWorkingWord->m_sTipo = GetRuleTable()->m_aGrammarType.m_FixedArray.GetAt(wproperty);
					break;
					}
				case WP_TranslationArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aTranslations.m_FixedArray.GetCount());
					ptrWorkingWord->m_Trad = GetRuleTable()->m_aTranslations.m_FixedArray.GetAt(wproperty);
					ptrWorkingWord->m_Traducida = TRUE;
					break;
					}
				case WP_InsertNum:
					{
					ptrWorkingWord->m_Articulo = wproperty;
					break;
					}
				case WP_InsertArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aInsert.m_FixedArray.GetCount());
					ptrWorkingWord->m_sInsert = GetRuleTable()->m_aInsert.m_FixedArray.GetAt(wproperty);
					break;
					}
				case WP_CategoryArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aCategory.m_FixedArray.GetCount());
					ptrWorkingWord->m_sSemantica = GetRuleTable()->m_aCategory.m_FixedArray.GetAt(wproperty);
					//ptrWorkingWord->m_sSemantica.MakeUpper();
					break;
					}
				case WP_AddKeyArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aAddKey.m_FixedArray.GetCount());
					ptrWorkingWord->m_AddKey = GetRuleTable()->m_aAddKey.m_FixedArray.GetAt(wproperty);
					break;
					}
				case WP_AddDataArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aAddData.m_FixedArray.GetCount());
					ptrWorkingWord->m_AddData = GetRuleTable()->m_aAddData.m_FixedArray.GetAt(wproperty);
					break;
					}
				case WP_NewKeyArrayIndex:
					{
					ASSERT(wproperty < GetRuleTable()->m_aKey.m_FixedArray.GetCount());
					ptrWorkingWord->m_Key = GetRuleTable()->m_aKey.m_FixedArray.GetAt(wproperty);
					ptrWorkingWord->m_Origlwr = ptrWorkingWord->m_Key;
					ptrWorkingWord->m_Orig = ptrWorkingWord->m_Key;
					ptrWorkingWord->m_Traducida = FALSE;
					ptrWorkingWord->m_Buscada = FALSE;
					ptrWorkingWord->m_NoBusca = FALSE;
					ptrWorkingWord->m_DiWord = DW_WORD;
					ptrWorkingWord->m_Trad.Empty();
					//ptrWorkingWord->m_Adiciona = 1;
					break;
					}
				case WP_NewPos:
					{
					ptrWorkingWord->m_PosFraseComodin = wproperty;
					break;
					}
				}
			}
		}

	return true;
  }
                                
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
