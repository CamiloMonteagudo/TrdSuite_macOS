//=========================================================================================================================================================
//  DictCore.mm
//  IdiomaXTranslator
//
//  Created by MacPC on 5/27/11.
//  Copyright 2011 IdiomaX. All rights reserved.
//=========================================================================================================================================================

#include "DictCore.h"
#include "WinUtil.h"
#include "UtilFun.h"
#include "ApiRed.h"
#include "DictMain.h"
//#include "DictUserIndex.h"
//#import "ColAndFont.h"
#import "AppData.h"

//=========================================================================================================================================================
static CGFloat FontSize   = [NSFont systemFontSize];                                                        // Tamaño de la letras estandard del sistema

// Significados
static NSFont* fontMean      = [NSFont systemFontOfSize:     FontSize];                                     // Fuente para los significados
static NSFont* fontMeanBold  = [NSFont boldSystemFontOfSize: FontSize];                                     // Fuente para los textos resaltados dentro del significado
static NSFont* fontMeanSmall = [NSFont boldSystemFontOfSize: FontSize];                                     // Fuente mas pequeña dentro del significado

static NSColor* ColMean      = [NSColor blackColor];                                                             // Color para el cuerpo del significado
static NSColor* ColMeanGray  = [NSColor darkGrayColor];                                                          // Color atenuado para las palabras que pueden cambiar dentro del significado
static NSColor* ColMeanType  = [NSColor colorWithRed:0.06 green:0.06 blue:0.80 alpha:1.00];                      // Color para las definiciones del tipo gramatical
static NSColor* ColMeanAttr  = [NSColor colorWithRed:0.10 green:0.10 blue:0.60 alpha:1.00];                      // Color para los atributos asociados al significado

static NSDictionary* attrKey   = @{ NSFontAttributeName:fontMeanBold , NSForegroundColorAttributeName:ColMean      };
static NSDictionary* attrBody  = @{ NSFontAttributeName:fontMean     , NSForegroundColorAttributeName:ColMean      };
static NSDictionary* attrBody2 = @{ NSFontAttributeName:fontMean     , NSForegroundColorAttributeName:ColMeanGray  };
static NSDictionary* attrType  = @{ NSFontAttributeName:fontMeanSmall, NSForegroundColorAttributeName:ColMeanType  };
static NSDictionary* attrAttr  = @{ NSFontAttributeName:fontMeanSmall, NSForegroundColorAttributeName:ColMeanAttr  };

static NSDictionary* attrTypeCol = @{ NSForegroundColorAttributeName:ColMeanType  };

//=========================================================================================================================================================
LPCSTR   GetTypeDesc( TGramMean &sType );
void     AddFixMean( NSMutableAttributedString* Str, CStringA &Mean );

NSAttributedString* WordDataFormated(CStringA &csKey, CStringA &csData, BOOL noKey);
NSAttributedString* WordDataKeyFormated(CStringA &csKey, CStringA &csData, BOOL noKey);

static void AddNSString( NSMutableAttributedString* Str, NSString* mStr,  NSDictionary* attrDict );

void SetNowWordData( CStringA sKey, CDictData* WrdData );

//=========================================================================================================================================================
@implementation DictCore

static CDictMain* _Dict    = NULL;
static bool       _Found   = FALSE;
static TInt       _LastIdx = -1;
static bool       _WordKey = FALSE;

static LPCSTR TypCodes[] = { "SS"    , "NP"       , "AA"  , "DD"  , "VT"      , "VI"        , "VR"    , "VA"     , "PP"   , "PT"      , "PI"        , "GT"      , "GI"        , "CC"     , "JJ"     , "AI"          };
static LPCSTR TypDesEn[] = { "Noun"  , "Prop.Noun", "Adj.", "Adv.", "Trans.V.", "Intrans.V.", "Ref.V.", "Aux.V." , "Prep.", "Trans.P.", "Intrans.P.", "Trans.G.", "Intrans.G.", "Conj."  , "Interj.", "Static Adj." };
static LPCSTR TypDesEs[] = { "Sust." , "N.Prop."  , "Adj.", "Adv.", "V.Trans.", "V.Intrans.", "V.Ref.", "V.Aux." , "Prep.", "P.Trans.", "P.Intrans.", "G.Trans.", "G.Intrans.", "Conj."  , "Interj" , "Adj.Estat."  };
static LPCSTR TypDesFr[] = { "Subst.", "N.Propre" , "Adj.", "Adv.", "V.Trans.", "V.Intrans.", "V.Ref.", "V.Aux." , "Prep.", "P.Trans.", "P.Intrans.", "G.Trans.", "G.Intrans.", "Conj."  , "Interj.", "Adj.Fixe"    };
static LPCSTR TypDesIt[] = { "Sost." , "N.Proprio", "Agg.", "Avv.", "V.Trans.", "V.Intrans.", "V.Rif.", "V.Auss.", "Prep.", "P.Trans.", "P.Intrans.", "G.Trans.", "G.Intrans.", "Coniug.", "Inter." , "Agg.Stat."   };

static LPCSTR *TypsDes[] = { TypDesEs, TypDesEn, TypDesIt, TypDesEn, TypDesFr };
static LPCSTR *TypDes = TypDesEn; 

//static CDictUserIndex* Index = NULL;
//static CDictUserIndex* SaveIndex = NULL;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Abre el diccionario actual
+(int) OpenDictSrc:(int) src Dest:(int) des
  {
  if( src==-1 || des==-1 ) return DICT_FAIL;
  
  CDictMain* LastDict = _Dict;
  _Dict = CDictGen::Get(src, des);
  if( LastDict == _Dict ) return DICT_OPEN;
  
  TypDes = TypsDes[des];

  LoadIndexFromFile( src, des );
    
//  if( LoadIndexFromFile() != DICT_NO_ERROR )
//    GenerateDictIndex( src, des );
  
  return DICT_OPENED;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Carga el fichero de indice para mostrar el diccionario
int LoadIndexFromFile( int src, int des )
  {
//  NSString*  fName = [NSString stringWithFormat:@"Index%@%@", LGAbrv(src), LGAbrv(des) ];
//  NSString*   Path = GetFullAppPath( fName );
//  CStringA  csPath = [Path cStringUsingEncoding:NSISOLatin1StringEncoding ];
//  
//  Index = new CDictUserIndex();
//  
//  if( Index->Load(csPath) == DICT_NO_ERROR )
//		return DICT_NO_ERROR;
//  
//  delete Index;
//  Index = NULL;
  return CAN_NOT_OPENFILE_IDX;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Crea un indice para el diccionario, ordenado sin mayusculas ni acentos y sin llaves 'especiales'
//void GenerateDictIndex( int src, int des )
//  {
//  if( Index     != NULL )   delete Index;
//  if( SaveIndex != NULL ) { delete Index; SaveIndex=NULL; }
//  
//  Index = new CDictUserIndex();
//  
//  int count = _Dict->GetCount();
//  Index->SetSize( count );
//  
//  CStringA csKey;
//  for( int i=0; i<count; ++i )
//    {
//    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
//    
//    _Dict->GetKeyAt( i, csKey);
//    
//    int iFind = csKey.Find( "xxxx" );
//    if( iFind >= 0 ) continue;
//    
//    iFind = csKey.Find( '[' );
//    if( iFind >= 0 ) continue;
//    
//    iFind = csKey.Find( '(' );
//    if( iFind >= 0 ) continue;
//    
//    iFind = csKey.Find( ')' );
//    if( iFind >= 0 ) continue;
//    
//    iFind = csKey.Find( '$' );
//    if( iFind >= 0 ) continue;
//    
//    iFind = csKey.Find( '<' );
//    if( iFind >= 0 ) continue;
//    
//    if( csKey[0] == ','   ) continue;
//    if( csKey[0] == '\''  ) continue;
//    if( csKey[0] == '-'   ) continue;
//    if( csKey[0] == '"'   ) continue;
//    if( csKey[0] == ';'   ) continue;
//    if( isdigit(csKey[0]) ) continue;
//    
//    NSString* sKey = [NSString stringWithCString:csKey.c_str() encoding:NSISOLatin1StringEncoding ];
//    
//    int Idx;
//    if( !Index->Find(_Dict, sKey, &Idx )  )
//      Index->InsertAt(Idx, i);
//    }
//  
//  NSLog(@"%d-%d de %8d a %8d eliminados %d",src, des , count, Index->Count(), count-Index->Count());
//  
////  SaveIndexToFile( src, des );
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Guarda el indice generado en ese momento hacia un fichero
//void SaveIndexToFile( int src, int des )
//  {
//  NSString*  fName = [NSString stringWithFormat:@"Index%@%@", LGAbrv(src), LGAbrv(des) ];
//  NSString*   Path = GetFullDocPath( fName );
//  CStringA  csPath = [Path cStringUsingEncoding:NSUTF8StringEncoding ];
//  
//  Index->Save( csPath );
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el camino completo del fichero en el directorio para documentos de la aplicación
 NSString* GetFullDocPath( NSString* fName )
  {
  NSFileManager *fMng = [[NSFileManager alloc] init];                             // Crea objeto para manejo de ficheros
  
  NSURL *url =[fMng URLForDirectory:NSDocumentDirectory                           // Le pide el directorio de los documentos
                           inDomain:NSUserDomainMask 
                  appropriateForURL:nil 
                             create:YES 
                              error:nil];
  
  return [[url path] stringByAppendingPathComponent:fName];               // Le adiciona el nombre del fichero para los datos
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el camino completo del fichero en el directorio de la aplicación
 NSString* GetFullAppPath( NSString* fName )
  {
  NSString *Path =[[NSBundle mainBundle] bundlePath];                             // Obtiene el directorio donde se instalo el paquete
  
  return [Path stringByAppendingPathComponent:fName];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si las llaves estan filtradas o no
//+(BOOL) IsFiltered
//  {
//  return ( SaveIndex != NULL );
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Filtra las llaves actales del diccionario
//+(void) KeysFilter:(NSString*) sFilter
//  {
//  if( Index==NULL ) return;                                 // No hay diccionario abierto, no hace nada
// 
//  if( SaveIndex != NULL )                                   // Si ya habia un filtro lo borra
//    {
//    [self RemoveFilter];                                    // Quita el filtro
//    return;                                                 // Retorna
//    }
//  
//  int fLen = (int)sFilter.length;
//  if( sFilter==nil || fLen==0 ) return;                     // Si el filtros es nulo o vacio no hace nada
//  
//  SaveIndex = Index;                                        // Guarda el indice original
//  Index = new CDictUserIndex();                             // Crea un nuevo indice vacio
//  
//  int count = SaveIndex->Count();                           // Obtiene tamaño del indice original
//  Index->SetSize( count );                                  // Reserva memoria, para el caso mas critico
//  
// 	CStringA csFilter = [sFilter cStringUsingEncoding:NSISOLatin1StringEncoding ];
//  
//  CStringA csKey;
//  for( int i=0; i<count; ++i )                              // Recorre todas las llaves del diccionario
//    {
//    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
//    
//    int idx = SaveIndex->IndexAt(i);                        // Toma indice real al diccionario
//    
//    _Dict->GetKeyAt( idx, csKey);                           // Obtiene la llave del diccionario
//    
//    int ini = csKey.Find( csFilter );                       // Busca si la llave contiene el filtro
//    
//    if( ini >= 0  )                                         // Si la llave contiene el filtro
//      {
//      int kLen = csKey.Length();                            // Longitud del la llave
//      int fin  = ini + fLen;                                // Indice de la ultima letra de la llave que machea con el filtro
//  
//      if( (ini==0    || csKey[ini-1]==' ')  &&            // Empieza en una frontera de palabra
//          (fin==kLen || csKey[fin  ]==' ') )              // Termina en una frotera de palabra
//        Index->Add(idx);                                    // Adiciona la llave al indice
//      }
//    }
//    
//  Index->SetSize( Index->Count() );                         // Reajusta el tamaño del indice
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra todos los datos, eliminando lo filtros existentes
//+(void) RemoveFilter
//  {
//  if( SaveIndex == NULL )                                     // No habia filtro
//    return;
//  
//  delete Index;
//  Index = SaveIndex;
//  SaveIndex = NULL;
//  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cierra el diccinario activo, si esta abierto
+(void) CloseDict
  {
  _Dict = NULL;
  
//  if(     Index != NULL ) { delete Index;   Index = NULL; }
//  if( SaveIndex != NULL ) { delete Index; SaveIndex=NULL; }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene los datos de una palabra en el diccionario actual, 'NoKey' inidica que no se incluya la llave en los datos
+(NSAttributedString*) getWDataFromIndex: (int)idx NoKey:(BOOL) noKy
  {
  if( _Dict == NULL ) return nil;
  
  CStringA sHtm, sKey, sData;
  
//  if( Index != NULL )
//    idx = Index->IndexAt( idx );
  
  _Dict->GetKeyAt (idx, sKey);
  _Dict->GetDataAt(idx, sData);
   
  if( sData.Length()>0 )
    return WordDataFormated( sKey, sData, noKy );
    
  return nil;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiiene una cadena con un mensage formateado
+(NSAttributedString*) FormatedMsg:(NSString*) sMsg Title:(NSString*) sTitle
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];
  
  if( sTitle && sTitle.length>0 )
    {
    AddNSString( Str, sTitle, attrKey );
    AddNSString( Str, @"\r\n", attrKey );
    }
  
  AddNSString( Str, sMsg, attrBody2  );
  
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la cantidad de palabras en el diccionario actual
+(int) getSize
  {
  if( _Dict == NULL ) return 0;
  
//  return (Index != NULL)? Index->Count() : _Dict->GetCount();
  return _Dict->GetCount();
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la palabra que se encuentra en el indice 'idx' en el diccionario actual
+(NSString *)getWordAt: (int)idx
	{
  if( _Dict == NULL ) return @"";
  
  CStringA csKey;
    
//  if( Index != NULL )
//    idx = Index->IndexAt( idx );
  
  _Dict->GetKeyAt( idx, csKey);

  return [NSString stringWithCString:csKey.c_str() encoding:NSISOLatin1StringEncoding ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la el indice de la palabra Key en el diccionario
+(int) getWordIdx: (NSString *) Key
  {
  if( _Dict == NULL ) return 0;
  
//  if( Index != NULL )
//    _Found = Index->Find(_Dict, Key, &_LastIdx );
//  else
    {
    CStringA csKey = [Key cStringUsingEncoding:NSISOLatin1StringEncoding];
    _Found = _Dict->FindIndex( csKey, _LastIdx);
    }
  
  return _LastIdx;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si la palabra con indice 'idx' en el diccionario actual esta en el diccionario de usuario
+(BOOL) isUserKeyIdx: (int)idx
  {
  if( _Dict == NULL ) return false;
  
  return _Dict->isUserKey( idx );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Indica si el diccionario fue modificado o no
+(BOOL) isDictModified
  {
  if( _Dict == NULL ) return false;
  
  return _Dict->isDictModified();
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Guarda el contenido de las palabras modificadas
+(BOOL) Save
  {
  if( _Dict == NULL ) return false;
  
  return _Dict->Save();
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Borra la palabra sKey del diccionario de usuario
+(BOOL) DelUserKey: (NSString*) sKey
  {
  CStringA csKey = [sKey cStringUsingEncoding:NSISOLatin1StringEncoding];
  
  return _Dict->DeleteKey(csKey);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Indica si la ultima palabra obtenida con 'getWordIdx' fue encontrada o  no
+(bool) Found { return _Found; }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Indica los últimos datos de palabra obtenidos con 'getWDataFromIndex' pertenecen a una palabra clave o no
+(bool) WordKey { return _WordKey; }

//--------------------------------------------------------------------------------------------------------------------------------------------------------

@end

//=========================================================================================================================================================

//--------------------------------------------------------------------------------------------------------------------------------------------------------
static void AddCString( NSMutableAttributedString* Str, CStringA &cStr,  NSDictionary* attrDict )
  {
  NSString* mStr = [NSString stringWithCString:cStr.c_str() encoding:NSISOLatin1StringEncoding ];
  
  [Str appendAttributedString: [[NSMutableAttributedString alloc] initWithString:mStr attributes:attrDict] ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
static void AddLPCSTR( NSMutableAttributedString* Str, LPCSTR cStr,  NSDictionary* attrDict )
  {
  NSString* mStr = [NSString stringWithCString:cStr encoding:NSISOLatin1StringEncoding ];
  
  [Str appendAttributedString: [[NSMutableAttributedString alloc] initWithString:mStr attributes:attrDict] ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
static void AddNSString( NSMutableAttributedString* Str, NSString* mStr,  NSDictionary* attrDict )
  {
  [Str appendAttributedString: [[NSMutableAttributedString alloc] initWithString:mStr attributes:attrDict] ];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene los datos de una palabra formateados
NSAttributedString* WordDataFormated(CStringA &csKey, CStringA &csData, BOOL noKey)
  {
  CDictData* oDictData = new CDictData();
  oDictData->ParseTxt(csData);
  
  SetNowWordData( csKey, oDictData );
  
  TInt nTypes = oDictData->GetTypesCount();
  
  if( oDictData->isKeyword() )
    {
    _WordKey = TRUE;
    return WordDataKeyFormated( csKey, csData, noKey );
    }
  else
    _WordKey = FALSE;

  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];
  
  if( noKey==FALSE )
    AddCString ( Str, csKey, attrKey );
  
  // para cada tipo gramatical
  for( int idxType=0; idxType < nTypes; idxType++)
    {
    // poner el tipo gramatical
    oDictData->put_ActualTypeIdx(idxType);
    TGramType sType;
    oDictData->get_ActualType(sType);
    
    if( Str.length > 0 )
      AddNSString( Str, @"\r\n", attrBody );
    
    AddLPCSTR  ( Str, GetTypeDesc(sType), attrType );
    AddNSString( Str, @" "              , attrBody );
    
    TInt nMean = oDictData->MeanCountAt(idxType);
      
    // para cada significado
    for(int idxMean=0; idxMean<nMean; idxMean++)
      {
      oDictData->put_ActualMeanIdx(idxMean);
        
      TGramMean sMean;
      oDictData->get_ActualMean(sMean);
        
      // la especialidad solo se pone si no esta vacia y no es la general
      TGramEsp esp;
      oDictData->get_ActualEsp(esp);
      if (!esp.IsEmpty() && esp != "GG")
        {
        AddCString ( Str, esp , attrAttr );
        AddNSString( Str, @" ", attrBody );
        }
        
      // el genero solo se pone si no esta vacio y no es masculino
      TText gender = oDictData->get_ActualGen();
      if( gender != 0 && gender != 'm' )
        {
        if (gender == 'f') AddNSString( Str, @"f. " , attrAttr );
        else               AddNSString( Str, @"n. " , attrAttr );
        }
        
      // el numero solo se pone si es plural
      TText number = oDictData->get_ActualNum();
      if( number == 'p' )
        AddNSString( Str, @"p. " , attrAttr );
      
      AddFixMean( Str, sMean );

      if( idxMean==(nMean-1) ) AddNSString( Str, @". ", attrBody );
      else                     AddNSString( Str, @", ", attrBody );
      }
    }
    
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la descripcion del tipo gramatical
LPCSTR GetTypeDesc( TGramMean &sType )
  {
  for( int i=0; i<sizeof(TypCodes)/sizeof(LPCSTR); ++i )
    if( sType.Compare(TypCodes[i]) == 0 )
      return TypDes[i];
    
  return "";
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Resalta las palabras dentro del significado que estan entre <>
void AddFixMean( NSMutableAttributedString* Str, CStringA &Mean )
  {
  int iIni = Mean.Find( '<' );
  if( iIni < 0 )
    {
    AddCString( Str, Mean, attrBody );
    return;
    }
  
  int iEnd = Mean.Find( '>', iIni );
  if( iEnd < 0 )
    {
    AddCString( Str, Mean, attrBody );
    return;
    }

  CStringA sLeft  = Mean.Left( iIni );
  CStringA sMidle = Mean.Mid( iIni+1, iEnd-iIni-1 );
  CStringA sRight = Mean.Mid( iEnd+1 );
  
  AddCString( Str, sLeft , attrBody  );
  AddCString( Str, sMidle, attrBody2 );
  AddCString( Str, sRight, attrBody  );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Adiciona todo lo que este entre comillas a la cadena formateada 'Str'
void AddEntreComillas( NSMutableAttributedString* Str, CStringA &aDataText, BOOL noKey)
  {
  int  pos1 = 0;
  bool first = true;
  
  int len = aDataText.Length();
  while( pos1 < len )
    {
    pos1 = aDataText.Find('\"', pos1 );
    if( pos1 == -1 ) break;
    
    int pos2 = aDataText.Find('\"', pos1 + 1);
    if( pos2 == -1 ) break;
    
    CStringA sMean   = aDataText.Mid( pos1+1, pos2-(pos1+1) );
    
    CStringA sUpMean;
    sUpMean.Copy(sMean);
    sUpMean.MakeUpper();
    
    if( sMean.GetLength()!=2 || sMean != sUpMean  )
      {
      if( first && !noKey   ) AddNSString( Str, @"\r\n", attrBody );
      else if( Str.length>0 ) AddNSString( Str, @", "  , attrBody );
    
      AddFixMean( Str, sMean );
      }
    
    pos1  = pos2 + 1;
    first = false;
    }
  }
  
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Quita todas las partes de la cadena 'aDataText' que esten entre parentisis
void RemoveParenthesis(CStringA& aDataText)
  {
  int pClose = 0;
  int pOpen = 0;
    
  // buscar el primer parentesis que cierra
  while ((pClose = aDataText.Find(')')) > 0)
    {
    // ir hacia atras buscando el primero que abre
    pOpen = pClose - 1;
    while (pOpen >= 0 && aDataText[pOpen] != '(')
      pOpen--;
      
    if (pOpen >= 0)
      // borrar el texto y los parentesis
      aDataText.Delete(pOpen, pClose - pOpen + 1);
    else // no se encontro el parentesis que cierra, terminar
      break;
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una cadena formateada con los datos de una entrada que tiene una palabra clave
NSAttributedString* WordDataKeyFormated(CStringA &csKey, CStringA &csData, BOOL noKey)
  {
  RemoveParenthesis(csData);
    
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] init];
  
  if( noKey==FALSE )
    AddCString ( Str, csKey  , attrKey  );
    
  int pos1 = 0, pos2 = 0;
  int len = csData.Length();
  bool first = TRUE;
    
  while( pos1 < len )                         // Si hay llaves solo coje la que esta adentro
    {
    pos1 = csData.Find('{', pos1 );           // Busca llave inicial
    if( pos1 == -1 )                          // No encontro llave inicial
      {
      if( first ){ pos1=0; pos2=len; }        // Si no hay llaves, lo coge todo
      else break;                             // No hay mas llaves, termina
      }
    else                                      // Encontro llave inicial
      {
      pos2 = csData.Find('}', pos1 + 1);      // Busca llave final
      if( pos2==-1 ) break;                   // No la encontro, termina (llaves no macheadas)
      }
      
    CStringA sMeans = csData.Mid( pos1+1, pos2-(pos1+1) );        // Coge el contenido
    AddEntreComillas( Str, sMeans, noKey );                       // Adiciona solo lo que esta entre comillas
    
    pos1  = pos2 + 1;                                             // Pasa a buscar la otra llave
    first = FALSE;                                                // Quita bandera de primera ves
    }
  
  return Str;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//
NSAttributedString* EdictTypeLabel( NSString* Label, NSString* sType )
  {
  NSMutableAttributedString* Str = [[NSMutableAttributedString alloc] initWithString:Label];
  
  AddNSString(Str, sType, attrTypeCol);
  
  return Str;
  }

//=========================================================================================================================================================
// Implementa el manejo de datos de una palabra
//=========================================================================================================================================================

CStringA   _NowKey;                     // Palabra actual en el diccionario
CDictData* _NowWordData = NULL;         // Datos de la palabra actual

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Establece los datos para la palabra actual
void SetNowWordData( CStringA sKey, CDictData* WrdData )
  {
  _NowKey = sKey;
  
  if( _NowWordData != NULL )
    delete _NowWordData;
  
  _NowWordData = WrdData;
  }

//=========================================================================================================================================================
@implementation WrdData

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la palabra que se esta editando
+ (NSString*) GetWord
  {
  return [NSString stringWithCString:_NowKey.c_str() encoding:NSISOLatin1StringEncoding];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Inicializa los datos para una palabra nueva
+ (void) SetNewWord:(NSString*) sWord
  {
  CStringA sKey = [sWord cStringUsingEncoding:NSISOLatin1StringEncoding ];
  
  SetNowWordData( sKey, new CDictData() );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la cantidad de tipos gramaticales que hay en los datos de la palabra
+ (int)CountTypes
  {
  if( _NowWordData == NULL )  return 0;
  
  return _NowWordData->GetTypesCount();
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el código del tipo con indice iTipo
+ (NSString*) CodeFromType:(int) iTipo
  {
  CStringA str;
  
  _NowWordData->put_ActualTypeIdx(iTipo);                         // Pone el tipo gramatical actual
  _NowWordData->get_ActualType(str);                              // Obtiene una cadena con el código del tipo gramatical
  
  return [NSString stringWithCString:str.c_str() encoding:NSISOLatin1StringEncoding];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene una cadena con la representación de todos los significados del tipo 'iTipo'
+ (NSString*) MeansStringFromType:(int) iTipo
  {
  CStringA str;

  TInt nMean = _NowWordData->MeanCountAt(iTipo);          // Obtiene el número de significados para el tipo actual
  
  for(int idxMean=0; idxMean<nMean; idxMean++)            // Recorre todos los significados
    {
    _NowWordData->put_ActualMeanIdx(idxMean);             // Pone todos los significados
    
    TGramMean sMean;
    _NowWordData->get_ActualMean(sMean);                  // Obtiene una cadena con el significado
    
    TGramEsp esp;
    _NowWordData->get_ActualEsp(esp);                     // Obtiene la especialidad asociada al significado
    if (!esp.IsEmpty() && esp != "GG")                    // Si no es vacia, o general
      str += esp + " ";                                   // La agrega al significado
    
    TText gender = _NowWordData->get_ActualGen();         // Obtiene una cadena con el genero del significado
    if( gender != 0 && gender != 'm' )                    // Si existe y no es masculino (por defecto)
      {
      if (gender == 'f') str += "f. ";                    // Agreaga el genero femenino a la cadena
      else               str += "n. ";                    // Agreaga el genero femenino a la cadena
      }
    
    TText number = _NowWordData->get_ActualNum();         // Obtiene el número asociado al significado
    if( number == 'p' ) str += "p. ";                     // Si es plural, lo agrega a la cadena
    
    str += sMean;                                         // Agraga el significado a la cadena
    
    if( idxMean<nMean-1 ) str += ", ";                    // Si no es el ultimo significado, agrega un separador
    }
  
  return [NSString stringWithCString:str.c_str() encoding:NSISOLatin1StringEncoding];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Borra todos los significados del tipo 'iTipo'
+ (void) DeleteMeansForType:(int) iTipo
  {
  int nMeans = _NowWordData->GetNMeans(iTipo);
  for( int i=nMeans-1; i>=0; --i )
    _NowWordData->DelMean(iTipo, i);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene los significados del tipo 'iTipo' a partir de la cadena 'strMeans'
+ (BOOL) MeansString:(NSString*)strMeans ToType:(int) iTipo
  {
  [self DeleteMeansForType: iTipo];
  
 	CStringA sMeans = [strMeans cStringUsingEncoding:NSISOLatin1StringEncoding ];
  
  int len = sMeans.GetLength();
  for( int i=0;; )                                                      // Recorre toda la cadena
    {
    CStringA Esp("GG");                                                 // Valores por defecto de los atributos
    BYTE   	 Gen = 0;
    BYTE     Num = 0;
    
    while( i<len && sMeans[i]==' ') ++i;                                // Salta los espacios
    if( i==len )                                                        // Llego al final de la cadena
      return (_NowWordData->GetNMeans(iTipo)>0);                        // Retorna verdadero, si se logro obtener significados
 
    for(;;)                                                             // Busca atributos, al inicio
      {
      if( i+1 >= len ) break;                                           // No queda espacio para atributos
      
      int attb = 0;
      char c1=sMeans[i], c2=sMeans[i+1];                                // Toma las 3 primeras letras
      
           if( Num==0    && c1=='p'     && c2=='.'     ) attb = 1;      // Attrb de plural
      else if( Gen==0    && c1=='f'     && c2=='.'     ) attb = 2;      // Attrb de femenino
      else if( Gen==0    && c1=='n'     && c2=='.'     ) attb = 3;      // Attrb de neutro
      else if( Esp=="GG" && isupper(c1) && isupper(c2) ) attb = 4;      // Es una especialidad
      else break;                                                       // No hay mas atributo, termina
      
      if( attb<4  && i+2 >= len       ) return FALSE;                   // Atributo al final del texto, ERROR ..
      if( attb<4  && sMeans[i+2]!=' ' ) return FALSE;                   // Atributo no seguido de espacio, ERROR ...
      if( attb==4 && sMeans[i+2]!=' ' ) break;                          // Dos mayúsculas si espacio, no hace nada
      
      switch( attb )                                                    // Toma valor del atributo encontrado
        {
        case 1: Num=1; break;
        case 2: Gen=1; break;
        case 3: Gen=2; break;
        case 4: Esp = sMeans.Mid(i,2); break;
        }
      
      i += 3;                                                           // Salta el atributo encontrado
      }
      
    while( i<len && sMeans[i]==' ') ++i;                                // Salta los espacios
    if( i==len ) return FALSE;                                          // Llego al final de la cadena, sin significado, ERROR ...
    
    int j = i;                                                          // Guarda el indice de inicio de la busqueda
    while( i<len && sMeans[i]!=',') ++i;                                // Busca el separador de fin de significado
    
    if( j==i ) return FALSE;                                            // No obtuvo ningún significado, ERROR ...
    
    CStringA sMean = sMeans.Mid(j, i-j);                                // Obtiene la cadena del significado
    
    int iMean = _NowWordData->AddMean(iTipo, sMean.c_str());            // Agrega el significado al tipo actual
    
    _NowWordData->SetEsp(iTipo, iMean, Esp.c_str());                    // Le pone la especialidad
    _NowWordData->SetNum(iTipo, iMean, Num);                            // Le pone el número
    _NowWordData->SetGen(iTipo, iMean, Gen);                            // Le pone la persona
    
    if( i<len ) ++i;                                                    // Salta la coma separadora
    }
  
  return TRUE;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina si el tipo 'sType' es valido para ser adicionado como un tipo nuevo
+ (BOOL) IsValidAddType:(NSString*) sType
  {
 	CStringA asType = [sType cStringUsingEncoding:NSISOLatin1StringEncoding ];
  
  int ret = _NowWordData->CanAddType(asType.c_str(), -1);
  
  return (ret >= _NowWordData->GetTypesCount() );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Adiciona un nuevo tipo gramatical a los datos de la palabra
+ (BOOL) AddType:(NSString*) sType
  {
 	CStringA asType = [sType cStringUsingEncoding:NSISOLatin1StringEncoding ];
  
  int nTipos = _NowWordData->GetTypesCount();
  _NowWordData->AddType(asType.c_str());
  
  return (nTipos != _NowWordData->GetTypesCount());
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Adiciona un nuevo tipo gramatical a los datos de la palabra
+ (BOOL) AddMean:(NSString*) sMean InType:(NSString*) sType
  {
 	CStringA asMean = [sMean cStringUsingEncoding:NSISOLatin1StringEncoding ];
 	CStringA asType = [sType cStringUsingEncoding:NSISOLatin1StringEncoding ];
  
  int nTipos = _NowWordData->GetTypesCount();
  for( int iTipo=0; iTipo<nTipos; ++iTipo )                   // Recorre todos los tipos gramaticales
    {
    CStringA sTipo;
    
    _NowWordData->put_ActualTypeIdx(iTipo);                    // Pone el tipo gramatical actual
    _NowWordData->get_ActualType(sTipo);                       // Obtiene una cadena con el código del tipo gramatical
    
    if( asType==sTipo )                                        // Si es el tipo indicado
      {
      _NowWordData->AddMean(iTipo, asMean.c_str());            // Agrega el significado al tipo actual
      return TRUE;
      }
    }
  
  return FALSE;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Quita el tipo gramatical 'sType' de los datos de la palabra
+ (void) RemoveType:(NSString*) sType
  {
 	CStringA asType = [sType cStringUsingEncoding:NSISOLatin1StringEncoding ];
  
  for(int i=0; i<_NowWordData->GetTypesCount(); ++i)
    {
    CStringA nowType;
    _NowWordData->get_TypeAt(i, nowType);
    
    if( asType == nowType ) _NowWordData->DelType(i);
    }
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Guarda los datos de la palabra actual, en el diccionario actual
+ (BOOL) SaveData
  {
  CStringA wrdData;
  
  if( _NowWordData->GetDataText(wrdData) ) return FALSE;
  
  _Dict->AddKey(_NowKey, wrdData);
  
  return TRUE;
  }

@end

//=========================================================================================================================================================

