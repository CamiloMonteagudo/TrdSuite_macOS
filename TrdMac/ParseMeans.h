//=========================================================================================================================================================
//  ParseMeans.h
//  TrdMac
//
//  Created by Camilo on 3/10/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Foundation/Foundation.h>

#define ITEM_GEN    1
#define ITEM_NUM    2
#define ITEM_ESP    4
#define ITEM_MEAN   8

//=========================================================================================================================================================
@interface MeanItem : NSObject

+(MeanItem*) ItemWithRange:(NSRange) rg Type:(int) type Value:(NSString*) sVal;

@property (nonatomic) NSRange     rg;
@property (nonatomic) int       tipo;
@property (nonatomic) NSString* sVal;

@end
//=========================================================================================================================================================

//=========================================================================================================================================================
@interface ParseMeans : NSObject

+(ParseMeans*) ParseWithString:(NSString*) str;

-(void) ParseString:(NSString*) str;

@property (nonatomic,readonly) NSArray* Items;

@end
//=========================================================================================================================================================
