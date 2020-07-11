//=========================================================================================================================================================
//  DictCore.h
//  IdiomaXTranslator
//
//  Created by MacPC on 5/27/11.
//  Copyright 2011 IdiomaX. All rights reserved.
//
//=========================================================================================================================================================

#import <Cocoa/Cocoa.h>

#define DICT_OPEN    1
#define DICT_OPENED  2
#define DICT_FAIL    0

extern NSAttributedString* EdictTypeLabel( NSString* Label, NSString* sType );

//=========================================================================================================================================================
@interface DictCore : NSObject

+(int) OpenDictSrc:(int) src Dest:(int) des;
+(void) CloseDict;

+(NSAttributedString*) getWDataFromIndex: (int)idx NoKey:(BOOL) noKy;
+(NSAttributedString*) FormatedMsg:(NSString*) sMsg Title:(NSString*) sTitle;

+(NSString *)getWordAt: (int)idx;

+(int)  getSize;
+(int)  getWordIdx: (NSString *) Key;
+(bool) Found;
+(bool) WordKey;
+(BOOL) isUserKeyIdx: (int)idx;
+(BOOL) DelUserKey: (NSString*) sKey;
+(BOOL) isDictModified;
+(BOOL) Save;


//+(void) KeysFilter:(NSString*) sFilter;
//+(void) RemoveFilter;
//+(BOOL) IsFiltered;

@end
//=========================================================================================================================================================

//=========================================================================================================================================================
// Mantiene los datos de la última palabra para su posible edicción
//=========================================================================================================================================================
@interface WrdData : NSObject

+ (int)CountTypes;
+ (NSString*) GetWord;

+ (void) SetNewWord:(NSString*) sWord;
+ (NSString*) CodeFromType:(int) iTipo;
+ (NSString*) MeansStringFromType:(int) iTipo;
+ (void) DeleteMeansForType:(int) iTipo;
+ (BOOL) MeansString:(NSString*)strMeans ToType:(int) iTipo;
+ (BOOL) IsValidAddType:(NSString*) sType;
+ (BOOL) AddType:(NSString*) sType;
+ (BOOL) AddMean:(NSString*) sMean InType:(NSString*) sType;
+ (void) RemoveType:(NSString*) sType;
+ (BOOL) SaveData;

@end
//=========================================================================================================================================================
