// RedWord.h : main header file for the REDWORD DLL
//

#ifndef __REDWORD__
#define __REDWORD__

#include "WinUtil.h"
#include "RWDefn.h"

//-------------------------------------------------------------------------------------------------------------------
// CRWType, clase que almacena los datos para cada posible reducción de una palabra
//-------------------------------------------------------------------------------------------------------------------

class CRWType
	{	
	public:
	CRWType()
		{
		m_Modo       = mINFINITIVO;	
		m_Tiempo     = PRESENTE;		
		m_Persona    = TERCERAP;		
		m_Numero     = SINGULAR;		
		m_Genero     = MASCULINO;		
		m_Grado      = POSITIVO;
		};

  CRWType(const CRWType &tp)
    { 
		m_PalReduc   = tp.m_PalReduc;
		m_Tipo       = tp.m_Tipo;
		m_Modo       = tp.m_Modo;	
		m_Tiempo     = tp.m_Tiempo;		
		m_Persona    = tp.m_Persona;		
		m_Numero     = tp.m_Numero;		
		m_Genero     = tp.m_Genero;		
		m_Grado      = tp.m_Grado;
		m_Prefijo    = tp.m_Prefijo;
		m_CDirecto   = tp.m_CDirecto;
		m_CIndirecto = tp.m_CIndirecto;
    }

	CRWType& operator=(const CRWType &tp)
    { 
		m_PalReduc   = tp.m_PalReduc;
		m_Tipo       = tp.m_Tipo;
		m_Modo       = tp.m_Modo;	
		m_Tiempo     = tp.m_Tiempo;		
		m_Persona    = tp.m_Persona;		
		m_Numero     = tp.m_Numero;		
		m_Genero     = tp.m_Genero;		
		m_Grado      = tp.m_Grado;
		m_Prefijo    = tp.m_Prefijo;
		m_CDirecto   = tp.m_CDirecto;
		m_CIndirecto = tp.m_CIndirecto;
    return *this;
    };

  void Copy( CRWType &tp )
    {
		m_PalReduc   = tp.m_PalReduc;
		m_Tipo       = tp.m_Tipo;
		m_Modo       = tp.m_Modo;	
		m_Tiempo     = tp.m_Tiempo;		
		m_Persona    = tp.m_Persona;		
		m_Numero     = tp.m_Numero;		
		m_Genero     = tp.m_Genero;		
		m_Grado      = tp.m_Grado;
		m_Prefijo    = tp.m_Prefijo;
		m_CDirecto   = tp.m_CDirecto;
		m_CIndirecto = tp.m_CIndirecto;
    };

	void Dump()
		{
		TRACE("  CRWType: %s %s, %d %d %d %d %d %d, %s %s %s", m_PalReduc.c_str(), m_Tipo.c_str(), m_Modo, m_Tiempo, m_Persona, m_Numero, m_Genero, m_Grado, 
			m_CDirecto.c_str(), m_CIndirecto.c_str(), m_Prefijo.c_str());
		}

	CStringA  m_PalReduc;	  //Palabra reducida
	CStringA  m_Tipo;		    //Tipo gramatical al que se redujo (SS,AA,AI,VV,VA)
	Modo	    m_Modo;		    //Si es verbo, el Modo
	Tiempo	  m_Tiempo;	    //Si es verbo, el Tiempo
	Persona	  m_Persona;	  //Si es verbo, la Persona
	Numero	  m_Numero;	    //Si es verbo o sust, el Número
	Genero	  m_Genero;	    //Si es sust o adj, el Género
	Grado	    m_Grado;	    //Si es adjetivo, grado de comparación
	CStringA  m_CDirecto;	  //Si es un verbo con enclítico, cuál es el complemento directo
	CStringA  m_CIndirecto; //Si es un verbo con enclítico, cuál es el complemento indirecto
	CStringA  m_Prefijo;	  //Prefijo que se le quitó a la palabra original para llegar a la reducida
    
  int GetType();
	};

//-------------------------------------------------------------------------------------------------------------------
// CRWhData, clase que almacena el handle a la palabra que se quiere reducir contiene la palabra a reducir, el 
// idioma en que se encuentra y una lista de objetos CRWType que contienen las palabras a las que se puede reducir
//-------------------------------------------------------------------------------------------------------------------
class CRWhData
	{	
public:
	int       m_Idioma;		          // Idioma en que se encuentra la palabra a reducir
	CStringA  m_Palabra;	          // Palabra a reducir

	CArray<CRWType> m_Types;  // Lista con datos de las palabras reducidas

	CRWhData( int idioma=-1 )	{	m_Idioma = idioma;};

	void Dump()
		{
		TRACE("CRWhData: %s", m_Palabra.c_str());
		for(int i=0; i<m_Types.GetCount(); i++)
			m_Types[i].Dump();
		}

	};

//-------------------------------------------------------------------------------------------------------------------
// CRedWord
//-------------------------------------------------------------------------------------------------------------------

class CRedWord
  {
  public:
    CStringF   m_DataPath;    // Camino donde se encuentran los diccionarios de reducción
    BOOL       m_isDicPro;    // Si el diccionario es de DicPro

    CRWDicRef* m_Dicts;       // Objeto que maneja los dicionarios de reducción

    HCONJ      m_hConj;       // Manipulador al API de reducción
    bool       m_FreehConj;   // Determina si la clase libera el API de conjugación o no

    CDict* m_GenDic;       // Puntero al diccionario general

    CRedWord();
    ~CRedWord();

    void   SetDataPath(CStringF dataPath)
			{
			m_DataPath = dataPath;
			if (!m_DataPath.IsEmpty() && m_DataPath.Right(1) != FOLDER_SEP)
				m_DataPath += FOLDER_SEP;
			}
    CStringF GetDataPath() {return m_DataPath;	}

    void  SetConjHandle( HCONJ hConj, bool free=false );
    void  SetGenDict( CDictMain* hGenDic, bool free=false );

    BOOL     Open( int lang );
    BOOL     Close();
    HREDDATA Reduce( const CStringA& Word );
    LPVOID   GetPtrDicRef( RWDicName name);

  private:
    //-------------------------------------------------------------------------------------------------------------------
    //                  FUNCIONES INTERNAS UTILIZADAS EN ESTA CLASE
    //-------------------------------------------------------------------------------------------------------------------
    BOOL LoadGenDic();
    BOOL GetType(CStringA &type, const CString& data);
    BOOL NounRedCheck(CRWhData* hData, const CStringA& root);
    BOOL AdjRedCheck(CRWhData* hData, const CStringA& root, const CStringA& tipo);
    BOOL CompareStrings(CStringA s1, CStringA s2);
    BOOL VerbRedCheck(CRWhData* hData, const CStringA& root, const CStringA& tipo);
    BOOL FindRedIrreg(CRWhData* hData, CStringA tipo);
    BOOL FindSufijo(CDictLoad* dicsuf, const CStringA& palabra, CStringA &sufijo, LPCSTR& datasuf);
    BOOL FindPrefijo(CDictLoad* dicpref, const CStringA& palabra, CStringA &prefijo);
    BOOL FindRedSuffix(CRWhData* hData, const CStringA& tipo);
    BOOL ReduceSimple(CRWhData* hData);
    int  FindSufInString(const CStringA& s, CSmartStrArray *array, int inipos);
    void EliminaAcento(CStringA *palabra);
    BOOL FindRedEncliticos(CRWhData* hData);
    BOOL IsInitialUpper(const CStringA& s);
    void InitialToUpper(CStringA& s);
    void InitialToLower(CStringA& s);
    BOOL ReduceCompElem(CRWhData* hData);
    BOOL FindCompElem(int lang, const CStringA& compuesto, const CStringA& palabra, int idx, int* div, CStringA& reducWord, bool checkVerb, CStringA& tipo);
    BOOL ReduceCompound(CRWhData* hData);

  };

#endif
