//=========================================================================================================================================================
//  DictCtrller.m
//  TrdMac
//
//  Created by Camilo on 2/26/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "DictCtrller.h"
#import "DictCore.h"
#import "AppData.h"
#import "EditDictCtrller.h"
#import "ConjCore.h"

//=========================================================================================================================================================
@interface DictCtrller ()
  {
  id Observer;
  
  EditDictCtrller* EditCtrller;
  
  NSString* TempWord;                                     // Palabra seleccionada, cuando se intercambian diccionarios
  }

@property (weak) IBOutlet NSSearchField *sWord;
@property (weak) IBOutlet NSTextField *lbWordsList;
@property (weak) IBOutlet NSTableView *WordsList;
@property (weak) IBOutlet NSButton *btnEditWord;
@property (unsafe_unretained) IBOutlet NSTextView *WordInfo;
@property (weak) IBOutlet NSButton *btnDelChanges;
@property (weak) IBOutlet NSButton *btnChangeLangs;
@property (weak) IBOutlet NSButton *btnSaveDict;
@property (weak) IBOutlet NSButton *btnWordRoots;

@property (weak) IBOutlet NSTextField *lbFlagSrc;
@property (weak) IBOutlet NSTextField *lbFlagDes;

- (IBAction)OnChangeWord:(id)sender;
- (IBAction)OnChangeLang:(id)sender;
- (IBAction)OnEdictWord:(id)sender;
- (IBAction)OnDelChanges:(id)sender;
- (IBAction)OnSaveDict:(id)sender;
- (IBAction)OnWordRoots:(id)sender;

@end

//=========================================================================================================================================================
@implementation DictCtrller

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void)awakeFromNib
  {
  NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
  Observer = [center addObserverForName: DirChangedNotification  object:nil queue:nil
                             usingBlock:^(NSNotification *note)
                                            {
                                            [self FindActualWord];
                                            }];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void)setWord:(NSString *)Word
  {
  if( Word!=nil && _sWord.stringValue != Word )
    {
    TempWord = nil;
    _sWord.stringValue = Word;
    [self FindActualWord];
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (NSString *)Word
  {
  return _sWord.stringValue;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se cambia la palabra manualmente
- (IBAction)OnChangeWord:(id)sender
  {
  TempWord = nil;
  [self FindActualWord];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama para intercambiar el idioma fuente y el idioma destino
- (IBAction)OnChangeLang:(id)sender
  {
//  int tmp = LGSrc;
//  LGSrc = LGDes;
//  LGDes = tmp;

  _Trd = !_Trd;                                           // Intercambia texto fuente y traducido
  
  NSRange rg = _WordInfo.selectedRange;
  if( rg.length > 0 )
    {
    TempWord = _sWord.stringValue;
    _sWord.stringValue = [_WordInfo.string substringWithRange:rg ];
    }
  else
    {
    if( TempWord != nil )
      {
      _sWord.stringValue = TempWord;
      TempWord = nil;
      }
    }
  
  [self FindActualWord];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de edicción de la palabra
- (IBAction)OnEdictWord:(id)sender
  {
  EditCtrller = [[EditDictCtrller alloc] initInWindow:self.view.window OnClose:@selector(OnChangedEditWord) Target:self];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de borrar los cambios de la palabar actual
- (IBAction)OnDelChanges:(id)sender
  {
  NSString* sKey = _sWord.stringValue;
  
  [DictCore DelUserKey:sKey];
  
  [self FindActualWord];
  _btnSaveDict.hidden = ![DictCore isDictModified];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de guardar los cambios en el diccionario
- (IBAction)OnSaveDict:(id)sender
  {
  [DictCore Save];
  _btnSaveDict.hidden = ![DictCore isDictModified];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton para buscar las raices de la palabra
- (IBAction)OnWordRoots:(id)sender
  {
//  FindRoot = FALSE;
  
  NSString* wrd = _sWord.stringValue;                                     // Toma la palabra actual
  NSString* word = [wrd lowercaseString];                                 // La lleva a minusculas
  
  NSMutableArray *Roots = [ConjCore GetRootListForWord:word Lang:LGSrc];  // Obtiene sus raices
  
  if( ![word isEqualToString:wrd] )                                       // Si tenia mayusculas
    {
    [DictCore getWordIdx:word];                                           // Busca la minuscula en el diccionario
    
    if( [DictCore Found] )                                                // Si esta en el diccionario
      [Roots insertObject:word atIndex:0];                                // La agrega como la primera raiz
    }

  [self CreateMenuRoots:Roots];                                           // Crea un menu con las raices y lo muestra
  
//  int nRoots = (int)Roots.count;                                          // Obtiene la cantidad de raices
//  if( nRoots == 0 ) return;                                               // Si no hay niguna, no hace nada
  
//  _sWord.stringValue = Roots[0];
//  [self FindActualWord];
//  
//  if( nRoots > 1 )
//    {
//    NSLog(@"Palabra '%@' raices %d", word, nRoots );
//    
//    for(int i=0; i<nRoots; ++i )
//      NSLog(@"Raiz %d: %@", i, Roots[i] );
//    }
//  
//  _PanelSrc.Text = Roots[0];
//  [self FindInDictWord];
//  
//  if( nRoots == 1 )
//    {
//    Roots = nil;
//    return;
//    }
//  
//  _BtnNextRoot.hidden = FALSE;
//  NextRoot = 1;
  
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un menu con todas las raices disponibles
- (void) CreateMenuRoots:(NSMutableArray*) roots
  {
  BOOL word = TRUE;
  if( roots.count == 0 )
    {
    word = FALSE;
    [roots insertObject:NSLocalizedString(@"NoRoot", @"") atIndex:0];
    }
  
  NSMenu* Mnu = [[NSMenu alloc] init];
  
  for( int i=0; i<roots.count; ++i )
    {
    NSMenuItem* Item = [[NSMenuItem alloc] init ];
    
    Item.title  = roots[i];
    Item.tag    = word;
    Item.target = self;
    Item.action = @selector(OnSelectRoot:);
    Item.state = NSOffState;
    
    [Mnu addItem:Item];
    }
  
 // _btnWordRoots.menu = Mnu;
  
  [self ShowMenu:Mnu AtButton:_btnWordRoots];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra el menú 'Mnu' sobre el boton 'Bnt'
- (void)ShowMenu:(NSMenu*) Mnu AtButton:(NSView*) Bnt
  {
  NSPopUpButtonCell* cel = [[NSPopUpButtonCell alloc] initTextCell:@"" pullsDown:NO];
  cel.menu = Mnu;
  
  NSSize szBtn = Bnt.frame.size;
//  NSRect rc = NSMakeRect(0, -Mnu.size.height, 0, 0);
//  NSRect rc = NSMakeRect(-(Mnu.size.width-szBtn.width), Mnu.size.height+szBtn.height, Mnu.size.width, Mnu.size.height);
  NSRect rc = NSMakeRect(szBtn.width-Mnu.size.width+10, szBtn.height, Mnu.size.width, Mnu.size.height);
  
  [cel performClickWithFrame:rc inView:Bnt];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona mostrar una raiz de una palabra
- (void)OnSelectRoot:(id)sender
  {
  NSMenuItem* Item = sender;
  
  if( !Item.tag )
    {
    _btnWordRoots.hidden = TRUE;
    }
  else
    {
    _sWord.stringValue = Item.title;
    [self FindActualWord];
    }
  }



//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se cambia una palabra mediante la edicción
- (void) OnChangedEditWord
  {
  [self FindActualWord];
  _btnSaveDict.hidden = ![DictCore isDictModified];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Abre el diccionario según la direccion actual
- (BOOL) OpenDict
  {
  int src = _Trd? LGDes : LGSrc;
  int des = _Trd? LGSrc : LGDes;
  
  int ret = [DictCore OpenDictSrc:src Dest:des];
  if( ret == DICT_FAIL   ) return FALSE;
  
  if( ret == DICT_OPENED )
    {
    _lbFlagSrc.stringValue = LGFlag(src);
    _lbFlagDes.stringValue = LGFlag(des);
    
    [_WordsList reloadData];
    _btnSaveDict.hidden = ![DictCore isDictModified];
    }
  
  return TRUE;
  }

//-------------------------------------------------------------------------------------------------------------------------
// Se llama para saber el número de palabras de la lista de palabras del diccionario
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
  {
  return [DictCore getSize];
  }

//-------------------------------------------------------------------------------------------------------------------------
// Se llama para conecer la palabra que se corresponde con la fila 'row'
- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
  {
  NSTableCellView* cel = [tableView makeViewWithIdentifier:@"DictKey" owner:tableView];
  
  NSString* sWord = [DictCore getWordAt:(int)row];
  
  cel.textField.stringValue = sWord;
  
  return cel;
  }

static bool NoSelChange;
//-------------------------------------------------------------------------------------------------------------------------
// Se llama cuando cambia la selección de la palabra actual en la lista de palabras del diccionario
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
  {
  if( NoSelChange ) return;
  
  int idx = (int)_WordsList.selectedRow;
  NSString* sKey = [DictCore getWordAt:idx];
  
  _sWord.stringValue = sKey;
  _btnWordRoots.hidden = true;
  
  [self UpdateWordDataAt:idx];                                      // Pone los datos de la palabra
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza el listado de palabras segun la palabra a buscar
- (void)FindActualWord
  {
  NoSelChange = true;
  
  if( ![self OpenDict] ) return;
  
  BOOL     First = FALSE;
  // Toma la palabra que esta en la vista de busqueda y le quita los espacios
  NSString *word  = [[_sWord stringValue] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
  
  if( word.length==0 )                                                // Si la palabra a buscar esta vacia
    {
    word = @"a";                                                      // Busca la 'a'
    First = TRUE;                                                     // Pone bandera de posicionamiento en la primera palabra
    }
  
  int Idx = [DictCore getWordIdx:word];                               // Obtiene el indice de la palabra en el diccionario
  
  if( [DictCore Found]  )                                             // Si la palabra fue encontrada
    {
    if( First )                                                       // Si solo se esta posicionando en la 'a'
      {
      _btnEditWord.hidden = TRUE;                                     // Esconde el boton de editar la palabra
      _btnDelChanges.hidden = TRUE;                                   // Esconde el boton de borrar cambios
      _WordInfo.string = @"";                                         // Pone la información de la palabra en blanco
      }
    else
      {
      [self UpdateWordDataAt:Idx];                                    // Pone los datos de la palabra
      }
    
    NSIndexSet* idxs = [NSIndexSet indexSetWithIndex:Idx ];           // Crea conjunto de indices (en el indice de la palabra)
    [_WordsList selectRowIndexes:idxs byExtendingSelection: NO];      // Selecciona la palabra
    
    _btnWordRoots.hidden = true;
    }
  else                                                                // Si no encontro la palabra
    {
    _btnEditWord.hidden = (word.length==0);                           // Solo oculta el botón de editar cuando no hay palabra
    _btnWordRoots.hidden = (word.length==0);
    _btnDelChanges.hidden = TRUE;                                     // Esconde el boton de borrar cambios
    [WrdData SetNewWord:word];                                        // Pone palabra nueva para editar
    _WordInfo.string = @"";                                           // Pone información de la palabra en blanco
    }
  
  // Posiciona la palabra actual en la parte superior de la lista
  float sep = _WordsList.intercellSpacing.height;                     // Determina el ecpacio entre las filas
  float y   = Idx * (_WordsList.rowHeight + sep) - sep;               // Determina posicion del scroll para la palabra
  
  [_WordsList scrollPoint: NSMakePoint(0, y) ];                       // Pon el scroll para que la palabra arriba
  
  NoSelChange = false;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Actualiza los datos de la palabra que se encuentra en el indice 'Idx'
- (void)UpdateWordDataAt:(int) Idx
  {
  NSAttributedString* wData = [DictCore getWDataFromIndex:Idx NoKey:TRUE];  // Obtine los datos de la llave en una cadena formateada
  
  [_WordInfo.textStorage setAttributedString: wData];
  
  _btnEditWord.hidden = [DictCore WordKey];                       // Oculta el boton de editar si es una palabra clave
  _btnDelChanges.hidden = ![DictCore isUserKeyIdx:Idx];           // Muestra boton borrar si la palabra esta en el diccionario de usuario
  }


@end

//=========================================================================================================================================================
