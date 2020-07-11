//=========================================================================================================================================================
//  Translate.h
//  TrdMac
//
//  Created by Camilo on 1/30/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Cocoa/Cocoa.h>

@interface Translate : NSObject <NSTextViewDelegate>

@property (unsafe_unretained) IBOutlet NSTextView *TextSrc;
@property (unsafe_unretained) IBOutlet NSTextView *TextTrd;

@property (weak,nonatomic) NSWindow *window;

-(BOOL) OpenTextAtUrl:(NSURL*) url;
-(void) NewText;

@end
//=========================================================================================================================================================
