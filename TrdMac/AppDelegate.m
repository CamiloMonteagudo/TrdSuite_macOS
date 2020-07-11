//=========================================================================================================================================================
//  AppDelegate.m
//  TrdMac
//
//  Created by Camilo on 1/29/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "AppDelegate.h"
#import "Translate.h"
#import "Util.h"
#import "TrdCore.h"
#import "SyncScrollView.h"
#import "AppData.h"

//=========================================================================================================================================================
@interface AppDelegate ()
  {
  BOOL SaveTrd;                         // Indica que hay que intentar guardar el texto traducido
  BOOL SaveSrc;                         // Indica que hay que intentar guardar el texto traducido
  int  SaveTipo;                        // Indica el tipo de salva, 0- Normal, 1- Cerrando App, 2-Nuevo documento, 2-Abrir un documento
  }

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet Translate *TrdView;
@property (weak)   IBOutlet SyncScrollView *ScrollSrc;
@property (weak)   IBOutlet SyncScrollView *ScrollTrd;
@property (weak)   IBOutlet NSPopUpButton *CbLangs;


- (IBAction)OpenFile:(id)sender;
- (IBAction)NewDocument:(id)sender;

@end

//=========================================================================================================================================================
@implementation AppDelegate

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
  {
  [self LoadUserDefaults];
  [self FillLanguajesCombo];
  
  [self SelectDirWithSrc:LGSrc AndDes:LGDes];
  [TrdCore initTrdApi];
  
  [_TrdView NewText];
  
//  NSString* lg = [[NSLocale currentLocale] identifier];
//  NSString* name = [[NSLocale currentLocale] displayNameForKey:NSLocaleIdentifier value:lg ];
//  
//  NSLog(@"'%@'-'%@'", lg, name);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama después de cargar el XIB files
- (void)awakeFromNib
  {
  [_TrdView.TextSrc scaleUnitSquareToSize:NSMakeSize(1.5, 1.5)];
  [_TrdView.TextTrd scaleUnitSquareToSize:NSMakeSize(1.5, 1.5)];
  
  _ScrollSrc.SyncTo = _ScrollTrd;
  _ScrollTrd.SyncTo = _ScrollSrc;
  
  _TrdView.window = _window;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Carga los valores por defecto de algunos parametros
-(void) LoadUserDefaults
  {
  NSUserDefaults* UserDef = [NSUserDefaults standardUserDefaults];
  
  NSNumber* pDir = [UserDef objectForKey:@"lastDir"];       // Última dirección de traducción utilizada
  if( pDir != nil )                                         // Si se obtuvo la dirección
    {
    int iDir = pDir.intValue;                               // Obtiene el valor
    
    LGSrc = DIRSrc(iDir);                                   // Obtiene el idioma fuente de la dirección
    LGDes = DIRDes(iDir);                                   // Obtiene el idioma destino de la dirección
    }
  else DIRFirst();                                          // Obtiene la primera dirección de traducción instalada
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Llena el combo de idiomas con los idiomas disponible
- (void) FillLanguajesCombo
  {
  [_CbLangs removeAllItems];                                // Borra todos los items del como de idioma
  
  for(int i=0; i<DIRCount; i++)                             // Recorre todas las direcciones de traducción
    [self AddInstalledDir:i];                               // Agrega las direcciones que ya estan instaladas

  if( _CbLangs.numberOfItems == DIRCount ) return;          // Si todas las direcciones estan instaladas, termina

  [_CbLangs.menu addItem:[NSMenuItem separatorItem] ];      // Adiciona un item separador
  
  for(int i=0; i<DIRCount; i++)                             // Recorre todas las direcciones de traducción
    [self AddPurchaseDir:i];                                // Agrega la dirección para comprar
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Adicciona la dirección 'iDir' al combo de idiomas si esta instalada
- (void)AddInstalledDir:(int) iDir
  {
  if( !DIRInstalled(iDir) ) return;                         // Si la dirección no esta instalada no hace nada
  
  NSString* sDir = DIRName(iDir);                           // Obtiene el nombre de la dirección
  
  [_CbLangs addItemWithTitle:sDir];                         // La adicona al combo de idiomas
  
  NSMenuItem* Item = _CbLangs.lastItem;                     // Obtiene el item añadido al menú
  Item.tag = iDir;                                          // Le pone la dirección que representa
  
  Item.target = self;                                       // Pone objeto para reportar las acciones
  Item.action = @selector(OnSelDir:);                       // Pone la función a la que se reporta la acción
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona una dirección de traducción
- (void)OnSelDir:(id)sender
  {
  int iDir = (int)((NSMenuItem*)sender).tag;                // Obtiene dirección del item seleccionado
  
  LGSrc = DIRSrc(iDir);                                     // Obtiene idioma fuente de la dirección
  LGDes = DIRDes(iDir);                                     // Obtiene idioma destino de la dirección
  
  // Guarda dirección seleccionada en los datos del usuario
  NSUserDefaults* UserDef = [NSUserDefaults standardUserDefaults];
  [UserDef setObject:[NSNumber numberWithInt:iDir] forKey:@"lastDir"];
  
  // Informa que se cambio la direccion de traducción
  NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
  [center postNotificationName:DirChangedNotification object:self];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

NSColor* col2 = [NSColor colorWithRed:0.5 green:0.1 blue:0.1 alpha:1.0];
NSFont*  Font = [NSFont systemFontOfSize:16];

NSDictionary* attrPurchaseItem = @{ NSForegroundColorAttributeName:col2, NSFontAttributeName:Font};

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Adicciona la dirección 'iDir' al combo de idiomas si no esta instalada
- (void)AddPurchaseDir:(int) iDir
  {
  if( DIRInstalled(iDir) ) return;                            // Si la direccion ya esta instalada, no hace nada
  
  NSString* sDir = DIRName(iDir);                             // Obtiene el nombre de la dirección
  
  [_CbLangs addItemWithTitle:sDir];                           // La adicona al combo de idiomas
  
  NSMenuItem* Item = _CbLangs.lastItem;                       // Obtien el item adicionado al combo de idiomas
  Item.tag = iDir;                                            // Pone la dirección que retpresenta
  
  // Pone atributos para mostrar el item del menú
  Item.attributedTitle = [[NSAttributedString alloc] initWithString:sDir attributes:attrPurchaseItem];
  
  Item.target = self;                                         // Pone objeto para reportar acciones
  Item.action = @selector(OnPucharseDir:);                    // Pone la función a la que se reporta la acción
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se selecciona una dirección de traducción para compar
- (void)OnPucharseDir:(id)sender
  {
  [self SelectDirWithSrc:LGSrc AndDes:LGDes];
  
//  NSInteger iDir = ((NSMenuItem*)sender).tag;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Selecciona en el combo de idiomas las dirección con los idiomas dados
- (void) SelectDirWithSrc:(int) src AndDes:(int) des
  {
  int iDir = DIRFromLangs( src, des );
  [_CbLangs selectItemWithTag:iDir];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando carga un documento desde el Finder o desde la lista de documentos recientes
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
  {
  [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
  
  NSURL* url = [NSURL fileURLWithPath:filename];
  [self LoadDocument:url];
  
  return YES;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Carga un documento con el panel de selección, desde el menú o desde el Toolbar
- (IBAction)OpenFile:(id)sender
  {
  NSOpenPanel* panel = [NSOpenPanel openPanel];
  NSArray* fileTypes = [NSAttributedString textTypes];
  
  fileTypes = [fileTypes arrayByAddingObject:@"pdf"];
  
  [panel setAllowedFileTypes:fileTypes];
  
  [panel beginSheetModalForWindow:_window completionHandler:^(NSInteger result)
    {
    if( result == 0 ) return;
    
    [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(EndTime:) userInfo:[panel URL] repeats:NO];
    }];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Al final del tiempo ejecuta la proxima etapa, si fue indicado
- (void)EndTime: (NSTimer *) timer
  {
  [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
  
  NSURL* url = timer.userInfo;
  
  [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:url];
  
  [self LoadDocument:url];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un documento vacio
- (IBAction)NewDocument:(id)sender
  {
  [self ClearDocument];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se cierra la ultima ventana de la aplicación, para confirmar que se cierre la aplicación
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
  {
  return YES;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando la aplicación va a terminar
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
  {
  if( SaveTipo==1 ) return NSTerminateNow;
  
  SaveTipo = 1;                           // Salva en proceso de cierre de la aplicación
  SaveTrd  = TRUE;
  SaveSrc  = TRUE;
  
  if( [self CheckForSave] ) return NSTerminateNow;
  else                      return NSTerminateCancel;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se cierra la ventana
- (BOOL)windowShouldClose:(id)sender
  {
  if( SaveTipo==1 ) return TRUE;
  
  SaveTipo = 1;                           // Salva en proceso de cierre de la aplicación
  SaveTrd = TRUE;
  SaveSrc = TRUE;
  
  return [self CheckForSave];             // Si no hay ninguna salva, cierra la ventana
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Chequea si hay que guardar algún texto y luego limpia el texto original y el traducido
- (void) ClearDocument
  {
  SaveTipo = 2;                           // Salva durante la creación de un documento en blanco
  SaveTrd = TRUE;
  SaveSrc = TRUE;
  
  _FileUrl = nil;
  if( [self CheckForSave] )               // Si no habia nada que guardar
    {
    [_TrdView NewText];                   // Crea el texto nuevo
    SaveTipo = 0;                         // Pone el modo de salva normal
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Chequea si hay que guardar algún texto y luego limpia el texto original y el traducido
- (void) LoadDocument:(NSURL*) url
  {
  SaveTipo = 3;                           // Salva durante el proceso de cargar otro documento
  SaveTrd = TRUE;
  SaveSrc = TRUE;
  
  _FileUrl = url;
  if( [self CheckForSave] )               // Si no hay nada que guardar
    {
    [_TrdView OpenTextAtUrl:_FileUrl];    // Abre el texto desde la url dada
    SaveTipo = 0;                         // Pone el modo se salva normal
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Chequea si es necesario guardar algún documento, de ser necesario muestra un dialogo al usuario para que decida
- (BOOL) CheckForSave
  {
  if( _TrdView.TrdModify && SaveTrd)      // Si se ha modificado el texto traducido y hay que guardarlo
    {
    [self AskForSaveForTextTrd:TRUE];     // Se solicita al usuario que hacer
    return FALSE;                         // Indica que hay que esperar por la salva
    }
  
  if( _TrdView.SrcModify && SaveSrc)      // Si se ha modificado el texto original y hay que guardarlo
    {
    [self AskForSaveForTextTrd:FALSE];    // Se solicita al usuario que hacer
    return FALSE;                         // Indica que hay que esperar por la salva
    }
  
  return TRUE;                            // No se realizo ninguna salva
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pregunta al usuario si quiere guardar el documento, ignoralo o cancelar la operación
- (void)AskForSaveForTextTrd:(BOOL) trd
  {
  NSString* txt = trd? @"TextTrdNotSaved" : @"TextSrcNotSaved";

  NSAlert *alert = [[NSAlert alloc] init];
  
  [alert addButtonWithTitle: NSLocalizedString(@"DontSave"    , @"")];
  [alert addButtonWithTitle: NSLocalizedString(@"Save"        , @"")];
  [alert addButtonWithTitle: NSLocalizedString(@"Cancel"      , @"")];
  [alert setMessageText:     NSLocalizedString(@"WarningTitle", @"")];
  [alert setInformativeText: NSLocalizedString(txt            , @"")];
  
  [alert setAlertStyle:NSWarningAlertStyle];
  
  [alert beginSheetModalForWindow:_window completionHandler:^(NSModalResponse returnCode)
   {
   [alert.window orderOut:nil];
   
   if( returnCode == NSAlertFirstButtonReturn )               // No guarda el texto
     {
     if( trd ) SaveTrd = FALSE;                               // Deciste de guardar el texto traducido
     else      SaveSrc = FALSE;                               // Deciste de guardar el texto fuente
     
     [self AfterOfSave];                                      // Termina el proceso de salva
     }
   else if( returnCode == NSAlertSecondButtonReturn )         // Salva el texto
     {
     [_TrdView SaveTextTrd: trd];                             // Manda a guardar el texto
     }
   else                                                       // Cancela la operación
     {
     SaveTipo = 0;                                            // Pasa al estado de salva normal
     }
   }];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama después de haber guardado el documento
- (void) AfterOfSave
  {
  if( SaveTipo==0 ) return;                                   // Para la salva normal, no hace nada
  
  if( [self CheckForSave] )                                   // Chequea si queda algo por guardar
    {                                                         // Si no queda nada por guardar, continua proceso que origino la salva
    if( SaveTipo==1 ) [NSApp terminate:nil];                  // Cierra la aplicacion
    if( SaveTipo==2 ) [_TrdView NewText];                     // Crea un fichero nuevo
    if( SaveTipo==3 ) [_TrdView OpenTextAtUrl:_FileUrl];      // Abre un documento nuevo
      
    SaveTipo = 0;                                             // Pasa al estado de salva normal
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra un mensaje con un texto a usuario
+ (void)ShowMsgText:(NSString*) txt InWindow:(NSWindow*) Wnd
  {
  NSAlert *alert = [[NSAlert alloc] init];
  
  [alert addButtonWithTitle: NSLocalizedString(@"OK"          , @"")];
  [alert setMessageText:     NSLocalizedString(@"WarningTitle", @"")];
  [alert setInformativeText: NSLocalizedString(txt            , @"")];
  
  [alert setAlertStyle:NSWarningAlertStyle];
  
  [alert beginSheetModalForWindow:Wnd completionHandler:^(NSModalResponse returnCode)
    {
    [alert.window orderOut:nil];
    }];
  }

@end
//=========================================================================================================================================================

