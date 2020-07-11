//=========================================================================================================================================================
//  ToolsCrtller.m
//  TrdMac
//
//  Created by Camilo on 2/12/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "ToolsCrtller.h"
#import "DictCtrller.h"
#import "ConjCtrller.h"

@interface ToolsCrtller ()
  {
  NSTabView* Tab;
  }

@property (weak) IBOutlet DictCtrller *DictCtrller;
@property (weak) IBOutlet ConjCtrller *ConjCtrller;

@end

//=========================================================================================================================================================
// Maneja el control lateral donde se ponen la herramientas de ayuda a la traducción
@implementation ToolsCrtller

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se termina de cargar el Nib de fichero
- (void)awakeFromNib
  {
  Tab = (NSTabView*)self.view;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Palabra o frase sobre la cual se solicita información
- (void)setInfoWord:(NSString *)infoWord
  {
//  if( _infoWord == infoWord ) return;
  
  _infoWord = infoWord;
  
  NSString* ID = Tab.selectedTabViewItem.identifier;
  
  if( [ID isEqualToString:@"1"] ) _DictCtrller.Word = infoWord;
  if( [ID isEqualToString:@"2"] ) _ConjCtrller.Verb = infoWord;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Implementa la propiedad de poner el texto traducido por defecto
- (void) setTrd:(BOOL)Trd
  {
  _Trd = Trd;
  
  _DictCtrller.Trd = Trd;
  _ConjCtrller.Trd = Trd;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cada vez que cambia el tipo de herramienta que se esta mostrando
- (void)tabView:(NSTabView *)tabView didSelectTabViewItem:(NSTabViewItem *)tabViewItem
  {
  NSString* ID = tabViewItem.identifier;
  
  NSString* word = _DictCtrller.Word;
  NSString* verb = _ConjCtrller.Verb;
  
  if( [ID isEqualToString:@"1"] )
    {
    _DictCtrller.Word = (verb!=nil)? verb : _infoWord;
    }
  else if( [ID isEqualToString:@"2"] )
    {
    _ConjCtrller.Verb =  (word!=nil)? word : _infoWord;
    }
  }

@end
//=========================================================================================================================================================
