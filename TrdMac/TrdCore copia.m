//=========================================================================================================================================================
//  TrdCore.m
//  TrdMac
//
//  Created by Camilo on 1/31/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "TrdCore.h"
#include "WinUtil.h"
#include "ApiTrd.h"
#include "DictMain.h"
#import "ConjCore.h"

#define LGCount 4

//=========================================================================================================================================================
// Datos estaticos para todas la traducciones
static BOOL initStatic;                         // Bandera para indicar que los datos estaticos fueron inicializados

static CStringA  cPathAPI;                      // Camino donde estan los diccionarios del API de traducción (C++)
static NSString* oPathAPI;                      // Camino donde estan los diccionarios del API de traducción (Ojective-C)

static CStringA  cPathUser;                     // Camino donde estan los diccionarios de usuario (C++)
static NSString* oPathUser;                     // Camino donde estan los diccionarios de usuario (Ojective-C)

//static TrdCore* _TrdCache[LGCount][LGCount] =
//  {
//  //         Es,   En,   It,  Fr
//  /*Es*/{ nil, nil, nil, nil },
//  /*En*/{ nil, nil, nil, nil },
//  /*It*/{ nil, nil, nil, nil },
//  /*Fr*/{ nil, nil, nil, nil }
//  };

//=========================================================================================================================================================
@interface TrdCore()
  {
  HTRD trdAPI;
  
  int Src;
  int Des;
  }

@end

//=========================================================================================================================================================
@implementation TrdCore

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (instancetype)init
  {
  self = [super init];
  if( !self ) return nil;
  
  [TrdCore initTrdApi];
  
  return self;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Inicializa todos los datos para el API de traducción
+(void) initTrdApi
  {
  if( initStatic ) return;
  
  InitMFCSystem();
  
  NSBundle* bd = [NSBundle mainBundle];
  
  oPathAPI = [bd.resourcePath stringByAppendingPathComponent: @"Dictionaries"];
  cPathAPI = (CStringA)[oPathAPI cStringUsingEncoding:NSUTF8StringEncoding ];
  
  NSFileManager* fm = [NSFileManager defaultManager];
  NSArray*   SupDir = [fm URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];
  if( SupDir.count == 0 ) return;
  
  oPathUser = [SupDir[0] URLByAppendingPathComponent:bd.bundleIdentifier].path;
  cPathUser = (CStringA)[oPathUser cStringUsingEncoding:NSUTF8StringEncoding ];
  
  [fm createDirectoryAtPath:oPathUser withIntermediateDirectories:YES attributes:nil error:nil];
 
  CDictGen::DictsPath     = cPathAPI + FOLDER_SEP_CH;
  CDictGen::DictsUserPath = cPathUser + FOLDER_SEP_CH;
  
  [ConjCore SetDictPath:oPathAPI];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un objeto para la traducción entre los idiomas 'src' y 'des'
+(TrdCore*) TrdWithDes:(int) src AndDes:(int) des
  {
//  TrdCore* trd = [TrdCore TrdFromCacheWithDes:src AndDes:des];
//  if( trd != nil ) return trd;
  
  TrdCore* trd = [[TrdCore alloc] init];

  trd->Src = src;
  trd->Des = des;
  
  if( ![trd Open] )
    {
    NSLog(@"No can´t translation");
    return nil;
    }

//  [TrdCore TrdToCache:trd];
  return trd;
  }

////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Obtiene una traducción desde el cache
//+(TrdCore*) TrdFromCacheWithDes:(int) src AndDes:(int) des
//  {
//  if( src<0 || src>=LGCount || des<0 || des>=LGCount ) return nil;
//  
//  return _TrdCache[src][des];
//  }
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pone una dirección de traducción en el cache de traducciones
//+(void) TrdToCache:(TrdCore*) trd
//  {
//  int src = trd->Src;
//  int des = trd->Des;
//  
//  [TrdCore TrdFreeWithDes:src AndDes:des];
//  
//  _TrdCache[src][des] = trd;
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Libera los recursos de la traducción entre 'src' y 'des' si estaba en el cache de traducciones
//+(void) TrdFreeWithDes:(int) src AndDes:(int) des
//  {
//  if( src<0 || src>=LGCount || des<0 || des>=LGCount ) return;
//  
//  TrdCore* trd = _TrdCache[src][des];
//  
//  if( trd  != NULL ) TDFree(trd->trdAPI);
//  
//  _TrdCache[src][des] = nil;
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Libera los recursos de la traducción 'trd' si estaba en el cache de traducciones
+(void) TrdFree:(TrdCore*) trd
  {
  int src = trd->Src;
  int des = trd->Des;
  
  [TrdCore TrdFreeWithDes:src AndDes:des];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Libera todas las traducciones que estan en el cache de traducciones
+(void) TrdFreeAll
  {
  for (int src=0; src<LGCount; ++src)
    for (int des=0; des<LGCount; ++des)
      [TrdCore TrdFreeWithDes:src AndDes:des];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Abre la direccion de traducción para los idiomas establecidos
-(BOOL) Open
  {
  HTRD trd = TDNew();
  
  TDSetPath(trd, cPathAPI);
  
  BOOL ret = TDOpen( trd, Src, Des );
  if( !ret )
    {
    TDFree( trd );
    return false;
    }
  
  trdAPI = trd;
  return TRUE;
  }

//------------------------------------------------------------------------------------------------------
// Traduce un texto en la direccion actual
-(void) TranslateFromParse:(ParseText*) ps
  {
  HUTRD hUser = TDOpenUser((void*)trdAPI);
  
  NSInteger offset = 0;
  NSInteger n = ps.Items.count;                                  // Obtiene numero de item en el parse
  
  for( TInt i=0; i<n; ++i )                                      // Recorre todos los items
    {
    ParseItem *Item = ps.Items[i];
    
    if( Item.Type == 0 )
      [Item setTrd:Item.txtSrc WithOffset:offset];
    else
      {
//      CStringA src2 = [Item.txtSrc cStringUsingEncoding:NSUTF8StringEncoding ];
      CStringA src2 = [Item.txtSrc cStringUsingEncoding:NSISOLatin1StringEncoding ];
      
      TDSetSentence( hUser,  src2);
      TDTranslate( hUser, 0 );
      
      CStringA cTrd( TDGetSentence( hUser ) );
      
      NSString* sTrd = [NSString stringWithCString:cTrd.c_str() encoding:NSISOLatin1StringEncoding];
      
      offset = [Item setTrd:sTrd WithOffset:offset];
      }
    
    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
    }
  
  TDFreeUser(hUser);
  }

@end

//=========================================================================================================================================================

@implementation TrdItem

//------------------------------------------------------------------------------------------------------
+(TrdItem*) ItemWithType:(unsigned char)tipo SrcText:(NSString*)Text At:(int) Idx
 {
 TrdItem* item = [TrdItem new];
 
 item->_Type  = tipo;
 item->_Src   = Text;
 item->_rgSrc = NSMakeRange(Idx, Text.length-1 );
 
 return item;
 }

@end

//=========================================================================================================================================================

