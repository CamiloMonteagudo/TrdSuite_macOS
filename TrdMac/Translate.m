//=========================================================================================================================================================
//  Translate.m
//  TrdMac
//
//  Created by Camilo on 1/30/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "Translate.h"
#import "TrdCore.h"
#import "ParseText.h"
#import "ToolsCrtller.h"
#import "EditUtils.h"
#import "SyncScrollView.h"
#import "AppData.h"
#import "ProgressCtrller.h"
#include "../PDFLibrary/PdfFilter/PdfToRtf.h"
#include "../PDFLibrary/PdfFilter/Errors.h"

//=========================================================================================================================================================
@interface Translate()
  {
  NSRange rgSrcMark;
  NSRange rgTrdMark;
  
  int     LastOraMark;
  
  NSDictionary* attrMatch1;
  NSDictionary* attrMatch2;
  
  BOOL edSrc;                             // Indica si se esta editando el texto fuente o el traducido
  BOOL showInfo;                          // Indica si se muestra la información de la palabra seleccionada
  
  ProgressCtrller* Progress;
  }

@property (nonatomic) BOOL modify;

@property (weak) IBOutlet ToolsCrtller *PanelTools;   // Panel de herramientas (Dicionario, conjugación)

- (IBAction)OnTranslate:(id)sender;
- (IBAction)OnNextTrd:(id)sender;

@end

//=========================================================================================================================================================
@implementation Translate

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se termina de cargar el Nib de fichero
- (void)awakeFromNib
  {
  NSColor* col1 = [NSColor colorWithRed:1.0 green:1.0 blue:0.0 alpha:1.0];
  NSColor* col2 = [NSColor colorWithRed:1.0 green:0.8 blue:0.8 alpha:1.0];
  
  attrMatch1 = [NSDictionary dictionaryWithObjectsAndKeys:col1, NSBackgroundColorAttributeName, nil];
  attrMatch2 = [NSDictionary dictionaryWithObjectsAndKeys:col2, NSBackgroundColorAttributeName, nil];
  
  [ParseText Now];                            // Inicializa los datos estaticos
  
  _TextSrc.delegate = self;
  _TextTrd.delegate = self;
  
  showInfo = TRUE;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el contenido del fichero localizado en 'url' y lo pone en la vista del texto original
-(BOOL) OpenTextAtUrl:(NSURL*) url
  {
  [self NewText];
  
  NSString* ext = [url pathExtension];
  if( [ext caseInsensitiveCompare:@"pdf"] == NSOrderedSame )      //El fichero es un pdf, iniciar el proceso de conversion
    return [self OpenPDFAtUrl:url];
  
  NSDictionary* Opts = [NSDictionary new];
  
  _modify = FALSE;
  
  BOOL ret = [_TextSrc.textStorage readFromURL:url options:Opts documentAttributes:nil error:nil];
  return ret;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Habre un fichero PDF y pone su contenido en el texto fuente
-(BOOL) OpenPDFAtUrl:(NSURL*) url
  {
  long idProc = InitProcess();
  
  LoadImages(idProc, false, false, false, false);
  
  char* sRtf = NULL;
  long sLen = 0;
  const char* szPDFFile = [[url path] cStringUsingEncoding:NSISOLatin1StringEncoding];
  
  int res = Pdf2Rtf(idProc, szPDFFile, &sRtf, &sLen);
  if( res == ERR_OK )
    {
    NSData* data = [NSData dataWithBytes:sRtf length:sLen];
    FreeRtf(sRtf);
    
    NSAttributedString* sAttrStr = [NSAttributedString alloc];
    
    NSDictionary *docOptions = nil, *docAttrs = nil;
    
    sAttrStr = [sAttrStr initWithData:data options:docOptions documentAttributes:&docAttrs error:nil];
    
    if( sAttrStr==nil ) [_TextSrc setString:@""];
    else                [_TextSrc.textStorage setAttributedString: sAttrStr];
    }
  
  EndProcess(idProc);
  
  return (res==ERR_OK);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Comienza la traducción con un texto nuevo
-(void) NewText
  {
//  if( _modify )
//    salva el texto;

  NSDictionary* defAttr = [[NSDictionary alloc] init];
  
  _TextSrc.typingAttributes = defAttr;
  _TextTrd.typingAttributes = defAttr;
  
  [_TextSrc setSelectedRange: NSMakeRange (0, 0)];
  [_TextTrd setSelectedRange: NSMakeRange (0, 0)];
  
  _TextSrc.string = @"";
  _TextTrd.string = @"";
  
  [self ClearMatchSentence];
  [ParseText Clear];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cuando se oprime el boton Traducir
- (IBAction)OnTranslate:(id)sender
  {
  Progress = [[ProgressCtrller alloc] initInWindow:_window];
  
  _TextSrc.delegate = nil;                                                // Desabilita la atención de las vitas
  _TextTrd.delegate = nil;
  
  [self ClearMatchSentence];                                              // Quita la oración actual
  
  ParseText* parse = [ParseText ParseWithText:_TextSrc.string];           // Divide el texto en oraciones
  Progress.Count = (int)parse.Items.count;

  [_TextTrd.textStorage setAttributedString:_TextSrc.attributedString ];  // Duplica el texto en ambas vistas
  
  TrdCore* TrdApi = [TrdCore TrdWithDes:LGSrc AndDes:LGDes];              // Crea objeto para traducir
  
  [TrdApi TranslateFromParse:parse Progress:Progress];                    // Traduce todas las oraciones

  [_TextTrd.textStorage beginEditing];                                    // Pone vista tradución en modo ediccion
  
  for( NSInteger i=parse.Items.count-1; i>=0; --i )                       // Recorre todas las oraciones del texto
    {
    ParseItem* Item = parse.Items[i];                                     // Toma oracion actual
  
    if( Item.Type == ITEM_TRD)                                            // Si un item traducido
      [_TextTrd replaceCharactersInRange:Item.rgSrc withString:Item.txtTrd];  // Lo sustituye en el texto traducido
    }
  
  [_TextTrd.textStorage endEditing];                                      // Quita modo edición, para vista traducción
  
  _TextSrc.delegate = self;                                               // Abilita la atención de ambas vistas
  _TextTrd.delegate = self;
  
  [Progress Finish];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Resalta la proxima oración en el texto a partir de la actual
- (IBAction)OnNextTrd:(id)sender
  {
  showInfo = !showInfo;
  
  NSToolbarItem* item = sender;
  item.image = [NSImage imageNamed:(showInfo)? @"ShowInfo" : @"NoShowInfo" ];
  
  if( showInfo )
    {
    NSTextView* NowText = (edSrc)? _TextSrc : _TextTrd;     // Texto que se esta editando
    
    [self InfoWord:NowText];                                // Muestra información sobre la palabra actual
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void)textViewDidChangeSelection:(NSNotification *)notification
  {
  NSTextView* NowText = notification.object;              // Toma el texto donde cambio la selección
  
  edSrc = (NowText == _TextSrc);                          // Indicar que la palabra esta en el texto fuente, o no
  
  [self InfoWord:NowText];                                // Muestra información sobre la palabra actual
  
  [self MarkSentenceFromText:NowText];                    // Resalta la oración actual en los 2 cuadros de textos
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra informacion sobre la palaba seleccionada o donde esta el cursor, en el panel lateral
- (void) InfoWord:(NSTextView*) txt
  {
  if( !showInfo ) return;                                     // Si no esta habilitado, no hace nada ...
  
  NSRange range = txt.selectedRange;                          // Toma el texto seleccionado
  if( range.length == 0 )                                     // No hay texto seleccionado
    range = [txt selectionRangeForProposedRange:range granularity:NSSelectByWord];  // Selecciona la palabra actual
  
  if( range.length == 0 ) return;                             // No se puedo seleccionar nada, termina
  
  NSString* sFind = [txt.string substringWithRange:range];    // Obtiene la palabra seleccionada
  
  _PanelTools.Trd      = !edSrc;                              // En en el panel el texto, donde esta la palabra
  _PanelTools.infoWord = sFind;                               // Pone en el panel la palabra actual
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cada vez que se modifican manualmente cualquiera de los dos textos
- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString
  {
  //NSLog(@"Range (%lu-%lu) '%@' (%lu)", affectedCharRange.location, affectedCharRange.length, replacementString, (unsigned long)replacementString.length);
  [self ClearMatchSentence];                                   // Quita la marca de la oración actual
  
  edSrc = (textView == _TextSrc);                           // Indica si se trabaja en texto fuente o no

  if( edSrc ) EditSrcRange( affectedCharRange, replacementString);  // Actualiza el parse de oraciones del texto fuente
  else        EditTrdRange( affectedCharRange, replacementString);  // Actualiza el parse de oraciones del texto traducido
  
  return TRUE;                                                 // Indica a la vista que puede realizar el cambio
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Marca la oración actual, según la posición del cursor en el texto 'txt'
- (void) MarkSentenceFromText:(NSTextView*) txt
  {
  NSInteger iPos = txt.selectedRange.location;              // Obtiene la posición del cursor
  
  int  iOra;
  Byte Tipo;
  
  if( edSrc ) Tipo = SentenceSrcAtPos( iPos, &iOra );        // Oración en la posición iPos del texto fuente
  else        Tipo = SentenceTrdAtPos( iPos, &iOra );        // Oración en la posición iPos del texto traducido

       if( Tipo==ITEM_TRD   ) [self SetMatchSentence:iOra];  // Señala la oración normal
  else if( Tipo==ITEM_CHGED ) [self SetMatchSentence:iOra];  // Señala la oración como cambiada
  else if( Tipo==ITEM_NOTRD ) [self ClearMatchSentence];     // No señala nimguna oración
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Resalta la oracion con indice 'idx' en el texto de origen y traducido
- (void) SetMatchSentence:(int) idx
  {
  if( idx == LastOraMark ) return;                            // Si ya esa oración estaba marcada, no ha nada
  
  [self ClearMatchSentence];                                  // Quita la oración marcada anteriormente
  
  ParseItem* Item = [ParseText Now].Items[idx];               // Obtiene los datos de la oración
  
  rgSrcMark = Item.rgSrc;                                     // Rango que ocupa la oración original
  rgTrdMark = Item.rgTrd;                                     // Rango que ocupa la oración traducida
  
  NSDictionary* attrCol1 = (Item.chgSrc)? attrMatch2 : attrMatch1;  // Atributo si la oración a sido modificada o no
  NSDictionary* attrCol2 = (Item.chgTrd)? attrMatch2 : attrMatch1;
  
  [_TextSrc.layoutManager setTemporaryAttributes:attrCol1 forCharacterRange:rgSrcMark];   // Marca oración fuente
  [_TextTrd.layoutManager setTemporaryAttributes:attrCol2 forCharacterRange:rgTrdMark];   // Marca oración traducida
  
  [SyncScrollView PauseSync];                                 // Pausa sincronismo entre los dos scrollers
  
  if( edSrc ) [_TextTrd scrollRangeToVisible:rgTrdMark];      // Asegura que este visible la oración original
  else        [_TextSrc scrollRangeToVisible:rgSrcMark];      // Asegura que esta visible la oración traducida
  
  [SyncScrollView ActiveSync];                                // Activa al sincronismo entre los scrollers
  
  LastOraMark = idx;                                          // Guarda indice de la oración marcada
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Quirta las oraciones que estaban marcadas si las habia
- (void) ClearMatchSentence
  {
  if( rgSrcMark.length > 0 )
    [_TextSrc.layoutManager removeTemporaryAttribute:NSBackgroundColorAttributeName forCharacterRange:rgSrcMark];
  
  if( rgTrdMark.length > 0 )
    [_TextTrd.layoutManager removeTemporaryAttribute:NSBackgroundColorAttributeName forCharacterRange:rgTrdMark];
  
  rgSrcMark.length = 0;
  rgTrdMark.length = 0;
  LastOraMark      = -1;
  }

@end

//=========================================================================================================================================================
