//-------------------------------------------------------------------------------------------------------------------
// RedWord.cpp : Defines the initialization routines for the DLL.
//-------------------------------------------------------------------------------------------------------------------

#include <ctype.h>
#include "ApiRed.h"
#include "RedWord.h"


//-------------------------------------------------------------------------------------------------------------------

static CStringA tipo_sust     ("SS;NP;XQ;UR;XA;QN;QF;TA;XB;ZB;UO;XH;QO;XJ;QA;SV;YF;XG;ZF;ZJ;ZZ;YG;XK;UA;QB;QC;QD;UZ;UC;UM;UT;UX;UI;UE;UJ;UL;UF;UG;UV;UY;QJ;QK;QL;QM;QZ;LK;MV");
static CStringA tipo_adj      ("AA;XQ;UR;XA;QF;TA;XB;ZB;UO;XH;XJ;QE;XD;XC;XI;ZI;XE;XM;XO;PB;UH;UB;UW;QB;QC;QD;UM;UT;UX;UK;UD;UN;UV;QL;QM;QZ;MV;MU;MG");
static CStringA tipo_adj_in   ("AI;QN;QO;XY;AO");
static CStringA tipo_verbo    ("VV;VR;VT;VI;VG;VP;VS;VC;PT;PI;VST;VSI;GT;GI;XQ;UR;XA;QN;QF;TA;XB;ZB;UO;QA;SV;YF;XG;ZF;ZJ;ZZ;YG;QE;XD;XC;XI;ZI;XE;ZV;ZH;XP;XL;VV;QG;XF;ZG;UH;UB;UW;QB;QC;QD;UZ;UC;UM;UT;UX;UI;UE;UJ;UK;UD;UN;UL;UF;UG;UY;QH;QI;QJ;QK;QL;QM;HT;HI;JT;JI;LK;DO");
static CStringA tipo_verbo_aux("VA;QJ;QK");
static CStringA tipo_pronombre("OO;OD;OG;OI;OS;OL;OA;AF;MG;MU;AE;AP;AL;AD;AO");
static CStringA tipo_BEHA     ("BE;HA");
static CStringA tipo_adverbio ("DD;XQ;UR;XA;QN;XH;QO;QA;XK;QE;XD;XC;XM;XP;XL;XY;QG;XF;QH;QI;QL;QM;QZ");
static CStringA tipo_articulo ("RD;RI");

//-------------------------------------------------------------------------------------------------------------------
// Clasifica el tipo de reducci—n
//-------------------------------------------------------------------------------------------------------------------
int CRWType::GetType()
  {
  if( tipo_verbo    .Find (m_Tipo)!=-1 ) return rVERBO;
  if( tipo_verbo_aux.Find (m_Tipo)!=-1 ) return rVERBO;
  if( tipo_BEHA     .Find (m_Tipo)!=-1 ) return rVERBO;
  
  if( tipo_adj.   Find (m_Tipo)!=-1 ) return rADJETIVE;
  if( tipo_adj_in.Find (m_Tipo)!=-1 ) return rADJETIVE;
  
  if( tipo_sust     .Find (m_Tipo)!=-1 ) return rSUSTANTIVE;
  if( tipo_pronombre.Find (m_Tipo)!=-1 ) return rPRONOMBRE;
  if( tipo_adverbio .Find (m_Tipo)!=-1 ) return rADVERBIO;
  if( tipo_articulo .Find (m_Tipo)!=-1 ) return rARTICLE;
  
  return -1;
  }

//-------------------------------------------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------------------------------------------
CRedWord::CRedWord()
  {
  m_Dicts      = NULL;
	m_isDicPro   = false;

  m_hConj      = NULL;
  m_FreehConj  = false;

  m_GenDic     = NULL;
  }

//-------------------------------------------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------------------------------------------
CRedWord::~CRedWord()
  {
  Close();
  }

//-------------------------------------------------------------------------------------------------------------------
// Establece el manipulador al API de conjuagaciones que se va a usar, el paramentro free determina si la clase
// debe liberarlo o no, si no se llama esta función, la clase crea un manipulador y lo libera
// *** Esta función debe llamarse antes de Open
//-------------------------------------------------------------------------------------------------------------------
void CRedWord::SetConjHandle(HCONJ hConj, bool free )
	{
  if( m_hConj && m_FreehConj )
    CVFree( m_hConj );

	m_hConj = hConj;
  m_FreehConj = free;
	}

//-------------------------------------------------------------------------------------------------------------------
// Establece el manipulador al dicionario general de palabras que se va a usar, el paramentro free determina si la 
// clase debe liberarlo o no, si no se llama esta función, la clase trata de abrir un diccionario, que debe estar
// en el directorio de los datos y se corresponde la dirección del idioma abierto hacia el inglés
// *** Esta función debe llamarse antes de Reduce
//-------------------------------------------------------------------------------------------------------------------
void CRedWord::SetGenDict( CDictMain* hGenDic, bool free )
	{
	m_GenDic = hGenDic;
	}

//-------------------------------------------------------------------------------------------------------------------
// Carga los recursos de reducción para un idioma especifico
//-------------------------------------------------------------------------------------------------------------------
BOOL CRedWord::Open( int lang )
  {
  if( m_Dicts != NULL )                           // Ya hay un idioma cargado
    return FALSE;                                 // Hay que cerrarlo primero

  m_Dicts = new CRWDicRef( lang, m_DataPath );    // Crea objeto DictRef
  if( m_Dicts && m_Dicts->Load() )                // Carga los diccionarios
    {
    if( !m_hConj )                                // No hay handle a conjugación
      {
      HCONJ hConj = CVNew();                      // Crea uno nuevo
      CVSetDataPath( hConj, m_DataPath );         // Le pone los camino a diccionarios

      if( CVOpen(hConj,lang) )                    // Abre conjucaciones para el idioma
        {
        m_hConj = hConj;                          // Todo OK
        m_FreehConj = true;                       // Bandera para liberarlo
        }
      else                                        // No pudo
        CVFree( hConj );                          // Lo libera
      }

    if( m_hConj )                                 // Hay conjugaciones
      return TRUE;                                // Retrona OK
    }

  if( m_Dicts )                                   // Exite handle a diccioarios
    {
    delete m_Dicts;                               // Lo libera
    m_Dicts = NULL;                               // Lo pone a NULL
    }

  return FALSE;                                   // No puedo cargar todo lo necesario
  }

//-------------------------------------------------------------------------------------------------------------------
// Libera todos los recursos asociados a la reducción 
//-------------------------------------------------------------------------------------------------------------------
BOOL CRedWord::Close()
	{
  if( m_Dicts != NULL ) 
    delete m_Dicts;	

  if( m_hConj && m_FreehConj )
    CVFree( m_hConj );

  m_Dicts      = NULL;
	m_isDicPro   = false;

  m_hConj      = NULL;
  m_FreehConj  = false;

  m_GenDic     = NULL;

	return TRUE;
	}

//-------------------------------------------------------------------------------------------------------------------
// Reduce la palabra 'Word' usando el diccionario 'GenDic', si se pasa NULL a en este parametro se trata de usar
// el diccionario general que se encuentre en el directorio especificado con SetDataPath.
//-------------------------------------------------------------------------------------------------------------------
HREDDATA CRedWord::Reduce( const CString& Word  )
	{
	if( m_Dicts == NULL )
		return NULL;

	if( m_GenDic == NULL && !LoadGenDic() )
    return NULL;

	CRWhData* hData = new CRWhData( m_Dicts->m_Lang );
	
	hData->m_Palabra = Word;

	BOOL reducida = FALSE;

	reducida = ReduceSimple(hData);

	if( reducida )
		return hData;

	//Buscar la pal. en el dicc. de reducción por prefijos
	CStringA /*prefijo,datapref,*/palorig,raiz,dataraiz;
	//BOOL fin;

	palorig = hData->m_Palabra;
	//fin = FALSE;
	while( !palorig.IsEmpty() )
		{
		CDictLoad* PrefDic = m_Dicts->GetPtrPreffixDict(); 

		if( PrefDic == NULL )
			{
			RWFreehData(hData);
			return NULL;
			}

		CStringA prefijo;
		if( !FindPrefijo(PrefDic,palorig,prefijo) )
			break;

		//PrefDic->GetKey(prefijo,datapref);
		
		raiz = Word;

		hData->m_Palabra = raiz;

		reducida = ReduceSimple(hData);

		hData->m_Palabra = Word;

		if( reducida )
			{
			for( int i=0; i < hData->m_Types.GetCount(); i++ )
				hData->m_Types[i].m_Prefijo = prefijo;

			return hData;
			}
				
		palorig = prefijo.Left(prefijo.GetLength()-1);
		}

	if( hData->m_Idioma == 1)   //Si es Inglés
		{
		RWFreehData(hData);
		return NULL;
		}

	// Tratar de reducir por pronombres enclíticos
	hData->m_Palabra = Word;

	reducida = FindRedEncliticos(hData);

	if( reducida )
		return hData;

	RWFreehData(hData);
	return NULL;
	}

//-------------------------------------------------------------------------------------------------------------------
// Función que retorna un puntero al diccionario o tabla especificado
//-------------------------------------------------------------------------------------------------------------------
LPVOID CRedWord::GetPtrDicRef( RWDicName name)
	{
	if( m_Dicts == NULL )
		return NULL;

    switch (name)
      {
      case drIrregNoun:
	      return (LPVOID)(m_Dicts->GetPtrIrregNounDict());
      case drIrregVerb:
	      return (LPVOID)(m_Dicts->GetPtrIrregVerbDict());
      case drNounSuffix:
	      return (LPVOID)(m_Dicts->GetPtrNounSuffixDict());
      case drPreffix:
	      return (LPVOID)(m_Dicts->GetPtrPreffixDict());
      case drVerbSuffix:
	      return (LPVOID)(m_Dicts->GetPtrVerbSuffixDict());
      }

	return NULL;
	}

//-------------------------------------------------------------------------------------------------------------------
//                  FUNCIONES INTERNAS UTILIZADAS EN ESTA CLASE
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// Intenta cargar un diccionario de palabras, para ello en el directorio de datos, busca un diccionario de traduccion,
// para eso prueba, con todas las direcciones desde la dirección actual y el primero que encuentre lo utiliza
//-------------------------------------------------------------------------------------------------------------------
BOOL CRedWord::LoadGenDic()
  {
  //CDict *Dic = CDictGen::GetLang(m_Dicts->m_Lang);

  CDictMain *Dic = CDictGen::Get(m_Dicts->m_Lang);
  if( Dic )
    {
    m_GenDic = Dic;
    return TRUE;
    }
  
  return FALSE;
  }

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::GetType(CStringA &type, const CString& data)
  {
	//COLLECT_INTERVAL("GetType"); // 0.01 14220
  if( !m_isDicPro )
    {
    GetSubCampo(type,data,'T','#');
    if( !type.IsEmpty() )
      {
      type.Truncate(2);// = type.Left(2);
      return TRUE;
      }
    return FALSE;
    }

  CStringA datos(data);
  CStringA strTipo;
  int nTipos = 0;

  if(datos.Find("#A")!=-1)
    {
    nTipos++;
    strTipo = "SS";
    }
  if(datos.Find("#C")!=-1)
    {
    nTipos++;
    strTipo = "AA";
    }
  if(datos.Find("#F")!=-1)
    {
    strTipo = "DD";
    nTipos++;
    }
  if(datos.Find("#G")!=-1)
    {
    strTipo = "OO";
    nTipos++;
    }
  if(datos.Find("#K")!=-1)
    {
    strTipo = "RD";
    nTipos++;
    }
  if( datos.Find("#D")!=-1 || datos.Find("#E")!=-1 || datos.Find("#P")!=-1)
    {
    strTipo = "VV";
    nTipos++;
    }

  switch(nTipos)
    {
    case 1:
      {
      type = strTipo;
      return TRUE;
      }
    case 2:
      {
      if( datos.Find("#A")!=-1 && datos.Find("#G")!=-1 )
        {
        type = "AO";
        return TRUE;
        }
      if( datos.Find("#A")!=-1 && strTipo=="VV" )
        {
        type = "LK";
        return TRUE;
        }
      if( datos.Find("#F")!=-1 && strTipo=="VV" )
        {
        type = "QG";
        return TRUE;
        }
      if( datos.Find("#C")!=-1 && strTipo=="VV" )
        {
        type = "UB";
        return TRUE;
        }
      if( datos.Find("#A")!=-1 && datos.Find("#C")!=-1 )
        {
        type = "XJ";
        return TRUE;
        }
      if( datos.Find("#A")!=-1 && datos.Find("#F")!=-1 )
        {
        type = "XK";
        return TRUE;
        }
      if( datos.Find("#C")!=-1 && datos.Find("#F")!=-1 )
        {
        type = "XM";
        return TRUE;
        }
      }
    case 3:
      {
      if( datos.Find("#A")!=-1 && datos.Find("#F")!=-1 &&  strTipo=="VV" )
        {
        type = "QA";
        return TRUE;
        }
      if( datos.Find("#A")!=-1 && datos.Find("#C")!=-1 &&  strTipo=="VV" )
        {
        type = "QB";
        return TRUE;
        }
      if( datos.Find("#C")!=-1 && datos.Find("#F")!=-1 &&  strTipo=="VV" )
        {
        type = "QE";
        return TRUE;
        }
      if( datos.Find("#A")!=-1 && datos.Find("#C")!=-1 && datos.Find("#F")!=-1 )
        {
        type = "QZ";
        return TRUE;
        }
      }
    case 4:
      {
      if( datos.Find("#A")!=-1 && datos.Find("#C")!=-1 && datos.Find("#F")!=-1 &&  strTipo=="VV" )
        {
        type = "QL";
        return TRUE;
        }

      break;
      }
    }

  return FALSE;
  }

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::NounRedCheck(CRWhData* hData, const CStringA& root)
  {
  //char language[4];
  //_itoa(hData->m_Idioma,language,10);

  CStringA sustconc;

  sustconc = CVConcordWords(m_hConj,root,MASCULINO,SINGULAR,MASCULINO,PLURAL,POSITIVO);

  if( sustconc == hData->m_Palabra )
    {
    // Se agrega la reducción a la lista m_Types
    CRWType redtype;

    redtype.m_PalReduc = root;
    redtype.m_Tipo = "SS";
    redtype.m_Numero = PLURAL;

    hData->m_Types.Add(redtype);
    return TRUE;
    }

  return FALSE;
  }

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::AdjRedCheck(CRWhData* hData, const CStringA& root, const CStringA& tipo)
	{
	//COLLECT_INTERVAL("AdjRedCheck"); // 0.04 358
	//char language[4];
	//_itoa_s(hData->m_Idioma,language,4,10);

	WORD genero,numero,grado;
	CStringA adjconc;

  for( grado=0; grado < 3; grado++ )
		for( genero=0; genero < 2; genero++ )
			for( numero=0; numero < 2; numero++ )
				{
				// Si es Inglés no se concuerda el adj. ni en género ni en número, solo en grado
				if( (hData->m_Idioma == 1) && ( (genero>0) || (numero>0) ) )
					continue;

				adjconc = CVConcordWords(m_hConj,root,MASCULINO,SINGULAR,genero,numero,grado);

				//TRACE("1 %s == %s", adjconc.c_str(), hData->m_Palabra.c_str());
				if( adjconc == hData->m_Palabra )
					{
					// Se agrega la reducción a la lista m_Types
						
					CRWType redtype;

					redtype.m_PalReduc = root;
					redtype.m_Tipo = tipo;
					redtype.m_Grado = (Grado)grado;
					redtype.m_Numero = (Numero)numero;
					redtype.m_Genero = (Genero)genero;

					hData->m_Types.Add(redtype);
					return TRUE;
					}
				}
    
	return FALSE;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::CompareStrings(CStringA s1, CStringA s2)
	{
	if(s1==s2)
		return TRUE;

	s1.Replace("ß","ss"); 
	s2.Replace("ß","ss"); 

	return (s1==s2);
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::VerbRedCheck(CRWhData* hData, const CStringA& root, const CStringA& tipo)
	{
	//COLLECT_INTERVAL("VerbRedCheck"); //0.71 2353
	//char language[4];
	//_itoa_s(hData->m_Idioma,language,4,10);
	BOOL reducida = FALSE;

	CStringA verbconj;
	Modo modo;
	Tiempo tiempo = PRESENTE;
	WORD persona = 0;
	WORD numero = 0;

	int cCount = CVCount(m_hConj);

	int i = 0;
	for( i = 0; i < cCount; i++ )
		{
		if( CVCompound(m_hConj,i) == 1 ) //Tiempo Compuesto
			continue;

		modo = (Modo)CVMood(m_hConj,i);
		if( modo==0 || modo==5 || modo==6 )
			continue;

		tiempo = (Tiempo)CVTime1(m_hConj,i);

		for( persona=0; persona < 3; persona++ )
			for( numero=0; numero < 2; numero++ )
				{
				//COLLECT_INTERVAL("VerbRedCheck_1"); // 0.66 

				verbconj = CVConjugate( m_hConj,root,modo,tiempo,persona,numero,FALSE,FALSE );

				//TRACE("3 %s == %s", verbconj.c_str(), hData->m_Palabra.c_str());
				if( CompareStrings(verbconj, hData->m_Palabra) )
					{
					//COLLECT_INTERVAL("VerbRedCheck_2"); // 0.0
					// Se agrega la reducción a la lista m_Types
					reducida = TRUE;

					CRWType redtype;

					redtype.m_PalReduc = root;
					redtype.m_Tipo = tipo;
					redtype.m_Modo = modo;
					redtype.m_Tiempo = tiempo;
					redtype.m_Persona = (Persona)persona;
					redtype.m_Numero = (Numero)numero;

					hData->m_Types.Add(redtype);
					}
				}
		}

	CWordArray OtherMoods;
	OtherMoods.Add(0);
	OtherMoods.Add(5);
	OtherMoods.Add(6);

	for( i=0; i < OtherMoods.GetCount(); i++ )
		{
		verbconj = CVConjugate(m_hConj,root,OtherMoods[i],tiempo,persona,numero,FALSE,FALSE );

		//TRACE("4 %s == %s", verbconj.c_str(), hData->m_Palabra.c_str());
		if( CompareStrings(verbconj,hData->m_Palabra) )
			{
			// Se agrega la reducción a la lista m_Types
			
			reducida = TRUE;

			CRWType redtype;

			redtype.m_PalReduc = root;
			redtype.m_Tipo = tipo;
			redtype.m_Modo = (Modo)OtherMoods[i];

			hData->m_Types.Add(redtype);
			}
		else
			{
			if( OtherMoods[i] == 6 && (hData->m_Idioma != 1) )
				{
				//TRACE("5 %s, %s", verbconj.c_str(), tipo.c_str());
				if( AdjRedCheck(hData, verbconj, tipo) == TRUE)
					{
					reducida = TRUE;

					CRWType redtype;
					int lastIndex;

					lastIndex = hData->m_Types.GetUpperBound();
					redtype = hData->m_Types.GetAt(lastIndex);  
					redtype.m_PalReduc = root;
					redtype.m_Modo = (Modo)OtherMoods[i];
					hData->m_Types.SetAt(lastIndex,redtype);  
					}
				}
			}
		}

	return reducida;
	}
//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::FindRedIrreg( CRWhData* hData, CStringA tipo)
//tipo --> SS:Sust   AA:Adj   AI:Adj inmóvil	VV: Verbo
	{
	//COLLECT_INTERVAL("FindRedIrreg"); // 0.39 20403
	CStringA root,data,datatype;
	BOOL reducida = FALSE;

	if( m_GenDic == NULL )
		return FALSE;

	if( tipo == "VV" || tipo == "VA" || tipo == "BE" || tipo == "HA")
		{
		CDictLoad* VIDic = m_Dicts->GetPtrIrregVerbDict();

		if( VIDic == NULL )
			return FALSE;

		LPCSTR szRoot;
		if( VIDic->GetKey(hData->m_Palabra.c_str(), szRoot))
			{
			root = szRoot;
			if( m_GenDic->GetKey(root,data) )
				{
				GetType(datatype,data);

				if( tipo_verbo.Find(datatype) != -1 )
					reducida = VerbRedCheck(hData,root,"VV");
				else
					{
					if( tipo_verbo_aux.Find(datatype) != -1 )
						reducida = VerbRedCheck(hData,root,"VA");
					else
						if( tipo_BEHA.Find(datatype) != -1 )
							reducida = VerbRedCheck(hData,root,datatype);
	
					}
				}
			}
		}
	else
		{
		CDictLoad* SAIDic = m_Dicts->GetPtrIrregNounDict(); 

		if( SAIDic == NULL )
			return FALSE;

		LPCSTR szRoot;
		if( SAIDic->GetKey(hData->m_Palabra.c_str(), szRoot))
			{
			root = szRoot;
			if( m_GenDic->GetKey(root,data) )
				{
				GetType(datatype,data);

				//Si tipo es sustantivo
				if( tipo == "SS")
					{
					if( tipo_sust.Find(datatype) != -1 )
						reducida = NounRedCheck(hData,root);
					}
				else
					{
					if( (tipo_adj.Find(datatype) != -1 || tipo_adj_in.Find(datatype) != -1) && tipo_pronombre.Find(datatype) != -1)
						reducida = AdjRedCheck(hData,root,"AO");
					else
						{
						if( tipo_adj.Find(datatype) != -1 )
							reducida = AdjRedCheck(hData,root,"AA");
						else
							{
							if( tipo_adj_in.Find(datatype) != -1 )
								reducida = AdjRedCheck(hData,root,"AI");
							else
								if( tipo_pronombre.Find(datatype) != -1 )
									reducida = AdjRedCheck(hData,root,datatype);
								else
									if( tipo_articulo.Find(datatype) != -1 )
										reducida = AdjRedCheck(hData,root,datatype);
							}
						}
					}
				}
			}
		}

	return reducida;
	}

//-------------------------------------------------------------------------------------------------------------------

//BOOL CRedWord::FindSufijo(CDictLoad* dicsuf, const CStringA& palabra, CStringA &sufijo, CStringA& datasuf)
BOOL CRedWord::FindSufijo(CDictLoad* dicsuf, const CStringA& palabra, CStringA &sufijo, LPCSTR& datasuf)
	{
	sufijo = palabra;

	while( !sufijo.IsEmpty())
		{
		if(dicsuf->GetKey(sufijo.c_str(), datasuf))
		//if(dicsuf->IsKey(sufijo))
			return TRUE;

		sufijo.Delete(0, 1);
		}

	return FALSE;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::FindPrefijo(CDictLoad* dicpref, const CStringA& palabra, CStringA &prefijo)
	{
	prefijo = palabra;

	while( !prefijo.IsEmpty())
		{
		if(dicpref->IsKey(prefijo))
			return TRUE;
		//prefijo = prefijo.Left(prefijo.GetLength()-1);
		prefijo.Truncate(prefijo.GetLength()-1);
		}

	return FALSE;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::FindRedSuffix( CRWhData* hData, const CStringA& tipo)
//tipo --> 0:Sust y Adj   1: Verbos
	{
	//COLLECT_INTERVAL("FindRedSuffix"); // 1.78 19438
	CStringA sufijo, palorig,datasuf,raiz,dataraiz,finraiz,iniraiz,datatype;
	//CDict* SufDict;
	CDictLoad* SufDict;
	int pos;
	BOOL reducida = FALSE;

	if( m_GenDic == NULL )
		return FALSE;

	if( tipo == "VV" || tipo == "VA" || tipo == "BE" || tipo == "HA")
		{
		SufDict = m_Dicts->GetPtrVerbSuffixDict(); 
		if( SufDict == NULL )
			return FALSE;
		}
	else
		{
		SufDict = m_Dicts->GetPtrNounSuffixDict(); 
		if( SufDict == NULL )
			return FALSE;
		}

	palorig = hData->m_Palabra;
	CStringA inicial(palorig);

	//bool checkParticip = false;

	for(int j=0; j<2; j++ )
		{
		while( !palorig.IsEmpty() )
			{
			LPCSTR szDatasuf;
			if(!FindSufijo(SufDict, palorig, sufijo, szDatasuf))
				break;
			//SufDict->GetKey(sufijo,datasuf);
			//AfxMessageBox("Sufijo: "+sufijo);
			//AfxMessageBox("Datasuf: "+datasuf);
			datasuf = szDatasuf;
			datasuf += ",";

			sufijo += "$";

			iniraiz = inicial + "$";
			
			pos = iniraiz.Find(sufijo);

			if(pos==-1)
				{
				//AfxMessageBox("ERROR");
				return FALSE;
				}

			iniraiz.Truncate(pos);// = iniraiz.Left(pos);
			
			sufijo.Truncate(sufijo.GetLength()-1);// = sufijo.Left(sufijo.GetLength()-1);

			do
				{
				pos = datasuf.Find(",");
				if(pos!=-1)
					{
					finraiz = datasuf.Left(pos);
					datasuf = datasuf.Mid(pos+1);
					}
				else
					{
					finraiz = datasuf;
					datasuf = "";
					}

				raiz = iniraiz + finraiz;

				if( raiz.GetLength() == 1 )
					continue;

				if(m_GenDic->GetKey(raiz,dataraiz))
					{
					GetType(datatype,dataraiz);

					//Si tipo es sustantivo
					if( tipo == "SS")
						{
						if( tipo_sust.Find(datatype) != -1 )
							reducida = NounRedCheck(hData,raiz);
						}
					else
						{
						if( tipo == "VV" || tipo == "VA" || tipo == "BE" || tipo == "HA")
							{
							if( tipo_verbo.Find(datatype) != -1 )
								reducida = VerbRedCheck(hData,raiz,"VV");
							else
								{
								if( tipo_verbo_aux.Find(datatype) != -1 )
									reducida = VerbRedCheck(hData,raiz,"VA");
								else
									if( tipo_BEHA.Find(datatype) != -1 )
										reducida = VerbRedCheck(hData,raiz,datatype);
								}
							}

						else
							{
							if( (tipo_adj.Find(datatype) != -1 || tipo_adj_in.Find(datatype) != -1) && tipo_pronombre.Find(datatype) != -1)
								reducida = AdjRedCheck(hData,raiz,"AO");
							else
								{
								if( tipo_adj.Find(datatype) != -1 )
									reducida = AdjRedCheck(hData,raiz,"AA");
								else
									{
									if( tipo_adj_in.Find(datatype) != -1 )
										reducida = AdjRedCheck(hData,raiz,"AI");
									else
										if( tipo_pronombre.Find(datatype) != -1 )
											reducida = AdjRedCheck(hData,raiz,datatype);
										else
											if( tipo_articulo.Find(datatype) != -1 )
												reducida = AdjRedCheck(hData,raiz,datatype);
									}
								}
							}
						}

					if( reducida )
						return TRUE;
					}
				}	
			while(!datasuf.IsEmpty());
			palorig = sufijo.Mid(1);
			}
		}

	return FALSE;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::ReduceSimple( CRWhData* hData)
	{
	//COLLECT_INTERVAL("ReduceSimple"); // 2.18 9561
	CStringA data,tipo, root, datatype;
	BOOL reducida = FALSE;

	//Buscar la palabra en el dicc. general

	if( m_GenDic == NULL )
		return FALSE;

	//CStringA DicType = DiccGetHdrField( m_GenDic->GetHandle(), HDR_ESP_ID );
	CStringA DicType = m_GenDic->GetHdrFieldS(HDR_ESP_ID );
	m_isDicPro = ( DicType=="DW" );
 
	if(m_GenDic->GetKey(hData->m_Palabra,data))
		{
		GetType(datatype,data);

		//Si tipo es sustantivo
		if( tipo_sust.Find(datatype) != -1 )		
			{
			reducida = TRUE;
			CRWType redtype;

			redtype.m_PalReduc = hData->m_Palabra;
			redtype.m_Tipo = "SS";

			hData->m_Types.Add(redtype);
			}


		//Si tipo es adjetivo (móvil o inmóvil)
		if( (tipo_adj.Find(datatype) != -1) || (tipo_adj_in.Find(datatype) != -1) || (tipo_pronombre.Find(datatype) != -1) || (tipo_articulo.Find(datatype) != -1)) 
			{
			reducida = TRUE;

			if( (tipo_adj.Find(datatype) != -1 || tipo_adj_in.Find(datatype) != -1) && tipo_pronombre.Find(datatype) != -1)
				tipo = "AO";
			else
				{
				if( tipo_adj.Find(datatype) != -1 )
					tipo = "AA";
				else
					{
					if( tipo_adj_in.Find(datatype) != -1 )
						tipo = "AI";
					else
						tipo = datatype;
					}
				}


			BOOL red = FALSE;
			// Si el idioma no es Inglés se reduce el adjetivo
			if( hData->m_Idioma != 1)
				{
				//Buscar la pal. en el dicc. de reducción de sust. y adj. irreg.
				red = FindRedIrreg(hData,tipo); 
				if( !red )
					//Buscar la pal. en el dicc. de reducción por sufijos de sust. y adj.
					red = FindRedSuffix(hData,tipo);
				}

			if( !red )
				{
				//Si no se pudo reducir es un tipo simple
				CRWType redtype;

				redtype.m_PalReduc = hData->m_Palabra;
				redtype.m_Tipo = tipo;

				hData->m_Types.Add(redtype);
				}
			}
		else   //Aunque el tipo no sea adjetivo en el dicc. se trata de reducir
			{
			
			//Buscar la pal. en el dicc. de reducción de adjetivos
			if( !FindRedIrreg(hData,"AA") )
				{
				//Buscar la pal. en el dicc. de reducción por sufijos de adjetivos
				if( FindRedSuffix(hData,"AA") )
					reducida = TRUE;
				}
			else
				reducida = TRUE;
			}


		//Si tipo es verbo (auxiliar o no)
		if( (tipo_verbo.Find(datatype) != -1) || (tipo_verbo_aux.Find(datatype) != -1) || (tipo_BEHA.Find(datatype) != -1)) 
			{
			reducida = TRUE;
			if( tipo_verbo.Find(datatype) != -1 )
				tipo = "VV";
			else
				if(tipo_verbo_aux.Find(datatype) != -1)
					tipo = "VA";
				else
					tipo = datatype;

			//Buscar la pal. en el dicc. de reducción de verbos irreg.
			if(!FindRedIrreg(hData,tipo))
				{
				//Buscar la pal. en el dicc. de reducción de sufijos verbales
				if( !FindRedSuffix(hData,tipo) )
					{
					//Si no se pudo reducir es un tipo simple
					CRWType redtype;

					redtype.m_PalReduc = hData->m_Palabra;
					redtype.m_Tipo = tipo;
					redtype.m_Modo = (Modo)0;

					hData->m_Types.Add(redtype);
					}
				}
			}
		else  //Aunque el tipo no sea verbo en el dicc. se trata de reducir
			{
			//Buscar la pal. en el dicc. de reducción de verbos irreg.
			if(!FindRedIrreg(hData,"VV"))
				{
				//Buscar la pal. en el dicc. de reducción de sufijos verbales
				if( FindRedSuffix(hData,"VV") )
					reducida = TRUE;
				}
			else
				reducida = TRUE;
			}

		//Si tipo es adverbio
		if( tipo_adverbio.Find(datatype) != -1 )		
			{
			reducida = TRUE;
			CRWType redtype;

			redtype.m_PalReduc = hData->m_Palabra;
			redtype.m_Tipo = "DD";

			hData->m_Types.Add(redtype);
			}

		}
	else   //Si no está en el diccionario general
		{
		//Buscar la pal. en el dicc. de reducción de sust. y adj. irreg.
		if( FindRedIrreg(hData,"SS") )
			reducida = TRUE;
		else
			//Buscar la pal. en el dicc. de reducción por sufijos de sust. y adj.
			if( FindRedSuffix(hData,"SS") )
				reducida = TRUE;

		//Buscar la pal. en el dicc. de reducción de adjetivos
		if( FindRedIrreg(hData,"AA") )
			reducida = TRUE;
		else
			//Buscar la pal. en el dicc. de reducción por sufijos de adjetivos
			if( FindRedSuffix(hData,"AA") )
				reducida = TRUE;

		//Buscar la pal. en el dicc. de reducción de verbos irreg.
		if( FindRedIrreg(hData,"VV") )
			reducida = TRUE;
		else
			//Buscar la pal. en el dicc. de reducción por sufijos verbales
			if( FindRedSuffix(hData,"VV") )
				reducida = TRUE;
		}

	if( reducida && hData->m_Idioma == 1 )
		{
		//Dejar una sola reduccion en Pasado
		int Past = 0;
		int i=0;
		while( i < hData->m_Types.GetCount() )
			{
			if( hData->m_Types[i].m_Tiempo == PASADO )
				{
				if( Past > 0 )
					hData->m_Types.RemoveAt(i);
				else
					{
					hData->m_Types[i].m_Persona = TERCERAP;		
					hData->m_Types[i].m_Numero = SINGULAR;		
					hData->m_Types[i].m_Modo = mINDICATIVO;	
					Past = 1;
					i++;
					}
				}
			else
				i++;
			}
		}
		
	return reducida;
	}

//-------------------------------------------------------------------------------------------------------------------

int CRedWord::FindSufInString(const CStringA& s, CSmartStrArray *array, int inipos)
	{
	CString ss(s);
	ss += '$'; 
	CStringA temp;

	for( int i=inipos; i < array->GetCount(); i++ )
		{
		temp = array->GetAtSZ(i);
		temp += '$';
		if( ss.Find(temp) != -1 )
			return i;
		}

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------

void CRedWord::EliminaAcento(CStringA *palabra)
	{
	CStringA vocal("aeiou"); 
	CStringA vocalAcent("áéíóú");

	for( int i=0; i < palabra->GetLength(); i++ )
		{
		int pos = vocalAcent.Find(palabra->GetAt(i));
		if( pos != -1 )
			palabra->SetAt(i,vocal[pos]);
		}
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::FindRedEncliticos( CRWhData* hData)
	{
	CStringA strEncliticos, strRaices, strCDirecto, strCIndirecto, strSufijos, strCambio;

	switch( hData->m_Idioma )
		{
		case 0:
			{
			strEncliticos	= "melos;melas;meles;melo;mela;mele;telos;telas;teles;telo;tela;tele;selos;selas;seles;selo;sela;sele;noslos;noslas;nosles;noslo;nosla;nosle;oslos;oslas;osles;oslo;osla;osle;los;las;lo;la;les;le;se;me;te;nos;os";
			strCIndirecto	= "me;me;me;me;me;me;te;te;te;te;te;te;se;se;se;se;se;se;nos;nos;nos;nos;nos;nos;os;os;os;os;os;os;;;;;les;le;se;me;te;nos;os";
			strCDirecto		= "los;las;les;lo;la;le;los;las;les;lo;la;le;los;las;les;lo;la;le;los;las;les;lo;la;le;los;las;les;lo;la;le;los;las;lo;la;;;;;;;";
			strSufijos		= "amo;emo;imo;a;e;í";
			strCambio		= "amos;emos;imos;ad;ed;id";
			break;
			}
		case 2:
			{
			strEncliticos	= "melo;mela;meli;mele;mene;telo;tela;teli;tele;tene;celo;cela;celi;cele;cene;velo;vela;veli;vele;vene;glielo;gliela;glieli;gliele;gliene;selo;sela;seli;sele;sene;lle;mmi;tti;cci;vvi;lo;la;li;le;ne;mi;ti;ci;vi;gli;si";
			strCIndirecto	= "mi;mi;mi;mi;mi;ti;ti;ti;ti;ti;ci;ci;ci;ci;ci;vi;vi;vi;vi;vi;gli;gli;gli;gli;gli;si;si;si;si;si;le;mi;ti;ci;vi;;;;;;mi;ti;ci;vi;gli;si";
			strCDirecto		= "lo;la;li;le;ne;lo;la;li;le;ne;lo;la;li;le;ne;lo;la;li;le;ne;lo;la;li;le;ne;lo;la;li;le;ne;;;;;;lo;la;li;le;ne;;;;;;";
			strSufijos		= "ar;er;ir";
			strCambio		= "are;ere;ire";
			break;
			}
		}
	
	CSmartStrArray Encliticos, CDirecto, CIndirecto, Sufijos, Cambio;
	CStringA raiz, cd, ci, encl, palabra;

	palabra = hData->m_Palabra;

	StringToArray(strEncliticos,&Encliticos,";");
	StringToArray(strCDirecto,&CDirecto,";");
	StringToArray(strCIndirecto,&CIndirecto,";");
	StringToArray(strSufijos,&Sufijos,";");
	StringToArray(strCambio,&Cambio,";");

	int pos,inipos;
	inipos = 0;
	pos = 0;
	
	do
		{
		pos = FindSufInString(palabra, &Encliticos,inipos);

		if( pos == -1 )
			return FALSE;

		CStringA temp = palabra + "$";

		int pr = temp.Find(CString(Encliticos[pos]) + "$");

		raiz = palabra.Left(pr);

		hData->m_Palabra = raiz;

		if( !ReduceSimple(hData) )
			{
			if( hData->m_Idioma == 0 )
				{
				EliminaAcento(&raiz);
				hData->m_Palabra = raiz;
				if( ReduceSimple(hData) )
					break;
				}

			/// Buscar caso en que la palabra reducida no está completa
			int pos1 = FindSufInString(raiz, &Sufijos,0);

			if( pos1 != -1 )
				{
				CStringA temp = raiz + "$";

				int pr = temp.Find(CStringA(Sufijos[pos1]) + "$");

				CStringA raiz1 = raiz.Left(pr) + Cambio[pos1];

				hData->m_Palabra = raiz1;
				if( ReduceSimple(hData) )
					break;
				}
			///

			}
		else
			break;

		inipos = pos + 1;
		}
	while(true);

	CStringA typeVerb("VV;VA;BE;HA");

	int i=0;
	while( i < hData->m_Types.GetCount() )
		{
		if( typeVerb.Find(hData->m_Types[i].m_Tipo) == -1 )
			hData->m_Types.RemoveAt(i);
		else
			{
			Modo temp = hData->m_Types[i].m_Modo;
			if( temp == mINFINITIVO || temp == mIMPERATIVO || temp == mGERUNDIO )
				{
				hData->m_Types[i].m_CDirecto = CDirecto[pos];
				hData->m_Types[i].m_CIndirecto = CIndirecto[pos];
				i++;
				}
			else
				hData->m_Types.RemoveAt(i);
			}
		}

	if( hData->m_Types.GetCount() == 0 )
		return FALSE;

	return TRUE;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::IsInitialUpper(const CStringA& s)
	{
	return isupper(s[0]) != 0;
	}

//-------------------------------------------------------------------------------------------------------------------

void CRedWord::InitialToUpper(CStringA& s)
	{
	s.SetAt( 0, CharAnsiToUpper(s[0]) );
	}

//-------------------------------------------------------------------------------------------------------------------

void CRedWord::InitialToLower(CStringA& s)
	{
	s.SetAt( 0, CharAnsiToLower(s[0]) );
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::ReduceCompElem(CRWhData* hData)
	{
	bool red = false;

	if(ReduceSimple(hData))
		{
		for(int i = 0; i < hData->m_Types.GetCount(); i++ )
			{
			CStringA tipo = (hData->m_Types.GetAt(i)).m_Tipo;
			if(tipo_sust.Find(tipo)!=-1 || tipo_adj.Find(tipo)!=-1 || tipo_adj_in.Find(tipo)!=-1 || tipo_verbo.Find(tipo)!=-1 || (tipo_verbo_aux.Find(tipo)!=-1 && tipo_BEHA.Find(tipo)!=-1))
				red = true;
			}
		}

	return red;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::FindCompElem(int lang, const CStringA& compuesto, const CStringA& palabra, int idx, int* div, CStringA& reducWord, bool checkVerb, CStringA& tipo)
	{
	CStringA prefijo = palabra.Left(idx);
//	DWORD hand;
	bool allowCVerb = false;
	bool isNoun = false;

	//Si es una palabra al final y el compuesto comienza con minúscula puede ser un verbo conjugado
	if(palabra.GetLength() == idx && !IsInitialUpper(compuesto)) 
		allowCVerb = true;

	//Si es una palabra al final y el compuesto comienza con mayúscula tiene que ser un sustantivo
	if(palabra.GetLength() == idx && IsInitialUpper(compuesto)) 
		isNoun = true;

	tipo = "SS";
	while( !prefijo.IsEmpty())
		{
		if( prefijo == 's' )
			{
			*div = 1;
			reducWord = prefijo;
			return TRUE;
			}

		CRWhData* handt = new CRWhData(lang);

		handt->m_Palabra = prefijo;

		if( prefijo.GetLength()>1 )
			{
			CStringA str(prefijo); 
			if(!IsInitialUpper(str))
				{
				//Si es minúscula se lleva a mayúscula el 1er caract
				InitialToUpper(str);
				handt->m_Palabra = str;
				}

			if( ReduceCompElem(handt) )
				{
				tipo = "SS";
				*div = str.GetLength();
				reducWord = str;
				RWFreehData(handt);
				return TRUE;
				}

			if(!isNoun)
				{
				//Se lleva a minúscula el 1er caract
				InitialToLower(str);
				handt->m_Palabra = str;

				if( ReduceCompElem(handt) )
					{
					bool red = true;
					if( allowCVerb==false )
						{
						for(int i = 0; i < handt->m_Types.GetCount(); i++ )
							{
							CStringA tipo = (handt->m_Types.GetAt(i)).m_Tipo;
							//Si tipo es verbo
							if(tipo_verbo_aux.Find(tipo) !=-1 || tipo_verbo.Find(tipo) !=-1 || tipo_BEHA.Find(tipo) !=-1)
								{
								//Si verbo conj <> de verbo en infinitivo
								if( (handt->m_Types.GetAt(i)).m_PalReduc != str )
									{
									red = false;
									handt->m_Types.RemoveAll(); 
									}
								}
							}
						}

					if(red)
						{
						*div = str.GetLength();
						reducWord = str;
						RWFreehData(handt);
						return TRUE;
						}
					}

				//2da pasada de Chequeo de Verbo sin -en
				if(checkVerb)
					{
					if( str[str.GetLength()-1]=='e')
						str += "n";
					else
						str += "en";

					handt->m_Palabra = str;
					if( ReduceCompElem(handt) )
						{
						*div = prefijo.GetLength();
						reducWord = str;
						tipo = (handt->m_Types.GetAt(0)).m_Tipo;
						RWFreehData(handt);
						return TRUE;
						}
					}
				}
			}
		prefijo    = prefijo.Left(prefijo.GetLength()-1);
		allowCVerb = false;
		isNoun     = false;
		RWFreehData(handt);
		}

	*div = 0;

	return FALSE;
	}

//-------------------------------------------------------------------------------------------------------------------

BOOL CRedWord::ReduceCompound( CRWhData* hData)
	{
	CStringA orig = hData->m_Palabra;
	int idx = orig.GetLength();
	CStrArray Words;
	CStrArray ReducWords;
	bool red = false;
	int div;
	bool checkVerb = false;
	CStringA redW,tipo;

	//Analizar si es un verbo separable con el infijo -zu

	int pos = orig.Find("zu");
	if(pos != -1)
		{
		CStringA pref = orig.Left(pos);
		CStringA verb = pref + orig.Mid(pos+2);
		CStringA data;
		CDict *prefDic = (CDict*)CVGetPtrDicRef( m_hConj, dcPreffixVerb );

		if(prefDic->GetKey(verb,data))
			{
			pref = "." + pref;
			if( pref == data )
				{
				CRWType redtype;

				redtype.m_PalReduc = verb;
				redtype.m_Tipo = "VV";

				hData->m_Types.Add(redtype);
				return TRUE;
				}
			}
		}
	
	//Analizar si es una palabra compuesta
	while(true)	
		{
		if( FindCompElem(hData->m_Idioma,hData->m_Palabra,orig,idx,&div,redW,checkVerb,tipo) )
			{
			if(div >= orig.GetLength())
				{
				Words.Add(orig);
				ReducWords.Add(redW);
				red = true;
				break;
				}
			else
				{
				Words.Add(orig.Left(div));
				ReducWords.Add(redW);
				orig = orig.Mid(div);
				idx = orig.GetLength(); 
				}
			}
		else
			{
			if(Words.GetCount() > 0)
				{
				int upp = Words.GetUpperBound();
				CStringA lastStr = Words.GetAt(upp);
				orig = lastStr + orig; 
				idx = lastStr.GetLength()-1; 
				Words.RemoveAt(upp); 
				ReducWords.RemoveAt(upp);
				}
			else
				{
				//No se pudo reducir
				//Reducir como verbo insertando -en
				if( checkVerb==false )
					{
					orig = hData->m_Palabra;
					checkVerb = true;
					idx = orig.GetLength();
					}
				else
					break;
				}
			}
		}

	CStringA compound;

	if(red)
		{
		compound = ReducWords[0];

		if( ReducWords.GetCount()==1 || compound=="s" || ReducWords[ReducWords.GetUpperBound()]=="s" )
			return FALSE;

		InitialToUpper(compound);
		for(int i=1; i < ReducWords.GetCount(); i++ )
			{
			CStringA rw = ReducWords[i];
			if( rw.GetLength() > 1 )
				{
				InitialToUpper(rw);
				compound = compound + "|" + rw;
				}
			}

		CRWType redtype;

		redtype.m_PalReduc = compound;
		redtype.m_Tipo = tipo;

		hData->m_Types.Add(redtype);
		}
	else
		return FALSE;

	return TRUE;
	}

//-------------------------------------------------------------------------------------------------------------------

