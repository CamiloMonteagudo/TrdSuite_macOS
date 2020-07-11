//=========================================================================================================================================================
//  ParseText.m
//  TrdMac
//
//  Created by Camilo on 2/3/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "ParseText.h"

static ParseText* Parse;

static BOOL InitStaticData;
static NSCharacterSet* charsEnd;
static NSCharacterSet* charsPnts;
static NSCharacterSet* charsSymbols;
static NSCharacterSet* charsAlphaNum;
static NSCharacterSet* charsAlpha;
static NSCharacterSet* charsSplit;
static NSCharacterSet* charsNum;

#define ClearLeft    0x0001
#define ClearRigth   0x0002
#define SplitOra     0x0004

//=========================================================================================================================================================
@interface ParseText()
  {
  NSString* Text;
  
  NSInteger idxLast;
  }

@end

//=========================================================================================================================================================
@implementation ParseText

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(void) InitData
  {
  if( !InitStaticData )
    {
    charsEnd      = [NSCharacterSet whitespaceAndNewlineCharacterSet];
    charsPnts     = [NSCharacterSet punctuationCharacterSet];
    charsSymbols  = [NSCharacterSet symbolCharacterSet];
    charsAlphaNum = [NSCharacterSet alphanumericCharacterSet];
    charsAlpha    = [NSCharacterSet letterCharacterSet];
    charsNum      = [NSCharacterSet decimalDigitCharacterSet];
    
    charsSplit    = [NSCharacterSet characterSetWithCharactersInString:@":;"];
    
    InitStaticData = TRUE;
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Divide el texto 'txt' en oraciones y retorna el resultado
+(ParseText*) ParseWithText:(NSString*) txt
  {
  [ParseText InitData];
  
  ParseText* ps = [ParseText new];
  
  ps->_Items = [NSMutableArray new];
  ps->Text   = txt;
  
  [ps DoParse];
  
  return ps;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Implementa la propiedad 'Main' para obtener y poner el parse principal
+(ParseText*) GetMain
  {
  return Parse;
  }

+(void) SetMain:(ParseText*) ps;
  {
  Parse = ps;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
-(void) DoParse
  {
  NSRange rgTxt = NSMakeRange(0, Text.length);
  
  [Text enumerateSubstringsInRange:rgTxt
                               options:NSStringEnumerationBySentences /*| NSStringEnumerationSubstringNotRequired*/
                            usingBlock:^(NSString *substring, NSRange rg, NSRange rg2, BOOL *stop)
      {
      [self ReduceLeftRange :&rg];    // Quita caracteres a la izquierda de la oración que no forman parte de ella
      [self ReduceRightRange:&rg];    // Quita caracteres a la derecha de la oración que no forman parte de ella
      [self SplitSentInRange:&rg];    // Divide la oración en varias si es necesario
      
      [self GetOraInRange:rg];        // Obtiene oreción y caracteres sobrantes a la izquierda
      
      NSInteger iEnd = rg2.location + rg2.length;                 // Indice del ultimo caracter analizado
      if( iEnd >= Text.length )                                   // Esta al final del texto
        {
        NSRange rgCascara = NSMakeRange(idxLast, iEnd-idxLast);   // Toma el texto sobrante al final de la oración
        if( rgCascara.length > 0 )                                // Si hay texto sobrante
          {
          NSString* cas = [Text substringWithRange: rgCascara];   // Extrae la cadena de texto
          
          [_Items addObject:[ParseItem ItemWithText:cas Range:rgCascara Type:ITEM_NOTRD] ];   // Lo adiciona a la lista de items
          }
        }
      }
   ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la oración contenida en el rango 'rg' y los caracteres intermedios de la oración anterior
-(void) GetOraInRange:(NSRange) rg
  {
  if( rg.length==0 ) return;

  NSRange rgCascara = NSMakeRange(idxLast, rg.location-idxLast);
  if( rgCascara.length >0 )
    {
    NSString* cas = [Text substringWithRange: rgCascara];
    
    [_Items addObject:[ParseItem ItemWithText:cas Range:rgCascara Type:ITEM_NOTRD] ];
    }
  
  NSString* s = [Text substringWithRange: rg];

  [_Items addObject:[ParseItem ItemWithText:s Range:rg Type:ITEM_TRD] ];
  
  idxLast = rg.location + rg.length;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Quita todos los caracteres que no son necesarios al final del rango
-(BOOL) ReduceRightRange:(NSRange*) rg;
  {
  int      dec = 0;
  NSUInteger i = rg->location + rg->length - 1;

  for(;i>rg->location; --i)
    {
    unichar c = [Text characterAtIndex:i];
  
    if( ![charsAlphaNum characterIsMember:c] && c!='?' && c!='!' ) --dec;
    else break;
    }

  
  if( dec==0 ) return FALSE;
    
  rg->length += dec;
  return TRUE;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Quita todos los caracteres que son necesarios al comienzo del rango
-(void) ReduceLeftRange:(NSRange*) rg;
  {
  if( rg->length==0 ) return;
  
  int dec = 0;
  NSUInteger end = rg->location + rg->length;
  
  for(NSUInteger i=rg->location; i<end; ++i)
    {
    unichar c = [Text characterAtIndex:i];
    
    if( ![charsAlphaNum characterIsMember:c] && c!= 0x00BF && c!=0x00A1) ++dec;
    else break;
    }
  
  // Verificar si es un buler
  //
  
  rg->location += dec;
  rg->length   -= dec;
  
  [self IsOnlyNumbers:rg];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Verifica si todos los caracteres dentro del rango son númericos
-(void) IsOnlyNumbers:(NSRange*) rg;
  {
  int dec = 0;
  NSUInteger end = rg->location + rg->length;
  
  for(NSUInteger i=rg->location; i<end; ++i)
    {
    unichar c = [Text characterAtIndex:i];
    
    if( [charsNum characterIsMember:c] ) ++dec;
    else break;
    }
  
  if( dec == 0 ) return;

  NSRange rgTemp = NSMakeRange(rg->location+dec, rg->length-dec);
  if( rgTemp.length == 0 )
    {
    *rg = rgTemp;
    return;
    }
  
  [self ReduceLeftRange:&rgTemp];
  if( rgTemp.length == 0 )
    {
    *rg = rgTemp;
    return;
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pica una oración en 2 o más partes si es necesario
-(void) SplitSentInRange:(NSRange*) rg;
  {
  if( rg->length == 0 ) return;
  
  NSRange rgSearch = *rg;
  
  BOOL split = FALSE;
  for(;;)
    {
    // Busca los caracteres clave por donde se debe cortar la oración
    NSRange rgRet = [Text rangeOfCharacterFromSet:charsSplit options:0 range: rgSearch];
    if( rgRet.length == 0 )
      {
      if( split )
        {
        [self ReduceLeftRange: &rgSearch];
        *rg = rgSearch;
        }
      
      return;
      }
  
    // Obtiene la oración a la derecha del separador
    NSRange rgLeft = NSMakeRange(rgSearch.location, rgRet.location-rgSearch.location);
    
    [self ReduceRightRange:&rgLeft];                // Quita caracteres a la derecha de la oración que no forman parte de ella
    [self GetOraInRange:rgLeft    ];                // Obtiene oración y caracteres a la izquierda
    
    // Reduce el rango de busqueda, empezando despues del caracter encontrado
    NSInteger lstChar = rgSearch.location + rgSearch.length;
    
    rgSearch.location = rgRet.location + rgRet.length;
    rgSearch.length   = lstChar - rgSearch.location;
    
    split = TRUE;                                               // Bandera que indica que se efectuo al menos un corte
    }
  }

@end

//=========================================================================================================================================================
@implementation ParseItem

+(ParseItem*) ItemWithText:(NSString*) txt Range:(NSRange) rg Type:(Byte) tipo
  {
  ParseItem* item = [ParseItem new];
  
  item.Type = tipo;
  item.txtSrc = txt;
  item.rgSrc = rg;
  
  return item;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pone el valor de la traducción, con el offset especificado y retorna el nuevo offset
-(NSInteger) setTrd:(NSString*) trd WithOffset:(NSInteger) off
  {
  _txtTrd = trd;
  
  NSInteger len1 = _txtSrc.length;
  NSInteger len2 = _txtTrd.length;
  
  _rgTrd = NSMakeRange( _rgSrc.location + off, len2);
  
  return off + len2 - len1;
  }

@end

//=========================================================================================================================================================

