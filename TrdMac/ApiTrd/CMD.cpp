/////////////////////////////////////////////////////////////////////////////
// Cmd.cpp
// funciones para el trabajo con un comodin
// asociado a una palabra o frase
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cmd.h"
#include "TranslateWordsInSentence.h"
#include "CoordinateWords.h"
#include "ConjugateVerbsInSentence.h"
#include "ReflexiveVerbInSentence.h"
#include "TranslateTwoTranslationVerb.h"
#include "CmdItKey.h"
#include "CmdItData.h"
#include "InsertOrAddWord.h"

/////////////////////////////////////////////////////////////////////////////
// Busca el tamaño del comodin a partir del nodo inicial y el final
/////////////////////////////////////////////////////////////////////////////

int CComodin::GetSizeByPosition(POSITION ptrInitialPos,POSITION ptrFinalPos)
	{
	if (ptrFinalPos == NULL)
		return 0;

	int iSize = 0;
	for(POSITION ptrNode = ptrInitialPos; ptrNode != ptrFinalPos && ptrNode != NULL/* && ptrFinalPos != NULL*/; GetNextPos(ptrNode))
		iSize++;
	if(iSize > 0)
		iSize++;
	return iSize;
	}
/////////////////////////////////////////////////////////////////////////////
// Restaura las palabras tal y como estaban en la oración
// se restauran las palabras que estaban en el arreglo de comodines
/////////////////////////////////////////////////////////////////////////////

void CComodin::Remove()
	{
	POSITION ptrNodo = GetPtrInitialPos();

	if(ptrNodo != NULL)
		{
		if(GetPtrWordComodin() != NULL && GetPtrWordComodin()->m_ComodinArray.GetSize() > 0)
			{
			m_sCmdType = GetPtrWordComodin()->m_sTipo;

			for(int i = 0; i < m_Size; i++)
				{
				LPCWORD ptrW = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];

				if(i == 0)
					GetPtrSentence()->m_Ora.SetAt(ptrNodo,ptrW);
				else
					ptrNodo = GetPtrSentence()->m_Ora.InsertAfter(ptrNodo,ptrW);
				}
			GetPtrWordComodin()->m_ComodinArray.RemoveAll();
			} 
		}
	else
		m_bError = true;

	GetFinalPos();
	}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Sustituye cada ocurrencia de $número por la traducción de la palabra
// correspondiente al número dado dentro del comodín
/////////////////////////////////////////////////////////////////////////////

void CComodin::ReplaceDollar(CStringA &sTraduccion)
	{
	ASSERT(GetPtrWordComodin() !=NULL);
	ASSERT(sTraduccion.IsEmpty() == FALSE);

	if(sTraduccion.IsEmpty() == FALSE && sTraduccion.Find('$') != -1)
		{
		for(int i = GetPtrWordComodin()->m_ComodinArray.GetSize()-1; GetPtrWordComodin() != NULL && i >= 0; i--)
			{
	#ifdef _DEBUG
			LPCWORD ws = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];
			ASSERT(ws!=NULL);
	#endif
			CStringA tmp("$", CStringA::FromInt_(i+1));
			//tmp.Format("$%i",i+1);
			if(sTraduccion.Find(tmp) != -1)
				{
				LPCWORD ws = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];
				ASSERT(ws!=NULL);


				CStringA TipoConjugar("VV,VT,VI,VG,GT,GI,BE,HA,PT,PI,VP,HT,HI,VS,VA,JT,JI,DO,VR");	// si es un verbo lo hago conjugar
//				CStringA TipoConjugar(GetPtrTranslationData()->GetTargetLanguage().GetVerbalType());
				if(TipoConjugar.Find(ws->m_sTipo)!= -1)
					{
					if (
						(ws->m_Modo == mINFINITIVO 
						&& ws->m_Trad.Find('<') != -1 
						&& ws->m_Trad.Find('>') != -1)
						|| GetPtrTargetLanguageData()->GetVerbalType().Find(ws->m_sTipo) != -1)
						{
						CConjugateVerbsInSentence objConjugateVerbsInSentence(GetTranslationUserPtr(),GetPtrSentence());
						objConjugateVerbsInSentence.ConjugateVerb(ws);

              {
              CReflexiveVerbInSentence objReflexiveVerbInSentence(GetTranslationUserPtr(),GetPtrSentence());
              objReflexiveVerbInSentence.PutReflexivePronoun(ws);
              }
						}
					}
				CStringA TipoConcordar;	// si es un sustantivo o un adjetivo o un participio lo hago concordar "SS,AA,AAI,RD,RI,AF,PT,PI,TI"
                
        TipoConcordar = GetPtrTargetLanguageData()->GetFemaleType() + GetPtrTargetLanguageData()->GetPluralType();
				//if(ws->m_sTipo != "NC" && (TipoConcordar.Find(ws->m_sTipo)!= -1 || (ws->m_Trad.Find("*") != -1 || ws->m_Trad.Find("~") || ws->m_Trad.Find("^") != -1)))
				if(ws->m_sTipo != "NC" && (TipoConcordar.Find(ws->m_sTipo)!= -1 || ws->m_Trad.FindOneOf("*~^") != -1))
					{
					if (ws->m_Trad.FindOneOf("*~^") != -1)
          //if(ws->m_Trad.Find("*") != -1 || ws->m_Trad.Find("~") != -1 || ws->m_Trad.Find("^") != -1)
            ws->m_Concordada = FALSE;
					CCoordinateWords objCoordinateWords(GetTranslationUserPtr(),GetPtrSentence());
					objCoordinateWords.MakeConcordance(ws);
					}
				if(ws->m_RtfFijoIni.IsEmpty() == FALSE)
					ws->m_Trad = ws->m_RtfFijoIni + ws->m_Trad;
				if(ws->m_RtfFijoFin.IsEmpty() == FALSE)
					ws->m_Trad = ws->m_Trad + ws->m_RtfFijoFin;
				sTraduccion.Replace(tmp,ws->m_Trad);
				/*CWord *wrtf = (CWord *)GetPtrWordComodin();*/
				//wrtf->m_RtfFijoIni += ws->m_RtfFijoIni;
				//wrtf->m_RtfFijoFin += ws->m_RtfFijoFin;

				}
			}
		GetPtrWordComodin()->m_Traducida = TRUE;
		GetPtrWordComodin()->m_Buscada = TRUE;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////////////////

void CComodin::ReplaceDollarKey(CStringA &sKey)
	{
	ASSERT(GetPtrWordComodin() !=NULL);
	ASSERT(sKey.IsEmpty() == FALSE);

	if(sKey.IsEmpty() == FALSE && sKey.Find('$') != -1)
		{
		for(int i = GetPtrWordComodin()->m_ComodinArray.GetSize()-1; GetPtrWordComodin() != NULL && i >= 0; i--)
			{
	#ifdef _DEBUG
			LPCWORD ws = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];
			ASSERT(ws!=NULL);
	#endif
			CStringA tmp("$", CStringA::FromInt_(i+1));
			//tmp.Format("$%i",i+1);
			if(sKey.Find(tmp) != -1)
				{
				LPCWORD ws = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];
				ASSERT(ws!=NULL);
				sKey.Replace(tmp,ws->m_Key);
				}
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// Sustituye cada ocurrencia de !número por la particula separable de la 
// palabra correspondiente al número dado dentro del comodín
/////////////////////////////////////////////////////////////////////////////

void CComodin::ReplaceParticle(CStringA &sTraduccion)
	{
	ASSERT(GetPtrWordComodin() !=NULL);
	//ASSERT(sTraduccion.IsEmpty() == FALSE);

	if(sTraduccion.IsEmpty() == FALSE)
		{
		for(int i = 0; GetPtrWordComodin() != NULL && i < GetPtrWordComodin()->m_ComodinArray.GetSize(); i++)
			{
	#ifdef _DEBUG
			LPCWORD ws = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];
			ASSERT(ws!=NULL);
	#endif
			CStringA tmp("!", CStringA::FromInt_(i+1));
			//tmp.Format("!%i",i+1);

      if(sTraduccion.Find(tmp) != -1)
				{
				LPCWORD ws = (LPCWORD)GetPtrWordComodin()->m_ComodinArray[i];
				ASSERT(ws!=NULL);
        CStringA sTemp(" ");
        sTemp += ws->m_sParticle; // le pone un espacio delante
				sTraduccion.Replace(tmp,sTemp);
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// recalcula el nodo final de la frase de comodines
/////////////////////////////////////////////////////////////////////////////

POSITION CComodin::GetFinalPos()
	{
	POSITION n = GetPtrInitialPos();   // recalcula el nodo final

	SetPtrFinalPos(n);

	for(int i = 0; i < m_Size; i++)
		{
		if(n != NULL)
			{
			SetPtrFinalPos(n);
			#if defined(_DEBUG)
				LPCWORD w = GetWordAt(n);   // Palabra que se analiza 
				ASSERT(w != NULL);
			#endif
			GetNextPos(n);
			}
		}
	return GetPtrFinalPos();
	}

/////////////////////////////////////////////////////////////////////////////
// Procesa los comodines de determinación de la función gramatical
/////////////////////////////////////////////////////////////////////////////

void CComodin::ProccessDataFunction()
	{
	//COLLECT_INTERVAL("ProccessDataFunction"); // despreciable
	ASSERT(m_ptrMatchItem != NULL);
	if(m_ptrMatchItem->m_Action.GetSize() > 0 && m_bError == false)
		{
		#if !defined(MERCADO)
			CStringA tmpXX = GetFileName();
			tmpXX = "[" + tmpXX + "]";
			tmpXX += m_strCmdKey;
			GetPtrSentence()->m_CmdInSentence.Add(tmpXX);
		#endif
		MakeArrayFrase(GetPtrWordComodin(),GetPtrInitialPos(),GetPtrFinalPos());
		SetSize(GetPtrWordComodin()->m_ComodinArray.GetSize());

		CComodinItemData cmdFunction(GetPtrWordComodin(),GetRuleTable());

		bool bWithoutError = cmdFunction.Execute(&m_ptrMatchItem->m_Action);

		Remove();

		if(bWithoutError == false)
			m_bError = true;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// Obtiene el nodo inicial de la frase de traducción
/////////////////////////////////////////////////////////////////////////////

POSITION CComodin::FindInitialTranslationNode(int &iIniSize, int &iPos)
	{
	POSITION n = GetPtrInitialPos();

	// Busca en inicio real de la frase con comodín eliminando las
	// palabras que al inicio o al final no forman parte del comodín

	iIniSize = 0;
	
	for(iPos = 0; n!=NULL && iPos < m_Size; iPos++)
		{
		LPCWORD w = GetWordAt(n);
		if(w->m_NoEnComodin == TRUE)
			{
			GetNextPos(n);
			iIniSize++;
			}
		else
			break;
		}
	return n;
	}

/////////////////////////////////////////////////////////////////////////////
// Obtiene el nodo final de la frase de traducción quitando las palabras que
// no deben formar parte de la misma
/////////////////////////////////////////////////////////////////////////////

POSITION CComodin::FindFinalTranslationNode(int iIniSize, int iPos)
	{
	ASSERT(GetPtrInitialPos() != NULL);
	POSITION n = GetPtrInitialPos();

	// quita las palabras del final que no deben fomar parte de la frase
	for(; n != NULL && iPos < m_Size; iPos++)
		{
		LPCWORD w = GetWordAt(n);
		if(w->m_NoEnComodin == TRUE)
			break;
		else
			{
			GetNextPos(n);
			#if defined(_DEBUG)
				if(n != NULL)
					{
					LPCWORD w = GetWordAt(n);
					ASSERT(w != NULL);
					}
			#endif
			}
		}

	m_Size = iPos - iIniSize;

	if(n != NULL)
		SetPtrFinalPos(n);

#if defined(_DEBUG)
	LPCWORD w = GetWordAt(GetPtrFinalPos());
#endif

	GetPtrSentence()->m_Ora.GetPrev(GetPtrFinalPos());

#if defined(_DEBUG)
	w = GetWordAt(GetPtrFinalPos());
#endif
	return GetPtrFinalPos();
	}


/////////////////////////////////////////////////////////////////////////////
// Procesa las frases de comodines de traducción
/////////////////////////////////////////////////////////////////////////////

bool CComodin::ProccessDataTranslation()
	{
	//COLLECT_INTERVAL("ProccessDataTranslation"); // 10%
	ASSERT(m_ptrMatchItem != NULL);
	if(m_ptrMatchItem->m_Translation.GetSize() > 0 && m_bError == false)
		{
		#if !defined(MERCADO)
			CStringA tmpXX = GetFileName();
			tmpXX = "[" + tmpXX + "]";
			tmpXX += m_strCmdKey;
			GetPtrSentence()->m_CmdInSentence.Add(tmpXX);
		#endif
		GetFinalPos();
		//int insert = 0;

		// se busca la traducción de las palabras del comodin

		int iIniSize = 0, iPos = 0;

		GetPtrInitialPos() = FindInitialTranslationNode(iIniSize,iPos);
		SetPtrFinalPos(FindFinalTranslationNode(iIniSize,iPos));

		POSITION ptrFin = GetPtrFinalPos();

		if(ptrFin != NULL)
			GetPtrSentence()->m_Ora.GetNext(ptrFin);

		// agrega a la oración las palabras a insertar o adicionar
		//CInsertOrAddWord InsertOrAddWord(GetPtrTranslationData(),GetPtrSentence());
		//InsertOrAddWord.Process();

		GetPtrSentence()->View(301);

    // Busca las palabras de la oración en el diccionario
		//COLLECT_INTERVAL("CFindWordSentenceInDict"); // 0%
    CFindWordSentenceInDict objFindWordSentenceInDict(GetTranslationUserPtr(),GetPtrSentence());

    objFindWordSentenceInDict.Process();

		GetPtrSentence()->View(302);

    {
		//COLLECT_INTERVAL("CTranslateWordsInSentence"); //6%
		// Traduce las palabras del comodín de traducción que no fueron excluídas
		CTranslateWordsInSentence TranslateWordsInSentence(GetTranslationUserPtr(),GetPtrSentence(),GetPtrInitialPos(),ptrFin);
	
		TranslateWordsInSentence.Process();
    }

		GetPtrSentence()->View(303);

    //{
    // Traducción correcta del verbo BE
		//CTranslateTwoTranslationVerb TwoTranslationVerb(GetTranslationUserPtr(),GetPtrSentence(),GetPtrInitialPos(),ptrFin);

		//TwoTranslationVerb.Process();
    //}
	
		LPCWORD wtmp2= (CWord *)GetPtrSentence()->m_Ora.GetAt(GetPtrInitialPos());

		MakeArrayFrase(GetPtrWordComodin(),GetPtrInitialPos(),GetPtrFinalPos());

		SetSize(GetPtrWordComodin()->m_ComodinArray.GetSize());

		ASSERT(GetWordAt(GetPtrInitialPos()) == GetPtrWordComodin());


		CComodinItemData cmdTranslation(GetPtrWordComodin(),GetRuleTable());

		bool bWithoutError = cmdTranslation.Execute(&m_ptrMatchItem->m_Translation);

//		Remove();   // elimina el arreglo con las palabras que forman parte de la frase de comodines

		if(bWithoutError == false)
			m_bError = true;

		bool EnFraseHecha = false;

		if(GetPtrWordComodin()->m_Trad.IsEmpty() == FALSE)
			EnFraseHecha = true;

		if(EnFraseHecha==false)
		  Concordance(true);
    else
		  Concordance(false);

		/// pone el orden en la frase traducida
		CStringA sTranslation;

		OrdenaFrase(sTranslation);

		if(EnFraseHecha==true)
      {
			ReplaceDollar(GetPtrWordComodin()->m_Trad);
			ReplaceParticle(GetPtrWordComodin()->m_Trad);
      }
		else
			GetPtrWordComodin()->m_Trad = sTranslation;

		//Concordance(true);

		if(GetPtrWordComodin()->m_AddKey.IsEmpty() == FALSE && GetPtrWordComodin()->m_AddData.IsEmpty() == FALSE)
			{
			ReplaceDollarKey(GetPtrWordComodin()->m_AddKey);
			ReplaceDollar(GetPtrWordComodin()->m_AddData);
			CStringA data,key(GetPtrWordComodin()->m_AddKey);
			CStringA tipo("#T");
			CStringA datatest(GetPtrWordComodin()->m_AddData);
			int itype = datatest.Find("//");
			if(itype != -1)
				{
				tipo += datatest.Left(itype);
				data = datatest.Right(datatest.GetLength()-itype-2);
				}
			else
				{
				tipo += "SS";
				data = GetPtrWordComodin()->m_AddData;
				}

			tipo += "#E\"";
			data = tipo + data + "\"";
			if(GetPtrWordComodin()->m_Genero == FEMENINO)
				data += "*";
			else if(GetPtrWordComodin()->m_Genero == NEUTRO)
				data += "-";
			if(GetPtrWordComodin()->m_Numero == PLURAL)
				data += "$";
			//CStringA datakk;
			}

		/*CStringA guion1(" \\- ");  // quita los blancos delante y detrás del guión 
		CStringA guion2("-");
		CSGrep quitaguion(guion1,guion2);    // compile a regexp to sgrep
		quitaguion.Replace(GetPtrWordComodin()->m_Trad,TRUE);*/
		GetPtrWordComodin()->m_Trad.Replace(" - ", "-");

		GetPtrWordComodin()->m_Buscada = TRUE;
		GetPtrWordComodin()->m_BuscadaC = TRUE;
		GetPtrWordComodin()->m_Traducida = TRUE;
		GetPtrWordComodin()->m_DiWord = DW_WORD;

			{
			for(int i = 0; i < GetPtrWordComodin()->m_ComodinArray.GetSize(); i++)
				{
				LPCWORD ptrTmpWord = (LPCWORD)GetPtrWordComodin()->m_ComodinArray.GetAt(i);
				if(ptrTmpWord != NULL)
					delete ptrTmpWord;
				}
			GetPtrWordComodin()->m_ComodinArray.RemoveAll();
			}

		LPCWORD wtmp= (LPCWORD)new CWord(GetPtrWordComodin()->m_Orig);
		/*LPCWORD wtmp3 = GetPtrWordComodin();*/
		
		wtmp->CopyWord(*GetPtrWordComodin());
		
		GetPtrSentence()->m_Ora.SetAt(GetPtrInitialPos(),wtmp);
		
		if(m_Size == 0 && wtmp2 != NULL)
			{
			wtmp->m_RtfFijoFin = wtmp2->m_RtfFijoFin;
			wtmp->m_RtfFijoIni = wtmp2->m_RtfFijoIni;
			delete wtmp2;
			}

		return true;
		}
	return false;
	}

/////////////////////////////////////////////////////////////////////////////
// Casa la frase de comodín en la oración a patir del nodo dado
// da valor a las posiciones iniciales y finales de la frase
/////////////////////////////////////////////////////////////////////////////

bool CComodin::Math(CMatchItem *ptrMatchItem, POSITION nodeStart, POSITION nodeEnd)
	{
	m_ptrMatchItem = ptrMatchItem;
	ASSERT(m_ptrMatchItem != NULL);

  if(m_cmdKey.Match(ptrMatchItem, nodeStart, nodeEnd) == true)
		{
#if !defined(MERCADO)
		m_strCmdKey = cmdKey.GetPtrPhrase();
#endif						
		SetPtrInitialPos(nodeStart);
		SetPtrFinalPos(nodeEnd);
		SetSize(GetSizeByPosition(nodeStart, nodeEnd));
		//SetPtrInitialPos(m_cmdKey.GetInitialPosition());
		//SetPtrFinalPos(m_cmdKey.GetFinalPosition());
		//SetSize(GetSizeByPosition(m_cmdKey.GetInitialPosition(),m_cmdKey.GetFinalPosition()));
		return true;
		}
	else
		{
		SetPtrInitialPos(NULL);
		SetPtrFinalPos(NULL);
		}
	return false;
	}

/////////////////////////////////////////////////////////////////////////////

void CComodin::Concordance(bool bMakeCorcondance)
	{
	// pone el género y el número de los adjetivos
	for(register int i = 0; i < GetPtrWordComodin()->m_ComodinArray.GetSize(); i++)
		{
		LPCWORD w = (LPCWORD)GetPtrWordComodin()->m_ComodinArray.GetAt(i);

		if(w->m_sTipo == "NC")		// no coordinar
			{
      w->m_sTipo = "PT";
      if(w->m_Modo == mPARTICIPIO) // CModo
        {
				if (
					(w->m_Modo == mINFINITIVO 
					&& w->m_Trad.Find('<') != -1 
					&& w->m_Trad.Find('>') != -1)
					|| GetPtrTargetLanguageData()->GetVerbalType().Find(w->m_sTipo) != -1)
					{
					CConjugateVerbsInSentence objConjugateVerbsInSentence(GetTranslationUserPtr(),GetPtrSentence());
					objConjugateVerbsInSentence.ConjugateVerb(w);
					}
        }
      w->m_sTipo = "NC";
			if(w->m_Trad.Find(' ') == -1 && w->m_Trad.Find('*') == -1)
				w->m_Trad += '*';
			}
		if(w->m_sTipo == "NG")		// no conjugar
			{
			if(w->m_Trad.Find('<') == -1 && w->m_Trad.Find(' ') == -1)
				{
				w->m_Trad = CStringA("<") + w->m_Trad + CStringA(">");
				}
			}
		if(w->m_sTipo != "NC" && w->m_sTipo != "XX" && bMakeCorcondance == true)
			{
			CCoordinateWords objCoordinateWords(GetTranslationUserPtr(),GetPtrSentence());
			objCoordinateWords.MakeConcordance(w);
			}
		}
	}
/////////////////////////////////////////////////////////////////////////////

void CComodin::OrdenaFrase(CStringA &sTranslation)
	{
  CObArray  ArrayInNewOrden;   // nuevo orden que tendrán las palabras dentro del comodín

	for(int j = 0; j < GetPtrWordComodin()->m_ComodinArray.GetSize(); j++)
		{
		for(register int i = 0; i < GetPtrWordComodin()->m_ComodinArray.GetSize(); i++)
			{
			LPCWORD w  = (LPCWORD)GetPtrWordComodin()->m_ComodinArray.GetAt(i);
			ASSERT(w != NULL);
			if(w != NULL && j == w->m_PosFraseComodin - 1)
				{
				if(w->m_Delete == FALSE)
					{
					if(w->m_sTipo == "AA")
						{
						if(w->m_Trad.Find(' ') == -1)
							w->m_Trad += '*';
						}
					if(w->m_sTipo == "VP")
						{
						if(w->m_Trad.Find('<') == -1)
							{
							w->m_Trad = '<' + w->m_Trad + '>';
							}
						}
					sTranslation += w->m_Trad;
					if(j < GetPtrWordComodin()->m_ComodinArray.GetSize() -1)
						sTranslation += ' ';
					}
				break;
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////

void CComodinPhrase::OrdenaFrase()
	{
  CObArray  ArrayInNewOrden;   // nuevo orden que tendrán las palabras dentro del comodín

	for(int j = 0; j < GetPtrWordComodin()->m_ComodinArray.GetSize(); j++)
		{
		for(register int i = 0; i < GetPtrWordComodin()->m_ComodinArray.GetSize(); i++)
			{
			LPCWORD w  = (LPCWORD)GetPtrWordComodin()->m_ComodinArray.GetAt(i);
			ASSERT(w != NULL);
			if(w != NULL && ((j == w->m_PosFraseComodin - 1) || (w->m_PosFraseComodin == 0 && i == j)))
				{
				ArrayInNewOrden.Add(w);
				break;
				}
			}
		}
	GetPtrWordComodin()->m_ComodinArray.RemoveAll();

	for(int i = 0; i < ArrayInNewOrden.GetSize(); i++)
		{
		GetPtrWordComodin()->m_ComodinArray.Add(ArrayInNewOrden.GetAt(i));
		}
	}

////////////////////////////////////////////////////////////////////////
// Ejecuta los comodines que vienen en el diccionario
////////////////////////////////////////////////////////////////////////

void CComodinPhrase::ProccessDataFunction(CMatchItem *ptrFrase)
	{
//	Split();
  WORD CmdType = ptrFrase->GetCmdType();
	if(CmdType != INVALID_COMAND_TYPE)
		{
		m_sType = GetRuleTable()->m_aCmdType.m_FixedArray.GetAt(CmdType);
		}
	else
		{
		m_sType = "FO";
		}

	if(/*m_sFunction.IsEmpty() == FALSE && m_sFunction.GetLength() > 4 &&*/ GetError() == false)
		{
		//int insert = 0;

		GetPtrWordComodin()->m_sTipo = m_sType;
		MakeArrayFrase(GetPtrWordComodin(),GetPtrInitialPos(),GetPtrFinalPos());
		SetSize(GetPtrWordComodin()->m_ComodinArray.GetSize());

		ASSERT(GetPtrSentence()->m_Ora.GetAt(GetPtrInitialPos()) == GetPtrWordComodin());

		CComodinItemData cmdTranslation(GetPtrWordComodin(),GetRuleTable());

//		SetPtrCmdCData(m_sFunction);

		bool bWithoutError = cmdTranslation.Execute(&ptrFrase->m_Execute);

		if(bWithoutError == false)
			SetError(true);

		if(GetError() == false)
			{
			OrdenaFrase();

			GetPtrWordComodin()->m_Buscada = TRUE;
			GetPtrWordComodin()->m_BuscadaC = TRUE;
			GetPtrWordComodin()->m_Traducida = TRUE;
			GetPtrWordComodin()->m_DiWord = DW_WORD;
			GetPtrWordComodin()->m_sTipo = m_sType;

			LPCWORD wtmp = (LPCWORD) new CWord(GetPtrWordComodin()->m_Orig);
			ASSERT(wtmp != NULL);
			if(wtmp != NULL)
				{
				wtmp->CopyWord(*GetPtrWordComodin());
				GetPtrSentence()->m_Ora.SetAt(GetPtrInitialPos(),wtmp);
				}
			}
		}

	}

/////////////////////////////////////////////////////////////////////////////
