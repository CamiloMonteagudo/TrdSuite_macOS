#pragma once

//------------------------------------------------------------------------------------
// Bit que define la caracteristica de cada caracter en el arreglo
// de definicion de caracteres.
//------------------------------------------------------------------------------------
#define  C_INI      1
#define  C_END      2
#define  C_NUM      4
#define  C_UP       8
#define  C_LOW      16
#define  C_ALFA     (C_LOW|C_UP)
#define  C_ALFANUM  (C_ALFA|C_NUM)

//------------------------------------------------------------------------------------
// Definción para los tipos de caracteres manejados por GetChar
//------------------------------------------------------------------------------------

// Caracter de sustitucion
//#define SUST_MARK '×'
#define SUST_MARK '\xD7'  //  ×

#define PACK(i,e,n,u,l) (i|(e<<1)|(n<<2)|(u<<3)|(l<<4) )

#define isc_ini(c)      (Keys[(BYTE)(c)] & C_INI)
#define isc_end(c)      (Keys[(BYTE)(c)] & C_END)
#define isc_num(c)      (Keys[(BYTE)(c)] & C_NUM)
#define isc_up(c)       (Keys[(BYTE)(c)] & C_UP)
#define isc_low(c)      (Keys[(BYTE)(c)] & C_LOW)
#define isc_alfa(c)     (Keys[(BYTE)(c)] & C_ALFA)
#define isc_alfanum(c)  (Keys[(BYTE)(c)] & C_ALFANUM)

/*------------------------------------------------------------------------------------*/
// Define un Item del parse de oraciones, estos item se caracterizan por su tipo, 
// por el texto que lo representa, por se traducción en el caso del tipo 't' y por la
// posición donde comienza su definición en el texto.
//<!----------------------------------------------------------------------------------->
class CItem: public CMFCObject
{ 
public:
  BYTE      m_Type;           // Tipo de información que el item representa
  CStringA  m_Text;           // Cadena de caracteres que componen el item
  CStringA  m_Trd;            // Tradución del item  (para los tipos 't' y 's')
  int       m_iSrc;           // Indice del inicio del item dentro del texto fuente
  int       m_iTrd;           // Indice del inicio del item dentro del texto traducido
  
  //CItem( LPCSTR s, BYTE t='t' ) : m_Text(s), m_Type(t) {}   
  CItem( const CStringA& s, BYTE t, int idx ) : m_Type(t), m_Text(s), m_iSrc(idx) {}
//  CItem() : m_Type('t') {}
  
  BOOL isText() { return m_Type == 't';}
};

/*------------------------------------------------------------------------------------*/
// Implementa el proseceso completo de separación de un texto en oraciones, hasta ahora
// se soportan textos con formatos TXT, RTF y HTML.
//<!----------------------------------------------------------------------------------->
class CParseText
  {
  public:
    CParseText(void);
    ~CParseText(void);

    CObArray  m_Items;       // Contiene todos los items analizados

    LPCSTR    m_Txt;          // Puntero al texto que se va a analizar
    int       m_len;          // Cantidad de caracteres del texto

    int       m_nNoTrd;       // Contador de macheo de las marcas de no traduccion
    CString   m_NTIni;        // Caracter(es) para marcar el inicio de las palabras que no se traducen
    CString   m_NTEnd;        // Caracter(es) para marcar el final de las palabras que no se traducen

    void SetText(const CStringA& Text);
    bool Parse();
    CStringA GetTrdText();

  private:
    // Variables actualizadas por GetChar
    int m_iVisible;     // Indice al ultimo caracter visible analizado
  
    void ClearSetting(void);

		char getChar          (int& _i);
    bool SkipNoText       (int& _i);
    bool GetTextOra       (int& _i);
    bool CheckNoTrdMark   (int& _i, CString* Ora);
    bool IsBullet         (int ini, int& i, CStringA& Cascara);
    bool isFile           (int& _i, CStringA& Ora);
    bool IsExt(int i);
    bool IsAbr(int i);
    void SplitByNoTrdMark(CString& Ora );


    bool AddItem(BYTE Type, const CStringA& Text, int cHiden );
    //bool AddItem(BYTE Type, LPCSTR Text);
  };
