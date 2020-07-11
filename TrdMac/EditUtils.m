//=========================================================================================================================================================
//  EditTxtSrc.m
//  TrdMac
//
//  Created by Camilo on 2/22/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "EditUtils.h"

//=========================================================================================================================================================
int  FindSrcItemAt( NSInteger iPos, int ItemIni, ParseText* ps);
int  FindTrdItemAt( NSInteger iPos, int ItemIni, ParseText* ps);

void ShiftSrc( int ItemIni, NSInteger Delta, ParseText* ps);
void ShiftTrd( int ItemIni, NSInteger Delta, ParseText* ps);

void JoinItems( int ItemIni, int ItemEnd, ParseText* ps);

//=========================================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cambia el item 'iPos' por el contenido del parse 'ps'
void ChangeItemWithParse( NSInteger iPos, ParseText* ps)
  {
  ParseText* psMain = [ParseText GetMain];                            // Obtiene el parse principal del texto
  if( psMain == nil ) return;                                         // Si aun no se ha parseado el texto, no hace nada
  
  ParseItem* chgItem = psMain.Items[iPos];                            // Item que se va a sustituir
  
  NSInteger   iniSrc = chgItem.rgSrc.location;                        // Posición en el texto fuente del item a sustituir
  NSInteger   iniTrd = chgItem.rgTrd.location;                        // Posición en el texto traducido del item a sustituir
  
  NSInteger   lenSrc = chgItem.rgSrc.length;                          // Tamaño en el texto fuente del item a sustituir
  NSInteger   lenTrd = chgItem.rgTrd.length;                          // Tamaño en el texto traducido del item a sustituir
  
  NSInteger   lenSustSrc = 0;                                         // Tamaño del texto sustituido en el texto fuente
  NSInteger   lenSustTrd = 0;                                         // Tamaño del texto sustituido en el texto traducido
  NSInteger   nSustItem  = ps.Items.count;                            // Cantidad de items para sustituir
  
  for( int i=0; i<nSustItem; ++i )                                    // Recorre todos los items para sustituir
    {
    ParseItem* Item = ps.Items[i];                                    // Toma item actual
    
    NSInteger lSrc = Item.rgSrc.length;                               // Longitud de la fuente
    NSInteger lTrd = Item.rgTrd.length;                               // Longitud de la traducción
    
    Item.rgSrc = NSMakeRange(Item.rgSrc.location+iniSrc, lSrc);       // Nueva posición del texto fuente, en el parse principal
    Item.rgTrd = NSMakeRange(Item.rgTrd.location+iniTrd, lTrd);       // Nueva posición del texto traducido, en el parse principal
    
    if( i==0 ) psMain.Items[iPos] = Item;                             // Si es el primer item, lo sustituye
    else       [psMain.Items insertObject:Item atIndex:iPos+i];       // En otro caso, lo adiciona
    
    lenSustSrc += lSrc;                                               // Cuenta caracteres adicionados al texto fuente
    lenSustTrd += lTrd;                                               // Cuenta carecteres adicionados al texto traducido
    }
  
  int lastItem =(int)(iPos + nSustItem - 1);                          // Determina el último item modificado
  
  if( lenSrc != lenSustSrc ) ShiftSrc(lastItem, lenSustSrc-lenSrc, psMain);   // Cambio tamaño del texto fuente, corre el resto
  if( lenTrd != lenSustTrd ) ShiftTrd(lastItem, lenSustTrd-lenTrd, psMain);   // Cambio tamaño del texto traducido, corre el resto
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra la primera oración del texto
NSInteger HomeSentence()
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return -1;                                            // Si no hay parse, no hace nada
  
  int n = (int)ps.Items.count;                                          // Obtiene cantidad de items en el parse
  
  for( NSInteger i=0; i<n; ++i )                                       // Recorre los items hacia adelante
    {
    if( ((ParseItem*)ps.Items[i]).Type>0 ) return i;                    // Tipo texto a traducir, retorna el indice
    }
  
  return -1;                                                            // No encontro la proxima oración
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra la proxima oración a partir de la posición 'iPos'
NSInteger NextSentenceAtPos( NSInteger iPos )
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return -1;                                            // Si no hay parse, no hace nada

  ++iPos;                                                               // Se mueve al proximo item
  
  int   n = (int)ps.Items.count;                                        // Obtiene cantidad de items en el parse
  if( iPos>=n || iPos<0 ) iPos = 0;                                     // Si esta fuera de rango, toma el primero
  
  for( NSInteger i=iPos; i<n; ++i )                                     // Recorre los items hacia adelante
    {
    if( ((ParseItem*)ps.Items[i]).Type>0 ) return i;                    // Tipo texto a traducir, retorna el indice
    }
  
  return HomeSentence();                                                // No encontro, retorna la primera
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra la oración anterior a partir de la posición 'iPos'
NSInteger PrevioSentenceAtPos( NSInteger iPos )
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return -1;                                            // Si no hay parse, no hace nada
  
  --iPos;                                                               // Se mueve al item anterior
  
  int   n = (int)ps.Items.count;                                        // Obtiene cantidad de items en el parse
  if( iPos>=n || iPos<0 ) iPos = n-1;                                   // Si esta fuera de rango, toma el último
  
  for( NSInteger i=iPos; i>=0; --i )                                    // Recorre los items hacia atras
    {
    if( ((ParseItem*)ps.Items[i]).Type>0 ) return i;                    // Tipo texto a traducir, retorna el indice
    }
  
  return LastSentence();                                                // No encontro, retorna la ultima
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra la última oracion del texto
NSInteger LastSentence()
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return -1;                                            // Si no hay parse, no hace nada
  
  int n = (int)ps.Items.count;                                          // Obtiene cantidad de items en el parse
  
  for( NSInteger i=n-1; i>=0; --i )                                     // Recorre los items hacia atras
    {
    if( ((ParseItem*)ps.Items[i]).Type>0 ) return i;                    // Tipo texto a traducir, retorna el indice
    }
  
  return -1;                                                            // No encontro la proxima oración
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si en el texto fuente permanece alguna oración cambiada
BOOL IsSrcChanged()
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return FALSE;                                         // Si no hay parse, no hay nada cambiado
  
  int n = (int)ps.Items.count;                                          // Obtiene cantidad de items en el parse
  
  for( NSInteger i=0; i<n; ++i )                                        // Recorre los items hacia adelante
    {
    if( ((ParseItem*)ps.Items[i]).chgSrc ) return TRUE;                 // El texto traducido del item esta cambiado
    }
  
  return FALSE;                                                         // No encontro ninguna oración cambiada
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra la proxima oración modificada por el usuario a partir de la posición 'iPos'
NSInteger NextSentenceChgdAtPos( NSInteger iPos )
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return -1;                                            // Si no hay parse, no hace nada
  
  ++iPos;                                                               // Se mueve al proximo item
  
  NSInteger n = (int)ps.Items.count;                                    // Obtiene cantidad de items en el parse
  if( iPos>=n || iPos<0 ) iPos = 0;                                     // Si esta fuera de rango, toma el primero
  
  for( NSInteger j=0; j<2; ++j )                                        // Repite el ciclo si se pasa del final
    {
    for( NSInteger i=iPos; i<n; ++i )                                   // Recorre los items hacia adelante
      {
      if( ((ParseItem*)ps.Items[i]).chgSrc ) return i;                  // Si el item esta cambiado, retorna el indice
      }
    
    n    = iPos;                                                        // Llega hasta el item donde empezo
    iPos = 0;                                                           // Empieza por el primero
    }
  
  return -1;                                                            // No encontro ninguno
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Encuentra la oración modificada por el usuario anterior a partir de la posición 'iPos'
NSInteger PrevioSentenceChgdAtPos( NSInteger iPos )
  {
  ParseText* ps = [ParseText GetMain];                                  // Obtiene el parse principal
  if( ps == nil ) return -1;                                            // Si no hay parse, no hace nada
  
  --iPos;                                                               // Se mueve al item anterior
  
  NSInteger n = (int)ps.Items.count;                                    // Obtiene cantidad de items en el parse
  if( iPos>=n || iPos<0 ) iPos = n-1;                                   // Si esta fuera de rango, toma el último
  
  NSInteger min = 0;
  for( NSInteger j=0; j<2; ++j )                                        // Repite el ciclo si se pasa del pricipio
    {
    for( NSInteger i=iPos; i>=min; --i )                                // Recorre los items hacia atras
      {
      if( ((ParseItem*)ps.Items[i]).chgSrc ) return i;                  // Si el item esta cambiado, retorna el indice
      }
    
    min  = iPos;                                                        // Llega hasta el item donde empezo
    iPos = n-1;                                                         // Empieza por el ultimo
    }
  
  return -1;                                                            // No encontro ninguno
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Busca la oración en el texto fuente que se encuentra en la posición 'iPos'
Byte SentenceSrcAtPos( NSInteger iPos, int* idx)
  {
  ParseText* ps = [ParseText GetMain];                                // Obtiene el parse principal
  if( ps == nil ) return ITEM_NODEF;                                  // Si no parse, retorna no definida
  
  for( int i=0; i<ps.Items.count; ++i )                               // Recorre todos los items
    {
    ParseItem* Item = ps.Items[i];                                    // Toma el item actual
    
    if( NSLocationInRange(iPos, Item.rgSrc) )                         // Si la posición 'iPos' esta dentro del texto fuente
      {
      *idx = i;                                                       // Retorna el indice del item (por argumento)
      return Item.Type;                                               // Retorna el tipo del item
      }
    }
  
  return ITEM_NODEF;                                                  // 'iPos' no estaba dentro de ningun item
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Busca la oración en el texto traducido que se encuentra en la posición 'iPos'
Byte SentenceTrdAtPos( NSInteger iPos, int* idx)
  {
  ParseText* ps = [ParseText GetMain];                                // Obtiene el parse principal
  if( ps == nil ) return ITEM_NODEF;                                  // Si no parse, retorna no definida
  
  for( int i=0; i<ps.Items.count; ++i )                               // Recorre todos los items
    {
    ParseItem* Item = ps.Items[i];                                    // Toma el item actual
    
    if( NSLocationInRange(iPos, Item.rgTrd) )                         // Si la posición 'iPos' esta dentro del texto traducido
      {
      *idx = i;                                                       // Retorna el indice del item (por argumento)
      return Item.Type;                                               // Retorna el tipo del item
      }
    }
  
  return ITEM_NODEF;                                                  // 'iPos' no estaba dentro de ningun item
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza el parse de oraciones según los caracteres editados en el texto fuente
BOOL EditSrcRange( NSRange Range, NSString* str, NSString* Text)
  {
  ParseText* ps = [ParseText GetMain];                                // Obtiene el parse principal
  if( ps==nil || str==nil ) return FALSE;                             // Si no hay parse o no se instroce ningun caracter
  
  NSInteger PosIni = Range.location;                                  // Indice del primer caracter modificado
  NSInteger PosEnd = PosIni + Range.length;                           // Indice del ultimo caracter modificado
  
  if( PosIni >= Text.length ) PosIni = Text.length-1;                 // Si inicio al final del texto, toma último caracter
  if( PosEnd >= Text.length ) PosEnd = Text.length-1;                 // Si final al final del texto, toma último caracter
  
  int ItemIni = FindSrcItemAt( PosIni, 0      , ps );                 // Indice al item donde esta el inicio
  int ItemEnd = FindSrcItemAt( PosEnd, ItemIni, ps );                 // Indice al item donde esta el final
  
  if( ItemIni==-1 || ItemEnd==-1 ) return FALSE;                      // Si no encuentra uno de los dos, no hace nada
  
  ParseItem* Item = ps.Items[ItemIni];                                // Toma el primer item modificado
  if( Item.Type == 0 && ItemIni>0 ) -- ItemIni;                       // Si esta sobre cascara, toma el anterior
  
  if( ItemIni != ItemEnd ) JoinItems(ItemIni, ItemEnd, ps);           // Si la modificación ocupa más de un item, los une
  
  NSInteger Delta = str.length - Range.length;                        // Diferencia de tamaño
  
  Item = ps.Items[ItemIni];                                           // Toma item modificado (se unifica en JoinItems si mas de uno)
  
  NSRange OldRg = Item.rgSrc;                                         // Guarda rango de la fuente
  Item.rgSrc    = NSMakeRange(OldRg.location, OldRg.length+Delta);    // Modifica tamaño según modificación
  Item.chgSrc   = TRUE;                                               // Pone bandera de que cambio la fuente
  Item.Type     = ITEM_CHGED;                                         // Cambia el tipo de item
  
  ShiftSrc( ItemIni, Delta, ps );                                     // Corre todo los items que le siguen
  return TRUE;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza el parse de oraciones según los caracteres editados en el texto traducido
void EditTrdRange( NSRange Range, NSString* str, NSString* Text)
  {
  ParseText* ps = [ParseText GetMain];
  if( ps == nil || str==nil  ) return;
  
  NSInteger PosIni = Range.location;
  NSInteger PosEnd = PosIni + Range.length;
  
  if( PosIni >= Text.length ) PosIni = Text.length-1;
  if( PosEnd >= Text.length ) PosEnd = Text.length-1;
  
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
