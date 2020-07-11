//=========================================================================================================================================================
//  AppData.h
//  PruTranslate
//
//  Created by Camilo on 31/12/14.
//  Copyright (c) 2014 Softlingo. All rights reserved.
//=========================================================================================================================================================

#import <Cocoa/Cocoa.h>

//=========================================================================================================================================================
#define LGCount  4
#define DIRCount 12

#define FindOpt    (NSCaseInsensitiveSearch|NSDiacriticInsensitiveSearch)
#define DirChangedNotification  @"DirChangedNotification"

//=========================================================================================================================================================
extern int  LGSrc;
extern int  LGDes;
extern int  iUser;

//=========================================================================================================================================================
extern NSString*  LGFlag( int lng);
//extern BOOL       LGIsInstSrc( int lng );
//extern BOOL       LGIsInstDes( int lng );
//extern int        LGInferedDes( int lng );
//extern int        LGFirstSrc();
//extern int        LGNextDes();

//extern int        LGInstCount();
//extern BOOL       LGIsInstDir( int src, int des );
//extern void       LGSetInstDir( int src, int des );

extern NSString*  LGAbrv( int lng );
extern NSString*  LGName( int lng );

extern NSString* DIRName( int iDir );
extern BOOL      DIRInstalled( int iDir );
extern int       DIRSrc( int iDir );
extern int       DIRDes( int iDir );
extern int       DIRFromLangs(int src, int des);
extern int       DIRFirst();

extern BOOL IsLetter( NSInteger idx, NSString* Txt );

//=========================================================================================================================================================


