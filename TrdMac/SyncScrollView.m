//=========================================================================================================================================================
//  SyncScrollView.m
//  TrdMac
//
//  Created by Camilo on 2/11/16.
//  Copyright (c) 2016 BigXSoft. All rights reserved.
//=========================================================================================================================================================

#import "SyncScrollView.h"

static bool IsSync;

//=========================================================================================================================================================
@interface SyncScrollView()
@end

//=========================================================================================================================================================
@implementation SyncScrollView

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pone el otro scroll sincronizado al este
- (void)setSyncTo:(NSScrollView *)SyncTo
  {
  [self stopSynchronizing];                               // Si ya estaba sincronizada quita la sicronización
  
  _SyncTo = SyncTo;                                       // Pone el otro scroll a que va sincronizar
  
  NSView *ContentView = [SyncTo contentView];             // Obtiene el la vista del contenido del scroll
  
  [ContentView setPostsBoundsChangedNotifications:YES];   // Asegura que se notifique cada ves que cambia el contenido
  
  // Se registra para ser notificado cada vez que cambia el contenido el otro scroll
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(ContentViewDidChange:)
                                               name:NSViewBoundsDidChangeNotification
                                             object:ContentView];
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Proboca que no se sincronicen los scrolles
+(void) PauseSync
  {
  IsSync = true;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Activa la sincronización entre los dos scrollers si estaba detenida
+(void) ActiveSync
  {
  IsSync = false;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
- (void)ContentViewDidChange:(NSNotification *)notification
  {
  if( IsSync ) return;
  IsSync = true;
  
  NSClipView *ContView1 = [notification object];                  // Obtine el ContentView que mando la notificación (el que cambia)
  NSClipView *ContView2 = [self contentView];                     // Obtine el ContentView del scroll actual

  float yOffset1 = ContView1.bounds.origin.y;                     // Posición del otro scroll (el que cambio)
//  float yOffset2 = ContView2.bounds.origin.y;                     // Posición del scroll actual
  
//  float HView1 = [ContentView bounds].size.height;
//  float HView2 = [[self contentView] bounds].size.height;
//  
  float HDoc1 = ((NSView*)ContView1.documentView).frame.size.height;  // Tamaño del scroll que cambio
  float HDoc2 = ((NSView*)ContView2.documentView).frame.size.height;  // Tamaño del scroll actual
  
  int newOff = (yOffset1 * HDoc2) / HDoc1;
  
  [ContView2 scrollToPoint: NSMakePoint(0, newOff) ];  // Pone el scroll en el nueva posición
    
  [self reflectScrolledClipView:[self contentView]];              // Rerfleja los cambios en el scroll
  
  IsSync = false;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Detiene la sincronización entre dos scrollers
- (void)stopSynchronizing
  {
  if( _SyncTo == nil ) return;

  // Deja de recibir las notificacion de los cambios en el otro scroll
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:NSViewBoundsDidChangeNotification
                                                object:[_SyncTo contentView] ];
    
  _SyncTo = nil;
  }

@end

//=========================================================================================================================================================

