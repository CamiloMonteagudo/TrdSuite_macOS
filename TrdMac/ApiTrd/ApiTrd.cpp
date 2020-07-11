//---------------------------------------------------------------------------
// cat.cpp : Defines the initialization routines for the DLL.
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "TranslationProcess.h"
#include "ApiTrd.h"

//-----------------------------------------------------------------------------------------------------------------
// Crea un objeto de trducción y retorna un handle para usar con las diferentes funciones del API
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(HTRD) TDNew()
  {
  return (HTRD)(new CTrdProc());
  }

//-----------------------------------------------------------------------------------------------------------------
// Libera todos los recursos relacionados con el objeto de traducción  'hTrd'
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDFree( HTRD hTrd )
  {
  delete ((CTrdProc *)hTrd);
	CFastHeap::Disable();
  }

//-----------------------------------------------------------------------------------------------------------------
// Pone el camino donde se encuentran los diccionarios de traducción
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDSetPath( HTRD hTrd, const CStringA& Path, int sw )
  {
  ((CTrdProc *)hTrd)->SetRootDir(Path, sw );
  }

//-----------------------------------------------------------------------------------------------------------------
// Abre la traducccion a partir del nombre en texto del idioma fuente y el de destino
//-----------------------------------------------------------------------------------------------------------------
//DLLENTRY(bool) TDOpen( HTRD hTrd, const CStringF& sSrc, const CStringF& sDes )
//  {
//	CFastHeap::CreateOrReset();
//
//  CTrdProc *TrdProc = (CTrdProc *)hTrd;
//
//  if( TrdProc != NULL )
//    {
//    TrdProc->SetTrdDir( sSrc, sDes );
//    if(TrdProc->LoadTranslationData())
//      return true;
//    }
//
//  return false;
//  }

//-----------------------------------------------------------------------------------------------------------------
// Salva los diccionarios de la traduccion
//-----------------------------------------------------------------------------------------------------------------
#ifdef ACTIVE_SAVE
DLLENTRY(bool) TDSave( HTRD hTrd)
  {
  CTrdProc *TrdProc = (CTrdProc *)hTrd;

  if( TrdProc != NULL )
    {
    if(TrdProc->SaveTranslationData())
      return true;
    }

  return false;
  }
#endif

//-----------------------------------------------------------------------------------------------------------------
// Abre la traducccion a partir del numero del idioma fuente y el de destino
// Tanto Source como Target pueden tomar los valores 
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(bool) TDOpen( HTRD hTrd, int iSrc, int iDes )
  {
  CFastHeap::CreateOrReset();
  
  CTrdProc *TrdProc = (CTrdProc *)hTrd;
  
  if( TrdProc != NULL )
    {
    TrdProc->SetTrdDir( iSrc, iDes );
    if(TrdProc->LoadTranslationData())
      return true;
    }
  
  return false;
  }

//-----------------------------------------------------------------------------------------------------------------
// Abre la traducción a partir de un número que da la dirección de traducción
//-----------------------------------------------------------------------------------------------------------------
//DLLENTRY(bool) TDOpen( HTRD hTrd,LPCSTR sLang )
//  {
//             // "0" ,"1" ,"2" ,"3" ,"4" ,"5" ,"6" ,"7" ,"8" ,"9" ,"10","11","12","13","14","15","16","17","18","19"};
//  LPCFSTR Src[]={LF"Es",LF"En",LF"It",LF"En",LF"It",LF"Es",LF"It",LF"De",LF"It",LF"Fr",LF"En",LF"Fr",LF"Es",LF"Fr",LF"Es",LF"Pt",LF"It",LF"Pt",LF"Es",LF"De"};
//  LPCFSTR Des[]={LF"En",LF"Es",LF"En",LF"It",LF"Es",LF"It",LF"De",LF"It",LF"Fr",LF"It",LF"Fr",LF"En",LF"Fr",LF"Es",LF"Pt",LF"Es",LF"Pt",LF"It",LF"De",LF"Es"};
//
//  int iLang = atoi(sLang);
//  if( iLang<0 || iLang>=(TInt)(sizeof(Src)/sizeof(Src[0])) )
//    return false;
//
//  return TDOpen( hTrd, Src[iLang], Des[iLang]);
//  }

//-----------------------------------------------------------------------------------------------------------------
// Crea los datos corespondientes a un usuario 
// hTrd - Handle a un idioma anteriormente abierto con CatOpenTranslation
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(HUTRD) TDOpenUser( HTRD hTrd ) 
	{
	CFastHeap::Enable();

	if( !hTrd ) return NULL;

  CTrdProc *TrdProc = (CTrdProc *)hTrd;

	return (HUTRD) new CTranslationUser(TrdProc->GetPtrTranslationData());
	}

//-----------------------------------------------------------------------------------------------------------------
// Libera los datos corespondientes al usuario idxUser
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDFreeUser( HUTRD hUTrd )
	{
	CFastHeap::CheckUse();

	if( hUTrd )
		delete (CTranslationUser *)hUTrd;

	CFastHeap::Disable();
	}

//-----------------------------------------------------------------------------------------------------------------
// Señala la marca a utilizar al inicio de las palabras no encontradas en el dcc para el usuario dado
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDSetIniNF( HUTRD hUTrd, LPCSTR sMark )
  {
  ((CTranslationUser *)hUTrd)->SetNotFoundInitialMark(sMark);
  }

//-----------------------------------------------------------------------------------------------------------------
// Señala la marca a utilizar al final de las palabras no encontradas en el dcc para el usuario dado
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDSetEndNF( HUTRD hUTrd, LPCSTR sMark )
  {
  ((CTranslationUser *)hUTrd)->SetNotFoundFinalMark(sMark);
  }

//-----------------------------------------------------------------------------------------------------------------
// Señala la marca a utilizar al inicio de las palabras a no traducir para el usuario dado
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDSetIniNT( HUTRD hUTrd, LPCSTR sMark )
  {
  ((CTranslationUser *)hUTrd)->SetNoTranslationInitialMark(sMark);
  }

//-----------------------------------------------------------------------------------------------------------------
// Señala la marca a utilizar al final de las palabras a no traducir para el usuario dado
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDSetEndNT( HUTRD hUTrd, LPCSTR sMark )
  {
  ((CTranslationUser *)hUTrd)->SetNotTranslationFinalMark(sMark);
  }

//-----------------------------------------------------------------------------------------------------------------
// Señala la oración que se desea traducir
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDSetSentence( HUTRD hUTrd, const CStringA& sSentence)
  {
	CNodeAllocator::Restart();  // resetear los nodos para cada oracion
	ResetStringData();					// no imprescindible pero recomendable

  ((CTranslationUser *)hUTrd)->SetSentence(sSentence,FMT_RTF);
  }

//-----------------------------------------------------------------------------------------------------------------
// Traduce la oración anteriormente señalada
// la variable wherebegin puede tomar los siguientes valores 
//
// TD_TRANSLATE					  - Traduce la oración
// TD_REFRESH						  - Refreca la traducción (Solo para compatibilidad)
// TD_RETRANSLATE					- Vuelve a traducir la oración (Solo para compatibilidad)
// TD_EMPTY_OPTION				- (Solo para compatibilidad)
// TD_FIND_PROPER_NOUN	  - (Solo para compatibilidad)
// TD_FIND_NOUN_PHRASE	  - Busca posibles frases sustantivas
// TD_SCAN_WORD_FRECUENCY	- Retorna lista de palabras de la oración para luego calcular frecuencia de uso
// TD_SCAN_SEM_VALUE_WORD	- Busca palabras con valor semantico
// TD_FILL_GRAM_TYPE_DCC  - Retorna una lista con todos los tipos gramaticales que aparecen en la oración
// TD_FILL_STAT_FOR_WORD	- Retorna las estadisticas de la palabras de la oración pra llenar dcc de estadisticas
// TD_SCAN_NOT_FOUND_WORD	- Busca las palabras no encontradas dentro de la oración
// TD_EVAL_INPUT					- Cuenta la cantidad de ngram 3 de la oracion presentes en el dcc de ngram
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) TDTranslate( HUTRD hUTrd, WORD wherebegin)
  {
  CTranslationUser *UTrd = (CTranslationUser *)hUTrd;

  switch(wherebegin)
    {
    case TD_TRANSLATE           : UTrd->TranslateSentence();                break;
    }
  }

//-----------------------------------------------------------------------------------------------------------------
// Obtiene la oración traducida
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(const CStringA&) TDGetSentence( HUTRD hUTrd )
  {
  return ((CTranslationUser *)hUTrd)->GetTranslatedSentence();
  }

//-----------------------------------------------------------------------------------------------------------------
// Obtiene diferentes datos del proceso de traduccion
// 0 obtiene la oracion traducida
// 1 obtiene los tipos gramaticales despues de buscar las palabras en el dcc
// 2 obtiene los tipos gramaticales antes aplicar el lenguaje para reducirlos
// 3 obtiene los tipos gramaticales al final de la traduccion
// 4 obtiene lista de palabras que conforman la oracion
// 5 obtiene las frases idiomaticas encontradas en el dcc
// 6 obtiene las palabras no encontradas en el dcc
// 7 obtiene traza detallada del proceso de traduccion
// 8 obtiene las distintas variantes de la traduccion separadas por ; por orden
//-----------------------------------------------------------------------------------------------------------------
/*DLLENTRY(const CStringA&) TDGetOutput( HUTRD hUTrd, int data )
  {
  CTranslationUser *UTrd = (CTranslationUser *)hUTrd;
  switch(data)
    {
      case 0:	return UTrd->GetTranslatedSentence();
      case 1:	return UTrd->GetInitialTypes();
      case 2:	return UTrd->GetMiddleTypes();
      case 3:	return UTrd->GetFinalTypes();
      case 4:	return UTrd->GetWordList();
      case 5:	return UTrd->GetFoundPhraseList();
      case 6: return UTrd->GetNotFoundWordList();
      case 7: return UTrd->GetTranslatedSentence();
      case 8: return UTrd->GetStatisticalSentences();
    }
  return CANull;
  }*/

//-----------------------------------------------------------------------------------------------------------------
