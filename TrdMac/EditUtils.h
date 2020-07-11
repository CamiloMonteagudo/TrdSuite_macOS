//=========================================================================================================================================================
//  EditTxtSrc.h
//  TrdMac
//
//  Created by Camilo on 2/22/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import <Cocoa/Cocoa.h>
#import "ParseText.h"

//=========================================================================================================================================================

Byte SentenceSrcAtPos( NSInteger iPos, int* idx);
Byte SentenceTrdAtPos( NSInteger iPos, int* idx);

BOOL EditSrcRange( NSRange Range, NSString* str, NSString* Text);
void EditTrdRange( NSRange Range, NSString* str, NSString* Text);

int  FindSrcItemAt( NSInteger iPos, int ItemIni, ParseText* ps);
int  FindTrdItemAt( NSInteger iPos, int ItemIni, ParseText* ps);

void ShiftSrc( int ItemIni, NSInteger Delta, ParseText* ps);
void ShiftTrd( int ItemIni, NSInteger Delta, ParseText* ps);

void ChangeItemWithParse( NSInteger iPos, ParseText* ps);

NSInteger HomeSentence();
NSInteger NextSentenceAtPos( NSInteger iPos );
NSInteger PrevioSentenceAtPos( NSInteger iPos );
NSInteger LastSentence();

BOOL      IsSrcChanged();
NSInteger NextSentenceChgdAtPos( NSInteger iPos );
NSInteger PrevioSentenceChgdAtPos( NSInteger iPos );

//=========================================================================================================================================================
