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
#import "ProgressCtrller.h"

#define LGCount 4

//=========================================================================================================================================================
// Datos estaticos para todas la traducciones
static BOOL initStatic;                         // Bandera para indicar que los datos estaticos fueron inicializados

static CStringA  cPathAPI;                      // Camino donde estan los diccionarios del API de traducción (C++)
static NSString* oPathAPI;                      // Camino donde estan los diccionarios del API de traducción (Ojective-C)

static CStringA  cPathUser;                     // Camino donde estan los diccionarios de usuario (C++)
static NSString* oPathUser;                     // Camino donde estan los diccionarios de usuario (Ojective-C)

static TrdCore* nowTrd;                         // Direccion de traducción actual

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
//  if( nowTrd!=nil &&  nowTrd->Src==src && nowTrd->Des==des )
//    {
//    CDictMain* dic = CDictGen::Get(src, des);
//    dic->ClearCache(TRUE);
//    return nowTrd;
//    }
  
  if( nowTrd!=nil && nowTrd->trdAPI!=NULL)
    {
    TDFree(nowTrd->trdAPI);
    }
  
  nowTrd = [[TrdCore alloc] init];

  nowTrd->Src = src;
  nowTrd->Des = des;
  
  if( ![nowTrd Open] )
    {
    NSLog(@"No can´t translation");
    return nil;
    }

  return nowTrd;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Abre la direccion de traducción para los idiomas establecidos
-(BOOL) Open
  {
  trdAPI = NULL;
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
// Traduce el texto represntado en ps segun la direccion actual
-(void) TranslateFromParse:(ParseText*) ps Progress:(ProgressCtrller*) Prog
  {
  HUTRD hUser = TDOpenUser((void*)trdAPI);                        // Obtiene un usuario para la traduccion
  
  NSInteger offset = 0;                                           // Desplazamiento por diferencia de tamaño con la traducción
  NSInteger n = ps.Items.count;                                   // Obtiene numero de item en el parse
  
  for( TInt i=0; i<n; ++i )                                       // Recorre todos los items
    {
    ParseItem *Item = ps.Items[i];                                // Obtiene el item actual
    
    if( Item.Type == 0 || Prog.Cancel )                           // Si es un item que no se traduce, o esta en modo cancelar
      {
      [Item setTrd:Item.txtSrc WithOffset:offset];                // Toma el item como esta
      
      if( Item.Type != 0 && Prog.Cancel )                         // Si se cancelo el proceso
        Item.chgTrd = TRUE;                                       // Marca que la traducción necesita cambiar
      }
    else                                                          // Si es un item que hay que traducir
      {
      CStringA src2;
      
      const char* cStr = [Item.txtSrc cStringUsingEncoding:NSISOLatin1StringEncoding ];   // Lo codifica en código ANSI
      if( cStr!=NULL )                                            // Si se puede codificar
        {
        src2 = cStr;                                              // Toma la cadena como esta
        }
      else                                                        // Si no se pudo, la codifica aproximadamente
        {
        NSData* sDat = [Item.txtSrc dataUsingEncoding:NSISOLatin1StringEncoding allowLossyConversion:YES];
        src2 = CStringA( (const char*)sDat.bytes, (int)sDat.length);
        }
      
      TDSetSentence( hUser,  src2);                               // Pone la traducción a traducir
      TDTranslate( hUser, 0 );                                    // La traduce
      
      CStringA cTrd( TDGetSentence( hUser ) );                    // Obtine la oración traducida
      
      NSString* sTrd = [NSString stringWithCString:cTrd.c_str() encoding:NSISOLatin1StringEncoding];   // La codifica en UNICODE
      
      offset = [Item setTrd:sTrd WithOffset:offset];              // Le pone la traducción al item y actualiza desplasamiento
      }
    
    Prog.Pos = i;                                                 // Actualiza posición del progreso
    
    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
    }
  
  TDFreeUser(hUser);                                              // Libera los recursos de traducción
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

