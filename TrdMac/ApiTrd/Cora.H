////////////////////////////////////////////////////////////////////////////
//   Class COracion                                                       //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         13/10/93                        //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se define class para el trabajo con la oración                        // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "CWord.h"
#include "DictLoad.h"

#if !defined(__CORA__)
#define __CORA__

//#define ACTIVEVIEW

    
#define FOREACH_WORD(SENT, ACTION) for(POSITION pNode = SENT->m_Ora.GetHeadPosition(); \
			  pNode != NULL; SENT->m_Ora.GetNext(pNode)) \
			{ LPCWORD ptrWord = (LPCWORD)(SENT)->m_Ora.GetAt(pNode); ACTION; }

#define FOREACH_WORD2(SENT, ACTION) for(POSITION pNode = SENT->m_Ora.GetHeadPosition(); \
			  pNode != NULL; SENT->m_Ora.GetNext(pNode)) \
			{ ACTION; }

enum InsertarSigno
  { 
  idSGNNINGUNO,
  idSGNINTERROGACION,
  idSGNEXCLAMACION
  };                    

#define FMT_TXT 0
#define FMT_RTF 1

class COracion : public CMFCObject
  {   

  DECLARE_SERIAL(COracion);

  public:
    CStringA   m_Orig;				// Oracion a traducir 
    CStringA   m_Dest;				// Oracion  Traducida 
    CStringA   m_Trace;				// Oracion  Traducida con la traza
    CStringA   m_EntreParentesis;
    CStrArray   m_WordsInSentence;  // Listado con todas las palabras de la oración en el momento que se determina la función gramatical
    CStrArray   m_WordsKeyInSentence;  // Listado con todas las palabras de la oración en el momento que se determina la función gramatical
    CStrArray	m_WordsTypeInSentence;  // Listado con todos los tipos gramaticales correspondiente a las palabras de la oración
    DWORD			m_SentenceNumber;		// Numero de la oracion dentro del arreglo de oraciones del texto
    CStringA			m_RtfIni;           // Marca de formato al inicio de la oracion
    CStringA			m_RtfFin;           // Marca de formato al fin de la oracion
    bool			m_Traducida;		// Si la oración ya se encuentra traducida por el memory tool

    //#if !defined(MERCADO)
    CStrArray   m_CmdInSentence;
    //#endif
    int       m_Format;				// Oración en formato texto, RTF, etc.
    int		  m_EntreQue;			// 0 entre nada, 1 entre parentesis, 2 entre comillas, 3 entre comillas simples, 4 entre corchetes

    BOOL      m_AllUpper;			// si toda la oracion está en mayúsculas
    bool      m_bAllFirstUpper;		// Si toda la oración tiene la primera letra de cada palabra en mayúsculas
    bool	  m_bInterrogative;		// Si la oración es interrogativa
    CCaso      m_Case;				// Si la oracion original esta 
    // en upper, lower o mixto 
  protected:                  
    WORD m_iCantWord;
    WORD m_iCantUppWord;
    WORD m_iCantLwrWord;

  public:                     

    CModo     m_Modo;       // Modo 
    CGenero   m_Genero;     // Genero 
    CNumero   m_Numero;     // Numero 
    CTiempo   m_Tiempo;     // Tiempo 
    CPersona  m_Persona;    // Persona 
    BOOL      m_FirstWordUpper;  // si primera palabra de la Ora. en mayúsculas
    BOOL      m_Voz;           //  TRUE ->Voz pasiva o FALSE ->activa 
    BOOL      m_PrimeraPasada; //  TRUE ->primera pasada FALSE segunda
    InsertarSigno m_InsSigno;  // Signo que se debe insertar a principio de oración

    CObList   m_Ora;        // lista de palabras de toda la oracion 

    POSITION  m_VerIni; // inicio de los verbos 
    POSITION  m_VerFin; // idem al fin 
    DWORD     m_TotalNGram;  // porciento de n-grams casados en la oracion traducida
    DWORD     m_MatchNGram;  // porciento de n-grams casados en la oracion traducida
    DWORD     m_PorcientoNGram;  // porciento de n-grams casados en la oracion traducida

  public:

    COracion(const CStringA& s,int formato = FMT_TXT):m_Orig(s)
      { 
      m_Format = formato;
      m_InsSigno = idSGNNINGUNO;
      m_EntreQue = -1;
			m_FirstWordUpper = FALSE;
			m_Voz = FALSE;
			m_PrimeraPasada = FALSE;

      m_iCantWord = 0;
      m_iCantUppWord = 0;
      m_iCantLwrWord = 0;
      m_AllUpper = FALSE;     // si toda la oracion está en mayúsculas
      m_bAllFirstUpper = false;
      m_bInterrogative = false;
      m_Traducida = false;
      m_WordsInSentence.RemoveAll();
      m_WordsTypeInSentence.RemoveAll();
      m_TotalNGram = 0;
      m_MatchNGram = 0;
      m_PorcientoNGram = 0;
      
      m_VerIni = NULL;
      m_VerFin = NULL;
      }

    COracion()
      { 
      m_Format = FMT_TXT;
      m_InsSigno = idSGNNINGUNO;
      m_EntreQue = 0;
			m_FirstWordUpper = FALSE;
			m_Voz = FALSE;
			m_PrimeraPasada = FALSE;

      m_iCantWord = 0;
      m_iCantUppWord = 0;
      m_iCantLwrWord = 0;
      m_AllUpper = FALSE;     // si toda la oracion está en mayúsculas
      m_Traducida = false;
      m_bInterrogative = false;
      m_WordsInSentence.RemoveAll();
      m_WordsTypeInSentence.RemoveAll();
      m_TotalNGram = 0;
      m_MatchNGram = 0;
      m_PorcientoNGram = 0;
      
      m_VerIni = NULL;
      m_VerFin = NULL;
      }

    COracion(COracion *ora)
      { 
      CopyOra(ora);
      }
    COracion(COracion &ora)
      { 
      CopyOra(ora);
      }

    ~COracion();

    virtual  COracion &CopyOra(COracion *ora)
      {
      return CopyOra(*ora);
      }
    virtual  COracion &CopyOra(COracion &ora)
      {
      m_Orig = ora.m_Orig;
      m_Dest = ora.m_Dest;
      m_Trace = ora.m_Trace;
      m_EntreParentesis = ora.m_EntreParentesis;
      for(int i = 0; i < ora.m_WordsInSentence.GetSize(); i++)
        {
        m_WordsInSentence.Add(ora.m_WordsInSentence.GetAt(i)); 
        }
      for(int i = 0; i < ora.m_WordsKeyInSentence.GetSize(); i++)
        {
        m_WordsKeyInSentence.Add(ora.m_WordsKeyInSentence.GetAt(i)); 
        }
      for(int i = 0; i < ora.m_WordsTypeInSentence.GetSize(); i++)
        {
        m_WordsTypeInSentence.Add(ora.m_WordsTypeInSentence.GetAt(i)); 
        }
      m_SentenceNumber = ora.m_SentenceNumber;
      m_RtfIni = ora.m_RtfIni;
      m_RtfFin = ora.m_RtfFin;
      m_Traducida = ora.m_Traducida;



#if !defined(MERCADO)
      for(int i = 0; i < ora.m_CmdInSentence.GetSize(); i++)
        {
        m_CmdInSentence.Add(ora.m_CmdInSentence.GetAt(i)); 
        }
#endif
      m_Format = ora.m_Format;
      m_EntreQue = ora.m_EntreQue;

      m_AllUpper = ora.m_AllUpper;
      m_bAllFirstUpper = ora.m_bAllFirstUpper;
      m_bInterrogative = ora.m_bInterrogative;
      m_Case = ora.m_Case;
      m_iCantWord = ora.m_iCantWord;
      m_iCantUppWord = ora.m_iCantUppWord;
      m_iCantLwrWord = ora.m_iCantLwrWord;
      m_Modo = ora.m_Modo;
      m_Genero = ora.m_Genero;
      m_Numero = ora.m_Numero;
      m_Tiempo = ora.m_Tiempo;
      m_Persona = ora.m_Persona; 
      m_FirstWordUpper = ora.m_FirstWordUpper;

      m_Voz = ora.m_Voz;
      m_PrimeraPasada = ora.m_PrimeraPasada;
      m_InsSigno = ora.m_InsSigno;

      for(POSITION pos = ora.m_Ora.GetHeadPosition(); pos != NULL; ora.m_Ora.GetNext(pos))
        {
        CWord *w = (CWord *)ora.m_Ora.GetAt(pos);
        m_Ora.AddTail(new CWord(w));
        }

      m_VerIni = ora.m_VerIni;
      m_VerFin = ora.m_VerFin;
      m_TotalNGram = ora.m_TotalNGram;
      m_MatchNGram = ora.m_MatchNGram;
      m_PorcientoNGram = ora.m_PorcientoNGram;
      return *this;
      }

#ifdef ACTIVEVIEW
		void View(int iStep);
#else
		void View(int /*iStep*/) { }
#endif

		/////////////////////////////////////////////////////////////////////////////
		// Obtiene la longitud de la oración
		/////////////////////////////////////////////////////////////////////////////
		int GetLength()
			{
			int numWords = 0;
			FOREACH_WORD2(this, numWords++);
			return numWords;
			}

    virtual  void Serialize(CArchive &ar);
    virtual  BOOL PoneXX();
    virtual  void QuitaXX();
    virtual  void ExpandeFrases();
    // Retorna el total de n-grams en la oracion
    virtual DWORD GetTotalNGram()
      {
      return m_TotalNGram;
      }
    // Retorna el numero de n-grams casados en la oracion
    virtual DWORD GetMatchNGram()
      {
      return m_MatchNGram;
      }
    // pone el numero de n-grams en la oracion
    virtual void SetTotalNGram(DWORD TotalNGram)
      {
      m_TotalNGram = TotalNGram;
      }
    // pone el numero de n-grams casados en la oracion
    virtual void SetMatchNGram(DWORD MatchNGram)
      {
      m_MatchNGram = MatchNGram;
      }
    virtual DWORD GetPorcientoNGram()
      {
      return m_PorcientoNGram;
      }
    virtual void SetPorcientoNGram(DWORD PorcientoNGram)
      {
      m_PorcientoNGram = PorcientoNGram;
      }
    /* virtual */ CWord *GetFirstWord();
    /* virtual */ CWord *GetLastWord();
    /* virtual */ OParte GetNextParte(POSITION n);
    /* virtual */ OParte GetLastParte(POSITION n);
    /* virtual */ void SetParte(POSITION ini,POSITION _fin,OParte p);
    /* virtual */ void SetLGen(POSITION ini,POSITION _fin,Genero p);
    /* virtual */ void SetLNum(POSITION ini,POSITION _fin,Numero p);
    /* virtual */ CWord *GetVerbo();
    /* virtual */ CWord *LastNotType(POSITION actual,const CStringA& s);
    /* virtual */ CWord *NextNotType(POSITION actual,const CStringA& s);
    /* virtual */ BOOL ExistLeft(POSITION actual,const CStringA& s,BOOL tipo=FALSE);
    /* virtual */ BOOL ExistRight(POSITION actual,const CStringA& s,BOOL tipo=FALSE);
    /* virtual */ void IsLastNotDDSetByType(POSITION actual,const CStringA& tipo,const CStringA& s,int morf=-1, const CStringA& ptrAdverbialType=CANull);
    /* virtual */ void IsLastNotDDSetByWord(POSITION actual,const CStringA& word,const CStringA& s,int morf=-1, const CStringA& ptrAdverbialType=CANull);
    /* virtual */ CWord *GetXNode(POSITION n,WORD x=1,BOOL to=TRUE);
    /* virtual */ POSITION GetXPosition(POSITION n,WORD x = 1,BOOL to=TRUE);
    /* virtual */ void SetWordTradMorf(CWord *w,const CStringA& s,int morf=-1);                
    /* virtual */ void DeleteArticulo();
    /* virtual */ void FindPatrones(CDictLoad *ptrDic = NULL); 
    /* virtual */ void IntercambiaPatron(POSITION nodoini,int l, LPCSTR patnewpos);
    /* virtual */ void EmptyWords();     
    void InsertWordAfter(POSITION ptrnodo, const CStringA& key, const CStringA& tipo);
    virtual void SetSentenceNumber(DWORD pos)
      {m_SentenceNumber = pos;}
    virtual DWORD GetSentenceNumber()
      {return m_SentenceNumber;}
  };
typedef COracion *LPCORACION;
#endif 
