//---------------------------------------------------------------------------
// ConjVerb.cpp : Defines the initialization routines for the DLL.
//---------------------------------------------------------------------------

#include <stdlib.h>
#include "ConjVerb.h"


//---------------------------------------------------------------------------
// CConjVerb
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
// Carga todos los diccionarios de datos necesarios para la conjugación de verbos segun el idioma dado 
//--------------------------------------------------------------------------------------------------------------
BOOL CConjVerb::Open( int Lang )                // Pareja de idiomas como un entero
	{
	if( m_Dicts != NULL )                         // Ya habia una dirección abierta
		{
    m_Dicts->Close();
		delete m_Dicts;                             // La libera
		}

  m_Dicts = new CDicRef( Lang, m_DataPath );
	if( m_Dicts == NULL )
		return FALSE;

  if( !m_Dicts->Load() )
	  {
	  delete m_Dicts;
	  m_Dicts = NULL;
	  return FALSE;
	  }

  m_Lang = Lang;
	return TRUE;
	}

//--------------------------------------------------------------------------------------------------------------
// Función que salva todos los diccionarios de datos si hubo modificaciones
//--------------------------------------------------------------------------------------------------------------
BOOL CConjVerb::Close()
	{
	if( m_Dicts )
    {
    m_Dicts->Close();
	  delete m_Dicts;

	  m_Dicts = NULL;
    m_Lang  = -1;
    }

	return TRUE;
	}

//--------------------------------------------------------------------------------------------------------------
// Devuelve el número total de conjugaciones posibles para un idioma dado
//--------------------------------------------------------------------------------------------------------------
int CConjVerb::Count()
	{
	if( m_Dicts == NULL )
		return (WORD)-1;

	CRefTabla* RefTab = m_Dicts->GetPtrReferTable();

	if( RefTab == NULL )
		return (WORD)-1;

	return(RefTab->m_DataConjArray.GetCount());
	}

//--------------------------------------------------------------------------------------------------------------
// Devuelve los datos de la conjugación con indice idx
//--------------------------------------------------------------------------------------------------------------
CDataConj* CConjVerb::DataConj(int idx)
	{
	if( m_Dicts == NULL )
		return NULL;

	CRefTabla* RefTab = m_Dicts->GetPtrReferTable();

	if( !RefTab || idx < 0 || idx >= RefTab->m_DataConjArray.GetCount() )
		return NULL;

	return((CDataConj*)RefTab->m_DataConjArray[idx]);
	}

//--------------------------------------------------------------------------------------------------------------
// Devuelve el nombre de la conjugación con indice idx
//--------------------------------------------------------------------------------------------------------------
CStringA CConjVerb::Name(int idx)
	{
  CDataConj* Dat = DataConj(idx);
	if( Dat == NULL )
		return CANull;

	return(Dat->m_Nombre);
	}

//--------------------------------------------------------------------------------------------------------------
// Función que devuelve el modo de la conjugación idx en el idioma dado
//--------------------------------------------------------------------------------------------------------------
int CConjVerb::Mood(int idx)
	{
  CDataConj* Dat = DataConj(idx);
	if( Dat == NULL )
		return -1;

	return(Dat->m_Modo);
	}

//--------------------------------------------------------------------------------------------------------------
// Función que devuelve el tiempo verbal de la conjugación idx en el idioma dado
//--------------------------------------------------------------------------------------------------------------
int CConjVerb::Time(int idx)
	{
  CDataConj* Dat = DataConj(idx);
	if( Dat == NULL )
		return -1;

	return(Dat->m_Tiempo);
	}

//--------------------------------------------------------------------------------------------------------------
// Función que devuelve si la conjugación idx corresponde a un tiempo compuesto o no en el idioma dado
//--------------------------------------------------------------------------------------------------------------
BOOL CConjVerb::Compound(int idx)
	{
  CDataConj* Dat = DataConj(idx);
	if( Dat == NULL )
		return FALSE;

	return(Dat->m_Compuesto);
	}

//--------------------------------------------------------------------------------------------------------------
// Función principal para conjugar un verbo
//--------------------------------------------------------------------------------------------------------------

CString CConjVerb::ConjugaVerb( const CString& verboi, Modo modo, Tiempo tiempo, Persona persona, Numero numero, BOOL negado, BOOL compuesto)
	{
	CString sEmpty;
	if( m_Dicts == NULL )
		return sEmpty;

	CTabla *Tabla = m_Dicts->GetPtrVerbTable(); 

	if( Tabla == NULL )
		return sEmpty;

	CString verbo(verboi);
  //_strlwr( (LPSTR)(LPCSTR)verbo );
	verbo.MakeLower();

  int fila = -1;
  int col = -1;

	//Buscar si el verbo es separable o no y hallar el prefijo
	CString verbprefix, rootverb, verbasic;

	if( FindVerbPrefix(verbo, verbprefix, verbasic) )
		verbo = verbasic;

	//Buscar si el verbo es reflexivo y hallar la raíz
	CString prefreflex;

	if( ReflexiveVerb(verbo,rootverb) )
		{
		prefreflex = ReflexivePronoun(persona,numero);
		verbo = rootverb;
		}

	CString verbsimp;
	if( (modo == mIMPERATIVO) && (!prefreflex.IsEmpty()))   //Imperativo Reflexivo
			verbsimp = "-";

	if(verbsimp != "-")
		verbsimp = ConjVerbSimple(verbo,modo,tiempo,persona,numero,&fila,&col);

	if( verbsimp.IsEmpty() || verbsimp == "-")              //Hubo error o no existe la conj.
		return (m_Ret = verbsimp);
	
	CString prefijo,sufijo;	                              //Prefijo reflexivo según la persona
	BOOL verboinf = FALSE;                                  //Si el verbo negado está en infinitivo

	//Si se quiere conjugar en un tiempo simple
	if( (compuesto == FALSE) || (modo == mIMPERATIVO) || (modo == mINFINITIVO) || (modo == mPARTICIPIO) || (modo == mGERUNDIO))
		{
		Prefijo_Conjugac( fila, col, negado, prefijo, sufijo, &verboinf );

		if( verboinf )	                                      //El verbo con afijo va en infinitivo
			verbsimp = verbo;

		if( !prefreflex.IsEmpty() )
			{
			verbsimp = prefreflex;
			verbsimp += " ";
			verbsimp += verbsimp;
			}

		if( !prefijo.IsEmpty() ) 
			{
			if( !verbprefix.IsEmpty() ) 
				{
				CString prefix = verbprefix;
				if( prefix[0] == '.' )                            // Es un verbo separable
					{
					prefix = prefix.Mid(1);
					if(modo == mPARTICIPIO)
						{
						verbsimp = prefix;
						verbsimp += prefijo;
						verbsimp += verbsimp;       //ausgearbeite
						}
					else
						{
						verbsimp = prefijo;
						verbsimp += prefix;
						verbsimp += verbsimp;       //ausgearbeite
						}
					}
				else                                              // Es un verbo no separable
					{
					if(modo == mPARTICIPIO)	
						verbsimp = prefix + verbsimp;                 //beratet
					else
						verbsimp = prefijo + prefix + verbsimp;       //ausgearbeite
					}
				}
			else
				verbsimp = prefijo + verbsimp;
			}
		else	                                                // Si no tiene prefijo
			{
			if( !verbprefix.IsEmpty() ) 
				{
				CString prefix = verbprefix;
				if( prefix[0] == '.' )                            // Es un verbo separable
					{
					prefix = prefix.Mid(1);
					if(modo == mPARTICIPIO || modo == mGERUNDIO || modo == mINFINITIVO)	
						verbsimp = prefix + verbsimp;                 //ausarbeiten
					else
						verbsimp = verbsimp + verbprefix;             //arbeite.aus
					}
				else
					verbsimp = verbprefix + verbsimp;               //beraten
				}
			}

		if( !sufijo.IsEmpty() ) 
			verbsimp = verbsimp + sufijo;

		return (m_Ret = verbsimp);
		}

	//Si se quiere conjugar en un tiempo compuesto
	CString verbaux;
	TDictData verbauxinf;
	int filavaux = -1;
	int colvaux = -1;

	// se busca en el diccionario el verbo auxiliar para tiempos compuestos
	CString suf,pref,sufn,prefn;

	if(!verbprefix.IsEmpty())
		verbo = verboi;

	CDict *AuxVerbs = m_Dicts->GetPtrAuxVerbDict(); 

	if( AuxVerbs == NULL )
		return sEmpty;
	
	if(!AuxVerbs->GetKey(verbo,verbauxinf))                // verbo aux.
		{
		CString defverb("1default_auxverb");
		if(!AuxVerbs->GetKey(defverb,verbauxinf))
			return sEmpty;
		}

	if( verbauxinf=="-" )                                   // No existe verbo auxiliar para tiempos compuestos
		return (m_Ret=verbauxinf);

	verbaux = ConjVerbSimple(verbauxinf,modo,tiempo,persona,numero,&filavaux,&colvaux);
	if( verbaux.IsEmpty())                                  //Hubo error
		return sEmpty;

	Prefijo_Conjugac( filavaux, colvaux, negado, prefijo, sufijo, &verboinf );

	if( verboinf )	                                        //El verbo con afijo va en infinitivo
		verbaux = verbo;

	if(!prefreflex.IsEmpty())
		verbaux = prefreflex + " " + verbaux;

	if( !prefijo.IsEmpty() ) 
		verbaux = prefijo + verbaux;

	if( !sufijo.IsEmpty() ) 
		verbaux = verbaux + sufijo;

	CString participio;

	CRefTabla *tabref = m_Dicts->GetPtrReferTable();  

	if( tabref == NULL )
		return sEmpty;

	int ipart = tabref->m_TabColIndex[md_ParticipioPasado];
	if( ipart > 0 )
		{
		CString sinf, rinf;

		if(!verbprefix.IsEmpty())
			verbo = verbasic;

		sinf = Tabla->GetCelda(fila,md_Infinitivo);           //sufijo del infinit.
		verbo += ' ';								                          //del verbo patrón
		int pos = verbo.Find(sinf+' ');	                      //Se busca suf. inf. en el verbo orig.
//		verbo.TrimRight();
        
		rinf = verbo.Left(pos);	                              //Raíz Infinitivo
		LPCSTR ter = Tabla->GetCeldaSZ(fila,ipart);	                  //Terminación del verbo conjugado
		
		participio = rinf+ter;                                //participio

		prefijo.Empty();
		Prefijo_Conjugac( fila, ipart, negado, prefijo, sufijo, &verboinf );

		if( !prefijo.IsEmpty() )                              //Si tiene prefijo el participio
			{
			if( !verbprefix.IsEmpty() ) 
				{
				CString prefix = verbprefix;
				if( prefix[0] == '.' )                            // Es un verbo separable
					{
					prefix = prefix.Mid(1);
					participio = prefix + prefijo + participio;     //ausgearbeitet
					}
				else                                              // Es un verbo no separable
					participio = prefix + participio;               //beratet (no lleva ge)
				}
			else
				participio = prefijo + participio;
			}
		
		return (m_Ret = verbaux + " " + participio);
		}

	return sEmpty;
	}

//--------------------------------------------------------------------------------------------------------------
// Función que declina un sustantivo según el género, el número y el caso
//--------------------------------------------------------------------------------------------------------------
CStringA CConjVerb::DeclineNoun( const CStringA& noun, Genero /*gen*/, Numero num, Declination decl)
	{
	if( m_Dicts == NULL )
		return CANull;

	CString strnoun(noun);
	CString declNoun(noun);

	return (m_Ret = declNoun);
	}

//--------------------------------------------------------------------------------------------------------------
// Función que declina un adjetivo según el género, el número el caso y el tipo de artículo que se le inserta 
//--------------------------------------------------------------------------------------------------------------
CStringA CConjVerb::DeclineAdjective( const CStringA& adj, Genero gen, Numero num, Declination decl, ArticleType artic)
	{
	if( m_Dicts == NULL )
		return CANull;

	CString declAdj(adj);

	return (m_Ret=declAdj);
	}

//--------------------------------------------------------------------------------------------------------------
// Función que concuerda un sustantivo o adjetivo en un género, número y grado determinado
//--------------------------------------------------------------------------------------------------------------
CStringA CConjVerb::ConcordWords( const CStringA& noun, Genero genfuente, Numero numfuente, Genero gendestino, Numero numdestino, Grado adjgrado)
	{
	if( m_Dicts == NULL )
		return CANull;

	CString strnoun(noun);

  strnoun.MakeLower();

	CString adjf(strnoun);
	TDictData adjconc;
  int pos;

	if( adjgrado == SUPERLATIVO )
		{
		//Se busca el adj. en el dicc de Superl. Irreg
		CDictLoad *AdjSupIDic = m_Dicts->GetPtrIrregSupDict(); 

		if( AdjSupIDic == NULL )
			return CANull;

		if( AdjSupIDic->GetKeyCS(adjf.c_str(), adjconc)==FALSE )
			{	
			//Se busca el adj. en el dicc de Sufijos de Superl.
			CDictLoad *AdjSupSufDic = m_Dicts->GetPtrSuffixSupDict(); 

			if( AdjSupSufDic == NULL )
				return CANull;

			LPCSTR szAdjconc;
			if( AdjSupSufDic->FindSuffix(adjf, szAdjconc)==TRUE)
				{
				LPCSTR p = strchr(szAdjconc, ',');
				CStringA s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

				//pos = adjconc.Find(',');
				//CStringA s1 = adjconc.Left(pos);
				//CStringA s2 = adjconc.Mid(pos+1);

				adjconc = adjf + '#';
				pos = adjconc.Find(s1+'#');
				adjconc = adjconc.Left(pos) + s2;
				}
			}   
		}

	if( adjgrado == COMPARATIVO )
		{
		//Se busca el adj. en el dicc de Comparat. Irreg
		CDictLoad *AdjCompIDic = m_Dicts->GetPtrIrregCompDict(); 

		if( AdjCompIDic == NULL )
			return CANull;

		if( AdjCompIDic->GetKeyCS(adjf.c_str(), adjconc)==FALSE )
			{	
			//Se busca el adj. en el dicc de Sufijos de Comparat.
			CDictLoad *AdjCompSufDic = m_Dicts->GetPtrSuffixCompDict(); 

			if( AdjCompSufDic == NULL )
				return CANull;
 
			LPCSTR szAdjconc;
			if( AdjCompSufDic->FindSuffix(adjf, szAdjconc)==TRUE)
				{
				LPCSTR p = strchr(szAdjconc, ',');
				CStringA s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

				//pos = adjconc.Find(',');
				//CStringA s1 = adjconc.Left(pos);
				//CStringA s2 = adjconc.Mid(pos+1);

				adjconc = adjf + '#';
				pos = adjconc.Find(s1+'#');
				adjconc = adjconc.Left(pos) + s2;
				}
			}   
		}
 
	if( (genfuente == MASCULINO) && (gendestino == FEMENINO) )
    {
		if( !adjconc.IsEmpty() )
			adjf = adjconc;
		//Se busca el sust. o adj. en el dicc de Fem. Irreg
		CDict *FIDic = m_Dicts->GetPtrIrregFemDict(); 

		if( FIDic == NULL )
			return CANull;
 
		if( FIDic->GetKey(adjf,adjconc)==TRUE )
			adjconc = GetIrregElement(adjf, adjconc.c_str(), 0);
		else
			{	
			//Se busca el verbo en el dicc de Sufijos Femeninos
			CDictLoad *SufFDic = m_Dicts->GetPtrSuffixFemDict(); 

			if( SufFDic == NULL )
				return CANull;

			LPCSTR szAdjconc;
			if( SufFDic->FindSuffix(adjf, szAdjconc)==TRUE)
				{
				LPCSTR p = strchr(szAdjconc, ',');
				CStringA s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

				adjconc = adjf + '#';
				pos = adjconc.Find(s1+'#');
				adjconc = adjconc.Left(pos) + s2;
				}
			}   
    }

	if( (numfuente == SINGULAR) && (numdestino == PLURAL) )
    {
		if( !adjconc.IsEmpty() )
			adjf = adjconc;
		//Se busca el sust. o adj. en el dicc de Plurales Irreg
		CDictLoad *PluralIDic = m_Dicts->GetPtrIrregPluralDict(); 

		if( PluralIDic == NULL )
			return CANull;
 
		LPCSTR szAdjconc;
		if( PluralIDic->GetKey(adjf.c_str(), szAdjconc)==TRUE )
			{
			adjconc = GetIrregElement(adjf, szAdjconc, 0);
			return (m_Ret=adjconc);
			}
		else
			{	
			//Se busca el sust. o adj. en el dicc de Sufijos Plurales
			CDictLoad *SufPluralDic = m_Dicts->GetPtrSuffixPluralDict(); 

			if( SufPluralDic == NULL )
				return CANull;
 
			LPCSTR szAdjconc;
			if( SufPluralDic->FindSuffix(adjf, szAdjconc)==TRUE)
				{
				LPCSTR p = strchr(szAdjconc, ',');
				CString s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

				//pos = adjconc.Find(',');
				//s1 = adjconc.Left(pos);
				//s2 = adjconc.Mid(pos+1);

				CString s3 = GetIrregElement(adjf, s2, 0);
				adjconc = adjf + '#';
				pos = adjconc.Find(s1+'#');
				adjconc = adjconc.Left(pos) + s3;

				return adjconc;
				}
			}   
    }

	return (m_Ret = (adjconc.IsEmpty()? strnoun: adjconc ));
	}

//--------------------------------------------------------------------------------------------------------------
// Función que declina un sustantivo según el género, el número y el caso para un indice dado
//--------------------------------------------------------------------------------------------------------------
CString CConjVerb::DeclineNounIndex( const CStringA& noun, Genero /*gen*/, Numero num, Declination decl, LPWORD decCount, WORD idx)
	{
	if( m_Dicts == NULL )
		return CANull;

	CStringA strnoun(noun);
	CStringA declNoun(noun);

	return (m_Ret=declNoun);
	}

//--------------------------------------------------------------------------------------------------------------
// Función que concuerda un sustantivo o adjetivo en un género,  número y grado determinado y un indice dado
//--------------------------------------------------------------------------------------------------------------
CStringA CConjVerb::ConcordWordsIndex( const CStringA& noun, Genero genfuente, Numero numfuente, Genero gendestino, Numero numdestino, Grado adjgrado, LPWORD decCount, WORD idx)
  {
  if( m_Dicts == NULL )
    return CANull;

  CStringA strnoun(noun);

  strnoun.MakeLower();

  CStringA adjf(strnoun);
  CStringA adjconc;
  CStringA s1, s2;
  int pos;


  if( adjgrado == SUPERLATIVO )
    {
    //Se busca el adj. en el dicc de Superl. Irreg
    CDictLoad *AdjSupIDic = m_Dicts->GetPtrIrregSupDict(); 

    if( AdjSupIDic == NULL )
      return CANull;

    if( AdjSupIDic->GetKeyCS(adjf.c_str(), adjconc)==FALSE )
      {	
      //Se busca el adj. en el dicc de Sufijos de Superl.
      CDictLoad *AdjSupSufDic = m_Dicts->GetPtrSuffixSupDict(); 

      if( AdjSupSufDic == NULL )
        return CANull;

			LPCSTR szAdjconc;
      if( AdjSupSufDic->FindSuffix(adjf, szAdjconc)==TRUE)
        {
				LPCSTR p = strchr(szAdjconc, ',');
				CStringA s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;
        //pos = adjconc.Find(",");
        //s1 = adjconc.Left(pos);
        //s2 = adjconc.Mid(pos+1);

        adjconc = adjf + "#";
        pos = adjconc.Find(s1+"#");
        adjconc = adjconc.Left(pos) + s2;
        }
      }   
    }


  if( adjgrado == COMPARATIVO )
    {
    //Se busca el adj. en el dicc de Comparat. Irreg
    CDictLoad *AdjCompIDic = m_Dicts->GetPtrIrregCompDict(); 

    if( AdjCompIDic == NULL )
      return CANull;

    if( AdjCompIDic->GetKeyCS(adjf.c_str(), adjconc)==FALSE )
      {	
      //Se busca el adj. en el dicc de Sufijos de Comparat.
      CDictLoad *AdjCompSufDic = m_Dicts->GetPtrSuffixCompDict(); 

      if( AdjCompSufDic == NULL )
        return CANull;

			LPCSTR szAdjconc;
      if( AdjCompSufDic->FindSuffix(adjf, szAdjconc)==TRUE)
        {
				LPCSTR p = strchr(szAdjconc, ',');
				CStringA s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

        //pos = adjconc.Find(",");
        //s1 = adjconc.Left(pos);
        //s2 = adjconc.Mid(pos+1);

        adjconc = adjf + "#";
        pos = adjconc.Find(s1+"#");
        adjconc = adjconc.Left(pos) + s2;
        }
      }   
    }

  if( (genfuente == MASCULINO) && (gendestino == FEMENINO) )
    {
    if( !adjconc.IsEmpty() )
      adjf = adjconc;
    //Se busca el sust. o adj. en el dicc de Fem. Irreg
    CDict *FIDic = m_Dicts->GetPtrIrregFemDict(); 

    if( FIDic == NULL )
      return CANull;

    if( FIDic->GetKey(adjf,adjconc)==FALSE )
      {	
      //Se busca el verbo en el dicc de Sufijos Femeninos
      CDictLoad *SufFDic = m_Dicts->GetPtrSuffixFemDict(); 

      if( SufFDic == NULL )
        return CANull;

			LPCSTR szAdjconc;
      if( SufFDic->FindSuffix(adjf, szAdjconc)==TRUE)
				{
				LPCSTR p = strchr(szAdjconc, ',');
				CString s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

        //pos = adjconc.Find(",");
        //s1 = adjconc.Left(pos);
        //s2 = adjconc.Mid(pos+1);
        adjconc = adjf + "#";
        pos = adjconc.Find(s1+"#");
        adjconc = adjconc.Left(pos) + s2;
        }
      }   
    }

  if( (numfuente == SINGULAR) && (numdestino == PLURAL) )
    {
    if( !adjconc.IsEmpty() )
      adjf = adjconc;
    //Se busca el sust. o adj. en el dicc de Plurales Irreg
    CDictLoad *PluralIDic = m_Dicts->GetPtrIrregPluralDict(); 

    if( PluralIDic == NULL )
      return CANull;

		LPCSTR szAdjconc;
    if( PluralIDic->GetKey(adjf.c_str(), szAdjconc)==TRUE )
      {
      adjconc = GetIrregElement(adjf, szAdjconc, idx, decCount);
      return adjconc;
      }
    else
      {	
      //Se busca el sust. o adj. en el dicc de Sufijos Plurales
      CDictLoad *SufPluralDic = m_Dicts->GetPtrSuffixPluralDict(); 

      if( SufPluralDic == NULL )
        return CANull;

			LPCSTR szAdjconc;
			if( SufPluralDic->FindSuffix(adjf, szAdjconc)==TRUE)
				{
				LPCSTR p = strchr(szAdjconc, ',');
				CString s1(szAdjconc, (int)(p - szAdjconc));
				LPCSTR s2 = p + 1;

        //pos = adjconc.Find(",");
        //s1 = adjconc.Left(pos);
        //s2 = adjconc.Mid(pos+1);

        CStringA s3 = GetIrregElement(adjf,s2,idx,decCount);
        adjconc = adjf + "#";
        pos = adjconc.Find(s1+"#");
        adjconc = adjconc.Left(pos) + s3;
        return (m_Ret = adjconc);
        }
      }   
    }

  return (m_Ret = (adjconc.IsEmpty()? strnoun:adjconc ));
	}
//--------------------------------------------------------------------------------------------------------------
// Función que retorna un puntero al diccionario o tabla especificado
//--------------------------------------------------------------------------------------------------------------
LPVOID CConjVerb::GetPtrDicRef(DicName name)
  {
  if( m_Dicts == NULL )
    return NULL;

  switch (name)
    {
    case dcIrregVerb:
      return (LPVOID)(m_Dicts->GetPtrIrregWordDict());
    case dcSuffixVerb:
      return (LPVOID)(m_Dicts->GetPtrSuffixDict());
    case dcAuxVerb:
      return (LPVOID)(m_Dicts->GetPtrAuxVerbDict());
    case tbVerb:
      return (LPVOID)(m_Dicts->GetPtrVerbTable());
    case tbPattern:
      return (LPVOID)(m_Dicts->GetPtrPatternTable());
    case tbReference:
      return (LPVOID)(m_Dicts->GetPtrReferTable());
    case dcIrregFem:
      return (LPVOID)(m_Dicts->GetPtrIrregFemDict());
    case dcSuffixFem:
      return (LPVOID)(m_Dicts->GetPtrSuffixFemDict());
    case dcIrregPlural:
      return (LPVOID)(m_Dicts->GetPtrIrregPluralDict());
    case dcSuffixPlural:
      return (LPVOID)(m_Dicts->GetPtrSuffixPluralDict());
    case dcIrregSup:
      return (LPVOID)(m_Dicts->GetPtrIrregSupDict());
    case dcIrregComp:
      return (LPVOID)(m_Dicts->GetPtrIrregCompDict());
    case dcSuffixSup:
      return (LPVOID)(m_Dicts->GetPtrSuffixSupDict());
    case dcSuffixComp:
      return (LPVOID)(m_Dicts->GetPtrSuffixCompDict());
    case dcPreffixVerb:
      return (LPVOID)(m_Dicts->GetPtrPreffixVerbDict());
    case dcIrregDecNoun:
      return (LPVOID)(m_Dicts->GetPtrIrregDecNounDict());
    case dcSuffixDecNoun:
      return (LPVOID)(m_Dicts->GetPtrSuffixDecNounDict());
    case dcIrregDecAdj:
      return (LPVOID)(m_Dicts->GetPtrIrregDecAdjDict());
    case dcSuffixDecAdj:
      return (LPVOID)(m_Dicts->GetPtrSuffixDecAdjDict());
    }

  return NULL;
	}

//--------------------------------------------------------------------------------------------------------------
// Función que devuelve el pronombre reflexivo para la persona, y el número dados
//--------------------------------------------------------------------------------------------------------------
CStringA CConjVerb::ReflexivePronoun( Persona persona, Numero numero )
	{
	if( m_Dicts == NULL )
		return CANull;

	CRefTabla* RefTab = m_Dicts->GetPtrReferTable();
	if( RefTab == NULL )
		return CANull;

	int i = 0;
  switch (persona)
    {
    case PRIMERAP:
      i = (numero==SINGULAR)? 0:3; break;
    case SEGUNDAP:
      i = (numero==SINGULAR)? 1:4; break;
    case TERCERAP:
      i = (numero==SINGULAR)? 2:5; break;
    }

	return (RefTab->m_PronomReflex[i]);

	}

//--------------------------------------------------------------------------------------------------------------
// Devuelve si el verbo RefVerb es o no reflexivo. En el parámetro RootVerb devuelve el verbo raíz
//--------------------------------------------------------------------------------------------------------------
BOOL CConjVerb::ReflexiveVerb( const CString& RefVerb, CString& RootVerb )
	{
	//CStringA Verbo(RefVerb);

	if( m_Dicts == NULL )
		return FALSE;

	CRefTabla* RefTab = m_Dicts->GetPtrReferTable();
	if( RefTab == NULL )
		return FALSE;

	for( int i=0; i < RefTab->m_TerminacReflex.GetCount(); i++ )
		{
		int pos = RefVerb.Find(RefTab->m_TerminacReflex[i]);
		if( pos >= 0 )
			{
			RootVerb = RefVerb.Left(pos) + RefTab->m_TerminacVerbal[i];
			return TRUE;
			}
		}

	return FALSE;
	}

//--------------------------------------------------------------------------------------------------------------
// Devuelve el pronombre personal para la persona, y el número dados como parámetros
//--------------------------------------------------------------------------------------------------------------
CString CConjVerb::PersonalPronoun( Persona persona, Numero numero )
	{
  if( m_Dicts == NULL )
    return CANull;

  CRefTabla* RefTab = m_Dicts->GetPtrReferTable();

  if( RefTab == NULL )
    return CANull;

  int i = 0;
  switch (persona)
    {
    case PRIMERAP:
      i = (numero==SINGULAR)? 0:3; break;
    case SEGUNDAP:
      i = (numero==SINGULAR)? 1:4; break;
    case TERCERAP:
      i = (numero==SINGULAR)? 2:5; break;
    }

  return CString(RefTab->m_PronomPerson[i]);
	}

///**************************************************************************************************************
//                                   FUNCIONES INTERNAS
//***************************************************************************************************************/
//
//--------------------------------------------------------------------------------------------------------------
// Devuelve el enum correspondiente a la conjugación que se da como parámetro 
//--------------------------------------------------------------------------------------------------------------
int CConjVerb::IndiceCnj( Modo modo, Tiempo tiempo, Persona persona, Numero numero)
  {                       
  CRefTabla* RefTab = m_Dicts->GetPtrReferTable();
  if( RefTab == NULL )
    return -1;

  int i = md_Infinitivo;

  switch(modo)
    {
    case mINFINITIVO:
      i = md_Infinitivo;       break;
    case mPARTICIPIO:
      i = md_ParticipioPasado; break;
    case mGERUNDIO:
      i = md_Gerundio;         break;
    case mINDICATIVO:
      {
      if(numero == SINGULAR)    
        {
        switch (tiempo)
          {
          case PRESENTE:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PresenteIndicativo1PerSing;  break;
              case SEGUNDAP: i = md_PresenteIndicativo2PerSing1; break;
              case TERCERAP: i = md_PresenteIndicativo3PerSing;  break;
              }
            break;
            }
          case PASADO:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PasadoIndicativo1PerSing;  break;
              case SEGUNDAP: i = md_PasadoIndicativo2PerSing1; break;
              case TERCERAP: i = md_PasadoIndicativo3PerSing;  break;
              }
            break;
            }
          case FUTURO:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_FuturoIndicativo1PerSing;  break;
              case SEGUNDAP: i = md_FuturoIndicativo2PerSing1; break;
              case TERCERAP: i = md_FuturoIndicativo3PerSing;  break;
              }
            break;
            }
          case PASADOIMP:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PasadoImpIndicativo1PerSing;  break;
              case SEGUNDAP: i = md_PasadoImpIndicativo2PerSing1; break;
              case TERCERAP: i = md_PasadoImpIndicativo3PerSing;  break;
              }
            break;
            }
          }
        }
      else             // Plural del indicativo
        {
        switch (tiempo)
          {
          case PRESENTE:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PresenteIndicativo1PerPlural;  break;
              case SEGUNDAP: i = md_PresenteIndicativo2PerPlural1; break;
              case TERCERAP: i = md_PresenteIndicativo3PerPlural;  break;
              }
            break;
            }
          case PASADO:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PasadoIndicativo1PerPlural;  break;
              case SEGUNDAP: i = md_PasadoIndicativo2PerPlural1; break;
              case TERCERAP: i = md_PasadoIndicativo3PerPlural;  break;
              }
            break;
            }
          case FUTURO:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_FuturoIndicativo1PerPlural;  break;
              case SEGUNDAP: i = md_FuturoIndicativo2PerPlural1; break;
              case TERCERAP: i = md_FuturoIndicativo3PerPlural;  break;
              }
            break;
            }
          case PASADOIMP:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PasadoImpIndicativo1PerPlural;  break;
              case SEGUNDAP: i = md_PasadoImpIndicativo2PerPlural1; break;
              case TERCERAP: i = md_PasadoImpIndicativo3PerPlural;  break;
              }
            break;
            }
          }
        }
      break;    
      }
    case mIMPERATIVO:
      {
      if(numero == SINGULAR)    
        {
        switch (persona)
          {
          case PRIMERAP: break;
          case SEGUNDAP: i = md_Imperativo2PerSing1; break;
          case TERCERAP: i = md_Imperativo3PerSing;  break;
          }
        }
      else                        
        {
        switch (persona)
          {
          case PRIMERAP: i = md_Imperativo1PerPlural1; break;
          case SEGUNDAP: i = md_Imperativo2PerPlural1; break;
          case TERCERAP: i = md_Imperativo3PerPlural;  break;
          }
        }
      break;
      }
    case mPOTENCIAL:
      {
      if(numero == SINGULAR)    
        {
        switch (persona)
          {
          case PRIMERAP: i = md_Potencial1PerSing;  break;
          case SEGUNDAP: i = md_Potencial2PerSing1; break;
          case TERCERAP: i = md_Potencial3PerSing;  break;
          }
        }
      else                        
        {
        switch (persona)
          {
          case PRIMERAP: i = md_Potencial1PerPlural;  break;
          case SEGUNDAP: i = md_Potencial2PerPlural1; break;
          case TERCERAP: i = md_Potencial3PerPlural;  break;
          }
        }
      break;
      }
    case mSUBJUNTIVO:
      {
      if(numero == SINGULAR)    
        {
        switch (tiempo)
          {
          case PRESENTE:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PresenteSubjuntivo1PerSing;  break;
              case SEGUNDAP: i = md_PresenteSubjuntivo2PerSing1; break;
              case TERCERAP: i = md_PresenteSubjuntivo3PerSing;  break;
              }
            break;
            }
          case PASADO:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PasadoSubjuntivo1PerSing;  break;
              case SEGUNDAP: i = md_PasadoSubjuntivo2PerSing1; break;
              case TERCERAP: i = md_PasadoSubjuntivo3PerSing;  break;
              }
            break;
            }
          default:
             break; // para eliminar warnings del compilador
          }
        }
      else             // Plural del indicativo
        {
        switch (tiempo)
          {
          case PRESENTE:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PresenteSubjuntivo1PerPlural;  break;
              case SEGUNDAP: i = md_PresenteSubjuntivo2PerPlural1; break;
              case TERCERAP: i = md_PresenteSubjuntivo3PerPlural;  break;
              }
            break;
            }
          case PASADO:
            {
            switch (persona)
              {
              case PRIMERAP: i = md_PasadoSubjuntivo1PerPlural;  break;
              case SEGUNDAP: i = md_PasadoSubjuntivo2PerPlural1; break;
              case TERCERAP: i = md_PasadoSubjuntivo3PerPlural;  break;
              }
            break;
            }
          default:
             break; // para eliminar warnings del compilador
          }
        }
      }
    default:
       break; // para eliminar warnings del compilador
    }

  if( i < 0 || i >= RefTab->m_TabColIndex.GetCount() )
    i = 0;
  else
    i = RefTab->m_TabColIndex.GetAt(i);

  return i;
  }

//--------------------------------------------------------------------------------------------------------------
// Conjuga un verbo simple (no compuesto)
//--------------------------------------------------------------------------------------------------------------
CString CConjVerb::ConjVerbSimple(const CStringA& verboi,Modo modo, Tiempo tiempo, Persona persona, Numero numero, int *nfila, int *ncol)
	{
	CString sEmpty;
	if( m_Dicts == NULL  )
		return sEmpty;

	CTabla* Tabla = m_Dicts->GetPtrVerbTable();

	if( Tabla == NULL || Tabla->m_Array.GetCount()==0)
		return sEmpty;

	if( modo == mIMPERATIVO)                              // Primera Pers. del Imperativo o reflexivo
		{
		if((persona == PRIMERAP) && (numero == SINGULAR))
			{
			return CString("-");
			}
		}

	CString verbo(verboi);
  //_strlwr( (LPSTR)(LPCSTR)verbo );
	verbo.MakeLower();

  int fila = -1;
  int col = -1;

	CDict* PIDic = m_Dicts->GetPtrIrregWordDict();
  if( PIDic == NULL )
		return sEmpty;

  CStringA data;    
  BOOL i = PIDic->GetKey(verbo,data);
  if( i == TRUE)	                                      // Está
		fila = Tabla->FindFila(data);                       // # de fila del verbo patrón en la tabla
  else
    {	                                                  // Se busca el verbo en el dicc de Sufijos
    CDictLoad* SufDic = m_Dicts->GetPtrSuffixDict(); 

		if( SufDic == NULL )
			return sEmpty;

		LPCSTR szData = NULL;
    i = SufDic->FindSuffix(verbo, szData);
    if( i == TRUE)	//está
			fila = Tabla->FindFila(szData);	                    // # de fila del verbo patrón en la tabla
    else
      return sEmpty;
    }   
    
  CStringA VerboC;
  if (fila > -1)	                                      // Se encontró un patrón para el verbo
    {
    CStringA sinf, rinf;
        
    sinf = Tabla->GetCelda(fila,md_Infinitivo);         // sufijo del infinit.
		verbo += ' ';								                        // del verbo patrón
    int pos = verbo.Find(sinf+' ');	                    // Se busca suf. inf. en el verbo orig.
    verbo.TrimRight();
            
    if (pos != -1)
      {      
			rinf = verbo.Left(pos);	                          // Raíz Infinitivo
			col = IndiceCnj( modo, tiempo, persona, numero);
			LPCSTR ter = Tabla->GetCeldaSZ(fila,col);	                // Terminación del verbo conjugado
			
			if (ter[0] == '-' && ter[1] == 0)	                                // Verbo defectivo. No existe esa conjugación
				{
				*nfila = fila;
				*ncol = col;
				return CStringA("-");
				}

			VerboC = rinf + ter;                              // Verbo conjugado
      }
    else
      {
			return sEmpty;
      }
    }
  else 
    {      
    return sEmpty;
    }     

	*nfila = fila;
	*ncol = col;
	
	if( verbo == "be" && modo == mINDICATIVO && m_Lang==1 && tiempo == PASADO && persona == SEGUNDAP && numero == SINGULAR)
		VerboC = "were";
	
	return VerboC;
	}

//--------------------------------------------------------------------------------------------------------------
// Identifica si el verbo tiene un prefijo (separable o no separable). Devuelve el prefijo y la raíz 
//--------------------------------------------------------------------------------------------------------------
BOOL CConjVerb::FindVerbPrefix(const CStringA& verbo, CStringA& verbprefix, CStringA& verbroot)
	{
	CStringA prefix, root;

	if( m_Dicts == NULL )
		return FALSE;

	CDictLoad* PrefVerbDic = m_Dicts->GetPtrPreffixVerbDict(); 

	if( PrefVerbDic == NULL )
		return FALSE;

	LPCSTR szPrefix;
	if(!PrefVerbDic->GetKey(verbo.c_str(), szPrefix)) 
		return FALSE;

	verbprefix = szPrefix;

	if( szPrefix[0] == '.' )
		prefix = szPrefix + 1;
	else
		prefix = szPrefix;
	
	verbroot = verbo.Mid(prefix.GetLength()); 

	return TRUE;
	}

//--------------------------------------------------------------------------------------------------------------
// Busca si el verbo en su conjugación afirmativa o negativa lleva prefijo o sufijo
//--------------------------------------------------------------------------------------------------------------
void CConjVerb::Prefijo_Conjugac( int fila,int col,BOOL negado, CStringA& prefijo, CStringA& sufijo, BOOL *verboinf )
	{
	//Se busca si el verbo en su conjugac. afirm. o negat. lleva pref. o suf.
	int ipat, tipopat;

	if( m_Dicts == NULL )
		return;

	CTabla *tablap = m_Dicts->GetPtrPatternTable(); 
	if( tablap == NULL )
		return;

	CTabla *Tabla = m_Dicts->GetPtrVerbTable(); 
	if( Tabla == NULL )
		return;
	
	// se busca en la tabla de referencias la col. donde está el # del patrón
	CRefTabla *tabref = m_Dicts->GetPtrReferTable(); 
	if( tabref == NULL )
		return;

	ipat = tabref->m_TabColIndex[md_PatronSufPref];
	if( ipat > 0 )
		{
		CStringA suf,pref,sufn,prefn;
		LPCSTR cadpatron = Tabla->GetCeldaSZ(fila,ipat); // # del patrón de suf. y pref.
		if(*cadpatron != 0)
			{
			tipopat = atoi(cadpatron);
			if( negado == FALSE )	
				{
				sufijo = tablap->GetCelda((4*tipopat),col);
				prefijo = tablap->GetCelda((4*tipopat)+1,col);
				}
			else
				{
				if (*tablap->GetCeldaSZ((4*tipopat),ipat) != 0)
					*verboinf = TRUE;

				sufijo = tablap->GetCelda((4*tipopat)+2,col);
				prefijo = tablap->GetCelda((4*tipopat)+3,col);
				}
			}
		}
	}

//--------------------------------------------------------------------------------------------------------------

CString CConjVerb::GetDeclElement(const CStringA& key, const CStringA& declList, Declination decl, int index, LPWORD declCount )
	{
	CSmartStrArray declArray;
	StringToArray(declList,&declArray,";");
	CStringA sufList;

	switch(decl)
		{
		case NOMINATIVE:
			sufList = declArray[0];	break;
		case ACCUSATIVE:
			sufList = declArray[1];	break;
		case DATIVE:
			sufList = declArray[2];	break;
		case GENITIVE:
			sufList = declArray[3];	break;
    default:
       break; // para eliminar warnings del compilador
		}

	CSmartStrArray sufArray;
	StringToArray(sufList,&sufArray,"|");
	if( declCount != NULL )
		*declCount = sufArray.GetCount(); 

	CStringA elem(sufArray[index]);

	if(elem[0] == '-')
		elem = key + elem.Mid(1);
	
	return elem;
	}

//--------------------------------------------------------------------------------------------------------------

CString CConjVerb::GetDeclArtType(const CString& declList, ArticleType artic)
	{
	CString decl;

	switch(artic)
		{
		case aNULL      : GetSubCampo(decl,declList,'N','#');	break;
		case aDEFINITE  : GetSubCampo(decl,declList,'D','#'); break;
		case aINDEFINITE: GetSubCampo(decl,declList,'I','#'); break;
		}

	return decl;
	}

//--------------------------------------------------------------------------------------------------------------

int CConjVerb::GetDeclIndex(Genero gen, Numero num)
	{
	if( num == PLURAL   )	return 3;
	if( gen == FEMENINO ) return 1;
	if( gen == NEUTRO   ) return 2;

	return 0;
	}

//--------------------------------------------------------------------------------------------------------------

CString CConjVerb::GetIrregElement(const CString& key, LPCSTR elemList, int index, LPWORD declCount)
	{
	CSmartStrArray elemArray;
	StringToArray(elemList,&elemArray,"|");
	if( declCount != NULL )
		*declCount = elemArray.GetCount(); 

	CString res(elemArray[index]);

	if(res[0] == '-')
		res = key + res.Mid(1);
	
	return res;
	}

//--------------------------------------------------------------------------------------------------------------
