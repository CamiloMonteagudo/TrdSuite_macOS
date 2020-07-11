//=========================================================================================================================================================
//  ParseText.h
//  TrdMac
//
//  Created by Camilo on 2/3/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Foundation/Foundation.h>

#define ITEM_NODEF -1
#define ITEM_NOTRD 0
#define ITEM_TRD   1
#define ITEM_CHGED 2

//=========================================================================================================================================================
@interface ParseItem : NSObject

  @property (nonatomic) Byte      Type;

  @property (nonatomic) NSString* txtSrc;
  @property (nonatomic) NSRange   rgSrc;
  @property (nonatomic) BOOL      chgSrc;

  @property (nonatomic) NSString* txtTrd;
  @property (nonatomic) NSRange   rgTrd;
  @property (nonatomic) BOOL      chgTrd;

  +(ParseItem*) ItemWithText:(NSString*) txt Range:(NSRange) rg Type:(Byte) tipo;

  -(NSInteger) setTrd:(NSString*) trd WithOffset:(NSInteger) off;

@end

//=========================================================================================================================================================
@interface ParseText : NSObject

@property (nonatomic) NSMutableArray* Items;

+(ParseText*) ParseWithText:(NSString*) txt;
+(ParseText*) Now;
+(void) Clear;

@end

//=========================================================================================================================================================
