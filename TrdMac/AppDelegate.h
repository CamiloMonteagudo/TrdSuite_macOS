//=========================================================================================================================================================
//  AppDelegate.h
//  TrdMac
//
//  Created by Camilo on 1/29/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (copy,nonatomic) NSURL *FileUrl;

- (void) AfterOfSave;

+ (void)ShowMsgText:(NSString*) txt InWindow:(NSWindow*) Wnd;

@end
//=========================================================================================================================================================

