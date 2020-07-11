//=========================================================================================================================================================
//  ConjCore.cpp
//  IdiomaXTranslator
//
//  Created by MacPC on 5/29/11.
//  Copyright 2011 IdiomaX. All rights reserved.
//
//=========================================================================================================================================================

#include "WinUtil.h"
//#include "ConjVerb.h"
#include "ConjCore.h"
#include "AppData.h"
//#import "ColAndFont.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
#define FindOpt    (NSCaseInsensitiveSearch|NSDiacriticInsensitiveSearch)

//=========================================================================================================================================================
@implementation ConjCore

static ConjData  _CData;
static ReducData _RedData;
CStringA         _DictPath;

float HMaxConj;
float WMaxConj;

CStringA ReduceWord(const CStringA& aVerbo, int Lang);

//void GetTimeConting( Tiempo Time, bool Comp, LPCSTR sMode, LPCSTR sTime );
bool IsVocal( CStringA &s, int idx );

static BOOL _IsVerb;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Carga la conjugación para un idioma determinado
+(bool) LoadConjLang:(int) lng
  {
  if( _CData.hConj != NULL )
    {
    if( _CData.cLang  == lng ) return true;
    CVFree( _CData.hConj );
    }
  
  HCONJ hConj  = CVNew();                                                   // Crea objeto para conjugar
  _CData.hConj = hConj;                                                     // Guarda objeto detro de los datos

  CVSetDataPath( hConj, _DictPath);                                         // Pone el camino a los diccionarios
  bool res = CVOpen( hConj, lng );                                          // Abre la conjugación
  if( res )                                                                 // La pudo abrir
    {
    _CData.cLang = lng;                                                     // Inicializa datos de la palabra
    _CData.Ok    = FALSE;
    
    switch( lng )                                                           // Obtiene conjugaciones de acuerdo al idioma
      {
      case 0: _CData.Conjs = EsConjData;                                    // Datos de las conjugaciones en ESPAÑOL
              _CData.Count = sizeof(EsConjData)/sizeof(cjData);             // Número de conjugaciones en español
              Strs = StrsEs;                                                // Cadenas de texto en español
              break;
      case 1: _CData.Conjs = EnConjData;                                    // Datos de las conjugaciones en INGLÉS
              _CData.Count = sizeof(EnConjData)/sizeof(cjData);             // Número de conjugaciones en ingles
              Strs = StrsEn;                                                // Cadenas de texto en inglés
              break;
      case 2: _CData.Conjs = ItConjData;                                    // Datos de las conjugaciones en ITALIANO
              _CData.Count = sizeof(ItConjData)/sizeof(cjData);             // Número de conjuaciones en italiano
              Strs = StrsIt;                                                // Cadenas de texto en italiano
              break;
      case 3: _CData.Conjs = NULL;                                          // Datos del ALEMÁN (no se usa)
              _CData.Count = 0;
              Strs = StrsEn;
              break;
      case 4: _CData.Conjs = FrConjData;                                    // Datos de las conjugaciones en FRANCÉS
              _CData.Count = sizeof(FrConjData)/sizeof(cjData);             // Número de conjugaciones en francés
              Strs = StrsFr;                                                // Cadenas de texto en francés
              break;
      }
    }

  return res;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
static CStringA pNoumPresnt[] = { "am "     , "are "    , "is "     , "are "    , "are "    , "are "     };
static CStringA pNoumPass[]   = { "was "    , "were "   , "was "    , "were "   , "were "   , "were "    };
static CStringA pNoumFuture[] = { "will be ", "will be ", "will be ", "will be ", "will be ", "will be " };

static CStringA* pNoumsAll[] = { pNoumPresnt, pNoumPass, pNoumFuture};
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene todas las conjugaciones de 'sVerb' en el idioma actual
+(bool) ConjVerb: (NSString *)sVerb
  {
  if( sVerb.length == 0 ) return FALSE;
  
  CStringA Verb = [sVerb cStringUsingEncoding:NSISOLatin1StringEncoding];   // Convierte la cadenas a C++
  
  Verb.MakeLower();                                                         // Convierte verbo a minusculas
  if( Verb.Left(3)=="to " ) Verb = Verb.Mid(3);                             // Le quita el to si lo tiene
  
  Verb = ReduceWord(Verb, _CData.cLang);                                    // Reduce las palabra
  
  if( Verb == _CData.Word  )                                                // Ya la palabra se conjugo
    return _CData.Ok;                                                       // No hace nada
    
  _CData.Word = Verb;
  
  _CData.Ok = FALSE;
  for( int i=0; i<_CData.Count; ++i )
    {
    cjData& cnj = _CData.Conjs[i];
    
    if( _CData.cLang == 1 && cnj.mode >= mMODO_FINAL )                      // Es un modo continuado del inglés
      {
      int iPer = (3*cnj.num) + cnj.Per;
      
      CStringA  Gerund      = _CData.Conjs[1].Conj;
      CStringA* pNoumsTime  = pNoumsAll[cnj.time];
      
      cnj.Conj = pNoumsTime[iPer] + Gerund;
      }
    else
      {
      cnj.Conj = CVConjugate( _CData.hConj,  Verb, cnj.mode, cnj.time, cnj.Per, cnj.num, FALSE, cnj.comp );
 
      if( cnj.Conj.Length()>0 && cnj.Conj != "-" )
        _CData.Ok = TRUE;
      }
    }
    
  return _CData.Ok;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si el verbo 'sVerb' se encuentra actualmente conjugado
+(BOOL) IsNowConjugate: (NSString *)sVerb Lang:(int) lng
  {
  _IsVerb = NO;
  if( lng<0 ) return FALSE;
  
  CStringA Verb = [sVerb cStringUsingEncoding:NSISOLatin1StringEncoding];   // Convierte la cadenas a C++
  
  Verb.MakeLower();                                                         // Convierte verbo a minusculas
  if( Verb.Left(3)=="to " ) Verb = Verb.Mid(3);                             // Le quita el to si lo tiene
  
  Verb = ReduceWord(Verb, lng);                                             // Reduce las palabra
  
  return (Verb==_CData.Word && lng==_CData.cLang);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(float) WMax
  {
  return WMaxConj;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(float) HMax
  {
  return HMaxConj;
  }

+(void) setHMax:(float) hMax
  {
  HMaxConj = hMax;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una lista de las conjugaciones en la lista 'lstData' ordenadas por la conjugacion
+(NSArray *) SortByConjList:(NSArray *) lstData
  {
  NSMutableArray* SortLst = [NSMutableArray new];
  
  NSString* lastCnj = @"";
  NSComparisonResult ret = 0;                         // Resultado de la comparación
  
  for( ConjAndTxt* Item in lstData )
    {
    NSString* sConj = Item.Conj;

    ret = [lastCnj compare:sConj options:FindOpt];    // Compara la palabra buscada con la actual

		if( ret == NSOrderedDescending )                  // Si es inferior a la última
      {
      for( int i=0; i<SortLst.count; ++i)
        {
        ConjAndTxt* Item2 = SortLst[i];
        ret = [sConj compare:Item2.Conj options:FindOpt];
        if( ret == NSOrderedAscending )
          {
          [SortLst insertObject:Item atIndex:i];
          break;
          }
        }
      }
    else
      {
      [SortLst addObject:Item];
      lastCnj = sConj;
      }
    }
  
  return SortLst;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una representación de las conjugaciones en formato html
+(ConjData &) conjData
  {
  return _CData;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el idioma de la conjugación actual
+(int) nowLang
  {
  return _CData.cLang;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pone la localización del los diccionarios
+(void) SetDictPath:(NSString*) path
  {
  _DictPath = (CStringA)[path cStringUsingEncoding:NSUTF8StringEncoding ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Abre la reducción para un idioma, si ya no esta abierta
bool OpenRedAPI( int lng )
  {
  if( _RedData.hRed != NULL && _RedData.rLang == lng  )     // Si esa reducción ya estaba abierta
    return true;                                            // Retorna sin hacer nada
    
  if( _RedData.hRed != NULL )                               // Si ya se habia creado una reducción anteriormente
    RWFree(_RedData.hRed);                                  // La libera
    
  HREDW hRed = RWNew() ;                                    // Crea una reducción nueva
  RWSetDataPath( hRed, _DictPath );                         // La pone el camino a los diccionarios
  
  if( !RWOpen( hRed, lng) )                                 // Trata de cargar todos sus diccionarios
    {
    RWFree( hRed );                                         // No pudo, libera el objeto reducción
    
    NSLog(@"No se pudo cargar la reduccón");
    return false;                                           // Retorna falso
    }
    
  _RedData.hRed  = hRed;                                    // Guarda la reducción
  _RedData.rLang = lng;                                     // Guarda el idioma de la reducción
  _RedData.Word  = "";                                      // Quita última palabra reducida
  _RedData.Root  = "";                                      // Quita útima raiz encontrada
  
  return true;                                              // Retorna OK
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si la palabra 'word' es un verbo en el idioma 'lng'
+(BOOL) IsVerbWord:(NSString*) word InLang:(int) lng
  {
  _IsVerb = NO;
  if( lng<0 || word.length==0) return FALSE;
  
  CStringA Verb = [word cStringUsingEncoding:NSISOLatin1StringEncoding];    // Convierte la cadenas a C++
  
  Verb.MakeLower();                                                         // Convierte verbo a minusculas
  if( Verb.Left(3)=="to " ) Verb = Verb.Mid(3);                             // Le quita el to si lo tiene
  
  ReduceWord(Verb, lng);                                                    // Reduce las palabra
  
  return _IsVerb;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
CStringA ReduceWord(const CStringA& aVerbo, int Lang)
  {
  _IsVerb = NO;
  if( !OpenRedAPI(Lang) ) return aVerbo;                    // Carga la reducción, si no puede, retorna la misma palabra

  if( _RedData.Word == aVerbo )                             // Mira si ya esa palabra se redujo
    {
    _IsVerb = YES;
    return _RedData.Root;                                   // Si es asi, retorna la raíz
    }
  
  HREDDATA Dat = RWReduce( _RedData.hRed, aVerbo );         // Reduce la palabra
  if( !Dat ) return aVerbo;                                 // No la pudo reducir, retorna la misma palabra
  
  int count = RWGetCount(Dat);                              // Obtiene la cantidad de reducciones
  
  for(int i=0; i<count; i++)                                // Recorre todas las reducciones
    {
    int nTipo = RWGetClassType(Dat, i);                     // Obtiene tipo de la redicción actual
    
    if( nTipo==rVERBO )                                     // Si es verbo
      {
      _IsVerb = YES;                                        // Pone bandera de reducción
      
      _RedData.Word = aVerbo;                               // Guarda el verbo reducido
      _RedData.Root = RWGetRoot(Dat,i);                     // Guarda la raiz del verbo reducido
      break;                                                // Termina la busqueda
      }
    }
      
  RWFreehData(Dat);                                         // Libera los datos de la reduccion
  
  return _IsVerb? _RedData.Root : aVerbo;                   // Si hubo reducción, retorna la raiz, sino la misma palabra
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

// ++++++++++++++++++++++++++++++++++++++  Definicion de los datos de las configuraciones por idioma +++++++++++++++++++++++++++++++++++++++++++++++++++++
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// ESPAÑOL
//--------------------------------------------------------------------------------------------------------------------------------------------------------
cjData EsConjData[] = {
  {mINFINITIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Infinitivo"                        , ""             },
  {mGERUNDIO,   PRESENTE , PRIMERAP, SINGULAR, false,  "Gerundio"                          , ""             },
  {mPARTICIPIO, PRESENTE , PRIMERAP, SINGULAR, false,  "Participio Pasado"                 , ""             },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Indicativo Presente"               , "yo"           },
  {mINDICATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Indicativo Presente"               , "t\372"        },
  {mINDICATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Indicativo Presente"               , "\351l"        },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Indicativo Presente"               , "nosotros"     },
  {mINDICATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Indicativo Presente"               , "vosotros"     },
  {mINDICATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Indicativo Presente"               , "ellos"        },
  
  {mINDICATIVO, PASADOIMP, PRIMERAP, SINGULAR, false,  "Indicativo Pret\351rito Imperfecto", "yo"           },
  {mINDICATIVO, PASADOIMP, SEGUNDAP, SINGULAR, false,  "Indicativo Pret\351rito Imperfecto", "t\372"        },
  {mINDICATIVO, PASADOIMP, TERCERAP, SINGULAR, false,  "Indicativo Pret\351rito Imperfecto", "\351l"        },
  
  {mINDICATIVO, PASADOIMP, PRIMERAP, PLURAL  , false,  "Indicativo Pret\351rito Imperfecto", "nosotros"     },
  {mINDICATIVO, PASADOIMP, SEGUNDAP, PLURAL  , false,  "Indicativo Pret\351rito Imperfecto", "vosotros"     },
  {mINDICATIVO, PASADOIMP, TERCERAP, PLURAL  , false,  "Indicativo Pret\351rito Imperfecto", "ellos"        },
  
  {mINDICATIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Indicativo Pret\351rito Indefinido", "yo"           },
  {mINDICATIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Indicativo Pret\351rito Indefinido", "t\372"        },
  {mINDICATIVO, PASADO   , TERCERAP, SINGULAR, false,  "Indicativo Pret\351rito Indefinido", "\351l"        },
  
  {mINDICATIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Indicativo Pret\351rito Indefinido", "nosotros"     },
  {mINDICATIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Indicativo Pret\351rito Indefinido", "vosotros"     },
  {mINDICATIVO, PASADO   , TERCERAP, PLURAL  , false,  "Indicativo Pret\351rito Indefinido", "ellos"        },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, SINGULAR, false,  "Indicativo Futuro Imperfecto"      , "yo"           },
  {mINDICATIVO, FUTURO   , SEGUNDAP, SINGULAR, false,  "Indicativo Futuro Imperfecto"      , "t\372"        },
  {mINDICATIVO, FUTURO   , TERCERAP, SINGULAR, false,  "Indicativo Futuro Imperfecto"      , "\351l"        },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, PLURAL  , false,  "Indicativo Futuro Imperfecto"      , "nosotros"     },
  {mINDICATIVO, FUTURO   , SEGUNDAP, PLURAL  , false,  "Indicativo Futuro Imperfecto"      , "vosotros"     },
  {mINDICATIVO, FUTURO   , TERCERAP, PLURAL  , false,  "Indicativo Futuro Imperfecto"      , "ellos"        },
  
  {mSUBJUNTIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Subjuntivo Presente"               , "que yo"       },
  {mSUBJUNTIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Subjuntivo Presente"               , "que t\372"    },
  {mSUBJUNTIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Subjuntivo Presente"               , "que \351l"    },
  
  {mSUBJUNTIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Subjuntivo Presente"               , "que nosotros" },
  {mSUBJUNTIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Subjuntivo Presente"               , "que vosotros" },
  {mSUBJUNTIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Subjuntivo Presente"               , "que ellos"    },
  
  {mSUBJUNTIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Subjuntivo Pret\351rito Imperfecto", "que yo"       },
  {mSUBJUNTIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Subjuntivo Pret\351rito Imperfecto", "que t\372"    },
  {mSUBJUNTIVO, PASADO   , TERCERAP, SINGULAR, false,  "Subjuntivo Pret\351rito Imperfecto", "que \351l"    },
  
  {mSUBJUNTIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Subjuntivo Pret\351rito Imperfecto", "que nosotros" },
  {mSUBJUNTIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Subjuntivo Pret\351rito Imperfecto", "que vosotros" },
  {mSUBJUNTIVO, PASADO   , TERCERAP, PLURAL  , false,  "Subjuntivo Pret\351rito Imperfecto", "que ellos"    },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, SINGULAR, false,  "Potencial Presente"                , "yo"           },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, SINGULAR, false,  "Potencial Presente"                , "t\372"        },
  {mPOTENCIAL , PRESENTE , TERCERAP, SINGULAR, false,  "Potencial Presente"                , "\351l"        },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, PLURAL  , false,  "Potencial Presente"                , "nosotros"     },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, PLURAL  , false,  "Potencial Presente"                , "vosotros"     },
  {mPOTENCIAL , PRESENTE , TERCERAP, PLURAL  , false,  "Potencial Presente"                , "ellos"        },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Imperativo"                        , ""             },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Imperativo"                        , ""             },
  };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// INGLÉS
//--------------------------------------------------------------------------------------------------------------------------------------------------------
cjData EnConjData[] = {
  {mINFINITIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Infinitive"                        , ""          },
  {mGERUNDIO,   PRESENTE , PRIMERAP, SINGULAR, false,  "Present Participle"                , ""          },
  {mPARTICIPIO, PRESENTE , PRIMERAP, SINGULAR, false,  "Past Participle"                   , ""          },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Indicative Present"                , "I"         },
  {mINDICATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Indicative Present"                , "you"       },
  {mINDICATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Indicative Present"                , "he/she/it" },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Indicative Present"                , "we"        },
  {mINDICATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Indicative Present"                , "you"       },
  {mINDICATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Indicative Present"                , "they"      },
  
  {mINDICATIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Indicative Past"                   , "I"         },
  {mINDICATIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Indicative Past"                   , "you"       },
  {mINDICATIVO, PASADO   , TERCERAP, SINGULAR, false,  "Indicative Past"                   , "he/she/it" },
  
  {mINDICATIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Indicative Past"                   , "we"        },
  {mINDICATIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Indicative Past"                   , "you"       },
  {mINDICATIVO, PASADO   , TERCERAP, PLURAL  , false,  "Indicative Past"                   , "they"      },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, SINGULAR, false,  "Indicative Future"                 , "I"         },
  {mINDICATIVO, FUTURO   , SEGUNDAP, SINGULAR, false,  "Indicative Future"                 , "you"       },
  {mINDICATIVO, FUTURO   , TERCERAP, SINGULAR, false,  "Indicative Future"                 , "he/she/it" },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, PLURAL  , false,  "Indicative Future"                 , "we"        },
  {mINDICATIVO, FUTURO   , SEGUNDAP, PLURAL  , false,  "Indicative Future"                 , "you"       },
  {mINDICATIVO, FUTURO   , TERCERAP, PLURAL  , false,  "Indicative Future"                 , "they"      },
  
  {mMODO_FINAL, PRESENTE , PRIMERAP, SINGULAR, false,  "Present Continuous"                , "I"         },
  {mMODO_FINAL, PRESENTE , SEGUNDAP, SINGULAR, false,  "Present Continuous"                , "you"       },
  {mMODO_FINAL, PRESENTE , TERCERAP, SINGULAR, false,  "Present Continuous"                , "he/she/it" },
  
  {mMODO_FINAL, PRESENTE , PRIMERAP, PLURAL  , false,  "Present Continuous"                , "we"        },
  {mMODO_FINAL, PRESENTE , SEGUNDAP, PLURAL  , false,  "Present Continuous"                , "you"       },
  {mMODO_FINAL, PRESENTE , TERCERAP, PLURAL  , false,  "Present Continuous"                , "they"      },

  {mMODO_FINAL, PASADO   , PRIMERAP, SINGULAR, false,  "Past Continuous"                   , "I"         },
  {mMODO_FINAL, PASADO   , SEGUNDAP, SINGULAR, false,  "Past Continuous"                   , "you"       },
  {mMODO_FINAL, PASADO   , TERCERAP, SINGULAR, false,  "Past Continuous"                   , "he/she/it" },
  
  {mMODO_FINAL, PASADO   , PRIMERAP, PLURAL  , false,  "Past Continuous"                   , "we"        },
  {mMODO_FINAL, PASADO   , SEGUNDAP, PLURAL  , false,  "Past Continuous"                   , "you"       },
  {mMODO_FINAL, PASADO   , TERCERAP, PLURAL  , false,  "Past Continuous"                   , "they"      },

  {mMODO_FINAL, FUTURO   , PRIMERAP, SINGULAR, false,  "Future Continuous"                 , "I"         },
  {mMODO_FINAL, FUTURO   , SEGUNDAP, SINGULAR, false,  "Future Continuous"                 , "you"       },
  {mMODO_FINAL, FUTURO   , TERCERAP, SINGULAR, false,  "Future Continuous"                 , "he/she/it" },
  
  {mMODO_FINAL, FUTURO   , PRIMERAP, PLURAL  , false,  "Future Continuous"                 , "we"        },
  {mMODO_FINAL, FUTURO   , SEGUNDAP, PLURAL  , false,  "Future Continuous"                 , "you"       },
  {mMODO_FINAL, FUTURO   , TERCERAP, PLURAL  , false,  "Future Continuous"                 , "they"      },

  {mPOTENCIAL , PRESENTE , PRIMERAP, SINGULAR, false,  "Conditional Present"               , "I"         },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, SINGULAR, false,  "Conditional Present"               , "you"       },
  {mPOTENCIAL , PRESENTE , TERCERAP, SINGULAR, false,  "Conditional Present"               , "he/she/it" },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, PLURAL  , false,  "Conditional Present"               , "we"        },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, PLURAL  , false,  "Conditional Present"               , "you"       },
  {mPOTENCIAL , PRESENTE , TERCERAP, PLURAL  , false,  "Conditional Present"               , "they"      },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Imperative"                        , ""          },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Imperative"                        , ""          },
  {mIMPERATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Imperative"                        , ""          },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Imperative"                        , ""          },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Imperative"                        , ""          },
  {mIMPERATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Imperative"                        , ""          },
  };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// ITALIANO
//--------------------------------------------------------------------------------------------------------------------------------------------------------
cjData ItConjData[] = {
  {mINFINITIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Infinito"                          , ""            },
  {mGERUNDIO,   PRESENTE , PRIMERAP, SINGULAR, false,  "Gerundio"                          , ""            },
  {mPARTICIPIO, PRESENTE , PRIMERAP, SINGULAR, false,  "Participio Passato"                , ""            },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Indicativo Presente"               , "io"          },
  {mINDICATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Indicativo Presente"               , "tu"          },
  {mINDICATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Indicativo Presente"               , "lui/lei"     },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Indicativo Presente"               , "noi"         },
  {mINDICATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Indicativo Presente"               , "voi"         },
  {mINDICATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Indicativo Presente"               , "loro"        },
  
  {mINDICATIVO, PASADOIMP, PRIMERAP, SINGULAR, false,  "Indicativo Imperfetto"             , "io"          },
  {mINDICATIVO, PASADOIMP, SEGUNDAP, SINGULAR, false,  "Indicativo Imperfetto"             , "tu"          },
  {mINDICATIVO, PASADOIMP, TERCERAP, SINGULAR, false,  "Indicativo Imperfetto"             , "lui/lei"     },
  
  {mINDICATIVO, PASADOIMP, PRIMERAP, PLURAL  , false,  "Indicativo Imperfetto"             , "noi"         },
  {mINDICATIVO, PASADOIMP, SEGUNDAP, PLURAL  , false,  "Indicativo Imperfetto"             , "voi"         },
  {mINDICATIVO, PASADOIMP, TERCERAP, PLURAL  , false,  "Indicativo Imperfetto"             , "loro"        },
  
  {mINDICATIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Indicativo Passato Remoto"         , "io"          },
  {mINDICATIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Indicativo Passato Remoto"         , "tu"          },
  {mINDICATIVO, PASADO   , TERCERAP, SINGULAR, false,  "Indicativo Passato Remoto"         , "lui/lei"     },
  
  {mINDICATIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Indicativo Passato Remoto"         , "noi"         },
  {mINDICATIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Indicativo Passato Remoto"         , "voi"         },
  {mINDICATIVO, PASADO   , TERCERAP, PLURAL  , false,  "Indicativo Passato Remoto"         , "loro"        },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, SINGULAR, false,  "Indicativo Futuro Semplice"        , "io"          },
  {mINDICATIVO, FUTURO   , SEGUNDAP, SINGULAR, false,  "Indicativo Futuro Semplice"        , "tu"          },
  {mINDICATIVO, FUTURO   , TERCERAP, SINGULAR, false,  "Indicativo Futuro Semplice"        , "lui/lei"     },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, PLURAL  , false,  "Indicativo Futuro Semplice"        , "noi"         },
  {mINDICATIVO, FUTURO   , SEGUNDAP, PLURAL  , false,  "Indicativo Futuro Semplice"        , "voi"         },
  {mINDICATIVO, FUTURO   , TERCERAP, PLURAL  , false,  "Indicativo Futuro Semplice"        , "loro"        },
  
  {mSUBJUNTIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Congiuntivo Presente"              , "che io"      },
  {mSUBJUNTIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Congiuntivo Presente"              , "che tu"      },
  {mSUBJUNTIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Congiuntivo Presente"              , "che lui/lei" },
  
  {mSUBJUNTIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Congiuntivo Presente"              , "che noi"     },
  {mSUBJUNTIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Congiuntivo Presente"              , "che voi"     },
  {mSUBJUNTIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Congiuntivo Presente"              , "che loro"    },
  
  {mSUBJUNTIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Congiuntivo Imperfetto"            , "che io"      },
  {mSUBJUNTIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Congiuntivo Imperfetto"            , "che tu"      },
  {mSUBJUNTIVO, PASADO   , TERCERAP, SINGULAR, false,  "Congiuntivo Imperfetto"            , "che lui/lei" },
  
  {mSUBJUNTIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Congiuntivo Imperfetto"            , "che noi"     },
  {mSUBJUNTIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Congiuntivo Imperfetto"            , "che voi"     },
  {mSUBJUNTIVO, PASADO   , TERCERAP, PLURAL  , false,  "Congiuntivo Imperfetto"            , "che loro"    },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, SINGULAR, false,  "Condizionale Presente"             , "io"          },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, SINGULAR, false,  "Condizionale Presente"             , "tu"          },
  {mPOTENCIAL , PRESENTE , TERCERAP, SINGULAR, false,  "Condizionale Presente"             , "lui/lei"     },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, PLURAL  , false,  "Condizionale Presente"             , "noi"         },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, PLURAL  , false,  "Condizionale Presente"             , "voi"         },
  {mPOTENCIAL , PRESENTE , TERCERAP, PLURAL  , false,  "Condizionale Presente"             , "loro"        },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Imperativo"                        , ""             },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Imperativo"                        , ""             },
  {mIMPERATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Imperativo"                        , ""             },
  };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// FRANCÉS
//--------------------------------------------------------------------------------------------------------------------------------------------------------
cjData FrConjData[] = {
  {mINFINITIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Infinitif"                       , ""              },
  {mGERUNDIO,   PRESENTE , PRIMERAP, SINGULAR, false,  "Participe Pr\351sent"            , ""              },
  {mPARTICIPIO, PRESENTE , PRIMERAP, SINGULAR, false,  "Participe Pass\351"              , ""              },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Indicatif Pr\351sent"            , "je"            },
  {mINDICATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Indicatif Pr\351sent"            , "tu"            },
  {mINDICATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Indicatif Pr\351sent"            , "il/elle"       },
  
  {mINDICATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Indicatif Pr\351sent"            , "nous"          },
  {mINDICATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Indicatif Pr\351sent"            , "vous"          },
  {mINDICATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Indicatif Pr\351sent"            , "ils/elles"     },
  
  {mINDICATIVO, PASADOIMP, PRIMERAP, SINGULAR, false,  "Indicatif Imparfait"             , "je"            },
  {mINDICATIVO, PASADOIMP, SEGUNDAP, SINGULAR, false,  "Indicatif Imparfait"             , "tu"            },
  {mINDICATIVO, PASADOIMP, TERCERAP, SINGULAR, false,  "Indicatif Imparfait"             , "il/elle"       },
  
  {mINDICATIVO, PASADOIMP, PRIMERAP, PLURAL  , false,  "Indicatif Imparfait"             , "nous"          },
  {mINDICATIVO, PASADOIMP, SEGUNDAP, PLURAL  , false,  "Indicatif Imparfait"             , "vous"          },
  {mINDICATIVO, PASADOIMP, TERCERAP, PLURAL  , false,  "Indicatif Imparfait"             , "ils/elles"     },
  
  {mINDICATIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Indicatif Pass\351 simple"       , "je"            },
  {mINDICATIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Indicatif Pass\351 simple"       , "tu"            },
  {mINDICATIVO, PASADO   , TERCERAP, SINGULAR, false,  "Indicatif Pass\351 simple"       , "il/elle"       },
  
  {mINDICATIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Indicatif Pass\351 simple"       , "nous"          },
  {mINDICATIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Indicatif Pass\351 simple"       , "vous"          },
  {mINDICATIVO, PASADO   , TERCERAP, PLURAL  , false,  "Indicatif Pass\351 simple"       , "ils/elles"     },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, SINGULAR, false,  "Indicativo Futuro Semplice"      , "je"            },
  {mINDICATIVO, FUTURO   , SEGUNDAP, SINGULAR, false,  "Indicativo Futuro Semplice"      , "tu"            },
  {mINDICATIVO, FUTURO   , TERCERAP, SINGULAR, false,  "Indicativo Futuro Semplice"      , "il/elle"       },
  
  {mINDICATIVO, FUTURO   , PRIMERAP, PLURAL  , false,  "Indicativo Futuro Semplice"      , "nous"          },
  {mINDICATIVO, FUTURO   , SEGUNDAP, PLURAL  , false,  "Indicativo Futuro Semplice"      , "vous"          },
  {mINDICATIVO, FUTURO   , TERCERAP, PLURAL  , false,  "Indicativo Futuro Semplice"      , "ils/elles"     },
  
  {mSUBJUNTIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Subjonctif Pr\351sent"           , "que je"        },
  {mSUBJUNTIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Subjonctif Pr\351sent"           , "que tu"        },
  {mSUBJUNTIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Subjonctif Pr\351sent"           , "que il/elle"   },
  
  {mSUBJUNTIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Subjonctif Pr\351sent"           , "que nous"      },
  {mSUBJUNTIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Subjonctif Pr\351sent"           , "que vous"      },
  {mSUBJUNTIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Subjonctif Pr\351sent"           , "que ils/elles" },
  
  {mSUBJUNTIVO, PASADO   , PRIMERAP, SINGULAR, false,  "Subjonctif Imparfait"            , "que je"        },
  {mSUBJUNTIVO, PASADO   , SEGUNDAP, SINGULAR, false,  "Subjonctif Imparfait"            , "que tu"        },
  {mSUBJUNTIVO, PASADO   , TERCERAP, SINGULAR, false,  "Subjonctif Imparfait"            , "que il/elle"   },
  
  {mSUBJUNTIVO, PASADO   , PRIMERAP, PLURAL  , false,  "Subjonctif Imparfait"            , "que nous"      },
  {mSUBJUNTIVO, PASADO   , SEGUNDAP, PLURAL  , false,  "Subjonctif Imparfait"            , "que vous"      },
  {mSUBJUNTIVO, PASADO   , TERCERAP, PLURAL  , false,  "Subjonctif Imparfait"            , "que ils/elles" },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, SINGULAR, false,  "Conditionnel Pr\351sent"         , "je"            },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, SINGULAR, false,  "Conditionnel Pr\351sent"         , "tu"            },
  {mPOTENCIAL , PRESENTE , TERCERAP, SINGULAR, false,  "Conditionnel Pr\351sent"         , "il/elle"       },
  
  {mPOTENCIAL , PRESENTE , PRIMERAP, PLURAL  , false,  "Conditionnel Pr\351sent"         , "nous"          },
  {mPOTENCIAL , PRESENTE , SEGUNDAP, PLURAL  , false,  "Conditionnel Pr\351sent"         , "vous"          },
  {mPOTENCIAL , PRESENTE , TERCERAP, PLURAL  , false,  "Conditionnel Pr\351sent"         , "ils/elles"     },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, SINGULAR, false,  "Imperatif"                       , ""              },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, SINGULAR, false,  "Imperatif"                       , ""              },
  {mIMPERATIVO, PRESENTE , TERCERAP, SINGULAR, false,  "Imperatif"                       , ""              },
  
  {mIMPERATIVO, PRESENTE , PRIMERAP, PLURAL  , false,  "Imperatif"                       , ""              },
  {mIMPERATIVO, PRESENTE , SEGUNDAP, PLURAL  , false,  "Imperatif"                       , ""              },
  {mIMPERATIVO, PRESENTE , TERCERAP, PLURAL  , false,  "Imperatif"                       , ""              },
  };

//  if( IsVocal( _CData.Word, 0 ) ) { p1[0] = "j'"; p2[0] = "que j'"; }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si el caracter 'idx' dentro de 's' es una vocal
//--------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsVocal( CStringA &s, int idx )
  {
  static CStringA vocales = "aeiouAEIOUáéíóúÁÉÍÓÚ";
  
  if( s.Length()<=idx  ) return false;
  
  return ( vocales.Find(s[idx])!= -1 );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static cjStrLoc Strs;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
cjStrLoc StrsEs = {
    /*PersNum1[6]*/  {@"1ra  sg. ", @"2da sg. ", @"3ra  sg. ",@"1ra   pl. ", @"2da  pl. ", @"3ra   pl. "},
    /*PersNum2[6]*/  {@"1ra Persona Singular ", @"2da Persona Singular ", @"3ra Persona Singular ",@"1ra Persona Plurar ", @"2da Persona Plurar ", @"3ra Persona Plurar "},
    /*ProNoums[6]*/  {@" (Yo)", @" (Tú)", @" (Él/Ella)",@" (Nosotros)", @" (Vosotros)", @" (Ellos/Ellas)" },
  
       /*Nums3[2]*/  {@"Singular",@"Plural"},
  
  
        /*Gens[3]*/  {@"Masculino",@"Femenino",@"Neutro"},
       /*Tipos[6]*/  {@"Verbo",@"Adjetivo",@"Sustantivo",@"Pronombre",@"Adverbio",@"Artíclulo"},
  
           /*From*/  @"De ",
           /*Mode*/  @"Verbo"
                  };

cjStrLoc StrsEn = {
    /*PersNum1[6]*/  {@"1ra  sg. ", @"2da sg. ", @"3ra  sg. ",@"1ra   pl. ", @"2da  pl. ", @"3ra   pl. "},
    /*PersNum2[6]*/  {@"First Person Singular ", @"Second Person Singular ", @"Third Person Singular ",@"First Person Plurar ", @"Second Person Plurar ", @"Third Person Plurar "},
    /*ProNoums[6]*/  {@" (I)", @" (You)", @" (He/She/It)",@" (We)", @" (You)", @" (They)" },
  
       /*Nums3[2]*/  {@"Singular",@"Plural"},
  
        /*Gens[3]*/  {@"Masculine",@"Femenine",@"Neutral"},
       /*Tipos[6]*/  {@"Verb",@"Adjective",@"Substantive",@"Pronoun",@"Adverb",@"Article"},
  
           /*From*/  @"From ",
           /*Mode*/  @"Verb"
                  };

cjStrLoc StrsFr = {
    /*PersNum1[6]*/  {@"1ra  sg. ", @"2da sg. ", @"3ra  sg. ",@"1ra   pl. ", @"2da  pl. ", @"3ra   pl. "},
    /*PersNum2[6]*/  {@"1ra Personne Singulier ", @"2da Personne Singulier ", @"3ra Personne Singulier ",@"1ra Personne Pluriel ", @"2da Personne Pluriel ", @"3ra Personne Pluriel "},
    /*ProNoums[6]*/  {@" (Je)", @" (Tu)", @" (Il/Elle)",@" (Nous)", @" (Vous)", @" (Ils/Elles)" },
  
       /*Nums3[2]*/  {@"Singulier",@"Pluriel"},
  
        /*Gens[3]*/  {@"Masculin",@"Féminin",@"Neutre"},
       /*Tipos[6]*/  {@"Verbe",@"Adjectif",@"Substantif",@"Pronom",@"Adverbe",@"Article"},
  
           /*From*/  @"De ",
           /*Mode*/  @"Verbe"
                  };

cjStrLoc StrsIt = {
    /*PersNum1[6]*/  {@"1ra  sg. ", @"2da sg. ", @"3ra  sg. ",@"1ra   pl. ", @"2da  pl. ", @"3ra   pl. "},
    /*PersNum2[6]*/  {@"1ra Persona Singolare ", @"2da Persona Singolare ", @"3ra Persona Singolare ",@"1ra Persona Plurale ", @"2da Persona Plurale ", @"3ra Persona Plurale "},
    /*ProNoums[6]*/  {@" (Io)", @" (Tu)", @" (Lui/Lei)",@" (Noi)", @" (Voi)", @" (Loro)" },
  
       /*Nums3[2]*/  {@"Singolare",@"Plurale"},
  
        /*Gens[3]*/  {@"Maschile",@"Femminile",@"Neutro"},
       /*Tipos[6]*/  {@"Verbo",@"Aggettivo",@"Sostantivo",@"Pronome",@"Avverbio",@"Articolo"},
  
           /*From*/  @"Da ",
           /*Mode*/  @"Verbo"
                  };

//-----------------------------------------------------------------------------------------------------------------------------------------------
static void AddLPCSTR( NSMutableAttributedString* Str, LPCSTR cStr,  NSDictionary* attrDict )
  {
  NSString* mStr = [NSString stringWithCString:cStr encoding:NSISOLatin1StringEncoding ];
  
  [Str appendAttributedString: [[NSMutableAttributedString alloc] initWithString:mStr attributes:attrDict] ];
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------
static void AddNSString( NSMutableAttributedString* Str, NSString* mStr,  NSDictionary* attrDict )
  {
  [Str appendAttributedString: [[NSMutableAttributedString alloc] initWithString:mStr attributes:attrDict] ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una lista con los datos y textos de las conjugaciones sin ordenar
+(NSArray *) GetConjsByWord
  {
  NSMutableArray* List = [NSMutableArray new];
  
  HMaxConj = 0;
  WMaxConj = 0;

  for( int i=3; i<_CData.Count; ++i )
    {
    NSAttributedString* Txt = [ConjCore TextFromConj:i TwoLines:YES];
    
    ConjAndTxt* obj = [ConjAndTxt ObjWithConj:i AndText:Txt];
    if( obj.Conj.length == 0 || [obj.Conj isEqualToString:@"-"] )
      continue;
    
    [List addObject: obj];
    
    if( WMaxConj < Txt.size.width  ) WMaxConj = Txt.size.width ;
    if( HMaxConj < Txt.size.height ) HMaxConj = Txt.size.height+5;
    }
  
  return List;
  }
  
//--------------------------------------------------------------------------------------------------------------------------------------------------------
static CGFloat FontSize = [NSFont systemFontSize]+2;

static NSFont* fontMeanBold        = [NSFont boldSystemFontOfSize: FontSize];
static NSFont* fontConjBold        = [NSFont boldSystemFontOfSize: FontSize];
static NSFont* fontConjSmallItalic = [NSFont systemFontOfSize: 0.75 *FontSize];
static NSFont* fontConjBoldMid     = [NSFont boldSystemFontOfSize: 0.9*FontSize];

static NSColor* ColMean     = [NSColor blackColor];                                                             // Color para el cuerpo del significado
static NSColor* colConj     = [NSColor blackColor];
static NSColor* colConjMode = [NSColor colorWithRed:0.06 green:0.43 blue:0.06 alpha:1.00];
static NSColor* colConjPers = [NSColor colorWithRed:0.06 green:0.06 blue:0.43 alpha:1.00];
static NSColor* colConjGray = [NSColor darkGrayColor];
static NSColor* ColErrInfo  = [NSColor colorWithRed:0.6 green:0.2 blue:0.2 alpha:1.00];                        // Color para mostrar infomación de errores

static NSDictionary* attrKey   = @{ NSFontAttributeName:fontMeanBold , NSForegroundColorAttributeName:ColMean };

static NSDictionary* attrModeBold        = @{ NSFontAttributeName:fontConjBold        , NSForegroundColorAttributeName:colConjMode };
static NSDictionary* attrModeBoldMid     = @{ NSFontAttributeName:fontConjBoldMid     , NSForegroundColorAttributeName:colConjMode };
static NSDictionary* attrModeSmall       = @{ NSFontAttributeName:fontConjSmallItalic , NSForegroundColorAttributeName:colConjMode };

static NSDictionary* attrConjBold        = @{ NSFontAttributeName:fontConjBold        , NSForegroundColorAttributeName:colConj     };
static NSDictionary* attrConjBoldMid     = @{ NSFontAttributeName:fontConjBoldMid     , NSForegroundColorAttributeName:colConj     };

static NSDictionary* attrPersBold        = @{ NSFontAttributeName:fontConjBold        , NSForegroundColorAttributeName:colConjPers };
static NSDictionary* attrPersItalicSmall = @{ NSFontAttributeName:fontConjSmallItalic , NSForegroundColorAttributeName:colConjPers };

static NSDictionary* attrConjGray = @{ NSFontAttributeName:fontConjSmallItalic, NSForegroundColorAttributeName:colConjGray };
static NSDictionary* attrErr = @{ NSFontAttributeName:fontConjBoldMid, NSForegroundColorAttributeName:ColErrInfo  };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//
+(NSAttributedString*) TextFromConj:(int) idx TwoLines:(BOOL) two
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];

  cjData& cnj = _CData.Conjs[idx];
  
  AddLPCSTR(Str, cnj.Conj.c_str(), attrConjBold);
  
  AddNSString(Str, @" ("     , attrModeSmall);
  AddLPCSTR  (Str, cnj.sMode , attrModeSmall);
  
  if( two ) AddNSString(Str, @")\r\n", attrModeSmall);
  else      AddNSString(Str, @") "   , attrModeSmall);
  
  if( idx<3 ) return Str;                                           // Infinitivo, gerundio y participio, no llevan pronombre ni persona
  
  int i = (3*cnj.num) + cnj.Per;
  AddNSString(Str, Strs.PersNum2[i], attrPersItalicSmall);

  if( strlen( cnj.Pronm ) > 0  )
    {
    AddNSString(Str, @" ("    , attrConjGray);
    AddLPCSTR  (Str, cnj.Pronm, attrConjGray);
    AddNSString(Str, @")"     , attrConjGray);
    }
  
  
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una lista con los datos de las conjugaciones agrupadas por modo y tiempo
+(NSArray *) GetConjsByMode
  {
  NSMutableArray* List = [NSMutableArray new];
  
  HMaxConj = 0;
  WMaxConj = 0;

//  NSAttributedString* TxtHrd = [ConjCore GroupHeader];
//  
//  ConjAndTxt* obj = [ConjAndTxt ObjWithText:TxtHrd];
//    
//  [List addObject: obj];
  
  for( int i=3; i<_CData.Count; i += 6 )
    {
    NSAttributedString* Txt = [ConjCore GroupModeTimeFrom:i];
    
    ConjAndTxt* obj = [ConjAndTxt ObjWithText:Txt];
    
    [List addObject: obj];
    }

  return List;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(NSAttributedString*) GroupModeTimeFrom:(int) idx
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];

  for( int i=0; i<6; ++i )
    {
    cjData cnj = _CData.Conjs[ idx+i ];
    if( i==0 )
      {
      AddLPCSTR  (Str, cnj.sMode , attrModeBold);
      AddNSString(Str, @"\r\n"   , attrModeBold);
      }

    AddNSString(Str, Strs.PersNum1[i], attrPersItalicSmall);
    
    AddLPCSTR  (Str, cnj.Conj.c_str(), attrConjBoldMid);
    
    if( strlen( cnj.Pronm ) > 0  )
      {
      AddNSString(Str, @"  ("    , attrConjGray);
      AddLPCSTR  (Str, cnj.Pronm , attrConjGray);
      AddNSString(Str, @")\r\n"  , attrConjGray);
      }
    else
      AddNSString(Str, @"\r\n" , attrConjBoldMid);
    }
    
  if( WMaxConj < Str.size.width  ) WMaxConj = Str.size.width ;
  if( HMaxConj < Str.size.height ) HMaxConj = Str.size.height;
    
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una lista con los datos de las conjugaciones agrupadas por persona y numero
+(NSArray *) GetConjsByPersons
  {
  NSMutableArray* List = [NSMutableArray new];
  
  HMaxConj = 0;
  WMaxConj = 0;

  for( int i=0; i<6; ++i )
    {
    NSAttributedString* Txt = [ConjCore GroupNumAndPerson:i];
    
    ConjAndTxt* obj = [ConjAndTxt ObjWithText:Txt];
    
    [List addObject: obj];
    }

  return List;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(NSAttributedString*) GroupNumAndPerson:(int) idx
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];
  if( idx+3 >= _CData.Count ) return Str;

  AddNSString(Str, Strs.PersNum2[idx], attrPersBold );
  AddNSString(Str, Strs.ProNoums[idx], attrConjGray );
  AddNSString(Str, @"\r\n"           , attrPersBold );
  
  for( int i=idx+3; i<_CData.Count; i+=6 )
    {
    cjData cnj = _CData.Conjs[i];
    
    CStringA sConj = cnj.Conj;
    
    if( sConj.Length()==0 || sConj=='-' )
      {
      AddLPCSTR  (Str, sConj.c_str(), attrConjBoldMid);
    
      AddNSString(Str, @" ("    , attrModeSmall);
      AddLPCSTR  (Str, cnj.sMode, attrModeSmall);
      AddNSString(Str, @")\r\n" , attrModeSmall);
      }
    }
    
  if( WMaxConj < Str.size.width  ) WMaxConj = Str.size.width ;
  if( HMaxConj < Str.size.height ) HMaxConj = Str.size.height;
    
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(NSAttributedString*) GetConjHeaderInMode:(int) Mode
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];

  for( int i=0; i<3; ++i )
    {
    cjData cnj = _CData.Conjs[i];
   
    CStringA sConj = cnj.Conj;

    if( Mode==0 || Mode==2)
      {
      AddLPCSTR(Str, cnj.Conj.c_str(), attrConjBold);
    
      AddNSString(Str, @" ("     , attrModeSmall);
      AddLPCSTR  (Str, cnj.sMode , attrModeSmall);
      AddNSString(Str, @") "     , attrModeSmall);
      }
    else if( Mode==1 )
      {
      AddLPCSTR  ( Str, cnj.sMode, attrModeBoldMid );
      AddNSString( Str, @": "    , attrModeBoldMid );
  
      AddLPCSTR  ( Str, sConj.c_str(), attrConjBoldMid);
      }
    
    AddNSString(Str, @"\r\n", attrConjBoldMid);
    }
    
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(NSAttributedString*) GetAttrError:(NSString*) sMsg Prefix: (NSString*) prefix
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];

  if( prefix != nil )
    {
    AddNSString( Str, prefix, attrKey );
    AddNSString( Str, @": " , attrKey );
    }

  NSString* Txt = NSLocalizedString(sMsg, nil);
  
  AddNSString( Str, Txt, attrErr );
  
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(NSString*) GetInfinitive
  {
  cjData cnj = _CData.Conjs[0];
   
  CStringA sConj = cnj.Conj;
  
  return [NSString stringWithCString:sConj.c_str() encoding:NSISOLatin1StringEncoding ];;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Llamada después de 'GetRootWord' indica si una de la raices de la palabra es verbo
+(BOOL) IsVerb
  {
  return _IsVerb;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene todas las raices de la palabra sWord y retorna una cadena formateada con todos los datos
+(NSAttributedString*) GetRootWord: (NSString *)sWord With:(float) w
  {
  _IsVerb = FALSE;
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];  // Crea cadena con palabra a reducir
  if( sWord.length == 0 ) return Str;
  
  AddNSString(Str, sWord, attrConjBold);
  
  CStringA aWord = [sWord cStringUsingEncoding:NSISOLatin1StringEncoding];    // Convierte la cadenas a C++
  
  if( !OpenRedAPI(_CData.cLang) ) return Str;                                 // Carga la reducción, si no puede, retorna cadena
    
  HREDDATA Dat = RWReduce( _RedData.hRed, aWord );
  if( !Dat )                                                                  // No se puede reducir
    {
    NSLog(@"No se pudo reducir la palabra");
    return Str;                                                               // Retorna cadena actual
    }
    
  int count = RWGetCount(Dat);
  for(int i=0; i<count; i++)
    {
    CString Root      = RWGetRoot(Dat, i );
    int     Mood      = RWGetMood(Dat, i );
    int     Time      = RWGetTime(Dat, i );
    int     Person    = RWGetPerson(Dat, i );
    int     Number    = RWGetNumber(Dat, i );
    int     Gender    = RWGetGender(Dat, i );

    if( aWord == Root ) Root = "";
    
    int nTipo = RWGetClassType(Dat, i);
    
    AddLPCSTR(Str, "\r\n" , attrConjBold);
    
    if( nTipo==rVERBO )
      {
      _IsVerb = TRUE;
      [ConjCore AddVerboRootStr:Str Root:Root.c_str() Mood:Mood Time:Time Person:Person Number:Number With:w];
      }
    else
      {
      [ConjCore AddGenAndNumStr:Str Root:Root.c_str() Type:nTipo Gener:Gender Number:Number];
      }
    }
      
  RWFreehData(Dat);
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Forma una cadena formateada con todos los datos suministrados
+(void) AddGenAndNumStr:(NSMutableAttributedString*) Str Root:(LPCSTR)sRoot Type:(int)Tipo Gener:(int)Genero Number:(int)Numero
  {
  if( sRoot[0] > 0 )
    {
    AddNSString(Str, Strs.From , attrConjGray);
    AddLPCSTR  (Str, sRoot     , attrConjBold);
    AddNSString(Str, @" "      , attrConjBold);
    }
  
  AddNSString(Str, Strs.Tipos[Tipo] , attrModeSmall);
  AddNSString(Str, @" "             , attrConjBold);
   
  AddNSString(Str, Strs.Gens[Genero], attrPersItalicSmall);
  AddNSString(Str, @" "             , attrConjBold);
  
  AddNSString(Str, Strs.Nums3[Numero], attrPersItalicSmall);
  
 // AddNSString(Str, @"\n" , attrConjBold);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Forma una cadena formateada con todos los datos suministrados
+(void) AddVerboRootStr:(NSMutableAttributedString*) Str Root:(LPCSTR)sRoot Mood:(int)Modo Time:(int)Tiempo Person:(int)Persona Number:(int)Numero With:(float) wSize
  {
  cjData cnj;
  int i = 0;
  for (; i<_CData.Count; ++i)
    {
    cnj = _CData.Conjs[i];
    if( cnj.mode == Modo && cnj.time==Tiempo && (i<3 || (cnj.Per==Persona && cnj.num==Numero)) )
      break;
    }
    
  if( i>=_CData.Count )
    {
    NSLog( @"Modo no encontrado" );
    return;
    }
  
  NSMutableAttributedString* sAttr = [[NSMutableAttributedString alloc] init];
  if( sRoot[0] > 0 )
    {
    AddNSString(sAttr, Strs.From, attrConjGray );
    AddLPCSTR  (sAttr, sRoot    , attrConjBold );
    AddNSString(sAttr, @" "     , attrConjBold );
    }
  
  AddNSString(sAttr, Strs.Mode, attrModeSmall );
  AddNSString(sAttr, @" "     , attrConjBold  );
  AddLPCSTR  (sAttr, cnj.sMode, attrModeSmall );
  
  if( i>2 )
    {
    AddNSString(sAttr, @" ", attrConjBold  );
    
    int idx = (3*cnj.num) + cnj.Per;
    
    NSString* PerLarge = Strs.PersNum2[idx];
    
    float w1 = sAttr.size.width;
    float w2 = [PerLarge sizeWithAttributes:attrPersItalicSmall].width;
    
    if( w1 + w2 < wSize )                                                     // Si hay espacio
      AddNSString(sAttr, PerLarge , attrPersItalicSmall );                    // Pone la descrición completa
    else
      AddNSString(sAttr, Strs.PersNum1[idx], attrPersItalicSmall );           // Pone abreviaturas
    }
  
 // AddNSString(sAttr, @"\n" , attrConjBold);
  
  [Str appendAttributedString: sAttr ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la primera raiz de la palabra 'sWord' que se encuentre en el diccionario
+(NSString *) FindRootWord: (NSString *)sWord Lang:(int) lng
  {
  if( !OpenRedAPI(lng) ) return nil;                                        // Carga la reducción, si no puede, retorna nil

  CStringA aWord = [sWord cStringUsingEncoding:NSISOLatin1StringEncoding];  // Convierte la cadenas a C++
  
  HREDDATA Dat = RWReduce( _RedData.hRed, aWord );
  if( !Dat ) return nil;                                                    // No se puede reducir, retorna nulo
  
  int count = RWGetCount(Dat);
  for(int i=0; i<count; i++)
    {
    CStringA sRoot = RWGetRoot( Dat, i );
    if( sRoot != aWord )
      {
      RWFreehData(Dat);
      return [NSString stringWithCString:sRoot.c_str() encoding:NSISOLatin1StringEncoding ];
      }
    }
      
  RWFreehData(Dat);
  return nil;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene todas las raices de la palabra 'sWrod' en el idioma 'lng'
+(NSMutableArray *) GetRootListForWord: (NSString *)sWord Lang:(int) lng
  {
  NSMutableArray* List = [NSMutableArray new];                              // Crea una lista vacia
  
  if( !OpenRedAPI(lng) ) return List;                                       // Carga la reducción, si no puede, retorna lista vacia

  CStringA aWord = [sWord cStringUsingEncoding:NSISOLatin1StringEncoding];  // Convierte la cadenas a C++
  
  HREDDATA Dat = RWReduce( _RedData.hRed, aWord );
  if( !Dat ) return List;                                                   // No la puede reducir, retorna lista vacia
  
  int count = RWGetCount(Dat);                                              // Obtiene la cantidad de reducciones
  for(int i=0; i<count; i++)                                                // Recorre todas las reducciones
    {
    CStringA sRoot = RWGetRoot( Dat, i );
    if( sRoot != aWord )
      {
      NSString* root = [NSString stringWithCString:sRoot.c_str() encoding:NSISOLatin1StringEncoding ];  // Convierte raiz a Objetive-C
      
      BOOL find = FALSE;
      for(int j=0; j<List.count; j++)                                        // Busca si la raiz ya esta en la lista
        {
        NSString* word = List[j];
        if( [word isEqualToString:root] )
          find = TRUE;
        }
      
      if( !find ) [List addObject:root];
      }
    }
      
  RWFreehData(Dat);
  return List;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
@end

//=========================================================================================================================================================
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Agrupa los datos de una conjugación y el texto que la representa
@implementation ConjAndTxt

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un objeto con el número de la conjugación y el texto
+(ConjAndTxt*) ObjWithConj:(int) idx AndText:(NSAttributedString*) AttrText;
  {
  ConjAndTxt* obj = [ConjAndTxt new];
  
  obj.AttrText = AttrText;
    
  cjData& cnj = _CData.Conjs[idx];
  obj.Conj = [NSString stringWithCString:cnj.Conj.c_str() encoding:NSISOLatin1StringEncoding ];
    
  return obj;
  }

+(ConjAndTxt*) ObjWithText:(NSAttributedString*) AttrText;
  {
  ConjAndTxt* obj = [ConjAndTxt new];
  
  obj.AttrText = AttrText;
    
  return obj;
  }

@end
//=========================================================================================================================================================


