// CmdCom.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "UtilFun.h"
#include "COra.h"
#include "StepInSentence.h"
#include "CmdItKey.h"

/////////////////////////////////////////////////////////////////////////////
// funciones para el trabajo con los comodines asociados a una palabra o frase
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Compara el tipo de la palabra con el de la frase hecha
/////////////////////////////////////////////////////////////////////////////
/**********
bool CComodinItemKey::TypeComparation(	
	CStringA tipos,		// Tipos de la frase hecha
	CStringA tipo)		// Tipo de la palabra que se analiza
	{
//	CStringA fs(";RD;RI;AA;AI;SS;NP;");
//	CStringA fv(";VV;VA;VT;VI;VP;BE;HA;");
	bool ret = false;

	if(tipos.Find(tipo) != -1)
		ret = true;

// *************
	else if(tipos == "FS")
		{
		if(fs.Find(tipo) != -1)
			ret = true;
		}
	else if(tipos == "FV")
		{
		if(fv.Find(tipo) != -1)
			ret = true;
		}
// *************
	return ret;
	}
************/
/////////////////////////////////////////////////////////////////////////////
// Compara el tipo de la palabra con el de la frase hecha
/////////////////////////////////////////////////////////////////////////////

bool CComodinItemKey::SemanticComparation(	
	const CStringA& sWordField,		// Tipos de la frase hecha
	LPCSTR sSemanticPatter)		// Tipo de la palabra que se analiza
	{
	//COUNTER("SemanticComparation"); // = 2039
	if (sWordField.IsEmpty())
		return false;

  CSmartStrArray Array;

  StringToArray(sWordField, &Array, ",;");

  for(int i = 0; i < Array.GetCount(); i++)
    {
	  if(strstr(sSemanticPatter, Array.GetAtSZ(i)) != NULL)
	  //if(sSemanticPatter.Find(Array[i]) != -1)
			return true;
    }

	return false;
	}
/////////////////////////////////////////////////////////////////////////////
// Compara el sufijo de la palabra con la lista de sufijos
/////////////////////////////////////////////////////////////////////////////

bool CComodinItemKey::SuffixComparation(	
	const CStringA& sWordField,		
	LPCSTR sSuffixes)
	{
	//COUNTER("SuffixComparation"); // = 3278
	if (sWordField.IsEmpty())
		return false;

  CSmartStrArray Array;

  StringToArray(sSuffixes, &Array, ",;");

  for(int i = 0; i < Array.GetCount(); i++)
    {
		if (sWordField.EndWith(Array.GetAtSZ(i)))
			return true;
    }

	return false;
	}
/////////////////////////////////////////////////////////////////////////////
// Compara el prefijo de la palabra con la lista de prefijos
/////////////////////////////////////////////////////////////////////////////

bool CComodinItemKey::PreffixComparation(	
	const CStringA& sWordField,		
	LPCSTR sPreffixes)
	{
	if (sWordField.IsEmpty())
		return false;

  CSmartStrArray Array;

  StringToArray(sPreffixes, &Array, ",;");

  for(int i = 0; i < Array.GetCount(); i++)
    {
		if (sWordField.StartWith(Array.GetAtSZ(i)))
		//CStringA tmp(sWordField.Left(sPreffix.GetLength()));
	  //if(sPreffix == tmp)
			return true;
    }

	return false;
	}

// asume que strMain siempre termina con cMark, o esta vacia
bool CComodinItemKey::StrFindBetweenMark3(LPCSTR strMain, LPCSTR aFind1, LPCSTR aFind2, LPCSTR aFind3, char cMark)
	{
	if( strMain==NULL || strMain[0]=='\x0' )
		return false;

	LPCSTR arrFind[3];
	arrFind[0] = aFind1;
	arrFind[1] = aFind2;
	arrFind[2] = aFind3;

	LPCSTR sFind_0;
	register LPCSTR sMain;
	register LPCSTR sFind;

	for(int i=0; i<3; i++)
		{
		if(*arrFind[i] != 0)
			{
			sMain = strMain;
			sFind_0 = arrFind[i];
			for(;;)
				{
				sFind = sFind_0;

				for(;; sMain++,sFind++)
					{
					if (*sFind == 0)
						{
						if (/**sMain == 0 || */*sMain == cMark)
							return true; // comparacion existosa
						else
							break; // se llego al fin de sFind pero sMain tiene mas caracteres
						}

					if (*sFind != *sMain)
						break; // las palabras difieren
					}

				if ((BYTE)*sFind < (BYTE)*sMain && /**sMain != 0 &&*/ *sMain != cMark)
					break; // como sMain esta ordenada, ya no vamos a encontrar a sFind

				for(; *sMain != cMark /*&& *sMain != 0*/; sMain++);

				//sMain++; // saltarse el cMark y ponerse al inicio de palabra
				if (*(++sMain) == 0)
					break; // no hay mas palabras que buscar en sMain
				}
			}
		}

	return false;
	}

bool FindTipo(const CStringA& aListaTipos, const CStringA& aTipo);

bool FindTipo(const CStringA& aListaTipos, const CStringA& aTipo)
	{
	/*if (aListaTipos.GetLength() > 500)
		{
		//COUNTER("goofd");
		return false;
		}*/
	//COUNTER(CStringA("tipo_" + aTipo).c_str());
	//COLLECT_INTERVAL("FindTipo");

#pragma pack( push , 1)
	 struct TipoEntry
		{
		WORD tipo;
		BYTE coma;
		};
#pragma pack( pop)

	TipoEntry* pListaTipos = (TipoEntry*)aListaTipos.c_str();
	WORD sTipo = *((WORD*)aTipo.c_str());

#if 0
	int len = aListaTipos.GetLength();// >> 1;
	if (len & 1)
		COUNTER(aListaTipos.c_str());
	len = len >> 1;

	for(; len > 0; pListaTipos += 1, len -= 1)
		{
		if (pListaTipos[0].tipo == sTipo)
			return true;
		}

	return false;

	for(; len > 0; pListaTipos += 4, len -= 12)
		{
		if (len > 8)
			{
			if (pListaTipos[0].tipo == sTipo)
				return true;
			if (pListaTipos[1].tipo == sTipo)
				return true;
			if (pListaTipos[2].tipo == sTipo)
				return true;
			if (pListaTipos[3].tipo == sTipo)
				return true;
			}
		else
			switch (len)
				{
				case 8: // 3 tipos
					if (pListaTipos[2].tipo == sTipo)
						return true;
				case 5: // 2 tipos
					if (pListaTipos[1].tipo == sTipo)
						return true;
				case 2: // 1 tipo
					if (pListaTipos[0].tipo == sTipo)
						return true;
				}
		}

	return false;

#else

	for(;; pListaTipos += 4)
		{
		if (pListaTipos[0].tipo == sTipo)
			return true;
		else if (pListaTipos[0].coma == 0)
		//else if (*((LPBYTE)&(pListaTipos[1].tipo)) == 0)
			return false;
		if (pListaTipos[1].tipo == sTipo)
			return true;
		//else if (*((LPBYTE)&(pListaTipos[2].tipo)) == 0)
		else if (pListaTipos[1].coma == 0)
			return false;
		if (pListaTipos[2].tipo == sTipo)
			return true;
		//else if (*((LPBYTE)&(pListaTipos[3].tipo)) == 0)
		else if (pListaTipos[2].coma == 0)
			return false;
		if (pListaTipos[3].tipo == sTipo)
			return true;
		//else if (*((LPBYTE)&(pListaTipos[4].tipo)) == 0)
		else if (pListaTipos[3].coma == 0)
			return false;
		}
#endif
	}

////////////////////////////////////////////////////////////////////////////////////
// para poder preguntar por el final lleve esta parte a funcion
////////////////////////////////////////////////////////////////////////////////////

bool CComodinItemKey::MatchItemFunction(CItemKeyWord* itemKeyWord, LPCWORD ptrWord)
	{
	//COLLECT_INTERVAL("MatchItemFunction"); // 0.69s  1450302
	ASSERT(ptrWord != NULL);

#if defined(_DEBUG)
				CStringA strDebug = GetPtrPhrase();
#endif					

	bool bMatched = true;

#ifdef LOAD_FROM_DCX
		{
		int j=0;
#else
	for(int j = 0; j < 2 && bMatched; j++)
		{
#endif

		BYTE kktmp = itemKeyWord->getOp(j);

#ifndef LOAD_FROM_DCX
		if(kktmp == eNull || kktmp == 0xcd)
			return true;
#endif

		WORD kkitmp = itemKeyWord->getIndex(j); // debia esta hecho de esta forma debo cambiarlo si viro atras

		switch(kktmp)
			{
			case  eWord:		// caso de lista de palabras
				{
				//COLLECT_INTERVAL("eWord"); // 0.28 67918
//					if(!strcmp(ptrWord->m_Key,",") || !strcmp(ptrWord->m_Key,";") )
//					if(ptrWord->m_Key == ',' || ptrWord->m_Key == ';' )
				LPCSTR sKey = ptrWord->m_Key.c_str();
				if ((sKey[0] == ',' || sKey[0] == ';') && sKey[1] == 0)
					{
					bMatched = false;
					}
				else
					{
#ifdef LOAD_FROM_DCX
					bMatched = StrFindBetweenMark3(GetRuleTable()->m_aWords.m_FixedArray.GetAt(kkitmp), 
#else
					bMatched = StrFindBetweenMark3(GetRuleTable()->m_aWords.m_FixedArray.GetAt(kkitmp).c_str(), 
#endif
							sKey, ptrWord->m_Orig.c_str(), ptrWord->m_Origlwr.c_str(), ',');
					}
				/*if(StrFindBetweenMark(TmpStr, ptrWord->m_Key,',') == false)
					{
					if(StrFindBetweenMark(TmpStr, ptrWord->m_Orig,',') == false)
						{
						if(StrFindBetweenMark(TmpStr, ptrWord->m_Origlwr,',') == false)
							mal = TRUE;
						}
					}*/
 				//TRACE(" word=%d", bMatched);
				break;
				}
			case eKwrd:	// siempre es true porque es la palabra
				{					
				//COLLECT_INTERVAL("eKwrd"); // 0.0 0
				break;
				}    
			case eFind:   // caso de buscar una palabra dentro de la oracion
				{
				//COLLECT_INTERVAL("eFind"); // 0.0 0
				CStringA TmpStr = GetRuleTable()->m_aFinds.m_FixedArray.GetAt(kkitmp);
				//if(strcmp(TmpStr,ptrWord->m_Key) && strcmp(TmpStr,ptrWord->m_Orig) && strcmp(TmpStr,ptrWord->m_Origlwr))
				if(TmpStr != ptrWord->m_Key && TmpStr != ptrWord->m_Orig && TmpStr != ptrWord->m_Origlwr)
					bMatched = false;
				break;
				}    
			case eFUpper:    // caso de primera mayusculas
				{				
				//COLLECT_INTERVAL("eFUpper"); // 0.0 3561
				if((ptrWord->m_Case != C_MIXTO && ptrWord->m_Case != C_UPPER) /*|| TypeComparation(tmp->m_Items[j],ptrWord->m_sTipo) == FALSE*/)
					bMatched = false;
				break;
				}    
			case eField:    // caso de campo semantico
				{				
				//COLLECT_INTERVAL("eField"); // 0.0 2039
				bMatched = SemanticComparation(ptrWord->m_sSemantica,
#ifdef LOAD_FROM_DCX
					GetRuleTable()->m_aFields.m_FixedArray.GetAt(kkitmp));
#else
					GetRuleTable()->m_aFields.m_FixedArray.GetAt(kkitmp).c_str());
#endif
				break;
				}    
			case eAUpper:    // caso de palabra completa en mayusculas
				{				
				//COLLECT_INTERVAL("eAUpper"); // 0.0 30
				if((ptrWord->m_Case != C_UPPER) /*|| TypeComparation(tmp->m_Items[j],ptrWord->m_sTipo) == FALSE*/)
					bMatched = false;
				break;
				}    
			case eSuffix:    // caso de los sufijos
				{				
				//COLLECT_INTERVAL("eSuffix"); // 0.0 76
#ifdef LOAD_FROM_DCX
				LPCSTR TmpStr = GetRuleTable()->m_aSuffixes.m_FixedArray.GetAt(kkitmp);
#else
				LPCSTR TmpStr = GetRuleTable()->m_aSuffixes.m_FixedArray.GetAt(kkitmp).c_str();
#endif
				bMatched = SuffixComparation(ptrWord->m_Origlwr, TmpStr)
								&& SuffixComparation(ptrWord->m_Orig, TmpStr);
				/*if(SuffixComparation(ptrWord->m_Origlwr, TmpStr) == false)
					bMatched = false;
				else
					bMatched &= SuffixComparation(ptrWord->m_Orig, TmpStr);*/
				break;
				}    
			case ePreffix:    // caso de los prefijos
				{				
				//COLLECT_INTERVAL("ePreffix"); // 0.0 0
#ifdef LOAD_FROM_DCX
				LPCSTR TmpStr = GetRuleTable()->m_aPreffixes.m_FixedArray.GetAt(kkitmp);
#else
				LPCSTR TmpStr = GetRuleTable()->m_aPreffixes.m_FixedArray.GetAt(kkitmp).c_str();
#endif
				bMatched = PreffixComparation(ptrWord->m_Origlwr, TmpStr)
								&& PreffixComparation(ptrWord->m_Orig, TmpStr);
				break;
				}    
			case eType:		// caso de lista de tipos gramaticales
				{
				//COLLECT_INTERVAL("eType"); // 0.27s (0.25, 0.22) 849590
				//mal = (strstr(TmpStr,ptrWord->m_sTipo)==NULL)?true:mal;
				//CStringA TmpStr = GetRuleTable()->m_aTypes.m_FixedArray.GetAt(kkitmp);
				//if (TmpStr.Find(ptrWord->m_sTipo)==-1)
				// produc cambios de "no existe" por "alli no esta"
				//bMatched = FindTipo(GetRuleTable()->m_aTypes.m_FixedArray.GetAt(kkitmp),
				// ptrWord->m_sTipo);
				//bMatched = GetRuleTable()->m_aTypesT.GetAt(kkitmp).ContainType(ptrWord->m_sTipo.c_str());
				bMatched = GetRuleTable()->m_aTypesT.GetAt(kkitmp).ContainType(ptrWord->m_Tipo);
				//TRACE(" type=%d (%s)", bMatched, ptrWord->m_Tipo.toString());
				//if (strstr(GetRuleTable()->m_aTypes.m_FixedArray.GetAt(kkitmp).c_str(), 
				//	ptrWord->m_sTipo.c_str()) == NULL)
				//	bMatched = false;
				/*if (bMatched1 != bMatched)
					TRACE(GetRuleTable()->m_aTypes.m_FixedArray.GetAt(kkitmp).c_str());*/
				break;
				}
			}
		}

	return bMatched;
	}
/////////////////////////////////////////////////////////////////////////////
//  Comprueba si la frase casa con la oración a partir del nodo dado
/////////////////////////////////////////////////////////////////////////////

bool CComodinItemKey::Match(CMatchItem *ptrMatchItem, POSITION posStart, POSITION posEnd)
  {
	//COLLECT_INTERVAL("CComodinItemKey::Match"); // n=1029461 time=1.19s

	if(GetActualPosition() == NULL)
		return false;

	// busca el nodo inicial y final
	//POSITION posStart = FindInitialPosition(ptrMatchItem->GetMainWordPos());
	//POSITION posEnd = FindFinalPosition(ptrMatchItem->GetMainWordPos(), matchLen);
		
	int matchLen = ptrMatchItem->m_ItemKeyword.GetSize();
	
	//SetInitialPosition(posStart);

	bool bMatched = true;
	int i = 0;
	for(POSITION node = posStart; i < matchLen && bMatched; i++)
		{
		ASSERT(node != NULL);

		CItemKeyWord *tmp = (CItemKeyWord *)&ptrMatchItem->m_ItemKeyword[i];
		if (tmp->getOp(0) != eKwrd)
			bMatched = MatchItemFunction(tmp, GetWordAt(node));

		if(i == 0 && bMatched && matchLen > 4)		// voy a preguntar por el ultimo
			{
			CItemKeyWord *tmp = (CItemKeyWord *)&ptrMatchItem->m_ItemKeyword[matchLen-1];
			if (tmp->getOp(0) != eKwrd)
				bMatched = MatchItemFunction(tmp, GetWordAt(posEnd));
			}
		
#ifdef LOAD_FROM_DCX
		if (!tmp->isAnd())
#endif
			GetNextPos(node);
		}

	if (bMatched)     // encontre la frase buscar lo que se hace en frases normales
		{                
		// busca el nodo final de la frase
		SetInitialPosition(posStart);
		SetFinalPosition(posEnd);
		return true;
		}

  return false;
  }

/////////////////////////////////////////////////////////////////////////////
// Asumiendo que GetActualPosition() representa la posicion del keyword (que 
// corresponde a aMainWordPos) esta funcion devuelve la posicion de la palabra 
// donde se debe empezar a machear (el inicio del match)
/////////////////////////////////////////////////////////////////////////////
/*POSITION CComodinItemKey::FindInitialPosition(int aMainWordPos)
	{
	POSITION ptrTempNode = GetActualPosition();

	for(; ptrTempNode != NULL && aMainWordPos > 0; aMainWordPos--)
		{
	#if defined(_DEBUG)
		LPCWORD ptrWord = GetWordAt(ptrTempNode);   // Palabra que se analiza 
		ASSERT(ptrWord != NULL);
	#endif
		GetPrevPos(ptrTempNode);
		}

	#if defined(_DEBUG)
		if(ptrTempNode != NULL)
			{
			LPCWORD ptrWord = GetWordAt(ptrTempNode);   // Palabra que se analiza 
			ASSERT(ptrWord != NULL);
			}
	#endif

	return ptrTempNode;
	}*/

/////////////////////////////////////////////////////////////////////////////

/*POSITION CComodinItemKey::FindFinalPosition(int aMainWordPos, int aMatchLen)
	{
	POSITION ptrNode = GetInitialPosition();
	POSITION ptrRetPos = ptrNode;

	ASSERT(ptrNode != NULL);

	for(int i = 0; i < size; i++)
		{
		if(ptrNode == NULL)
			break;
		ASSERT(ptrNode != NULL);
		ptrRetPos = ptrNode;
		GetNextPos(ptrNode);

		#if defined(_DEBUG)
			LPCWORD ptrWord = GetWordAt(ptrRetPos);   // Palabra que se analiza 
			ASSERT(ptrWord != NULL);
		#endif
		}
	return ptrRetPos;
	}*/

/////////////////////////////////////////////////////////////////////////////
