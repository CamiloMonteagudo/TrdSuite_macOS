//========================================================================
//
// pdftortf.cpp
//
//
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <aconf.h>
#include <time.h>
//#include "parseargs.h"
#include "GString.h"
#include "gmem.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "RtfOutputDev.h"
#include "GlobalParams.h"
#include "Error.h"
#include "config.h"
#include "gfile.h"
#include "ErrorCodes.h"
#include "Errors.h"
#include "PdfToRtf.h"
#include "ProcOptions.h"

//static GString* getInfoString(Dict *infoDict, char *key);
//static GString* getInfoDate(Dict *infoDict, char *key);
int TranslateError(ProcOptions* p,int code);


//Traduce los errores internos a los definidos por esta libreria
int TranslateError(ProcOptions* p,int code)
{
	switch (code)
	{
		case errNone:			return ERR_OK;
		case errOpenFile:	return ERR_PDF_NOT_FOUND;
		case errBadCatalog:
		case errDamaged:	return ERR_BAD_PDF;
		case errEncrypted:	
			if (p->userPassword != NULL)
				return ERR_BAD_USR_PWD;
			else
				return ERR_NO_USR_PWD;
		case errPermission:	
			if (p->ownerPassword != NULL)
				return ERR_BAD_OWN_PWD;
			else
				return ERR_NO_OWN_PWD;
		default: 
			if (code>=-1)
				return ERR_UNKNOWN;
			else
				return code;
	}
	
};

long InitProcess()
{
  return (long) new ProcOptions();
}

void EndProcess(long procOptions)
{
  delete (ProcOptions *)procOptions;
}

void LoadImages(long idProcess, bool bImages, bool bJPG, bool bPNG, bool bBinary)
{
  ((ProcOptions*)idProcess)->loadImages( (GBool)bImages, (GBool)bJPG, (GBool)bPNG, (GBool)bBinary);
}

/*void SetProgress(long idProcess, HWND hWnd, UINT Message)
{
  ((ProcOptions*)idProcess)->setProgress( hWnd, Message);
}

void GetProgress(long idProcess, long *page, long *total)
{
  ((ProcOptions*)idProcess)->getProgress(page, total);
}

void SetPasswords(long idProcess, char* aOwnerPassword,char* aUserPassword)
{
  ((ProcOptions*)idProcess)->setPasswords( aOwnerPassword, aUserPassword);
}*/

void SetRange(long idProcess, int firstPage, int lastPage)
{
  ((ProcOptions*)idProcess)->setRange( firstPage, lastPage);
}

int FreeRtf(char * sRtf)
{
  if (sRtf) delete []sRtf;
    return 0;
};

int Pdf2Rtf(long idProcess, const char* inPDFFile, char** outRtf, long* sLen)
{
  PDFDoc *doc = NULL;
  GString *fileName = NULL;
  //GString *docTitle = NULL;
  //GString *author = NULL, *keywords = NULL, *subject = NULL, *date = NULL;
  RtfOutputDev *rtfOut = NULL;
  GString* sRtf = NULL;
  Object info;
  ProcOptions *procOptions = (ProcOptions*)idProcess;
  int firstPage;
  int lastPage;
  double scale;
  char textEncName[128] = "";

  // init error file
  //errorInit();
  *outRtf = NULL;
	*sLen = 0;

  //seteo de las opciones
  scale = 1.0;

  // read config file
  globalParams = new GlobalParams("");

  if (textEncName[0]) {
    globalParams->setTextEncoding(textEncName);
    if( !globalParams->getTextEncoding() )  {
			goto err1;    
    }
  }

  // open PDF file

  fileName = new GString(inPDFFile);

  doc = new PDFDoc(fileName, procOptions->ownerPassword, procOptions->userPassword);
  SetError(doc->getErrorCode());

  if (!doc->isOk()) {
    goto err1;
  }

  // check for copy permission
/*  if (!doc->okToCopy()) {
	SetError(errPermission);
  //  error(-1, "Copying of text from this document is not allowed.");
    goto err2;
  }*/

  if (scale>3.0) scale=3.0;
  if (scale<0.5) scale=0.5;
   
  // get page range
  firstPage = procOptions->firstPage;
  lastPage = procOptions->lastPage;

  if (firstPage < 1)
    firstPage = 1;
  if (lastPage < 1 || lastPage > doc->getNumPages())
    lastPage = doc->getNumPages();

  /*doc->getDocInfo(&info);
  if (info.isDict()) {
    docTitle = getInfoString(info.getDict(), "Title");
    author = getInfoString(info.getDict(), "Author");
    keywords = getInfoString(info.getDict(), "Keywords");
    subject = getInfoString(info.getDict(), "Subject");
    date = getInfoDate(info.getDict(), "ModDate");
    if (!date)
			date = getInfoDate(info.getDict(), "CreationDate");
  }
  info.free();*/

  //Redireccionar las salidas hacia un fichero
//#ifdef _DEBUG
//  {
//    FILE *file = freopen("C:\\Projects\\PDF2Text\\Output.txt", "w", stdout );
//    setbuf( file, NULL);
//  }
//#endif

  sRtf = new GString();
  rtfOut = new RtfOutputDev( sRtf, doc, procOptions);

  /*if (docTitle)
      delete docTitle;

  if (author)
      delete author;

  if (keywords)
      delete keywords;

  if (subject)
      delete subject;

  if (date)
      delete date;*/

  if (rtfOut->isOk()) {
    doc->displayPages(rtfOut, firstPage, lastPage, (72*scale), (72*scale), 0, gTrue, gFalse);

    rtfOut->WriteHeader();
  }

  //Obtener el buffer con el contenido del fichero
  if (GetError() == ERR_OK)	{
		*sLen = sRtf->getLength();
    *outRtf = sRtf->detach();
	}

  delete rtfOut;
  delete sRtf;

  // clean up
// err2:
 err1:
  if(doc) delete doc;
  if(globalParams) delete globalParams;

  int iError = TranslateError( procOptions, GetError());
  return iError;
}

/*static GString* getInfoString(Dict *infoDict, char *key) {
  Object obj;
  GString *s1 = NULL;

  if (infoDict->lookup(key, &obj)->isString()) {
    s1 = new GString(obj.getString());
  }
  obj.free();
  return s1;
}

static GString* getInfoDate(Dict *infoDict, char *key) {
  Object obj;
  char *s;
  int year, mon, day, hour, min, sec;
  struct tm tmStruct;
  GString *result = NULL;
  char buf[256];

  if (infoDict->lookup(key, &obj)->isString()) {
    s = obj.getString()->getCString();
    if (s[0] == 'D' && s[1] == ':') {
      s += 2;
    }
    if (sscanf_s(s, "%4d%2d%2d%2d%2d%2d",
               &year, &mon, &day, &hour, &min, &sec) == 6) {
      tmStruct.tm_year = year - 1900;
      tmStruct.tm_mon = mon - 1;
      tmStruct.tm_mday = day;
      tmStruct.tm_hour = hour;
      tmStruct.tm_min = min;
      tmStruct.tm_sec = sec;
      tmStruct.tm_wday = -1;
      tmStruct.tm_yday = -1;
      tmStruct.tm_isdst = -1;
      mktime(&tmStruct); // compute the tm_wday and tm_yday fields
      if (strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S+00:00", &tmStruct)) {
	result = new GString(buf);
      } else {
        result = new GString(s);
      }
    } else {
      result = new GString(s);
    }
  }
  obj.free();
  return result;
}*/


