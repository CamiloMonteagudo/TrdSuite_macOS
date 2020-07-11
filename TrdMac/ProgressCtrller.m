//=========================================================================================================================================================
//  ProgressCtrller.m
//  TrdMac
//
//  Created by Camilo on 2/28/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "ProgressCtrller.h"

//=========================================================================================================================================================
@interface ProgressCtrller ()
  {
  NSWindow* winParent;
  NSModalSession Session;
  CFAbsoluteTime startTime;
  CFAbsoluteTime lastTime;
  }

@property (weak) IBOutlet NSProgressIndicator *Progress;
@property (weak) IBOutlet NSTextField *lbTraduciendo;

- (IBAction)OnCancel:(id)sender;

@end

//=========================================================================================================================================================
@implementation ProgressCtrller

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Muestra la ventana con un indicador de progreso
- (id)initInWindow:(NSWindow*) window
  {
  self = [super initWithWindowNibName:@"ProgressCtrller"];
  if( !self ) return nil;
  
  winParent = window;
  [self loadWindow];
  
  startTime = CFAbsoluteTimeGetCurrent();
  Session = nil;
  
  return self;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Cierra la ventana del indicador de progreso
- (void) Finish
  {
  if( Session != nil )
    {
    [winParent endSheet:self.window];
    [NSApp endModalSession:Session];
  
    Session = nil;
    }
  
  [self.window orderOut:self];
  startTime += 3000;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Se llama cuando se oprime el boton de cancelar la traducción
- (IBAction)OnCancel:(id)sender
  {
  _Cancel = TRUE;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Establece la cantidad de elementos del progreso
- (void)setCount:(int)Count
  {
  _Progress.minValue = 0;
  _Progress.maxValue = Count;
  
  if( Count>1000 ) startTime -=3;
  }

- (int)Count
  {
  return _Progress.maxValue;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Establece la posición actual del progreso
- (void)setPos:(int)Pos
  {
  CFAbsoluteTime time = CFAbsoluteTimeGetCurrent();
  
  if( time-lastTime > 0.5 )
    {
    if( Session == nil && time > (startTime+1))
      {
      [winParent beginSheet:self.window completionHandler:nil];
  
      Session = [NSApp beginModalSessionForWindow:self.window];
      }
  
    _Progress.doubleValue = Pos;
    
    lastTime = time;
    }
  
  if( Session != nil && [NSApp runModalSession:Session] != NSModalResponseContinue )
    {
    [self Finish];
    _Cancel = TRUE;
    }
  
  [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate: [NSDate date] ];   // Procesa los mensajes
  }

- (int)Pos
  {
  return _Progress.doubleValue;
  }

@end

//=========================================================================================================================================================
