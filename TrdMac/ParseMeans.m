//=========================================================================================================================================================
//  PaseMeans.m
//  TrdMac
//
//  Created by Camilo on 3/10/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "ParseMeans.h"

//=========================================================================================================================================================
@implementation MeanItem

+(MeanItem*) ItemWithRange:(NSRange) rg Type:(int) type Value:(NSString*) sVal
  {
  MeanItem* Item = [MeanItem new];
  
  Item.rg   = rg;
  Item.tipo = type;
  Item.sVal = sVal;
  
  return Item;
  }

@end
//=========================================================================================================================================================

//=========================================================================================================================================================
@interface ParseMeans()
  {
  NSMutableArray* Items;
  NSString* lastStr;
  
  int MeanAttrs;
  }
@end
//=========================================================================================================================================================

static NSCharacterSet* EspChrSet;      // Conjunto de caracteres de una especialidad
static NSCharacterSet* SepMeanSet;     // Conjunto de caracteres que pueden servir de separadores de significados
static BOOL iniStatict;

#define ClearAttr    (MeanAttrs = 0)
#define SetAttr(x)   (MeanAttrs = (MeanAttrs | x) )

#define IsAttrGen   ((MeanAttrs & ITEM_GEN) != 0)
#define IsAttrNum   ((MeanAttrs & ITEM_NUM) != 0)
#define IsAttrEsp   ((MeanAttrs & ITEM_ESP) != 0)

//=========================================================================================================================================================
@implementation ParseMeans

//--------------------------------------------------------------------------------------------------------------------------------------------------------
+(ParseMeans*) ParseWithString:(NSString*) str
  {
  if( !iniStatict )
    {
    EspChrSet  = [NSCharacterSet characterSetWithCharactersInString:@"ABCDEFGHIJLNMOPQRSUTVZ"];
    SepMeanSet = [NSCharacterSet characterSetWithCharactersInString:@",;"];
    
    iniStatict = TRUE;
    }
  
  ParseMeans* ps = [ParseMeans new];
  [ps ParseString:str];
  
  return ps;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
-(void) ParseString:(NSString*) str
  {
  if( [str isEqualToString:lastStr] ) return;
  lastStr = [str copy];
  
  if( Items==nil ) Items = [NSMutableArray new];
  else             [Items removeAllObjects];
  
  NSScanner* sc = [NSScanner scannerWithString:str];
  sc.caseSensitive = TRUE;
  
  for(;;)
    {
    [self FindAttrsInScan:sc];
    
    NSString* sMean;
    if( [sc scanUpToCharactersFromSet:SepMeanSet intoString:&sMean] )
      {
      NSInteger pos    = sc.scanLocation;
      NSInteger nChars = sMean.length;
      
      NSRange rg = NSMakeRange(pos-nChars, nChars);
      
      [Items addObject:[MeanItem ItemWithRange:rg Type:ITEM_MEAN Value:sMean] ];
      }
    else break;
    
    [sc scanCharactersFromSet:SepMeanSet intoString:nil];
    }
  
  }
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra todos de los atributos de un signiicado desde la posición del escaner 'sc'
- (void) FindAttrsInScan:(NSScanner*) sc
  {
  ClearAttr;
  unichar c1, c2, c3;
  NSInteger pos;
  for( int i=0; i<3; ++i )
    {
    for(;;)
      {
      pos = sc.scanLocation;
      if( pos+3>=sc.string.length  ) return;
      
      c1 = [sc.string characterAtIndex:pos];
      if( c1 != ' ' ) break;
      
      sc.scanLocation += 1;
      }
    
    c2 = [sc.string characterAtIndex:pos+1];
    c3 = [sc.string characterAtIndex:pos+2];
    
    if( !IsAttrNum && c1=='p' && c2=='.' && c3==' ')
      {
      [self AttrFromScan:sc AttrType:ITEM_NUM Value:@"p"];
      }
    else if( !IsAttrGen && c1=='f' && c2=='.' && c3==' ')
      {
      [self AttrFromScan:sc AttrType:ITEM_GEN Value:@"f"];
      }
    else if( !IsAttrGen && c1=='n' && c2=='.' && c3==' ')
      {
      [self AttrFromScan:sc AttrType:ITEM_GEN Value:@"n"];
      }
    else if( !IsAttrEsp && [EspChrSet characterIsMember:c1] && [EspChrSet characterIsMember:c1] && c3==' ' )
      {
      NSString* sEsp = [sc.string substringWithRange:NSMakeRange(pos, 2)];
      
      [self AttrFromScan:sc AttrType:ITEM_ESP Value:sEsp];
      }
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene un rango a partir de la posición del escaner 'NChar' hacia atras
- (void) AttrFromScan:(NSScanner*) sc AttrType:(int) aType Value:(NSString*) sVal
  {
  SetAttr(aType);
  
  NSInteger pos = sc.scanLocation;
  NSRange    rg = NSMakeRange(pos, 3);

  [Items addObject:[MeanItem ItemWithRange:rg Type:aType Value:sVal] ];
  
  sc.scanLocation += 3;
  }


//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (NSArray *)Items
  {
  return Items;
  }
//--------------------------------------------------------------------------------------------------------------------------------------------------------

@end
//=========================================================================================================================================================
