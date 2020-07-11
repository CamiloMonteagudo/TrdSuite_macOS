////////////////////////////////////////////////////////////////////////////
//   Class CWord                                                          //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         9/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se define class para el trabajo con la palabra                        // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#if !defined(__WORD__)
#define __WORD__

#include "UtilFun.h"
#include "ApiConj.h"

#include "ccaso.h"
#include "Declension.h"
#include "CGrade.h"
#include "ArticleType.h"
#include "cgenero.h"
#include "cnumero.h"
#include "cpersona.h"
#include "ctiempo.h"
#include "cmodo.h"
#include "CDiWord.h"
#include "CDicType.h"
#include "COParte.h"
#include "GramType.h"
             
/////////////////////////////////////////////////////////////
//  struct para extraer identificador según llave          //
/////////////////////////////////////////////////////////////

struct PalClave
    {
    const char *w;      // llave 
    int       i;       // identificador asociado
    };

/******
struct PATRON_TIPO
       {
       char tipo[3];
       char patron;
       };
*******/       
class CWordEsp : public CMFCObject
    {                   
    DECLARE_SERIAL(CWordEsp);
    public:
    
    //CFixedStringT< CStringA, 256 > m_Esp;
    //CFixedStringT< CStringA, 256 > m_Word;
    CStringA m_Esp;
    CStringA m_Word;
    CSmartStrArray m_Semantica;
    CGenero m_Genero;
    CNumero m_Numero;
    int     m_Morf;       // Morfologia si verbo 
    BOOL    m_Reflexivo;  // si verbo reflexivo
    
    /* virtual */ void Serialize(CArchive &ar);

    CWordEsp()
        {
        m_Morf      = -1;
				m_Reflexivo = FALSE;
        }

    CWordEsp(CWordEsp &w)
        {
        m_Esp  = w.m_Esp;
        m_Word = w.m_Word;
        m_Semantica = w.m_Semantica;
		//for(int i = 0; i < m_Semantica.GetSize(); i++)
		//	m_Semantica.SetAt(i,w.m_Semantica.GetAt(i));
        m_Genero = w.m_Genero;
        m_Numero = w.m_Numero;
        m_Morf = w.m_Morf;
        m_Reflexivo = w.m_Reflexivo;
        }
    CWordEsp(CWordEsp *w)
        {
        m_Esp  = w->m_Esp;
        m_Word = w->m_Word;
        m_Semantica = w->m_Semantica;
		//for(int i = 0; i < m_Semantica.GetSize(); i++)
		//	m_Semantica.SetAt(i,w->m_Semantica.GetAt(i));
        m_Genero = w->m_Genero;
        m_Numero = w->m_Numero;
        m_Morf = w->m_Morf;
        m_Reflexivo = w->m_Reflexivo;
        }
    };

class CWord : public CMFCObject
  {                   
  DECLARE_SERIAL(CWord);
  public:
    // datos 
    CStringA    m_Orig;      // Palabra original
    CStringA    m_Origlwr;   // Pal original en minus la que se busca en dic
    CStringA    m_Dest;      // Pal original del dicc en campo Destino
    CStringA    m_Trad;      // Traduccion de la Palabra 
    CStringA    m_Key;       // campo key de diccionario
    CStringA    m_Data;      // campo data de diccionario
    CStringA    m_DataC;     // campo data de diccionario de comodines
    CStringA    m_MinMay;    // campo data de diccionario para mayús/minúsculas
    CStringA    m_sTipo;     // Tipo de la palabra según diccionario
    //    CFixedStringT< CStringA, 256 >    m_sTipo;     // Tipo de la palabra según diccionario
    CStringA    m_sTipo2;    // Tipo auxiliar para verbos irreg, etc
    CStringA    m_sPatron;   // Patron que se utilizó para intercambio
    CStringA    m_sComodin;  // Comodín que se utilizó para determinar género, etc
    CStringA    m_sPrefijo;  // cadena con la ejecución del prefijo
    CStringA   m_sSufijo;   // cadena con la ejecución del sufijo
    CStringA    m_sKeyPrefijo;// Prefijo de reducción encontrado
    CStringA    m_sKeySufijo; // Sufijo de reducción encontrado
    CStringA    m_OfAs;      // cadena con la traducción de of en posesivos y en sust en aposición
    CStringA    m_Prep;       // lista de preposiciones con las que no es necesario que la palabra lleve artículo
    CStringA    m_EntreParentesis;	// Si lleva una frase entre parentesis despues de la palabra 
    CStringA    m_sInsert;          // palabra a insertar delante
    CStringA    m_sAdd;             // palabra a adicionar detrás
    CStringA    m_sParticle;        // particula separable de los verbos del alemán
    CStringA    m_sSemantica;       // Campo semantica del diccionario
    int		    m_EntreQue;			// 0 entre nada, 1 entre parentesis, 2 entre comillas, 3 entre comillas simples, 4 entre corchetes
    int			m_AltMeaning;		// meaning a utilizar por la traducción -1 si no tiene meaning
    CStringA		m_AddKey;			// llave de frase idiomatica a agregar al dcc despues de una regla
    CStringA		m_AddData;			// datos de frase idiomatica a agregar al dcc despues de una regla
    CStringA		m_Trace;			// Traza de la palabra

		CGramType   m_Tipo;				// equivalente a m_sTipo pero es mas eficiente para hacer los Match

    // datos del formateo en RTF fijas las que no se mueven con la palabra cuando esta cambia de posición
    //    CStringA m_RtfMovilIni;  // instrucciones RTF que no permanecen  fijas al inicio de palabra
    //    CStringA m_RtfMovilFin;  // instrucciones RTF que no permanecen  fijas al final de palabra

    // por ahora las trataremos com que siempre son fijas

    CStringA m_RtfFijoIni;   // instrucciones RTF que permanecen  fijas al inicio de palabra
    CStringA m_RtfFijoFin;   // instrucciones RTF que permanecen  fijas al final de palabra


    CModo        m_Modo;         // Modo en que esta el verbo 
    CCaso        m_Case;         // Si la pal original esta en upr, lwr o mixto
    CGenero      m_Genero;       // genero  
    CNumero      m_Numero;       // numero  
    CPersona     m_Persona;      // persona 
    CTiempo      m_Tiempo;       // Tiempo del verbo      
    CDiWord      m_DiWord;       // Tipo de la palabra según división de la oración
    CDicType     m_DicType;      // Tipo de la palabra según el diccionario
    COParte      m_OParte;       // Parte de la oración en que se encuentra la palabra
    CDeclination m_Declination;  // Declinación
    CArticleType m_ArticleType;  // Tipo de articulo para la declinación de adjetivos
    CGrado       m_Grade;        // Grado de significación del adjetivo superlativo, comparativo, superrlativo absoluto

    // Variables booleanas
    short int m_Articulo;   // 0 no lleva artículo 
    // 1    lleva artículo
    // 2 lleva determinativo del otro sust 
    // 3 si lleva of 
    // 4 si lleva of the                      
    // 5 si lleva que                      
    WORD m_Adiciona;   // 0 no adicionar nada
    // 1 adiciona it      

    BOOL      m_NoBusca;    // Si la palabra se busca en dicc o no 
    BOOL      m_Buscada;    // Si la palabra fue buscada en dicc o no 
    BOOL      m_BuscadaC;   // Si la palabra fue buscada en dicc o no 
    BOOL      m_Traducida;  // Si la palabra fue Traducida o no 
    BOOL      m_Delete;     // Si la palabra esta borrada o no 
    BOOL      m_Plural;     // si es plural 
    BOOL      m_Romano;     // si puede ser # romano 
    BOOL      m_EntreComillas; // si la palabra se encuentra entre comillas
    BOOL      m_Posesivo; // si la palabra se encuentra entre comillas
    BOOL      m_TradTipo;   // si ya se ejecutó la busqueda preliminar de tipo
    BOOL      m_Reflexivo;  // si es verbo reflexivo o no
    BOOL      m_LastPrepNoArt; // si la anterior es prep no insertar artículo
    BOOL      m_Femenino;   // si la palbra esta en femenino por sufijo               
    BOOL      m_Abreviatura; // si la palabra puede ser abreviatura
    BOOL      m_NotArtInOf; // si se pone articulo después de of de posesivos y sust en aposición                    
    BOOL      m_ArtAtFirst; // si se pone artículo cuando la palabra es primera de oración
    BOOL      m_Negado;     // Si es verbo y se encuentra negado
    BOOL      m_Compuesto;  // Si el verbo se encuentra en un tiempo compuesto
    BOOL      m_NoEnComodin;// Si la palabra forma parte del comodín
    BOOL      m_Concordada;  // Si la palabra ya fue concordada
    int	      m_PosFraseComodin;	// Posicion si frase comodin
    WORD      m_SustDerivado;    // Aumentativo y diminutivo
    WORD      m_Reserva2;
    WORD      m_Reserva3;
    WORD      m_Reserva4;
    WORD      m_Reserva5;
    CStringA   m_StrReserva1;     // Salva de traducción
    CStringA   m_Frases;          // frases de comodines
    CStringA   m_FrasesTipo;
    CStringA   m_StrFormat;
    CStringA   m_StrReserva5;
    CStringA   m_StrSaveTrad;
    CStringA   m_StrIniBlank;
    DWORD      m_Frecuencia;
    CStringA   m_TipoTrd;
    CStringA   m_EspTrd;

    CStringA   m_Mascara;    // Mascara para palabras en las frases
    DWORD     m_MascaraC;   // Mascara para palabras en las frases de comodines
    int       m_Morf;       // Morfologia si verbo 
    char      m_Patron;     // patron según parsing 
    int       m_Prefijo;    // prefijo que tiene la palabra -1 si no tiene
    CObArray  m_ComodinArray; // Lista de palabras que se agruparon en el comodin
    CObArray  m_AcepArray;    // Lista de acepciones clasificadas por especialidad

    // Variables static de la class

    // ---------------------------------------------------------
    //  Definición de la class
    // ---------------------------------------------------------
  public:

    CWord(const CStringA& s)
      {               
      m_Orig      = s;
      m_Morf      = -1;
			m_NoBusca = m_Buscada = m_BuscadaC = m_Traducida = m_Delete = m_Plural =    
				m_Romano = m_EntreComillas = m_Posesivo = m_TradTipo = m_Reflexivo = m_LastPrepNoArt = 
				m_Femenino = m_Abreviatura = m_NotArtInOf = m_ArtAtFirst = m_Negado = m_Compuesto =   
				m_NoEnComodin = m_Concordada = FALSE;
      //            m_ParseType = dtNULL;
      //            m_WordType  = dtNULL;
      m_Patron    = ' ';
      m_Prefijo   = -1;
      m_MascaraC  = 0L;
      m_Articulo  = 0;
      m_SustDerivado = 0;
      m_Adiciona = 0;
      m_Reserva2 = 0;
      m_Reserva3 = 0;
      m_Reserva4 = 0;
      m_Reserva5 = 0;
      m_Frecuencia = 0;
      m_EntreQue = 0;
      m_PosFraseComodin = 0;
      m_AltMeaning = -1;
      }         

    CWord(CWord &w)         // Copy constructor
      {               
      CopyWord(w);
      }         

    CWord(CWord *w)         // Copy constructor
      {               
      CopyWord(*w);
      }         

    CWord()
      {
      m_Morf      = -1;
			m_NoBusca = m_Buscada = m_BuscadaC = m_Traducida = m_Delete = m_Plural =    
				m_Romano = m_EntreComillas = m_Posesivo = m_TradTipo = m_Reflexivo = m_LastPrepNoArt = 
				m_Femenino = m_Abreviatura = m_NotArtInOf = m_ArtAtFirst = m_Negado = m_Compuesto =   
				m_NoEnComodin = m_Concordada = FALSE;
      //            m_ParseType = dtNULL;
      //            m_WordType  = dtNULL;
      m_Patron    = ' ';
      m_Prefijo   = -1;
      m_MascaraC  = 0L;
      m_Articulo  = 0;
      m_SustDerivado = 0;
      m_Adiciona = 0;
      m_Reserva2 = 0;
      m_Reserva3 = 0;
      m_Reserva4 = 0;
      m_Reserva5 = 0;
      m_Frecuencia = 0;
      m_EntreQue = 0;
      m_PosFraseComodin = 0;
      m_AltMeaning = -1;
      }         

    ~CWord()
      {
      for(int i = 0; i < m_AcepArray.GetSize(); i++)
        delete m_AcepArray[i];                    
      m_AcepArray.RemoveAll();
      //for(i = 0; i < m_ComodinArray.GetSize(); i++)
      //    delete m_ComodinArray[i];                    
      m_ComodinArray.RemoveAll();
      }

		void Dump();
			
    virtual void Serialize(CArchive &ar);
    virtual CWord &CopyWord(CWord &w);

    BOOL IsInMode(Modo m);

    // ---------------------------------------------------------
    //  operadores para facilitar poner género número,etc
    // ---------------------------------------------------------
  public:
    // operadores para modo del verbo
    CModo& operator=(Modo m)
      {
      m_Modo = m;
      return m_Modo;
      }

    CModo& operator=(CModo m)
      {
      m_Modo = m;
      return m_Modo;
      }
    // operadores para Caso de la palabra
    CCaso& operator=(Caso m)
      {
      m_Case = m;
      return m_Case;
      }

    CCaso& operator=(CCaso m)
      {
      m_Case = m;
      return m_Case;
      }
    // operadores para Género de la palabra
    CGenero& operator=(Genero m)
      {
      m_Genero = m;
      return m_Genero;
      }

    CGenero& operator=(CGenero m)
      {
      m_Genero = m;
      return m_Genero;
      }
    // operadores para Número de la palabra
    CNumero& operator=(Numero m)
      {
      m_Numero = m;
      return m_Numero;
      }

    CNumero& operator=(CNumero m)
      {
      m_Numero = m;
      return m_Numero;
      }
    // operadores para Persona de la palabra
    CPersona& operator=(Persona m)
      {
      m_Persona = m;
      return m_Persona;
      }

    CPersona& operator=(CPersona m)
      {
      m_Persona = m;
      return m_Persona;
      }
    // operadores para Tiempo de la palabra
    CTiempo& operator=(Tiempo m)
      {
      m_Tiempo = m;
      return m_Tiempo;
      }

    CTiempo& operator=(CTiempo m)
      {
      m_Tiempo = m;
      return m_Tiempo;
      }
    // operadores para Tipo según división en palabras de la oración
    CDiWord& operator=(DiWord m)
      {
      m_DiWord = m;
      return m_DiWord;
      }

    CDiWord& operator=(CDiWord m)
      {
      m_DiWord = m;
      return m_DiWord;
      }

    CWord& operator=(CWord &m)
      {              
      CopyWord(m);
      return *this;
      }
  };

typedef CWord *LPCWORD;

/*static*/ int FindHotWord(const CStringA &s , PalClave *p,int total);
int DumpClave(PalClave *p,int total);
#endif 
