//=========================================================================================================================================================
//  AppData.m
//  PruTranslate
//
//  Created by Camilo on 31/12/14.
//  Copyright (c) 2014 Softlingo. All rights reserved.
//=========================================================================================================================================================

#import "AppData.h"

//=========================================================================================================================================================
int LGSrc = -1;
int LGDes = -1;
int iUser = 0;

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Matrix de direcciones de traduccion instaladas

//                              EsEn, EsIt, EsFr, EnEs, EnIt, EnFr, ItEs, ItEn, ItFr, FrEs, FrEn, FrIt
//static BOOL _InstDir[DIRCount] = { 1,    0,    0,    1,    1,    1,    0,    1,    0,    0,    1,    0   };

//=========================================================================================================================================================
// Abreviatura de de los idiomas segun el codigo ISO
static NSString *_AbrvLng[] = { @"Es", @"En", @"It", @"Fr" };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Nombre de los idiomas de traduccion segun la interfaz de usuario
static NSString * _LngNames[LGCount][LGCount] =
  {  //Español  , Ingles        , Italiano        , Frances
    {@"Español" , @"Inglés   "  , @"Italiano  "   , @"Francés "  },   // IUser Español
    {@"Spanish" , @"English "   , @"Italian   "   , @"French  "  },   // IUser Inglés
    {@"Spagnolo", @"Inglese   " , @"Italiano    " , @"Francese " },   // IUser Italiano
    {@"Espagnol", @"Anglais   " , @"Italien     " , @"Français " },   // IUser Francés
  };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Banderas para representar los idiomas
static NSString * _LngFlags[LGCount] =  {@"🇪🇸" , @"🇺🇸"  , @"🇮🇹"   , @"🇫🇷"  };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Define los idiomas fuente i destino para cada una de las direcciones de traducción
//static int _DirSrc[] = { 0,   0,  0,   1,  1,   1,   2,  2,   2,   3,   3,  3 };
//static int _DirDes[] = { 1,   2,  3,   0,  2,   3,   0,  1,   3,   0,   1,  2 };
//                      EsEn, EsIt, EsFr, EnEs, EnIt, EnFr, ItEs, ItEn, ItFr, FrEs, FrEn, FrIt

static int _InstSrc[] = { 3, 3, 3, 0, 1, 2 };
static int _InstDes[] = { 0, 1, 2, 3, 3, 3 };

//static int _InstSrc[] = { 1, 1, 1, 0, 0, 0, 2, 2, 2, 3, 3, 3 };
//static int _InstDes[] = { 0, 2, 3, 1, 2, 3, 0, 1, 3, 0, 1, 2 };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
int DIRCount()
  {
  return sizeof(_InstSrc)/sizeof(_InstSrc[0]);
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
NSString* LGAbrv( int lng )
  {
  if( lng<0 || lng>LGCount ) return @"";

  return _AbrvLng[lng];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el nombre del idioma 'lng'
NSString* LGName( int lng )
  {
  if( lng<0 || lng>LGCount ) return @"";

	return _LngNames[iUser][lng];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene el nombre de la dirección de traducción con indice 'iDir'
NSString* DIRName( int iDir )
  {
  if( iDir<0 || iDir>DIRCount() ) return @"";
  
  int iSrc = _InstSrc[iDir];
  int iDes = _InstDes[iDir];
  
  NSString* sSrc = _LngNames[iUser][iSrc];
  NSString* sDes = _LngNames[iUser][iDes];

  NSString* flgSrc = _LngFlags[iSrc];
  NSString* flgDes = _LngFlags[iDes];
  
  return [NSString stringWithFormat:@"%@ %@ ➔ %@ %@", flgSrc, sSrc, flgDes, sDes];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina el idioma fuente de la dirección 'iDir'
int DIRSrc( int iDir )
  {
  if( iDir<0 || iDir>DIRCount() ) return -1;
  
  return _InstSrc[iDir];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Determina el idioma destino de la dirección 'iDir'
int DIRDes( int iDir )
  {
  if( iDir<0 || iDir>DIRCount() ) return -1;
  
  return _InstDes[iDir];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la dirección de traducción que esta compuesta por los dos idiomas dados
int DIRFromLangs(int src, int des)
  {
  for( int i=0; i<DIRCount(); ++i )
    if( _InstSrc[i]==src && _InstDes[i]==des )
      return i;
  
  return -1;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Obtiene la primera dirección de traduccion instalada y la pone activa
int DIRFirst()
  {
  LGSrc = _InstSrc[0];
  LGDes = _InstDes[0];
  
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
NSString* LGFlag( int lng )
  {
  if( lng<0 || lng>LGCount ) return @"";

  return _LngFlags[lng];
  }

////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Determina las cantidad de direcciones de traducción instaladas
//int LGInstCount()
//  {
//  int count = 0;
//  
//  for( int src=0; src<LGCount; ++src )
//    {
//    for( int des=0; des<LGCount; ++des )
//      {
//      if( src==des ) continue;
//      if( _Inst[src][des] != 0 ) ++count;
//      }
//    }
//  
//  return count;
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Determina si el lenguaje fuente 'lng' esta instalado
//BOOL LGIsInstSrc( int lng )
//  {
//  if( lng<0 || lng>=LGCount ) return false;
//
//  int *InstSrcs = _Inst[lng];
//    
//  for( int j=0; j<LGCount; ++j )
//    if( InstSrcs[j] ) return TRUE;
//  
//  return FALSE;
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Determina si el lenguaje destino 'lng' esta instalado para el idioma fuente actual
//BOOL LGIsInstDes( int lng )
//  {
//  if( lng<0 || lng>LGCount || LGSrc<0 ) return false;
//
//  int *InstSrcs = _Inst[LGSrc];
//  
//  return InstSrcs[ lng ];
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Determina si la direccion de traducción src -> des esta instalada o no
//BOOL LGIsInstDir( int src, int des )
//  {
//  if( src<0 || src>=LGCount || des<0 || des>=LGCount ) return false;
//  
//  return _Inst[src][des];
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//void LGSetInstDir( int src, int des )
//  {
//  if( src<0 || src>=LGCount || des<0 || des>=LGCount ) return;
//  
//  _Inst[src][des] = 1;
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Obtiene un idioma destino inferido entre los idiomas instalados
//int LGInferedDes( int srcOld )
//  {
//  if( LGSrc==LGDes && LGIsInstDes(srcOld) ) return srcOld;
//  if( LGIsInstDes(LGDes) ) return LGDes;
//  
//  for( int j=0; j<LGCount; ++j )
//    if( LGIsInstDes(j) ) return j;
//    
//  return -1;
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Obtiene el primer idioma fuente que tenga al menos una dirección instalada
//int LGFirstSrc()
//  {
//  for( int j=0; j<LGCount; ++j )
//    if( LGIsInstSrc(j) ) return j;
//    
//  return -1;
//  }
//
////--------------------------------------------------------------------------------------------------------------------------------------------------------
//// Obtiene el proximo idioma destino disponible
//int LGNextDes()
//  {
//  for( int j=LGDes+1; j<LGCount; ++j )
//    if( LGIsInstDes(j) ) return j;
//    
//  for( int j=0; j<LGDes; ++j )
//    if( LGIsInstDes(j) ) return j;
//    
//  return -1;
//  }

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// Identifica si el caracter 'i' de la cadena 'str' en una letra
BOOL IsLetter( NSInteger idx, NSString* Txt )
  {
  unichar c = [Txt characterAtIndex:idx];
  return [[NSCharacterSet alphanumericCharacterSet] characterIsMember:c];
  }

//=========================================================================================================================================================
