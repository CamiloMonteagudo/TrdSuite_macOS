//////////////////////////////////////////////////////////////////////
// FindPhrasesInSentence.cpp: implementation of the CScanWordsInDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindPhrasesInSentence.h"
#include "LengDic.h"
#include "TranslateSentence.h"

/////////////////////////////////////////////////////////////////////////////
// Crea una frase idiomática con lo que existe en la oración desde ini hasta fin
/////////////////////////////////////////////////////////////////////////////

void CFindPhrasesInSentence::MakePhrase(
						LPCWORD ptrWord, 
						POSITION ini, 
						POSITION _fin)
    {                   /* MakeFrase */
    ASSERT(ptrWord != NULL);
    ASSERT(ini != NULL);
    ASSERT(_fin != NULL);
    
    POSITION fin = _fin;
    GetNextPos(fin);
    POSITION nodo = ini;
    GetNextPos(nodo);
	int entreque = 0;
	CStringA entrep;

	for (; nodo != fin; )
        {
        ASSERT(nodo != NULL);
        LPCWORD tmp = GetWordAt(nodo);
        ASSERT(tmp != NULL);
		/******
		if(tmp->m_Persona != TERCERAP)
			ptrWord->m_Persona = tmp->m_Persona;
		if(tmp->m_Modo != MINFINITIVO)
			ptrWord->m_Modo = tmp->m_Modo;
		*****/

		if(tmp->m_EntreParentesis.IsEmpty() == FALSE)
			{
			entreque = tmp->m_EntreQue;
			entrep = tmp->m_EntreParentesis;
			}

        POSITION OldNodo = nodo;        
        GetNextPos(nodo);
        GetWordList().RemoveAt(OldNodo); // cambia de posicion antes de borrar el nodo
        delete tmp;
        }
        
    LPCWORD old = GetWordAt(ini);
    ASSERT(old != NULL);
    old->m_Key       = ptrWord->m_Key; 
    old->m_Orig      = ptrWord->m_Key;
    old->m_Origlwr   = ptrWord->m_Key;
    old->m_Data      = ptrWord->m_Data;
	old->m_Genero    = ptrWord->m_Genero;
    old->m_Trad      = ptrWord->m_Trad;
    old->m_Traducida = ptrWord->m_Traducida;
    old->m_Buscada   = ptrWord->m_Buscada;
    old->m_Modo      = ptrWord->m_Modo;
    old->m_Plural    = ptrWord->m_Plural;
    old->m_Tiempo    = ptrWord->m_Tiempo;
    old->m_Femenino  = ptrWord->m_Femenino;
    old->m_Case      = ptrWord->m_Case;
    old->m_Traducida = (ptrWord->m_Trad.IsEmpty() == FALSE && ptrWord->m_Traducida == TRUE)?ptrWord->m_Traducida: FALSE;  // caso de las frases en el diccionario de nombres p. del texto
    old->m_NoBusca   = FALSE;
	old->m_EntreParentesis = entrep;
	old->m_EntreQue = entreque;
	old->m_Frases  = ptrWord->m_Frases;
	old->m_FrasesTipo  = ptrWord->m_FrasesTipo;
	old->m_RtfFijoFin  = ptrWord->m_RtfFijoFin;
	old->m_RtfFijoIni  = ptrWord->m_RtfFijoIni;
	old->m_StrIniBlank  = ptrWord->m_StrIniBlank;
//    old->m_DicType = w->m_DicType;
    old->m_sTipo   = ptrWord->m_sTipo;
    //W_SetParseType(old,W_GetParseType(w));
    //W_SetDicType(old,W_GetDicType(w));
    //old->MakeIdParse();
    }                   /* MakeFrase */

/////////////////////////////////////////////////////////////////////////////
// Crea una frase idiomática con lo que existe en la oración desde ini hasta fin
/////////////////////////////////////////////////////////////////////////////

void CFindBracketPhraseInSentence::MakePhrase(
						LPCWORD ptrWord, 
						POSITION ini, 
						POSITION _fin)
    {                   /* MakeFrase */
    ASSERT(ptrWord != NULL);
    ASSERT(ini != NULL);
    ASSERT(_fin != NULL);
    
    POSITION fin = _fin;
    GetNextPos(fin);
    POSITION nodo = ini;
    GetNextPos(nodo);
	int entreque = 0;
	CStringA entrep;
	//bool bFraseTComodin = false;		// Si existen frases de comodin de tipo gramatical
	//bool bFraseCComodin = false;		// Si existen frases de comodin de categoria semantica

	CStringA strFrase = ptrWord->m_Key;
	CStrArray ComodinesTPresentes;	// nos da los comodines presentes en la frase [DD1], [DD3], [NP5]
	CStrArray ComodinTTranslation;
	CStrArray ComodinesCPresentes;	// nos da los comodines presentes en la frase <DD1>, <DD3>, <NP5>
	CStrArray ComodinCTranslation;

	if(strFrase.Find(']') != -1)
		{
		//bFraseTComodin = true;

		GetComodinList(strFrase,&ComodinesTPresentes,"[","]");
		FillComodinTranslation(&ComodinTTranslation,ComodinesTPresentes,ini,fin);
		}
	if(strFrase.Find('<') != -1)
		{
		//bFraseCComodin = true;

		GetComodinList(strFrase,&ComodinesCPresentes,"<",">");
		FillCComodinTranslation(&ComodinCTranslation,ComodinesCPresentes,ini,fin);
		}
    for (; nodo != fin; )
        {
        ASSERT(nodo != NULL);
        LPCWORD tmp = GetWordAt(nodo);
        ASSERT(tmp != NULL);
		/******
		if(tmp->m_Persona != TERCERAP)
			ptrWord->m_Persona = tmp->m_Persona;
		if(tmp->m_Modo != MINFINITIVO)
			ptrWord->m_Modo = tmp->m_Modo;
		*****/

		if(tmp->m_EntreParentesis.IsEmpty() == FALSE)
			{
			entreque = tmp->m_EntreQue;
			entrep = tmp->m_EntreParentesis;
			}

        POSITION OldNodo = nodo;        
        GetNextPos(nodo);
        GetWordList().RemoveAt(OldNodo); // cambia de posicion antes de borrar el nodo
        delete tmp;
        }
        
	ASSERT(ComodinesTPresentes.GetCount() == ComodinTTranslation.GetCount());
	for(int ic = 0; ic < ComodinesTPresentes.GetCount() && ic < ComodinTTranslation.GetCount(); ic++)
		{
		ptrWord->m_Key.Replace(ComodinesTPresentes.GetAt(ic),ComodinTTranslation.GetAt(ic));
		ptrWord->m_Data.Replace(ComodinesTPresentes.GetAt(ic),ComodinTTranslation.GetAt(ic));
		ptrWord->m_Buscada = true;
		}

	ASSERT(ComodinesCPresentes.GetCount() == ComodinCTranslation.GetCount());
	for(int ic = 0; ic < ComodinesCPresentes.GetCount() && ic < ComodinCTranslation.GetCount(); ic++)
		{
		ptrWord->m_Data.Replace(ComodinesCPresentes.GetAt(ic),ComodinCTranslation.GetAt(ic));
		ptrWord->m_Buscada = true;
		}

	LPCWORD old = GetWordAt(ini);
    ASSERT(old != NULL);
    old->m_Key       = ptrWord->m_Key; 
    old->m_Orig      = ptrWord->m_Key;
    old->m_Origlwr   = ptrWord->m_Key;
    old->m_Data      = ptrWord->m_Data;
	old->m_Genero    = ptrWord->m_Genero;
    old->m_Trad      = ptrWord->m_Trad;
    old->m_Traducida = ptrWord->m_Traducida;
    old->m_Buscada   = ptrWord->m_Buscada;
    old->m_Modo      = ptrWord->m_Modo;
    old->m_Plural    = ptrWord->m_Plural;
    old->m_Tiempo    = ptrWord->m_Tiempo;
    old->m_Femenino  = ptrWord->m_Femenino;
    old->m_Case      = ptrWord->m_Case;
    old->m_Traducida = (ptrWord->m_Trad.IsEmpty() == FALSE && ptrWord->m_Traducida == TRUE)?ptrWord->m_Traducida: FALSE;  // caso de las frases en el diccionario de nombres p. del texto
    old->m_NoBusca   = FALSE;
	old->m_EntreParentesis = entrep;
	old->m_EntreQue = entreque;
	old->m_Frases  = ptrWord->m_Frases;
	old->m_FrasesTipo  = ptrWord->m_FrasesTipo;
	old->m_RtfFijoFin  = ptrWord->m_RtfFijoFin;
	old->m_RtfFijoIni  = ptrWord->m_RtfFijoIni;
	old->m_StrIniBlank  = ptrWord->m_StrIniBlank;
//    old->m_DicType = w->m_DicType;
    old->m_sTipo   = ptrWord->m_sTipo;

    //W_SetParseType(old,W_GetParseType(w));
    //W_SetDicType(old,W_GetDicType(w));
    //old->MakeIdParse();
    }                   /* MakeFrase */

/////////////////////////////////////////////////////////////////////////////

POSITION CFindPhrasesInSentence::Execute(POSITION ptrNode, LPCWORD ptrWord)
	{
	//COLLECT_INTERVAL("FindFrase1");
	
	CDictMain* dict = static_cast<CDictMain*>(GetDictionary());

  CStringA sFraseAnterior;			// Última frase buscada en el diccionario
  CStringA sLastPhrase;			// Última frase formada para buscar en el diccionario
  CStringA sNewPhrase;				// Nueva frase formada para buscarla en el diccionario

	bool bEntre = false;			// Si se encontró una frase en el diccionario y llegó a concretarse

  POSITION ptrIniNode = ptrNode;	// nodo inicio de la frase 
  POSITION ptrEndPhrase = NULL;	// nodo posible fin de frase 
  POSITION ptrPos = ptrIniNode;   // nodo posible
  BOOL bEncontre = FALSE;

  CStrArray FraseOld;

	// Llena el arreglo con las partes de la palabra que se desea buscar en el diccionario
	FillWordsToFindArray(FraseOld,ptrWord);

  ASSERT(ptrWord != NULL);

  for(int j = 0; j < FraseOld.GetCount() && bEncontre == false; j++)
    {
    CStringA sFraseEncontrada;
    sNewPhrase = FraseOld.GetAt(j);
    if(sNewPhrase.IsEmpty() == TRUE || sNewPhrase == sLastPhrase)
        continue;

		FormatPhrase(sNewPhrase,sNewPhrase);

		//CDict *ptrdccpru = GetDictionary();
    sLastPhrase = sNewPhrase;
		int idxFrase;
    bEncontre = dict->FindInitFrase(sNewPhrase, idxFrase);
    sFraseAnterior = sNewPhrase;

    if(bEncontre) // por lo menos la palabra tiene una frase
			{
			ptrEndPhrase = ptrPos;
			bEncontre = FALSE;
			ptrPos = ptrIniNode;
			int iPhraseSize = 1;
			for (GetNextPos(ptrPos);  ptrPos != NULL; GetNextPos(ptrPos))
				{   
				CStringA LastFrase;
				BOOL entrefrase = FALSE;
				 
				iPhraseSize++;
				ASSERT(ptrPos != NULL);
				ptrWord = GetWordAt(ptrPos);
				ASSERT(ptrWord != NULL);
				CStrArray FraseOld1;

				FillWordsToFindArray(FraseOld1,ptrWord);

        for(int k = 0; k < FraseOld1.GetCount(); k++)
          {
          sNewPhrase = FraseOld.GetAt(j);
          sNewPhrase += ' ';
          sNewPhrase += FraseOld1.GetAt(k);
          if(sNewPhrase == LastFrase)
          	continue;

					FormatPhrase(sNewPhrase,sNewPhrase);

          LastFrase = sNewPhrase;
          if(sFraseAnterior > sNewPhrase)
            {
            dict->FindInitFrase(sNewPhrase, idxFrase);
            sFraseAnterior = sNewPhrase;
            }
          int ffind = dict->FindNextFrase(sNewPhrase, idxFrase);
          sFraseAnterior = sNewPhrase;

					if( ffind != FRS_NOFOUND)
						{
						entrefrase = TRUE;
						if(IsFoundPhraseInDic(sNewPhrase,ffind) == true)
							{
							sFraseEncontrada = sNewPhrase;
							ptrEndPhrase = ptrPos;
							}
						FraseOld.SetAt(j, sNewPhrase);
						break;
						}
					else if(sFraseEncontrada.IsEmpty() == FALSE && k == (FraseOld1.GetCount()-1))
						{
						break;
						}
          }
	      if(entrefrase == TRUE)
          {
          continue;
          }
	      else
          {
          iPhraseSize--;
          break;
          }
        }

      if(sFraseEncontrada.IsEmpty() == TRUE)
        continue;
      
			{
			POSITION inifrase = ptrIniNode;
			POSITION finfrase = ptrEndPhrase;
			LPCWORD iniword = NULL;
			LPCWORD finword = NULL;

			if(inifrase != NULL)
				GetPrevPos(inifrase);
			if(finfrase != NULL)
				GetNextPos(finfrase);
			if(inifrase != NULL)
				iniword=GetWordAt(inifrase);
			if(finfrase != NULL)
				finword=GetWordAt(finfrase);
			if(iniword != NULL && iniword->m_sTipo == "GG")
				continue;
			if(finword != NULL && finword->m_sTipo == "GG")
				continue;
			}
			
      ptrPos = ptrEndPhrase;

      LPCWORD ptrTmpWord = (LPCWORD) new CWord;
      ASSERT(ptrTmpWord != NULL);

      ptrTmpWord->m_Key = sFraseEncontrada;
      sFraseEncontrada.Empty();

			IfFoundPhrase(ptrTmpWord,ptrIniNode,ptrPos,iPhraseSize,ptrWord);
      ptrNode = ptrPos = ptrIniNode;
      bEntre = TRUE;
      delete ptrTmpWord;
      break;
      }
    
    if(bEntre == TRUE)
        break;
    }
  
	return ptrNode;
	}

//////////////////////////////////////////////////////////////////////
//	Si la frase es encontrada en el diccionario
//	LPCWORD ptrTmpWord,	// Nodo temporal donde se almacena la frase
//	POSITION ptrIniNode,// Nodo de inicio de la frase
//	POSITION ptrPos,    // Nodo final de la frase
//	int iPhraseSize,	// Tamaño de la frase
//	LPCWORD ptrWord)	// Palabra de trabajo
//////////////////////////////////////////////////////////////////////

void CFindPhrasesInSentence::IfFoundPhrase(
		LPCWORD ptrTmpWord,	// Nodo temporal donde se almacena la frase
		POSITION ptrIniNode,// Nodo de inicio de la frase
		POSITION ptrPos,    // Nodo final de la frase
		int iPhraseSize,	// Tamaño de la frase
		LPCWORD ptrWord)	// Palabra de trabajo
	{
  VERIFY(FindInDic(ptrTmpWord));

  BOOL plural = FALSE;
  BOOL pasado = FALSE;
  BOOL PuseModo = FALSE;
  CModo       modo;
  CTiempo     tiempo;
  CPersona    persona;
  CCaso       caso;
	CStringA     sTypeFirstWord;
	CStringA     sRtfFijoIni;
	CStringA     sRtfFijoFin;
	CStringA     sStrIniBlank;

	sTypeFirstWord.Empty();
  int i = 0;				// Longitud que va teniendo la frase

  for(POSITION ptrTmpNode = ptrIniNode; 
		ptrTmpNode != NULL && i < iPhraseSize; 
		GetNextPos(ptrTmpNode),i++)
    {
    ASSERT(ptrTmpNode != NULL);

    ptrWord = GetWordAt(ptrTmpNode);// Palabra que se analiza 
    ASSERT(ptrWord != NULL);
    plural = (plural == FALSE) ? ptrWord->m_Plural : plural;
        /// ********
		if(sTypeFirstWord.IsEmpty() == TRUE && (ptrWord->m_sTipo == "VC" || ptrWord->m_sTipo == "VP" || ptrWord->m_sTipo == "JK" || ptrWord->m_sTipo == "JL" || ptrWord->m_sTipo == "VD" || ptrWord->m_sTipo == "JT" || ptrWord->m_sTipo == "JI"))
			{
			sTypeFirstWord = ptrWord->m_sTipo;
			}
		if(i < 3)
			{
			pasado = (pasado == FALSE) ? ((ptrWord->m_Tiempo == PASADO) ? TRUE : FALSE) : pasado;
			}
        // *********/
		if(PuseModo == FALSE)
			{
			modo = (modo == mINFINITIVO && ptrWord->m_Modo != mINFINITIVO)?ptrWord->m_Modo:modo;
			if(ptrWord->m_sTipo == "VT" || ptrWord->m_sTipo == "VI" || ptrWord->m_sTipo == "BE" || ptrWord->m_sTipo == "VA" || ptrWord->m_sTipo == "HA")
				PuseModo = TRUE;
			}

		if(ptrWord->m_RtfFijoIni.IsEmpty() == FALSE)
			sRtfFijoIni += ptrWord->m_RtfFijoIni;
		if(ptrWord->m_RtfFijoFin.IsEmpty() == FALSE)
			sRtfFijoFin += ptrWord->m_RtfFijoFin;
		if(ptrWord->m_StrIniBlank.IsEmpty() == FALSE && sStrIniBlank.IsEmpty() == TRUE)
			sStrIniBlank += ptrWord->m_StrIniBlank;

    tiempo = (tiempo == PRESENTE && ptrWord->m_Tiempo != PRESENTE)?ptrWord->m_Tiempo : tiempo;
    persona = (persona == TERCERAP && ptrWord->m_Persona != TERCERAP)?ptrWord->m_Persona : persona;
    if(ptrTmpNode == ptrIniNode)
        caso = ptrWord->m_Case;
       
    if( ptrTmpNode == ptrPos)
        break;
	  }

	if(sTypeFirstWord.IsEmpty() == FALSE && (ptrTmpWord->m_sTipo == "VV" || ptrTmpWord->m_sTipo == "VT" || ptrTmpWord->m_sTipo == "VI"))
		ptrTmpWord->m_sTipo = sTypeFirstWord;

	if(sTypeFirstWord.IsEmpty() == FALSE && (ptrTmpWord->m_sTipo == "VA"))
		{
		ptrTmpWord->m_sTipo = "VA";
		pasado = TRUE;
		}

  ptrTmpWord->m_Plural     = plural;
  ptrTmpWord->m_Tiempo     = tiempo;
  ptrTmpWord->m_Tiempo     = (pasado == TRUE) ? PASADO : ptrTmpWord->m_Tiempo;
  ptrTmpWord->m_Modo       = modo;
  ptrTmpWord->m_Persona    = persona;
  ptrTmpWord->m_Case       = caso;
	ptrTmpWord->m_RtfFijoFin = sRtfFijoFin;
	ptrTmpWord->m_RtfFijoIni = sRtfFijoIni;
	ptrTmpWord->m_StrIniBlank= sStrIniBlank;
  MakePhrase(ptrTmpWord, ptrIniNode, ptrPos);
	GetTranslationUserPtr()->AddPhraseToList(ptrTmpWord->m_Key);
	}


/////////////////////////////////////////////////////////////////////////////
// Crea un arreglo con los comodines que existen en la frase
/////////////////////////////////////////////////////////////////////////////

void CFindBracketPhraseInSentence::FillComodinTranslation(CStrArray *ComodinesTranslation, CStrArray &ComodinesPresentes,POSITION ini,POSITION fin)
	{
	for(int i = 0; i <  ComodinesPresentes.GetCount(); i++)
		{
		CStringA str(ComodinesPresentes.GetAt(i));
		CStringA npos;
		CStringA tipo;
		int ipos;
		tipo = str.Mid(1,2);
		tipo.MakeUpper();
    //_strupr( (LPSTR)(LPCSTR)tipo );

		npos = str.Mid(3);
		ipos = atoics(npos);
		int pos = 1;
		bool entre = false;
		for(POSITION nodo = ini; nodo != NULL && nodo != fin ; GetNextPos(nodo))
			{
			// me falta hacer la parte que llena las traducciones de los comodines
			// segun el orden del tipo gramatical en la frase
			LPCWORD ptrWord = GetWordAt(nodo);
			if(ptrWord != NULL)
				{
				if(ptrWord->m_sTipo == tipo)
					{
					if(pos == ipos)
						{
						CStringA Translation;
						if(tipo == "NN" || tipo == "NU")
							{
							Translation = ptrWord->m_Trad;
							}
						else
							{
							if(tipo == "NP" && ptrWord->m_Key.IsEmpty())
								{
								Translation = ptrWord->m_Trad;
								//Translation = ptrWord->m_Orig;
								}
							else if(ptrWord->m_Traducida == FALSE)
								{
								CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
								l.Execute(nodo,ptrWord->m_Data,FALSE,TRUE);
						    
								Translation = ptrWord->m_Trad = ptrWord->m_Data;
								ptrWord->m_Traducida = TRUE;
								}
							else
								Translation = ptrWord->m_Trad;
							}
				
						ComodinesTranslation->Add(Translation);
						entre = true;
						break;
						}
					else
						pos++;
					}
				}
			}
		ASSERT(entre == true);
		}
	}
/////////////////////////////////////////////////////////////////////////////
// Crea un arreglo con los comodines que existen en la frase
/////////////////////////////////////////////////////////////////////////////

void CFindBracketPhraseInSentence::FillCComodinTranslation(CStrArray *ComodinesTranslation, CStrArray &ComodinesPresentes,POSITION ini,POSITION fin)
	{
	for(int i = 0; i <  ComodinesPresentes.GetCount(); i++)
		{
		CStringA str(ComodinesPresentes.GetAt(i));
		CStringA npos;
		CStringA tipo;
		int ipos;
		tipo = str.Mid(1,2);
		tipo.MakeUpper();
    //_strupr( (LPSTR)(LPCSTR)tipo );

		npos = str.Mid(3);
		ipos = atoics(npos);
		int pos = 1;
		bool entre = false;
		for(POSITION nodo = ini; nodo != NULL && nodo != fin ; GetNextPos(nodo))
			{
			// me falta hacer la parte que llena las traducciones de los comodines
			// segun el orden del tipo gramatical en la frase
			LPCWORD ptrWord = GetWordAt(nodo);
			if(ptrWord != NULL)
				{
				if(ptrWord->m_sSemantica.Find(tipo) != -1)
					{
					if(pos == ipos)
						{
						CStringA Translation;
						if(tipo == "LL" || tipo == "LU")
							{
							ptrWord->m_Data = ptrWord->m_Orig;
							}
						else
							{
							CLengDic l(GetTranslationUserPtr(),GetPtrSentence());
							l.Execute(nodo,ptrWord->m_Data,FALSE,TRUE);
							}
					
						Translation = ptrWord->m_Trad = ptrWord->m_Data;
						ptrWord->m_Traducida = TRUE;


						ComodinesTranslation->Add(Translation);
						entre = true;
						break;
						}
					else
						pos++;
					}
				}
			}
		ASSERT(entre == true);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// Crea un arreglo con los comodines que existen en la frase
/////////////////////////////////////////////////////////////////////////////

void CFindBracketPhraseInSentence::GetComodinList( const CStringA& ptrFrase, CStrArray *ComodinesPresentes,LPCSTR strOpenBrace,LPCSTR strCloseBrace)
	{
	//ASSERT(ptrFrase != NULL);

	CStringA str(ptrFrase);

	for(;;)
		{
		int i = str.Find(strOpenBrace);		// busca la frase [DD1]
		if( i == -1)
			break;
		else
			{
			if(str[i+4] != strCloseBrace[0])
				break;
			CStringA temp;
			temp = str.Mid(i,5);
			ComodinesPresentes->Add(temp);
			str = str.Right(str.GetLength()-(i+5));
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////

POSITION CFindBracketPhraseInSentence::Execute(POSITION ptrNode, LPCWORD ptrWord)
	{
	COLLECT_INTERVAL("FindFrase2");
	
	CDictMain* dict = static_cast<CDictMain*>(GetDictionary());

  CStringA sFraseAnterior;			// Última frase buscada en el diccionario
  CStringA sLastPhrase;			// Última frase formada para buscar en el diccionario
  CStringA sNewPhrase;				// Nueva frase formada para buscarla en el diccionario
  //CStringA sData;					// Datos asociados a la frase buscada en el diccionario

	bool bEntre = false;			// Si se encontró una frase en el diccionario y llegó a concretarse

  POSITION ptrIniNode = ptrNode;	// nodo inicio de la frase 
  POSITION ptrEndPhrase = NULL;	// nodo posible fin de frase 
  POSITION ptrPos = ptrIniNode;   // nodo posible
  BOOL bEncontre = FALSE;

  CStrArray FraseOld;

	// Llena el arreglo con las partes de la palabra que se desea buscar en el diccionario
	ResetCounter();
	FillWordsToFindArray(FraseOld,ptrWord);

  ASSERT(ptrWord != NULL);

  for(int j = 0; j < FraseOld.GetCount() && !bEncontre; j++)
    {
    CStringA sFraseEncontrada;
    sNewPhrase = FraseOld.GetAt(j);
    if(sNewPhrase.IsEmpty() == TRUE || sNewPhrase == sLastPhrase)
      continue;

		FormatPhrase(sNewPhrase,sNewPhrase);

    sLastPhrase = sNewPhrase;
		//TRACE("Init frase1: \"%s\"", sNewPhrase.c_str())
		int idxFrase = 0;
    bEncontre = dict->FindInitFrase(sNewPhrase, idxFrase);
    sFraseAnterior = sNewPhrase;

    if(bEncontre) // por lo menos la palabra tiene una frase
			{
			ptrEndPhrase = ptrPos;
			bEncontre = FALSE;
			ptrPos = ptrIniNode;
			int iPhraseSize = 1;
			for (GetNextPos(ptrPos);  ptrPos != NULL; GetNextPos(ptrPos))
        {   
        CStringA LastFrase;
        BOOL entrefrase = FALSE;

        iPhraseSize++;
        ASSERT(ptrPos != NULL);
        ptrWord = GetWordAt(ptrPos);
        ASSERT(ptrWord != NULL);
        CStrArray FraseOld1;

				FillWordsToFindArray(FraseOld1,ptrWord);

        for(int k = 0; k < FraseOld1.GetCount(); k++)
          {
          sNewPhrase = FraseOld.GetAt(j);
          sNewPhrase += ' ';
          sNewPhrase += FraseOld1.GetAt(k);
          if(sNewPhrase == LastFrase || sNewPhrase=="xxxx [NP1] xxxx")
              continue;

					FormatPhrase(sNewPhrase,sNewPhrase);

          LastFrase = sNewPhrase;
          if(sFraseAnterior > sNewPhrase)
            {
						//TRACE("Init frase2: \"%s\"", sNewPhrase.c_str())
            dict->FindInitFrase(sNewPhrase, idxFrase);
            sFraseAnterior = sNewPhrase;
            }

          int ffind = dict->FindNextFrase(sNewPhrase, idxFrase);
					//TRACE("result: %d", ffind);
          sFraseAnterior = sNewPhrase;

					if( ffind != FRS_NOFOUND)
						{
						entrefrase = TRUE;
						if(IsFoundPhraseInDic(sNewPhrase,ffind) == true)
							{
							sFraseEncontrada = sNewPhrase;
							ptrEndPhrase = ptrPos;
							}
						FraseOld.SetAt(j, sNewPhrase);
						break;
						}
					else if(sFraseEncontrada.IsEmpty() == FALSE && k == (FraseOld1.GetCount()-1))
						{
						break;
						}
          }
        
        if(entrefrase == TRUE)
        	{
          continue;
          }
        else
          {
          iPhraseSize--;
          break;
          }
        }

      if(sFraseEncontrada.IsEmpty() == TRUE)
         continue;
      
			{
			POSITION inifrase = ptrIniNode;
			POSITION finfrase = ptrEndPhrase;
			LPCWORD iniword = NULL;
			LPCWORD finword = NULL;

			if(inifrase != NULL)
				GetPrevPos(inifrase);
			if(finfrase != NULL)
				GetNextPos(finfrase);
			if(inifrase != NULL)
				iniword=GetWordAt(inifrase);
			if(finfrase != NULL)
				finword=GetWordAt(finfrase);
			if(iniword != NULL && iniword->m_sTipo == "GG")
				continue;
			if(finword != NULL && finword->m_sTipo == "GG")
				continue;
			}
      ptrPos = ptrEndPhrase;

      LPCWORD ptrTmpWord = (LPCWORD) new CWord;
      ASSERT(ptrTmpWord != NULL);

			//TRACE(">>>>>> Phrase Found: \"%s\"", sFraseEncontrada.c_str());
      ptrTmpWord->m_Key = sFraseEncontrada;
      sFraseEncontrada.Empty();

			IfFoundPhrase(ptrTmpWord,ptrIniNode,ptrPos,iPhraseSize,ptrWord);
      ptrNode = ptrPos = ptrIniNode;
      bEntre = TRUE;
      delete ptrTmpWord;
      break;
			}
    
    if(bEntre == TRUE)
    	break;
    }
  
	return ptrNode;
	}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//int CharacterCount(LPCSTR str, char c)
int CharacterCount(const CStringA& str, char c)
	{
	//ASSERT(str != NULL);
	//CStringA s(str);
	int count = 0;
	int i = -1;
	for(;;)
		{
		i = str.Find(c, i+1);
		if(i != -1)
			{
			count++;
			//s = s.Right(s.GetLength()-i-1);
			}
		else
			break;
		}
	return count;
	}

