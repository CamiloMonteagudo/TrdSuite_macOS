/*
 *  ConjCore.h
 *  IdiomaXTranslator
 *
 *  Created by MacPC on 5/29/11.
 *  Copyright 2011 IdiomaX. All rights reserved.
 *
 */

#import <Foundation/Foundation.h>
#include "WinUtil.h"
//#include "ConjVerb.h"
#include "ApiRed.h"

//=========================================================================================================================================================
// Almecena los datos de una palabra conjugada
struct cjData
  {
  public:
  Modo    mode;
  Tiempo  time;
  Persona Per;
  Numero  num;
  bool    comp;
  
  LPCSTR sMode;
  LPCSTR Pronm;
  
  CStringA Conj;
  };

//=========================================================================================================================================================
// Almecena un grupo de cadenas localizadas según el idioma de conjugación
struct cjStrLoc
  {
  public:
  NSString* PersNum1[6];
  NSString* PersNum2[6];
  NSString* ProNoums[6];
  
  NSString* Nums3[2];
  
  NSString* Gens[3];
  NSString* Tipos[6];

  NSString* From;
  NSString* Mode;
  };

//=========================================================================================================================================================
// Clase que almacena los datos de una conjugación y el texto que los representa
@interface ConjAndTxt: NSObject

  @property (nonatomic) NSString* Conj;
  @property (nonatomic) NSAttributedString* AttrText;

  +(ConjAndTxt*) ObjWithText:(NSAttributedString*) AttrText;
  +(ConjAndTxt*) ObjWithConj:(int) idx AndText:(NSAttributedString*) AttrText;
@end

//=========================================================================================================================================================
// Clase que almacena los datos de todas las conjugaciones
class ConjData 
  {
  public:
  int        cLang;
  CStringA   Word;
  HCONJ      hConj;
  
  cjData*    Conjs;
  int        Count;
  
  BOOL       Ok;
  
  ConjData() { Ok=FALSE; hConj=NULL; cLang = -1; }
  ~ConjData(){ }
  };

//=========================================================================================================================================================
// Clase que almacena los datos de la ultima reducción realizada
class ReducData
  {
  public:
  int          rLang;
  CStringA     Word;
  HREDW        hRed;
  CStringA     Root;
  
  ReducData() { hRed=NULL; rLang = -1; }
  ~ReducData(){ }
  };


//=========================================================================================================================================================
// Clase para interactuar con el API de conjugación
@interface ConjCore : NSObject

+(void) SetDictPath:(NSString*) path;

+(float) WMax;
+(float) HMax;
+(void) setHMax:(float) hMax;

+(bool) LoadConjLang:(int) lng;
+(bool) ConjVerb: (NSString *)Verb;
+(int)  nowLang;

+(ConjData &) conjData;

+(NSString *) FindRootWord: (NSString *)sWord Lang:(int) lng;

+(NSArray *) GetConjsByMode;
+(NSArray *) GetConjsByWord;
+(NSArray *) GetConjsByPersons;

+(NSMutableArray *) GetRootListForWord: (NSString *)sWord Lang:(int) lng;

+(NSArray *) SortByConjList:(NSArray *) lstData;

+(NSAttributedString*) GetRootWord: (NSString *)sWord With:(float) w;
+(NSAttributedString*) GetConjHeaderInMode:(int) Mode;
+(NSAttributedString*) GetAttrError:(NSString*) sMsg Prefix: (NSString*) prefix;

+(BOOL) IsVerb;
+(BOOL) IsNowConjugate: (NSString *)sVerb Lang:(int) lng;
+(BOOL) IsVerbWord:(NSString*) word InLang:(int) lng;

+(NSString*) GetInfinitive;

@end
//=========================================================================================================================================================
