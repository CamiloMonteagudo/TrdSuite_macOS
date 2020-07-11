
#include <string.h>
#include <stdlib.h>
#include "WinUtil.h"

//------------------------------------------------------------------
/*
void AnsiToUnicode(TDes16& aUnicode, const TDesC8& aAnsi)
	{
	aUnicode.Zero();
	
  CCnvCharacterSetConverter* oConverter = CCnvCharacterSetConverter::NewL();
  CCnvCharacterSetConverter::TAvailability avail = oConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, gFsSession);
  
  TInt state = CCnvCharacterSetConverter::KStateDefault;
  
  if (avail == CCnvCharacterSetConverter::EAvailable)
  	{
	  oConverter->ConvertToUnicode(aUnicode, aAnsi, state);
  	}
  
	delete oConverter;
 	}

void UnicodeToAnsi(TDes8& aAnsi, const TDesC16& aUnicode)
	{
	aAnsi.Zero();
	
  TInt error = 0;
  CCnvCharacterSetConverter* oConverter = CCnvCharacterSetConverter::NewL();
  CCnvCharacterSetConverter::TAvailability avail = oConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, gFsSession);
  
  if (avail == CCnvCharacterSetConverter::EAvailable)
  	{
  	error = oConverter->ConvertFromUnicode(aAnsi, aUnicode);
  	}
  
	delete oConverter;
 	}

CStringW AnsiToUnicode(const TDesC8& aAnsi)
	{
	CStringW sUnicode(aAnsi.Length() + 10);
	TPtr16 ptr = sUnicode.GetDes();
	
	AnsiToUnicode(ptr, aAnsi);
	
	return sUnicode;
 	}

CStringW AnsiToUnicode(const char* aAnsi)
	{
	return AnsiToUnicode(CStringA(aAnsi));
	}

CStringA UnicodeToAnsi(const TDesC16& aUnicode)
	{
	CStringA sAnsi(aUnicode.Length() + 10);
	TPtr8 ptr = sAnsi.GetDes();
	
	UnicodeToAnsi(ptr, aUnicode);
	
	return sAnsi;
	}

void StrSafeCopy(TDes& aDest, const TDesC& aSource)
	{
	aDest = aSource.Left(aDest.MaxLength());
	}
*/


#ifndef __STDC_WANT_SECURE_LIB__

errno_t strncpy_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count)
	{
	ASSERT(numberOfElements > count);
	strncpy(strDest, strSource, count);
	return 0;
	}

errno_t strcpy_s(char *strDest, size_t numberOfElements, const char *strSource)
	{
	ASSERT(strlen(strSource) < numberOfElements);
	strcpy(strDest, strSource);
	return 0;
	}

#endif

#if !defined(_MSC_VER) && !defined(strnlen)

size_t strnlen(const char *str, size_t maxlen)
	{
  size_t n;

  for(n = 0; n < maxlen && *str; n++, str++)
		;

  return n;
	}

#endif

int atoics(const CStringA& s) 
	{
	CStringA s1(s);
	return atoi(CS2SZ(s1));
	}
