//=========================================================================================================================================================
//  EditDictCtrller.h
//  TrdMac
//
//  Created by Camilo on 2/28/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Cocoa/Cocoa.h>

//=========================================================================================================================================================
@interface EditDictCtrller : NSWindowController <NSTextViewDelegate>

- (id)initInWindow:(NSWindow*) window OnClose:(SEL) fn Target:(id) target;

@end
//=========================================================================================================================================================
