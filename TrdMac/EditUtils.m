//=========================================================================================================================================================
//  EditTxtSrc.m
//  TrdMac
//
//  Created by Camilo on 2/22/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "EditUtils.h"
#import "ParseText.h"

//=========================================================================================================================================================
int  FindSrcItemAt( NSInteger iPos, int ItemIni, ParseText* ps);
int  FindTrdItemAt( NSInteger iPos, int ItemIni, ParseText* ps);

void ShiftSrc( int ItemIni, NSInteger Delta, ParseText* ps);
void ShiftTrd( int ItemIni, NSInteger Delta, ParseText* ps);

void JoinItems( int ItemIni, int ItemEnd, ParseText* ps);

//=========================================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Busca la oración en el texto fuente que se encuentra en la posición 'iPos'
Byte SentenceSrcAtPos( NSInteger iPos, int* idx)
  {
  ParseText* ps = [ParseText Now];
  if( ps == nil ) return ITEM_NODEF;
  
  for( int i=0; i<ps.Items.count; ++i )
    {
    ParseItem* Item = ps.Items[i];
    NSRange      rg = Item.rgSrc;
    
    if( NSLocationInRange(iPos, rg) )
      {
      *idx = i;
      return Item.Type;
      }
    }
  
  return ITEM_NODEF;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Busca la oración en el texto traducido que se encuentra en la posición 'iPos'
Byte SentenceTrdAtPos( NSInteger iPos, int* idx)
  {
  ParseText* ps = [ParseText Now];
  if( ps == nil ) return ITEM_NODEF;
  
  for( int i=0; i<ps.Items.count; ++i )
    {
    ParseItem* Item = ps.Items[i];
    NSRange      rg = Item.rgTrd;
    
    if( NSLocationInRange(iPos, rg) )
      {
      *idx = i;
      return Item.Type;
      }
    }
  
  return ITEM_NODEF;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza el parse de oraciones según los caracteres editados en el texto fuente
void EditSrcRange( NSRange Range, NSString* str)
  {
  ParseText* ps = [ParseText Now];
  if( ps == nil ) return;
  
  NSInteger PosIni = Range.location;
  NSInteger PosEnd = PosIni + Range.length;
  
  int ItemIni = FindSrcItemAt( PosIni, 0      , ps );
  int ItemEnd = FindSrcItemAt( PosEnd, ItemIni, ps );
  if( ItemIni==-1 || ItemEnd==-1 ) return;
  
  if( ItemIni != ItemEnd ) JoinItems(ItemIni, ItemEnd, ps);
  
  NSInteger Delta = str.length - Range.length;
  ParseItem* Item = ps.Items[ItemIni];
  
  NSRange OldRg = Item.rgSrc;
  Item.rgSrc    = NSMakeRange(OldRg.location, OldRg.length+Delta);
  Item.chgSrc   = TRUE;
  Item.Type     = ITEM_CHGED;
  
  ShiftSrc( ItemIni, Delta, ps );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza el parse de oraciones según los caracteres editados en el texto traducido
void EditTrdRange( NSRange Range, NSString* str)
  {
  ParseText* ps = [ParseText Now];
  if( ps == nil ) return;
  
  NSInteger PosIni = Range.location;
  NSInteger PosEnd = PosIni + Range.length;
  
  int ItemIni = FindTrdItemAt( PosIni, 0, ps );
  if( ItemIni<0 || ItemIni>=ps.Items.count ) return;
  
  int ItemEnd = FindTrdItemAt( PosEnd, ItemIni, ps );
  if( ItemIni != ItemEnd ) JoinItems(ItemIni, ItemEnd, ps);
  
  NSInteger Delta = str.length - Range.length;
  ParseItem* Item = ps.Items[ItemIni];
  
  NSRange OldRg = Item.rgTrd;
  Item.rgTrd    = NSMakeRange(OldRg.location, OldRg.length+Delta);
  Item.chgTrd   = TRUE;
  Item.Type     = ITEM_CHGED;
  
  ShiftTrd( ItemIni, Delta, ps );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Busca el item en la posición 'iPos' del texto fuente, en el parse 'ps' a partir del item 'iniItem'
int FindSrcItemAt( NSInteger iPos, int ItemIni, ParseText* ps)
  {
  for( int i=ItemIni; i<ps.Items.count; ++i )
    {
    ParseItem* Item = ps.Items[i];
    
    if( NSLocationInRange(iPos, Item.rgSrc) )
      return i;
    }
  
  return -1;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Busca el item en la posición 'iPos' del texto traducido, en el parse 'ps' a partir del item 'iniItem'
int FindTrdItemAt( NSInteger iPos, int ItemIni, ParseText* ps)
  {
  for( int i=ItemIni; i<ps.Items.count; ++i )
    {
    ParseItem* Item = ps.Items[i];
    
    if( NSLocationInRange(iPos, Item.rgTrd) )
      return i;
    }
  
  return -1;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Une todos los items que hay entre 'ItemIni' e 'ItemEnd' en un solo item
void JoinItems( int ItemIni, int ItemEnd, ParseText* ps)
  {
  ParseItem* Item = ps.Items[ItemIni];
  
  for( int i=ItemEnd; i>ItemIni; --i )
    {
    ParseItem* now = ps.Items[i];
    
    Item.txtSrc = [Item.txtSrc stringByAppendingString: now.txtSrc];
    Item.txtTrd = [Item.txtTrd stringByAppendingString: now.txtTrd];
    
    Item.rgSrc = NSUnionRange(Item.rgSrc, now.rgSrc);
    Item.rgTrd = NSUnionRange(Item.rgTrd, now.rgTrd);
    
    [ps.Items removeObjectAtIndex:i];
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Corre todos los items del texto fuente, en le parse 'ps' a partir de 'ItemIni' una cantidad 'Delta'
void ShiftSrc( int ItemIni, NSInteger Delta, ParseText* ps)
  {
  for( int i=ItemIni+1; i<ps.Items.count; ++i )
    {
    ParseItem* Item = ps.Items[i];
    NSRange   OldRg = Item.rgSrc;
    
    Item.rgSrc = NSMakeRange(OldRg.location+Delta, OldRg.length);
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Corre todos los items del texto traducido, en le parse 'ps' a partir de 'ItemIni' una cantidad 'Delta'
void ShiftTrd( int ItemIni, NSInteger Delta, ParseText* ps)
  {
  for( int i=ItemIni+1; i<ps.Items.count; ++i )
    {
    ParseItem* Item = ps.Items[i];
    NSRange   OldRg = Item.rgTrd;
    
    Item.rgTrd = NSMakeRange(OldRg.location+Delta, OldRg.length);
    }
  }

//=========================================================================================================================================================
