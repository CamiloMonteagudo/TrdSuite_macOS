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

#define  C_ANSI					0     // Caracter ANSI normal 
#define  C_RTF					1     // Caracter definido según RTF
#define  C_HTML					2     // Caracter definido según HTML
#define  C_HTML_UNICODE 3     // Caracter unicode definido según HTML

// Caracter de sustitucion
//#define SUST_MARK '×'
#define SUST_MARK '\xD7'  //  ×
// Caracter para separar items
#define ITEM_MARK '\xC5'  //  Å

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
  BYTE      m_Type;
  CStringA   m_Text;
  CStringA   m_Trd;
    
  //CItem( LPCSTR s, BYTE t='t' ) : m_Text(s), m_Type(t) {}   
  CItem( const CStringA& s, BYTE t='t' ) : m_Type(t), m_Text(s) {}   
  CItem() : m_Type('t') {}
  
  BOOL isText() { return m_Type == 't';}
};

/*------------------------------------------------------------------------------------*/
// Define los datos de los tags que estan en el interior de una oración
//<!----------------------------------------------------------------------------------->

#define MAX_INNTAGS 30
class CInnerTags
{
  int _n;                         // Número de tags actuales
  CString _Txt [MAX_INNTAGS];     // Texto de los tags internos
  int     _iOra[MAX_INNTAGS];     // Posición de los tag en la oración
  int     _iTxt[MAX_INNTAGS];     // Posicion de los tag en el texto

public:
  CInnerTags(void) {_n = 0;}

  void Add( CString& Ora, const CString& Txt, int iOra, int iTxt )
    { 
    if( _n>0 && _iOra[_n-1] == iOra-1 )           // Las marcas estan consecutivas
      {
      _Txt [_n-1] += Txt;
      return;
      }

    if( _n < MAX_INNTAGS )
      {
      _Txt [_n] = Txt;
      _iOra[_n] = iOra;
      _iTxt[_n] = iTxt;

      ++_n;

      Ora += SUST_MARK;                           // Pone un marcador a la oración
      }
    }

  void Copy( int nOld, int nNew )
    {
    ASSERT( nOld>=0 && nOld < _n && nNew>=0 && nNew < _n);

    _Txt [nOld] = _Txt [nNew];
    _iOra[nOld] = _iOra[nNew];
    _iTxt[nOld] = _iTxt[nNew];
    }

  void DeleteAt( int n )
    {
    ASSERT( n>=0 && n < _n );

    for( int i=n; i < _n-1; ++i )
      Copy( i, i+1 );

    --_n;
    }

  CString& GetTxt(int n) {ASSERT( n>=0 && n < _n ); return _Txt[n]; }
  int GetiOra(int n)     {ASSERT( n>=0 && n < _n ); return _iOra[n];}
  int GetiTxt(int n)     {ASSERT( n>=0 && n < _n ); return _iTxt[n];}
  int Len()              { return _n; }

  void SetTxt (int n, CString s) {ASSERT( n>=0 && n < _n ); _Txt[n]  = s;   }
  void SetiOra(int n, int iOra ) {ASSERT( n>=0 && n < _n ); _iOra[n] = iOra;}
  void SetiTxt(int n, int iTxt ) {ASSERT( n>=0 && n < _n ); _iTxt[n] = iTxt;}
  void SetLen ( int nNew       ) {_n = nNew; }
};

/*------------------------------------------------------------------------------------*/
// Implementa el proseceso completo de separación de un texto en oraciones, hasta ahora
// se soportan textos con formatos TXT, RTF y HTML.
//<!----------------------------------------------------------------------------------->
class CParseText
  {
  public:
    enum TAddItemMark { NO_ITEM_MARK, ADD_ITEM_MARK};
  
    CParseText(void);
    ~CParseText(void);

    CObArray  m_Items;       // Contiene todos los items analizados

		bool      m_isRtf;
    LPCSTR    m_Txt;          // Puntero al texto que se va a analizar
    int       m_len;          // Cantidad de caracteres del texto

    int       m_nNoTrd;       // Contador de macheo de las marcas de no traduccion
    CString m_NTIni;        // Caracter(es) para marcar el inicio de las palabras que no se traducen
    CString m_NTEnd;        // Caracter(es) para marcar el final de las palabras que no se traducen

    void SetText(const CStringA& Text, bool aIsRtf);
    bool Parse();
    CStringA GetTrdText(TAddItemMark aAddMark);

		bool isText() { return m_isRtf == false; }
		bool isRtf()  { return m_isRtf; }
		bool isHtml() { return false; }

  private:
    // Variables actualizadas por GetChar
    int     m_typeChar;     // Tipo de caracter, C_NORMAL, C_HTML, C_RTF
    int     m_ic;           // Indice al inicio del caracter analizado
    CString m_sChar;        // Representación del caracter especial
    int     m_nSp;          // Numero de &nbsp; seguidos
    int     m_iSp;          // Indice donde comienza el primer &nbsp;
    int     m_icmd;         // Indice del ultimo comando RTF analizado

    // Variables actualizadas por GetHtmlTag
    CString m_TagName;      // Nombre del ultimo tag analizado.
    CString m_TagValue;     // Valor del ultimo comando analizado.
    bool    m_TagEnd;       // Si el ultimo tag analizado es un terminador o no.
    bool    m_SkipAll;      // Si el ultimo tag analizado salto hasta el terminador o no.

    void ClearSetting(void);

		bool isCharText()    { return m_typeChar==C_ANSI; }
		bool isCharRtf()     { return m_typeChar==C_RTF;  }
		bool isCharHtml()    { return m_typeChar==C_HTML; }

		char getChar          (int& _i);
    char getCharRtf       (int& _i);
    bool getRtfTag        (int& _i, CString& Text);
    int  getInnerRtfCmd   (int& _i, CString& iCmd);
    bool SkipNoText       (int& _i);
    bool GetTextOra       (int& _i);
    bool CheckNoTrdMark   (int& _i, CString* Ora);
    bool IsBullet         (int ini, int& i, CStringA& Cascara);
    bool isFile           (int& _i, CStringA& Ora);
    bool IsExt(int i);
    bool IsAbr(int i);
    bool isInnerParagraf(LPCSTR Tag, LPCSTR Value);
    bool FindMatch        (int& _i, char cIni, char cEnd);
    void SplitByNoTrdMark(CString& Ora, bool NtIni, bool NtEnd, CInnerTags& ITags );
    bool GetBullet2( CString& Ora, CInnerTags& ITags );
    void FixApostrofes( CString& Ora, CInnerTags& ITags );
    void JoinSustMarks( CString& Ora, CInnerTags& ITags );
    void DelIniSustMarks( CString& Ora, CInnerTags& ITags );
    int  DelEndSustMark(int& i, CString& Ora, CInnerTags& ITags);
		void CheckEndOfRtfTab( CString &Txt );


    bool AddItem(BYTE Type, const CStringA& Text);
    //bool AddItem(BYTE Type, LPCSTR Text);
  };
