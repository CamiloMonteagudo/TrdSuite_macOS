//=========================================================================================================================================================
//  ConjCtrller.m
//  TrdMac
//
//  Created by Camilo on 2/26/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "ConjCtrller.h"
#import "AppData.h"
#import "ConjCore.h"

#define BY_WORDS   0
#define BY_MODES   1
#define BY_PERSONS 2

//=========================================================================================================================================================
@interface ConjCtrller ()
  {
  BOOL First;
  id Observer;
  int viewMode;
  
  NSArray* CnjCells;
  }

@property (weak) IBOutlet NSSearchField *sWord;
@property (weak) IBOutlet NSButton *btnConj;
@property (weak) IBOutlet NSButton *btnChgMode;
@property (weak) IBOutlet NSTableView *ConjsList;
@property (weak) IBOutlet NSTextField *lbConjMode;
@property (weak) IBOutlet NSTextField *ConjHdr;

- (IBAction)OnChangeVerb:(NSSearchField *)sender;
- (IBAction)OnClickConj:(id)sender;
- (IBAction)OnChangeMode:(id)sender;

@end

//=========================================================================================================================================================
@implementation ConjCtrller

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama despué de cargar el formulario
- (void)awakeFromNib
  {
  if( !First )
    {
    First = TRUE;
    viewMode = BY_WORDS;
    _lbConjMode.stringValue = NSLocalizedString(@"ConjViewWords", @"");
  
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    Observer = [center addObserverForName: DirChangedNotification  object:nil queue:nil
                             usingBlock:^(NSNotification *note)
                                          {
                                          [self ConjugateVerbAlways:NO];
                                          }];
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cambio de la propiedad que establece el verbo actual
- (void)setVerb:(NSString *)Verb
 {
 if( Verb!=nil && _sWord.stringValue != Verb )
   {
   _sWord.stringValue = Verb;
   [self ConjugateVerbAlways:NO];
   }
 }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Propiedad que retorna el verbo actual
- (NSString *)Verb
  {
  return _sWord.stringValue;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cada ves que se cambia el verbo manualmente
- (IBAction)OnChangeVerb:(NSSearchField *)sender
  {
  [self ConjugateVerbAlways:NO];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de conjugar
- (IBAction)OnClickConj:(id)sender
  {
  [self ConjugateVerbAlways:YES];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton para cambiar el modo
- (IBAction)OnChangeMode:(id)sender
  {
  NSString* sKey;
  
  switch (viewMode)
    {
    case BY_WORDS  : viewMode=BY_MODES  ; sKey=@"ConjViewModes"   ; break;
    case BY_MODES  : viewMode=BY_PERSONS; sKey=@"ConjViewPersons" ; break;
    case BY_PERSONS: viewMode=BY_WORDS  ; sKey=@"ConjViewWords"   ; break;
    default: return;
    }

  _lbConjMode.stringValue = NSLocalizedString(sKey, @"");
  
  CnjCells = nil;
  [_ConjsList reloadData];
  
  [self UpdateConjugate];                         // Actualiza los datos de la conjución
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Conjuga el verbo actual
- (void) ConjugateVerbAlways:(BOOL) alway
  {
  int lng = (_Trd)? LGDes : LGSrc;
  
  if( [ConjCore nowLang] != lng )                           // Si cambio el idioma de conjugación
    [ConjCore LoadConjLang:lng];                            // Carga la conjugacion para el idiom actual
  
  NSString* sVerb = _sWord.stringValue;                     // Toma el contenido del editor
  
  BOOL IsVerb = [ConjCore IsVerbWord:sVerb InLang:lng];     // Determina si el texto es un verbo o alguna conjugación
  
  if( IsVerb || alway) [self ConjugateVerb:sVerb];
  else                 [self ClearConjData];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Conjuga la palabra actual
- (void)ConjugateVerb:(NSString*) sVerb
  {
  if( [ConjCore ConjVerb:sVerb] )                          // Si se puedo obtener las conjugaciones
    {
    [self UpdateConjugate];                         // Actualiza los datos de la conjución
//    [_LstConjs SelectConj:sVerb  ];                         // Selecciona las conjugaciones tecleadas
//    
//    _btnConj.hidden = TRUE;
//    [_HdrConjs ShowDataVerb:IsVerb];
    }
  else                                                     // No se puedo obtener las conjugaciones
    [self ClearConjData];
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Quita los datos de las conjugaciones
- (void) ClearConjData
  {
  _ConjHdr.stringValue = @"";
  
  CnjCells = nil;
  [_ConjsList reloadData];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza el listado de conjugaciones, según el modo vigente
- (void) UpdateConjugate
  {
  if( viewMode == BY_WORDS )
    {
    CnjCells = [ConjCore GetConjsByWord];
    CnjCells = [ConjCore SortByConjList:CnjCells];
    }
  else if( viewMode == BY_MODES )
    {
    CnjCells = [ConjCore GetConjsByMode];
    }
  else if( viewMode == BY_PERSONS )
    {
    CnjCells = [ConjCore GetConjsByPersons];
    }
  else return;
  
  _ConjHdr.attributedStringValue = [ConjCore GetConjHeaderInMode:viewMode];
  
  float h = ConjCore.HMax;
  
  _ConjsList.rowHeight = (h>15)? h:15;
  [_ConjsList reloadData];
  }


//-------------------------------------------------------------------------------------------------------------------------
// Se llama para saber el número de palabras de la lista de palabras del diccionario
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
  {
  if( CnjCells==nil ) return 0;
  
  return CnjCells.count;
  }

//-------------------------------------------------------------------------------------------------------------------------
// Se llama para conecer la palabra que se corresponde con la fila 'row'
- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
  {
  NSTableCellView* cel = [tableView makeViewWithIdentifier:@"ConjWrd" owner:tableView];
  
  ConjAndTxt* data = CnjCells[row];                                // Obtiene los datos

  cel.textField.attributedStringValue = data.AttrText;
  
  return cel;
  }

//- (NSView *)tableView:(NSTableView *)tableView objectValueForTableColumn:(nullable NSTableColumn *)tableColumn row:(NSInteger)row
//  {
//  
//  }

//-------------------------------------------------------------------------------------------------------------------------
// Se llama cuando cambia la selección de la palabra actual en la lista de palabras del diccionario
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
  {
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

@end

//=========================================================================================================================================================
