//
//  Observer.h
//  Translator
//
//  Created by MacPC on 2/13/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#ifndef Translator_Observer_h
#define Translator_Observer_h

class CObsMaster;

// Clase que recibe notificaciones cuando el objeto master cambia
class CObserver
  {
  friend class CObsMaster;
  
  private:
  CObsMaster* mMaster;
  
  void removeMaster(CObsMaster* aMaster)
    {
    ASSERT(mMaster == aMaster);
    mMaster = NULL;
    }
  
  void setMaster(CObsMaster* aMaster)
    {
    ASSERT(mMaster == NULL);
    mMaster = aMaster;
    }

  public:
  CObserver():mMaster(NULL) {}
  
  ~CObserver()
    {
    detach();
    }
  
  void detach();
    
  virtual void onModify() = 0;
  
  };

// En realidad esta clase debia tener una lista de observadores, pero solo nos hace falta uno
class CObsMaster
  {
  friend class CObserver;
  
  private:
  
  CObserver* mObserver;
  
  void removeObserver(CObserver* aObserver)
    {
    ASSERT(mObserver == aObserver);
    mObserver = NULL;
    }
  
  public:
  
  CObsMaster():mObserver(NULL) {}
  
  ~CObsMaster()
    {
    detach();
    }

  void detach()
    {
    if (mObserver)
      {
      mObserver->removeMaster(this);
      mObserver = NULL;
      }
    }
  
  void setObserver(CObserver* aObserver)
    {
    ASSERT(mObserver == NULL);
    mObserver = aObserver;
    mObserver->setMaster(this);
    }

  void sendOnModify()
    {
    if (mObserver)
      mObserver->onModify();
    }
  };

inline void CObserver::detach()
  {
  if (mMaster)
    {
    mMaster->removeObserver(this);
    mMaster = NULL;
    }
  }

#endif
