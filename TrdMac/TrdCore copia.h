//=========================================================================================================================================================
//  TrdCore.h
//  TrdMac
//
//  Created by Camilo on 1/31/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Foundation/Foundation.h>
#import "ParseText.h"

//=========================================================================================================================================================
@interface TrdItem : NSObject

  @property (nonatomic,readonly) Byte Type;

  @property (nonatomic,readonly) NSString* Src;
  @property (nonatomic,readonly) NSString* Trd;

  @property (nonatomic,readonly) NSRange rgSrc;
  @property (nonatomic,readonly) NSRange rgTrd;

  +(TrdItem*) ItemWithType:(unsigned char)tipo SrcText:(NSString*)Text At:(int) Idx;

@end

//=========================================================================================================================================================
@interface TrdCore : NSObject

+(void)     initTrdApi;
+(TrdCore*) TrdWithDes:(int) src AndDes:(int) des;
+(void)     TrdFreeWithDes:(int) src AndDes:(int) des;
+(void)     TrdFree:(TrdCore*) trd;
+(void)     TrdFreeAll;

-(void) TranslateFromParse:(ParseText*) ps;

@end

//=========================================================================================================================================================
