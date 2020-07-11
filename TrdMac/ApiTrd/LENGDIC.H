////////////////////////////////////////////////////////////////////////////
//   Funciones LEX y YACC para lenguaje de diccionario                    //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se define class para lenguaje de diccionario                          // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(_LENGUAGE_DICTIONARY_INCLUDE_)
#define _LENGUAGE_DICTIONARY_INCLUDE_
#include "TranslationDataObject.h"
#include "SentenceDataObject.h"

////////////////////////////////////////////////////////////////////////////

typedef struct  _yystype
    {
    int     integer;
    CStringA string;
    CWord  *word;
    } YYSTYPE;
    

class CLengDic: public CTranslationDataObject, public CSentenceDataObject
    {
private:
    POSITION  _PtrNodo;			// puntero al nodo de la palabra en la lista
    CWord    *m_ptrWord;			// puntero a la palabra
    const char *_PtrIni;			// puntero al inicio del token que se analiza en m_Data
    int token;					// input token 
    int errct;					// error count 
    int errfl;					// error flag 
    int m_ColNo;				// Índice del carácter que se analiza      
    BOOL directo;				// si modo directo o dos pases

public:					// Variables para el yacc del lenguaje de diccionario
    
    //static CGrepNA *RE_PatDic; // RE para división en token
    CStringA yyerrtok;             // token que produjo el error en el texto
    CStringA m_TmpDato;
    YYSTYPE yyv[YYMAXDEPTH];    
    CStringA m_Trad;     // resultado de la traducción
    CStringA  m_Dato;     // Dato inicial para realizar la Traducción
    BOOL m_SyntaxChk;  // si chequeo de sintaxis o ejecución
    BOOL m_Error;      // si ocurrio error en el parsing 
    BOOL m_LlenaAcep;  // Si debe llenarse lista de acepciones con dato general
	YYSTYPE yylval, yyval;
    
    CLengDic(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL):CTranslationDataObject(ptrTranslationUser),CSentenceDataObject(ptrSentence)
      {
			directo = m_SyntaxChk = m_Error = m_LlenaAcep = FALSE;
			ASSERT(ptrTranslationUser != NULL);
			}
        
    virtual void Execute(POSITION nodo,CStringA &Trad,BOOL SintaxChk = FALSE,BOOL LlenaAcep = FALSE);
protected:
    virtual int  yylex();
    virtual int yyparse();
    virtual void yyerror(const char *s, const char *w);
    virtual const char *yydisplay(int ch);
	virtual LPCWORD GetPtrWord();
	virtual void SetPtrWord(LPCWORD ptrWord);
    };
#endif
////////////////////////////////////////////////////////////////////////////
