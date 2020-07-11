//------------------------------------------------------------------------------------------------------------
// Definiciones para el uso del API de conjugaciones
//------------------------------------------------------------------------------------------------------------

#ifndef __API_CONJVERB__
#define __API_CONJVERB__

#include "WinUtil.h"

//--------------------------------------------------------------------------
// Manipulador de diccionario
//--------------------------------------------------------------------------
#define HCONJ  LPVOID

/////////////////////////////////////////////////////////////////////////////
// enum que define los modos
/////////////////////////////////////////////////////////////////////////////

enum Modo{  mINFINITIVO,
            mINDICATIVO,
            mIMPERATIVO,
            mPOTENCIAL,
            mSUBJUNTIVO,
            mGERUNDIO,
            mPARTICIPIO,
            mSUBJUNTIVO_IMPERATIVO,
            mSUBJUNTIVO_IMPERATIVO_INDICATIVO,
            mIMPERATIVO_PARTICIPIO_INDICATIVO,
            mSUBJUNTIVO_IMPERATIVO_INDICATIVO_PARTICIPIO,
            mPARTICIPIO_INDICATIVO,
            mIMPERATIVO_INDICATIVO,
            mIMPERATIVO_INFINITIVO,
            mSUBJUNTIVO_INDICATIVO,
            mPOTENCIAL_INDICATIVO,
            mSUBJUNTIVO_PARTICIPIO,
            mMODO_FINAL };

/////////////////////////////////////////////////////////////////////////////
// enum que define los tiempos verbales
/////////////////////////////////////////////////////////////////////////////

enum Tiempo{PRESENTE,PASADO,FUTURO,PASADOIMP/* pasado imperfecto*/};

/////////////////////////////////////////////////////////////////////////////
// enum que define las personas
/////////////////////////////////////////////////////////////////////////////

enum Persona{PRIMERAP,SEGUNDAP,TERCERAP};

/////////////////////////////////////////////////////////////////////////////
// enum que define el género en que pueden estar los sust. o adj.
////////////////////////////////////////////0/////////////////////////////////

enum Genero{MASCULINO,FEMENINO,NEUTRO};

/////////////////////////////////////////////////////////////////////////////
// enum que define el número en que pueden estar las personas
////////////////////////////////////////////0/////////////////////////////////

enum Numero{SINGULAR,PLURAL};

/////////////////////////////////////////////////////////////////////////////
// enum que define el grado de comparación de los adjetivos
////////////////////////////////////////////0/////////////////////////////////

enum Grado{POSITIVO,COMPARATIVO,SUPERLATIVO};

/////////////////////////////////////////////////////////////////////////////
// enum que define el caso para la declinación
////////////////////////////////////////////0/////////////////////////////////

enum Declination{NODECLINE,NOMINATIVE,ACCUSATIVE,DATIVE,GENITIVE,VOCATIVE};

/////////////////////////////////////////////////////////////////////////////
// enum que define el tipo de artículo que acompaña al adjetivo
////////////////////////////////////////////0/////////////////////////////////

enum ArticleType{aNULL,aDEFINITE,aINDEFINITE};

/////////////////////////////////////////////////////////////////////////////
// enum que define los diccionarios y tablas de datos
////////////////////////////////////////////0/////////////////////////////////

enum DicName{ dcIrregVerb    , dcSuffixVerb , dcAuxVerb     , tbVerb,tbPattern,
              tbReference    , dcIrregFem   , dcSuffixFem   , dcIrregPlural,
              dcSuffixPlural , dcIrregSup   , dcIrregComp   , dcSuffixSup,
              dcSuffixComp   , dcPreffixVerb, dcIrregDecNoun,
              dcSuffixDecNoun, dcIrregDecAdj, dcSuffixDecAdj };

/////////////////////////////////////////////////////////////////////////////
// enum que define los diccionarios y tablas de datos
////////////////////////////////////////////0/////////////////////////////////
enum TableName{tabVerb,tabPattern};

/////////////////////////////////////////////////////////////////////////////
// enum para conjugación de verbos
/////////////////////////////////////////////////////////////////////////////

enum  _md{
          // Modo infinitivo
          md_Infinitivo = 0,                  // 0
          md_Gerundio,                        // 42
          md_ParticipioPasado,                // 43
          md_ParticipioPresente,              // 0

          // Modo indicativo    Tiempo presente  singular
          md_PresenteIndicativo1PerSing,      // 1
          md_PresenteIndicativo2PerSing1,     // 3
          md_PresenteIndicativo2PerSing2,     // 2
          md_PresenteIndicativo2PerSing3,     // 5
          md_PresenteIndicativo3PerSing,      // 3

          // Modo indicativo    Tiempo presente  plural
          md_PresenteIndicativo1PerPlural,    // 4
          md_PresenteIndicativo2PerPlural1,   // 6
          md_PresenteIndicativo2PerPlural2,   // 5
          md_PresenteIndicativo2PerPlural3,   // 5
          md_PresenteIndicativo3PerPlural,    // 6

          // Modo indicativo    Tiempo pasado  singular
          md_PasadoIndicativo1PerSing,        // 7
          md_PasadoIndicativo2PerSing1,       // 9
          md_PasadoIndicativo2PerSing2,       // 8
          md_PasadoIndicativo2PerSing3,       // 11
          md_PasadoIndicativo3PerSing,        // 9

          // Modo indicativo    Tiempo pasado  plural
          md_PasadoIndicativo1PerPlural,      // 10
          md_PasadoIndicativo2PerPlural1,     // 12
          md_PasadoIndicativo2PerPlural2,     // 11
          md_PasadoIndicativo2PerPlural3,     // 11
          md_PasadoIndicativo3PerPlural,      // 12


          // Modo indicativo    Tiempo pretérito imperfecto singular
          md_PasadoImpIndicativo1PerSing,        // 7
          md_PasadoImpIndicativo2PerSing1,       // 9
          md_PasadoImpIndicativo2PerSing2,       // 8
          md_PasadoImpIndicativo2PerSing3,       // 11
          md_PasadoImpIndicativo3PerSing,        // 9

          // Modo indicativo    Tiempo pretérito imperfecto plural
          md_PasadoImpIndicativo1PerPlural,      // 10
          md_PasadoImpIndicativo2PerPlural1,     // 12
          md_PasadoImpIndicativo2PerPlural2,     // 11
          md_PasadoImpIndicativo2PerPlural3,     // 11
          md_PasadoImpIndicativo3PerPlural,      // 12

          // Modo indicativo    Tiempo Futuro  singular
          md_FuturoIndicativo1PerSing,        // 13
          md_FuturoIndicativo2PerSing1,       // 15
          md_FuturoIndicativo2PerSing2,       // 14
          md_FuturoIndicativo2PerSing3,       // 17
          md_FuturoIndicativo3PerSing,        // 15

          // Modo indicativo    Tiempo Futuro  plural
          md_FuturoIndicativo1PerPlural,      // 16
          md_FuturoIndicativo2PerPlural1,     // 18
          md_FuturoIndicativo2PerPlural2,     // 17
          md_FuturoIndicativo2PerPlural3,     // 17
          md_FuturoIndicativo3PerPlural,      // 18

          // Modo Potencial singular
          md_Potencial1PerSing,               // 19
          md_Potencial2PerSing1,              // 21
          md_Potencial2PerSing2,              // 20
          md_Potencial2PerSing3,              // 23
          md_Potencial3PerSing,               // 21

          // Modo Potencial plural
          md_Potencial1PerPlural,             // 22
          md_Potencial2PerPlural1,            // 24
          md_Potencial2PerPlural2,            // 23
          md_Potencial2PerPlural3,            // 23
          md_Potencial3PerPlural,             // 24

          // Modo subjuntivo    Tiempo presente  singular
          md_PresenteSubjuntivo1PerSing,      // 25
          md_PresenteSubjuntivo2PerSing1,     // 27
          md_PresenteSubjuntivo2PerSing2,     // 26
          md_PresenteSubjuntivo2PerSing3,     // 29
          md_PresenteSubjuntivo3PerSing,      // 27

          // Modo Subjuntivo    Tiempo presente  plural
          md_PresenteSubjuntivo1PerPlural,    // 28
          md_PresenteSubjuntivo2PerPlural1,   // 30
          md_PresenteSubjuntivo2PerPlural2,   // 29
          md_PresenteSubjuntivo2PerPlural3,   // 29
          md_PresenteSubjuntivo3PerPlural,    // 30

          // Modo Subjuntivo    Tiempo pasado  singular
          md_PasadoSubjuntivo1PerSing,        // 31
          md_PasadoSubjuntivo2PerSing1,       // 33
          md_PasadoSubjuntivo2PerSing2,       // 32
          md_PasadoSubjuntivo2PerSing3,       // 35
          md_PasadoSubjuntivo3PerSing,        // 33

          // Modo Subjuntivo    Tiempo pasado  plural
          md_PasadoSubjuntivo1PerPlural,      // 34
          md_PasadoSubjuntivo2PerPlural1,     // 36
          md_PasadoSubjuntivo2PerPlural2,     // 35
          md_PasadoSubjuntivo2PerPlural3,     // 35
          md_PasadoSubjuntivo3PerPlural,      // 36

          // Modo Imperativo singular
          md_Imperativo2PerSing1,             // 38
          md_Imperativo2PerSing2,             // 37
          md_Imperativol2PerSing3,            // 40
          md_Imperativo3PerSing,              // 38

          // Modo Imperativo plural
          md_Imperativo1PerPlural1,           // 39
          md_Imperativo2PerPlural1,           // 41
          md_Imperativo2PerPlural2,           // 40
          md_Imperativo2PerPlural3,           // 40
          md_Imperativo3PerPlural,            // 41

          // Número de patrón de sufijo y prefijo de conjugación
          md_PatronSufPref

          // Verbo Auxiliar para los tiempos compuestos
          //	md_VerboAuxCompuesto
          };


DLLENTRY(HCONJ)  CVNew();
DLLENTRY(void)   CVFree( HCONJ hConj );

DLLENTRY(BOOL)   CVOpen( HCONJ hConj, LPCSTR sLang );
DLLENTRY(BOOL)   CVOpen( HCONJ hConj, int iLang );
DLLENTRY(void)   CVSetDataPath( HCONJ hConj, const CStringF& Path );

DLLENTRY(CString) CVConjugate( HCONJ hConj, const CString& verboi, int modo, int tiempo, int persona, int numero, BOOL negado, BOOL compuesto );
DLLENTRY(CString) CVConcordWords( HCONJ hConj, const CString& noun, int srcGen, int srcNum, int desGen, int desNum, int grado);
DLLENTRY(CString) CVConcordWordsIndex( HCONJ hConj, const CString& noun, int srcGen, int srcNum, int desGen, int desNum, int grado, LPWORD decCount, WORD idx);
DLLENTRY(CString) CVDeclineNoun( HCONJ hConj, const CString& noun, int gen, int num, int decl);
DLLENTRY(CString) CVDeclineNounIndex( HCONJ hConj, const CString& noun, int gen, int num, int decl, LPWORD decCount, WORD idx);
DLLENTRY(CString) CVDeclineAdjective( HCONJ hConj, const CString& adj, int gen, int num, int decl, int artic);

DLLENTRY(int)    CVCount(HCONJ hConj);
DLLENTRY(BOOL)   CVCompound(HCONJ hConj, WORD idx);
DLLENTRY(WORD)   CVMood(HCONJ hConj, WORD idx);
DLLENTRY(WORD)   CVTime1(HCONJ hConj, WORD idx);
DLLENTRY(LPVOID) CVGetPtrDicRef( HCONJ hConj, int name);


/*

/////////////////////////////////////////////////////////////////////////////
// Declaración de funciones exportables para conjugación de verbos
/////////////////////////////////////////////////////////////////////////////

DLLENTRY(BOOL) CVOpen( LPCSTR language);

DLLENTRY(BOOL) CVClose( LPCSTR language );

//---------------------------------------------------------------------------
// Funcion para el trabajo multiusuario
//---------------------------------------------------------------------------

DLLENTRY(DWORD) CVOpenUser();

DLLENTRY(void) CVCloseUser(DWORD idxUser);

DLLENTRY(LPCSTR) CVUserConjugaVerb( DWORD idxUser,
                                   LPCSTR language, LPCSTR verboi,Modo modo,
                                   Tiempo tiempo, Persona persona,
                                   Numero numero, BOOL negado, BOOL compuesto);

DLLENTRY(LPCSTR) CVUserConcordWords( DWORD idxUser, LPCSTR language, LPCSTR noun, Genero genfuente, Numero numfuente, Genero gendestino, Numero numdestino, Grado adjgrado);
DLLENTRY(LPCSTR) CVUserDeclineAdjective( DWORD idxUser, LPCSTR language, LPCSTR adj, Genero gen, Numero num, Declination decl, ArticleType artic);
DLLENTRY(LPCSTR) CVUserDeclineNoun( DWORD idxUser, LPCSTR language, LPCSTR noun, Genero gen, Numero num, Declination decl);
DLLENTRY(LPCSTR) CVUserAuxiliaryVerb( DWORD idxUser, LPCSTR language, LPCSTR verbo);
DLLENTRY(LPCSTR) CVUserDeclineNounIndex( DWORD idxUser, LPCSTR language, LPCSTR noun, Genero gen, Numero num, Declination decl, LPWORD decCount, WORD idx);
DLLENTRY(LPCSTR) CVUserConcordWordsIndex( DWORD idxUser, LPCSTR language, LPCSTR noun, Genero genfuente, Numero numfuente, Genero gendestino, Numero numdestino, Grado adjgrado, LPWORD decCount, WORD idx);
DLLENTRY(WORD)   CVDataConjCount(LPCSTR language);
DLLENTRY(WORD)   CVDataConjTime(LPCSTR language, WORD idx);
DLLENTRY(WORD)   CVDataConjMood(LPCSTR language, WORD idx);
DLLENTRY(WORD)   CVDataConjCompound(LPCSTR language, WORD idx);
DLLENTRY(LPVOID) CVGetPtrDicRef(LPCSTR language, DicName name);
DLLENTRY(void)   CVSetDataPath(LPCSTR dataPath);
DLLENTRY(LPCSTR) CVGetDataPath();

//---------------------------------------------------------------------------

DLLENTRY(LPCSTR) CVReflexivePronoun(LPCSTR language, Persona persona,Numero numero);
DLLENTRY(BOOL) CVReflexiveVerbRoot(LPCSTR language, LPCSTR RefVerb, LPCSTR RootVerb);
DLLENTRY(LPCSTR) CVPersonalPronoun(LPCSTR language, Persona persona,Numero numero);
DLLENTRY(LPCSTR) CVDataConjName(LPCSTR language, WORD idx);
DLLENTRY(LPCSTR) CVConjugaVerb( LPCSTR language, LPCSTR verbo, Modo modo,
                               Tiempo tiempo, Persona persona,
                               Numero numero, BOOL negado,BOOL compuesto);
DLLENTRY(LPCSTR) CVConcordWords( LPCSTR language, LPCSTR noun,
                                Genero genfuente, Numero numfuente,
                                Genero gendestino, Numero numdestino,
                                Grado adjgrado);
DLLENTRY(LPCSTR) CVAuxiliaryVerb( LPCSTR language, LPCSTR verbo);
DLLENTRY(BOOL) CVSetCelda(LPCSTR language, int fila, int col,LPCSTR s,TableName tbname);
DLLENTRY(LPCSTR) CVDeclineNoun( LPCSTR language, LPCSTR noun, Genero gen, Numero num, Declination decl);
DLLENTRY(LPCSTR) CVDeclineAdjective( LPCSTR language, LPCSTR noun, Genero gen, Numero num, Declination decl, ArticleType artic);
DLLENTRY(LPCSTR) CVConcordWordsIndex( LPCSTR language, LPCSTR noun,
                                     Genero genfuente, Numero numfuente,
                                     Genero gendestino, Numero numdestino,
                                     Grado adjgrado, LPWORD decCount, WORD idx);
DLLENTRY(LPCSTR) CVDeclineNounIndex( LPCSTR language, LPCSTR noun, Genero gen, Numero num, Declination decl, LPWORD decCount, WORD idx);
DLLENTRY(void) CVSetRegPath( LPCSTR RegPath );
DLLENTRY(BOOL) CVGetCelda(LPCSTR language, int fila, int col, LPSTR sval, TableName tbname);
DLLENTRY(BOOL) CVInsFila(LPCSTR language, int fila, LPCSTR namefila, TableName tbname);
DLLENTRY(BOOL) CVAddFila(LPCSTR language, LPCSTR namefila, TableName tbname);
DLLENTRY(BOOL) CVDelFila(LPCSTR language, int fila, TableName tbname);
DLLENTRY(BOOL) CVInsCol(LPCSTR language, int col, LPCSTR namecol, LPCSTR s, TableName tbname);
DLLENTRY(BOOL) CVAddCol(LPCSTR language, LPCSTR namecol, LPCSTR s, TableName tbname);
DLLENTRY(BOOL) CVDelCol(LPCSTR language, int col, TableName tbname);

*/
#endif
