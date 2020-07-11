
#include "WinUtil.h"
#ifdef _ANDROID_
#include <android/log.h>
#endif


TInt CLogger::iLogIndent = 0;
BOOL CLogger::iFirstTimeLoad = TRUE;

CLogger::CLogger(const char* aFileName):iFile(NULL)
{
  Open(aFileName);
}

CLogger::CLogger():iFile(NULL)
{
#ifdef _ANDROID_
  Open("/sdcard/trdlog.txt");
#else
  Open("trdlog.txt");
#endif
}

CLogger::~CLogger()
{
  if (iFile)
    {
    fclose(iFile);
    iFile = NULL;
    }
}

BOOL CLogger::Open(const char* aFileName)
{
  // si es la primera vez que se abre el logger, se reemplaza el log existente, sino se
  // abre el que ya existe para añadir datos
  //#if !defined(_APPLE_) && !defined(_BLACKBERRY_)
#if !defined(_BLACKBERRY_)
  
  if (iFirstTimeLoad)
    iFile = fopen_(aFileName, "w");
  else
    iFile = fopen_(aFileName, "a+");
		
  if (iFile == NULL)
    {
    ASSERT(FALSE);
    }
  
  iFirstTimeLoad = FALSE;
#endif
  
  return (iFile != NULL);
}

void CLogger::Log(const char* sLine)
{
  static int idxLog = 0;
  //for(TInt i=0; i<iLogIndent; i++)
  //	fputs("    ", iFile);
  
  // escribir el texto hacia la consola del IDE
#ifdef _ANDROID_
  //__android_log_print(ANDROID_LOG_DEBUG, "TranslatorJNI", "%s\n", sLine);
  __android_log_print(ANDROID_LOG_INFO, "TranslatorJNI", "%s\n", sLine);
  //__android_log_print(ANDROID_LOG_ERROR, "TranslatorJNI", "%s\n", sLine);
#elif defined(_WINDOWS_)
  char s[20000];
#ifdef ACTIVE_SAVE
		sprintf_(s, "%s\n", sLine);
#else
		//sprintf_(s, "%4d %s\n", idxLog, sLine);
		sprintf_(s, "%s\n", sLine);
#endif
  printf(s);
  OutputDebugStringA(s);
#elif defined(_APPLE_)
  //NSLog(@"%@", [NSString stringWithCString:sLine encoding:NSISOLatin1StringEncoding]);
#elif defined(_BLACKBERRY_)
  for(const char* s = sLine; *s != 0; s++)
    {
    char c = *s;
    //if (c <= 0 || c > 127)
    //	c = '#';
    fputc(c, stdout);
    }
  
  //fprintf(stdout, sLine);
  fprintf(stdout, "\n");
  fflush(stdout);
#endif
  
  // escribir hacia el fichero de log (trdlog.txt)
  if (iFile)
    {
#ifdef ACTIVE_SAVE
    fprintf(iFile, "%s\n", sLine);
#else
    //fprintf(iFile, "%4d %s\n", idxLog, sLine);
    fprintf(iFile, "%s\n", sLine);
#endif
    fflush(iFile);
    }
  //if (idxLog == 248)
  //	int k=0;
  idxLog++;
}
