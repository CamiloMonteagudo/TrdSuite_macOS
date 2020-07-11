//=========================================================================================================================================================
//  EditDictCtrller.m
//  TrdMac
//
//  Created by Camilo on 2/28/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "EditDictCtrller.h"
#import "DictCore.h"
#import "ParseMeans.h"
#import "AppData.h"

//=========================================================================================================================================================
@interface EditDictCtrller ()
  {
  NSWindow* winParent;
  NSModalSession Session;
  
  NSString* Word;
  NSTextField* vwNowMean;         // Vista de significados que esta activa
  
  MeanItem* itemGen;              // Item que contine el genero del significado actual
  MeanItem* itemNum;              // Item que contine el número del significado actual
  MeanItem* itemEsp;              // Item que contine la especialidad del significado actual
  MeanItem* itemMean;             // Item que contine el significado actual
  
  NSBox*       DataBoxs[5];       // Cuadro de los significados para un tipo gramatical
  NSTextField* lbTypeDesc[5];     // Descrición del tipo gramatical para cada cuadro de datos
  NSString*    TypeCode[5];       // Código del tipo gramatical para cada cuadro de datos
  NSTextField* Means[5];          // Editor para cada una de las cadenas de significados posibles
  ParseMeans*  Parses[5];         // Datos parseados para cada cadena de significado
  int          nTipos;            // Número de tipos iniciales de la palabra
  
  NSMenu *TypesMenu;              // Menú con todos los tipod gramaticales disponibles
  NSMenu *EspMenu;                // Menú con todas las especalidades disponibles
  NSMenu *GenerMenu;              // Menú con todos los generos disponibles
  NSMenu *NumberMenu;             // Menú con todos los numeros disponibles
  
  NSColor* ColMeanText;           // Color del texto de los significado
  NSColor* ColMeanAttr;           // Color de los atributos del significado
  
  NSMutableDictionary* Cache;     // Cache de los sinnificados de un tipo gramatical
  
  SEL fnOnClose;                  // Función que se llama al terminar la edicción
  id  ObjFun;                     // Objeto al que pertenece la función
  }

@property (weak) IBOutlet NSButton* btnAdd;
@property (weak) IBOutlet NSButton* btnExit;
@property (weak) IBOutlet NSButton* btnSave;
@property (weak) IBOutlet NSButton* btnGenre;
@property (weak) IBOutlet NSButton* btnNumber;
@property (weak) IBOutlet NSButton* btnEspec;
@property (weak) IBOutlet NSTextField *WordTitle;

- (IBAction)AddType:(id)sender;
- (IBAction)OnExit:(id)sender;
- (IBAction)OnSave:(id)sender;
- (IBAction)OnDeleteType:(NSButton *)sender;
- (IBAction)OnChangeGenere:(id)sender;
- (IBAction)OnChangeNumber:(id)sender;
- (IBAction)OnChangeEspec:(id)sender;

@end

NSArray* TypesAbrev   = @[@"",@"SS"        , @"AA"       , @"NP"           , @"DD"      , @"VT"              , @"VI"                , @"VR"              , @"VA"              , @"PP"          , @"PT"                   , @"PI"                     , @"GT"                         , @"GI"                           , @"CC"          , @"JJ"          , @"AI"                                 ];
NSArray* TypesTitleEn = @[@"",@"Noun"      , @"Adjective", @"Proper noun"  , @"Adverb"  , @"Transitive Verb" , @"Intransitive Verb" , @"Reflexive Verb"  , @"Auxiliary Verb"  , @"Preposition" , @"Transitive Participle", @"Intransitive Participle", @"Transitive Gerund"          , @"Intransitive Gerund"          , @"Conjunction" , @"Interjection", @"Static adjective"                   ];
NSArray* TypesTitleEs = @[@"",@"Sustantivo", @"Adjetivo" , @"Nombre Propio", @"Adverbio", @"Verbo Transitivo", @"Verbo Intransitivo", @"Verbo Reflexivo" , @"Verbo Auxiliar"  , @"Preposición" , @"Participio Transitivo", @"Participio Intransitivo", @"Gerundio Transitivo"        , @"Gerundio Intransitivo"        , @"Conjunción"  , @"Interjección", @"Adjetivo Estático"                  ];
NSArray* TypesTitleIt = @[@"",@"Sostantivo", @"Aggettivo", @"Nome Proprio" , @"Avverbio", @"Verbo Transitivo", @"Verbo Intransitivo", @"Verbo Riflessivo", @"Verbo Ausiliare" , @"Preposizione", @"Participio Transitivo", @"Participio Intransitivo", @"Gerundio Transitivo"        , @"Gerundio Intransitivo"        , @"Congiunzione", @"Interiezione", @"Aggettivo con posizione invariabile"];
NSArray* TypesTitleFr = @[@"",@"Substantif", @"Adjectif" , @"Nom Propre"   , @"Adverbe" , @"Verbe Transitif" , @"Verbe Intransitif" , @"Verbe Réfléchi"  , @"Verbe Auxiliaire", @"Préposition" , @"Participe Transitif"  , @"Participe Intransitif"  , @"Participe Présent Transitif", @"Participe Présent Intransitif", @"Conjonction" , @"Interjection", @"Adjectif Fixe"                      ];

NSArray* TypesTitles[] = {TypesTitleEs, TypesTitleEn, TypesTitleIt, TypesTitleFr};
NSArray* TypesTitle;

NSArray* EspAbrev   = @[@"",@"GG"      , @"AE"          , @"AG"         , @"AN"      , @"AQ"          , @"AJ"  , @"AU"           , @"BI"      , @"BT"           , @"BO"       , @"CC"     , @"CL"           , @"CO"                    , @"CP"          , @"CS"      , @"DE"      , @"LA"     , @"AB"      , @"ED"          , @"EN"         , @"FA"       , @"FE"            , @"FG"        , @"FL"        , @"FI"      , @"GO"        , @"GR"        , @"HI"      , @"PR"        , @"IN"         , @"JE"     , @"LI"         , @"LO"     , @"NA"  , @"MA"          , @"ME"      , @"MT"          , @"ML"       , @"MN"         , @"MI"        , @"MU"     , @"PE"      , @"PO"       , @"EC"       , @"QM"      , @"SO"        , @"TP"         , @"VE"                 , @"ZO"      ];
NSArray* EspTitleEs = @[@"",@"General" , @"Aeronáutica" , @"Agricultura", @"Anatomía", @"Arquitectura", @"Arte", @"Automovilismo", @"Biología", @"Biotecnología", @"Botánica" , @"Cocina" , @"Coloquial"    , @"Comercio y economía"   , @"Computación" , @"Costura" , @"Deportes", @"Derecho", @"Ecología", @"Educación"   , @"Entomología", @"Farmacia" , @"Ferrocarriles" , @"Figurado"  , @"Filosofía" , @"Física"  , @"Geografía" , @"Gramática" , @"Historia", @"Impresión" , @"Ingeniería" , @"Jerga"  , @"Literatura" , @"Lógica" , @"Mar" , @"Matemáticas" , @"Medicina", @"Meteorología", @"Militar"  , @"Mineralogía", @"Mitología" , @"Música" , @"Poesía"  , @"Política" , @"Religión" , @"Química" , @"Sociología", @"Topografía" , @"Veterinaria"        , @"Zoología"];
NSArray* EspTitleEn = @[@"",@"General" , @"Aeronautics" , @"Agriculture", @"Anatomy" , @"Architecture", @"Art" , @"Automobile"   , @"Biology" , @"Biotechnology", @"Botany"   , @"Cooking", @"Colloquial"   , @"Commerce and Economics", @"Computer"    , @"Sewing"  , @"Sport"   , @"Law"    , @"Ecology" , @"Education"   , @"Entomology" , @"Pharmacy" , @"Railway"       , @"Figurative", @"Philosophy", @"Physics" , @"Geography" , @"Grammar"   , @"History" , @"Print"     , @"Engineering", @"Slang"  , @"Literature" , @"Logic"  , @"Sea" , @"Mathematics" , @"Medicine", @"Meteorology" , @"Military" , @"Mineralogy" , @"Mythology" , @"Music"  , @"Poetical", @"Politics" , @"Religion" , @"Chemical", @"Sociology" , @"Topography" , @"Veterinary medicine", @"Zoology" ];
NSArray* EspTitleIt = @[@"",@"Generale", @"Areonautica" , @"Agricoltura", @"Anatomia", @"Architettura", @"Arte", @"Automobilismo", @"Biologia", @"Biotecnologia", @"Botanica" , @"Cucina" , @"Colloquiale"  , @"Commercio ed economia" , @"Informatica" , @"Sartoria", @"Sport"   , @"Diritto", @"Ecologia", @"Educazione"  , @"Entomologia", @"Farmacia" , @"Ferroviaria"   , @"Figurata"  , @"Filosofia" , @"Fisica"  , @"Geografia" , @"Grammatica", @"Storia"  , @"Stampa"    , @"Ingenieria" , @"Tessile", @"Letteratura", @"Logica" , @"Mare", @"Matematica"  , @"Medicina", @"Meteorologia", @"Militare" , @"Mineralogia", @"Mitologia" , @"Musica" , @"Poesia"  , @"Politica" , @"Religione", @"Chimica" , @"Sociologia", @"Topografia" , @"Veterinaria"        , @"Zoologia"];
NSArray* EspTitleFr = @[@"",@"Général" , @"Aéronautique", @"Agriculture", @"Anatomie", @"Architecture", @"Art" , @"Aumobilisme"  , @"Biologie", @"Biotecnologie", @"Botanique", @"Cuisine", @"Langue parlée", @"Commerce et Économie"  , @"Informatique", @"Couture" , @"Sports"  , @"Droit"  , @"Ecologie", @"Enseignement", @"Entomologie", @"Pharmacie", @"Chemins de fer", @"Figuré"    , @"Philosofie", @"Physique", @"Géographie", @"Grammaire" , @"Histoire", @"Impression", @"Génie"      , @"Jargon" , @"Littérature", @"Logique", @"Mar" , @"Mathématique", @"Médecine", @"Metéorologie", @"Militaire", @"Minéralogie", @"Mythologie", @"Musique", @"Poesie"  , @"Politique", @"Réligion" , @"Chimie"  , @"Sociologie", @"Topographie", @"Vétérinaire"        , @"Zoologie"];

NSArray* EspTitles[] = {EspTitleEs, EspTitleEn, EspTitleIt, EspTitleFr};
NSArray* EspTitle;

//=========================================================================================================================================================
@implementation EditDictCtrller

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (id)initInWindow:(NSWindow*) window OnClose:(SEL) fn Target:(id) target
  {
  TypesTitle = TypesTitles[iUser];
  EspTitle   = EspTitles[iUser];
  
  fnOnClose = fn;                                 // Función que se llama al terminar la edicción
  ObjFun    = target;                             // Objeto al que pertenece la función

  ColMeanText = [NSColor blackColor];
  ColMeanAttr = [NSColor blueColor ];
  
  Cache = [NSMutableDictionary new];
  
  self = [super initWithWindowNibName:@"EditDictCtrller"];
  if( !self ) return nil;
  
  winParent = window;
  [self loadWindow];
  [self GetSubViews];
  
  [winParent beginSheet:self.window completionHandler:nil];
  
  Session = [NSApp beginModalSessionForWindow:self.window];
  
  [self CreateGenerMenu];
  [self CreateNumberMenu];
  [self CreateEspMenu];
  
  _WordTitle.stringValue = [WrdData GetWord];
  
  nTipos = [WrdData CountTypes];
  [self UpdateWordData];

  if( nTipos==0 ) [self AddType:nil];

  return self;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene todas la vista necesarias
- (void)GetSubViews
  {
  int base = 0;
  NSView* view = self.window.contentView;
  for (int i=0; i<5; ++i)
    {
    NSBox* Box = view.subviews[i];
    
    DataBoxs[i] = Box;
    
    lbTypeDesc[i] = [Box viewWithTag:base+1];
    Means[i]      = [Box viewWithTag:base+4];
    Parses[i]     = [ParseMeans ParseWithString:@""];
    
    base += 10;
    }
  
  [self SetNotications];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void) SetNotications
  {
  NSNotificationCenter* Nfc = [NSNotificationCenter defaultCenter];
  
  NSTextView* Ed = (NSTextView*)[self.window fieldEditor:YES forObject:nil ];
  [Nfc addObserver:self selector:@selector(TextChangeSelection:) name:NSTextViewDidChangeSelectionNotification object:Ed];
  
  for (int i=0; i<5; ++i)
    [Nfc addObserver:self selector:@selector(TextDidChange:) name:NSControlTextDidChangeNotification object:Means[i]];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void) RemoveEditNotications
  {
  NSNotificationCenter* Nfc = [NSNotificationCenter defaultCenter];
  
  NSTextView* Ed = (NSTextView*)[self.window fieldEditor:YES forObject:nil ];
  [Nfc removeObserver:self name:NSTextViewDidChangeSelectionNotification object:Ed];
  
  for (int i=0; i<5; ++i)
    [Nfc removeObserver:self name:NSControlTextDidChangeNotification object:Means[i]];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra los datos de los tipos gramaticales para la palabra actual
- (void) UpdateWordData
  {
  int nTypes = [WrdData CountTypes];
  for( int i=0; i<nTypes && i<5; ++i )
    {
    NSString* typCode = [WrdData CodeFromType:i];
    NSString* typDesc = [self TypeDescWithCode:typCode];
    
    NSString* sLabel = NSLocalizedString(@"TrdAsTipo"   , @"");
    lbTypeDesc[i].attributedStringValue = EdictTypeLabel(sLabel, typDesc);
    TypeCode[i] = typCode;
    
    NSTextField* Mean = Means[i];
    [Mean becomeFirstResponder];
    
    NSAttributedString* sData = Cache[ typCode ];
    if( sData!=nil )
      {
      Mean.attributedStringValue = sData;
      }
    else
      {
      Mean.hidden = FALSE;
      Mean.stringValue = [WrdData MeansStringFromType:i];
      [self MarkAttrInText];
      }
    
    [Mean.currentEditor setSelectedRange:NSMakeRange(0, 0) ];
    }
  
  for( int i=0; i<5; ++i )
    DataBoxs[i].hidden = (i>=nTypes);
  
  [self CreateGramTypesMenu];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cachea todos los datos de los significados que haya hasta el momento
- (void) CacheTypeMeans
  {
  for( int i=0; i<5; ++i )
    {
    if( DataBoxs[i].isHidden ) break;
    
    NSString* sType1 = [WrdData CodeFromType:i];
    NSString* sType  = TypeCode[i];
    if( ![sType isEqualToString:sType1] )
      NSLog(@"No coinciden los tipos");
    
    [Cache setObject:Means[i].attributedStringValue forKey:sType];
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la descripción del tipo gramatical para el código dado
- (NSString*) TypeDescWithCode:(NSString*) typCode
  {
  for (int i=0; i< TypesAbrev.count; ++i)
    {
    NSString* NowCode = TypesAbrev[i];
    if( [NowCode isEqualToString:typCode] )
      return TypesTitle[i];
    }
  
  return @"";
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cada vez que se cambia el texto de un significado
- (void)TextDidChange:(NSNotification *)notification
  {
  [self MarkAttrInText];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pone los atributos al texto que se esta editando en el momento de llamada
- (void) MarkAttrInText
  {
  int idx =  (int)vwNowMean.tag/10;
  NSText*  Text = vwNowMean.currentEditor;
  NSString* str = Text.string;
  
  [Text setTextColor:ColMeanText range:NSMakeRange(0, str.length)];
  
  ParseMeans* ps = Parses[idx];
  [ps ParseString:str];
  
  for (MeanItem* item  in ps.Items)
    {
    if( item.tipo != ITEM_MEAN )
      [Text setTextColor:ColMeanAttr range:item.rg ];
    }
  }

BOOL inSelText;
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cada ves que se cambia la seleccion del texto (cuando se mueve el cursor)
- (void)TextChangeSelection:(NSNotification *)notification
  {
  if( inSelText ) return;
  inSelText = true;
  
  if( vwNowMean.currentEditor != nil )
    {
    inSelText = FALSE;
    return;
    }
  
  for( int i=0; i<5; ++i )
    if( Means[i].currentEditor != nil )
      {
      vwNowMean = Means[i];
      [self CheckButonsState];
      inSelText = FALSE;
      return;
      }
  
  inSelText = FALSE;
  vwNowMean = nil;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene los atributos del significado donde se encuentra el cursor
- (void) GetActualMeanAttibutes
  {
  if( vwNowMean == nil ) return;
  
  NSText*  Text = vwNowMean.currentEditor;
  NSString* str = Text.string;
  NSInteger pos = Text.selectedRange.location;
  int idx =  (int)vwNowMean.tag/10;
  
  ParseMeans* ps = Parses[idx];
  [ps ParseString:str];
  
  itemMean = itemEsp = itemGen = itemNum = nil;
  for( MeanItem* item  in ps.Items )
    {
         if( item.tipo == ITEM_ESP  ) itemEsp = item;
    else if( item.tipo == ITEM_GEN  ) itemGen = item;
    else if( item.tipo == ITEM_NUM  ) itemNum = item;
    else if( item.tipo == ITEM_MEAN )
      {
      itemMean = item;
      NSRange rg = item.rg;
      NSInteger nowPos = rg.location + rg.length;
    
      if( pos<=nowPos )  return;
      
      itemEsp = itemGen = itemNum = nil;
      }
    }
  
  NSRange rg = NSMakeRange(str.length, 0);
  itemMean = [MeanItem ItemWithRange:rg Type:ITEM_MEAN Value:@""];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Borra el atributo que se encuentra en el rango rg
- (void)DeleteAttrAtRange:(NSRange) rg
  {
  NSText*  Text = vwNowMean.currentEditor;
  [Text replaceCharactersInRange:rg withString:@""];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Adicona el atributo con 'sVal' delante del significado actual
- (void)AddAttr:(NSString*) sVal
  {
  NSInteger pos = itemMean? itemMean.rg.location : 0;
  NSRange    rg = NSMakeRange(pos, 0);
  
  NSText*  Text = vwNowMean.currentEditor;
  [Text replaceCharactersInRange:rg withString:sVal];
  [self MarkAttrInText];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cambia la que hay en el rango 'rg' por el atributo con 'sVal'
- (void)ChangeAttr:(NSString*) sVal InRange:(NSRange) rg
  {
  NSText*  Text = vwNowMean.currentEditor;
  [Text replaceCharactersInRange:rg withString:sVal];
  [self MarkAttrInText];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Establece el estado de los botones de genero y número de acuerdo al tipo gramatical actual
- (void) CheckButonsState
  {
  int idx =  (int)vwNowMean.tag/10;
  
  NSString* sType1 = [WrdData CodeFromType:idx];
  NSString* sType  = TypeCode[idx];
  if( ![sType isEqualToString:sType1] )
    NSLog(@"No coinciden los tipos");
  
  BOOL show = [sType isEqualToString:@"SS"] || [sType isEqualToString:@"AA"] || [sType isEqualToString:@"NP"];
  
  _btnGenre.hidden  = !show;
  _btnNumber.hidden = !show;
  
  [vwNowMean becomeFirstResponder];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea el menú con los tipos gramaticales disponibles
- (void) CreateGramTypesMenu
  {
  TypesMenu = [[NSMenu alloc] init];
  
  for( int i=0; i<TypesTitle.count; ++i )
    {
    if( ![WrdData IsValidAddType:TypesAbrev[i]] )
      continue;
    
    NSMenuItem* Item = [[NSMenuItem alloc] init ];
    
    Item.title = TypesTitle[i];
    Item.tag = i;
    Item.target = self;
    Item.action = @selector(OnAddNewType:);
    //    Item.state = NSOnState;
    
    [TypesMenu addItem:Item];
    }
  
  _btnAdd.hidden = ( TypesMenu.itemArray.count==0 || [WrdData CountTypes]>=5 );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona un tipo gramatical
- (void)OnAddNewType:(id)sender
  {
  [self CacheTypeMeans];
  
  int  iType = (int)((NSMenuItem*)sender).tag;                // Obtiene dirección del item seleccionado
  NSString* cType = TypesAbrev[iType];
  
  [WrdData AddType:cType];
  if( [cType isEqualToString:@"NP"] )
    [WrdData AddMean:[WrdData GetWord] InType:cType];

  [self UpdateWordData];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un menú con todas las especialidades disponibles
- (void) CreateEspMenu
  {
  EspMenu = [self CreateMenuWithTitles:EspTitle
                              AnAction:@selector(OnSelEsp:) ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona una especialidad
- (void)OnSelEsp:(id)sender
  {
  NSMenuItem* Item = sender;
  if( Item.state == NSOnState ) return;
  
  int iEsp = (int)Item.tag;
  if( itemEsp && iEsp<=1 )
    {
    [self DeleteAttrAtRange: itemEsp.rg];
    return;
    }
  
  NSString* sEsp = [NSString stringWithFormat:@"%@ ", EspAbrev[iEsp]];
    
  if( !itemEsp ) [self AddAttr:sEsp];
  else           [self ChangeAttr:sEsp InRange:itemEsp.rg];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un menu con los generos
- (void) CreateGenerMenu
  {
  NSString* sMac = NSLocalizedString(@"mnuItemMasc", @"");
  NSString* sFem = NSLocalizedString(@"mnuItemFem" , @"");
  NSString* sNtr = NSLocalizedString(@"mnuItemNtr" , @"");
  
  GenerMenu = [self CreateMenuWithTitles:@[@"",sMac,sFem,sNtr]
                                 AnAction:@selector(OnSelGener:) ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona un genero
- (void)OnSelGener:(id)sender
  {
  NSMenuItem* Item = sender;
  if( Item.state == NSOnState ) return;
  
  int iGen = (int)Item.tag;                // Obtiene dirección del item seleccionado
  
  if( itemGen && iGen<=1)
    {
    [self DeleteAttrAtRange: itemGen.rg];
    return;
    }
  
  NSString* sGen = ( iGen==2 )? @"f. " : @"n. ";
  if( !itemGen ) [self AddAttr:sGen];
  else           [self ChangeAttr:sGen InRange:itemGen.rg];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un menú con los números
- (void) CreateNumberMenu
  {
  NSString* sSig = NSLocalizedString(@"mnuItemSng", @"");
  NSString* sPlr = NSLocalizedString(@"mnuItemPlr", @"");
  
  NumberMenu = [self CreateMenuWithTitles:@[@"",sSig,sPlr]
                                 AnAction:@selector(OnSelNumber:) ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un menú con una lista de items y le asigna una acción
- (NSMenu*) CreateMenuWithTitles:(NSArray*) Titles AnAction:(SEL) action
  {
  NSMenu* Mnu = [[NSMenu alloc] initWithTitle:@""];
  
  for( int i=0; i<Titles.count; ++i )
    {
    NSMenuItem* Item = [[NSMenuItem alloc] init ];
    
    Item.title = Titles[i];
    Item.tag = i;
    Item.target = self;
    Item.action = action;
    
    [Mnu insertItem:Item atIndex:i];
    }
  
  return Mnu;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona una dirección de traducción
- (void)OnSelNumber:(id)sender
  {
  NSMenuItem* Item = sender;
  if( Item.state == NSOnState ) return;
  
  int iNum = (int)Item.tag;                // Obtiene dirección del item seleccionado
  if( itemNum )
    [self DeleteAttrAtRange: itemNum.rg];
  else if( iNum==2 )
    [self AddAttr:@"p. "];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra el menú 'Mnu' sobre el boton 'Bnt'
- (void)ShowMenu:(NSMenu*) Mnu AtButton:(NSView*) Bnt
  {
  NSPopUpButtonCell* cel = [[NSPopUpButtonCell alloc] initTextCell:@"" pullsDown:YES];
  cel.menu = Mnu;
  
  NSRect rc = NSMakeRect(0, -Mnu.size.height, 0, 0);

  [cel performClickWithFrame:rc inView:Bnt];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de adicionar tipo
- (IBAction)AddType:(id)sender
  {
  [self ShowMenu:TypesMenu AtButton:_btnAdd];     // Se muestra la lista de tipos disponibles
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de salvar
- (IBAction)OnSave:(id)sender
  {
  int nTypes = [WrdData CountTypes];
  
  BOOL save = ( nTypes != nTipos );
  for( int i=0; i<nTypes && i<5; ++i )
    {
    NSString* nowMeans = Means[i].stringValue;
    
    NSString* srcMeans = [WrdData MeansStringFromType:i];
    if( nowMeans.length==0 || ![srcMeans isEqualToString:nowMeans] )
      {
      save = TRUE;
      if( ![WrdData MeansString:nowMeans ToType:i] )
        {
        [self ShowFormatErrorInType:i];
        return;
        }
      }
    }
  
  if( save )
    {
    [WrdData SaveData];
    if( fnOnClose && ObjFun )                                                     // Si se establecio a quien notificar
      {
      NSThread* nowThread = [NSThread currentThread];
      [ObjFun performSelector:fnOnClose onThread:nowThread withObject:nil waitUntilDone:NO];       // Realiza la notificacion
      }
    }
  
  [self OnExit:sender];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra un mensaje de error cuando el formato de la definición de significados para un tipo es incorrecto
- (void) ShowFormatErrorInType:(int)iType
  {
  NSString* typCode = [WrdData CodeFromType:iType];
  NSString* typDesc = [self TypeDescWithCode:typCode];
  
  NSString* str = NSLocalizedString(@"DataFrmError", @"");
  
  NSAlert* msg = [NSAlert new];
  msg.informativeText = [NSString stringWithFormat:str, typDesc];
  msg.messageText = NSLocalizedString(@"Error", @"");
  msg.alertStyle = NSCriticalAlertStyle;
  
  [msg beginSheetModalForWindow:self.window completionHandler:nil];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (IBAction)OnDeleteType:(NSButton *)sender
  {
  [self CacheTypeMeans];
  
  int idx = (int)sender.tag/10;
  
  NSString* sType1 = [WrdData CodeFromType:idx];
  NSString* sType  = TypeCode[idx];
  if( ![sType isEqualToString:sType1] )
    NSLog(@"No coinciden los tipos");
  
  [WrdData RemoveType:sType];
  [self UpdateWordData];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (IBAction)OnChangeGenere:(id)sender
  {
  [self GetActualMeanAttibutes];
  
  int idx = (itemGen==nil)? 1 : ([itemGen.sVal isEqualToString:@"f"]? 2 : 3);
  
  [self CheckMenu:GenerMenu AtItem:idx];
  
  [self ShowMenu:GenerMenu AtButton:_btnGenre];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (IBAction)OnChangeNumber:(id)sender
  {
  [self GetActualMeanAttibutes];
  
  int idx = (itemNum==nil)? 1 : 2;
  
  [self CheckMenu:NumberMenu AtItem:idx];
  
  [self ShowMenu:NumberMenu AtButton:_btnNumber];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (IBAction)OnChangeEspec:(id)sender
  {
  [self GetActualMeanAttibutes];
  
  int idx = 1;
  if( itemEsp!= nil )
    {
    NSString* sEsp = itemEsp.sVal;
    for( int i=0; i<EspAbrev.count; ++i )
      if( [EspAbrev[i] isEqualToString:sEsp] )
        {
        idx = i;
        break;
        }
    }

  [self CheckMenu:EspMenu AtItem:idx];
  
  [self ShowMenu:EspMenu AtButton:_btnEspec];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (IBAction)OnExit:(id)sender
  {
  [self RemoveEditNotications];
  
  [winParent endSheet:self.window];
  [NSApp endModalSession:Session];
  
  Session = nil;
  
  [self.window orderOut:self];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void) CheckMenu:(NSMenu*) Mnu AtItem:(int) idx
  {
  NSArray* Items = Mnu.itemArray;
  
  for( int i=0; i<Items.count; ++i )
    {
    NSMenuItem* Item = Items[i];
    Item.state = (i==idx)? NSOnState : NSOffState;
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
@end

//=========================================================================================================================================================
