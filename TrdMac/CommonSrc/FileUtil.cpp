
#include "WinUtil.h"


BOOL FileExists(const CStringA& aFileName)
	{
	CFile file;
	BOOL res = file.Open(aFileName, CFile::modeRead);
	file.Close();
	return res;
	}

#ifdef __STDC_WANT_SECURE_LIB__

FILE* fopen_(const char * aFileName, const char * mode)
	{
	FILE* file = NULL;
	errno_t err = fopen_s(&file, aFileName, mode);
	return (err == 0) ? file : NULL;
	}

FILE* tmpfile_()
	{
	FILE* aFile = NULL;
	errno_t err = tmpfile_s(&aFile);
	return (err == 0) ? aFile : NULL;
	};

#else

FILE* fopen_(const char * aFileName, const char * mode)
	{
	return fopen(aFileName, mode);
	}

FILE* tmpfile_()
	{
	return tmpfile();
	}

#endif

// Pointer mapping constants
#define wNullTag        ((WORD)0)           // special tag indicating NULL ptrs
#define wNewClassTag    ((WORD)0xFFFF)      // special tag indicating new CRuntimeClass
#define wClassTag       ((WORD)0x8000)      // 0x8000 indicates class tag (OR'd)
#define dwBigClassTag   ((DWORD)0x80000000) // 0x8000000 indicates big class tag (OR'd)
#define wBigObjectTag   ((WORD)0x7FFF)      // 0x7FFF indicates DWORD object tag

#define CHUNK_SIZE 32

void CFile::ReadAsSZString(CStringA& aStr)
	{
	TInt startPos = GetPosition();
	char* pBuffer = STAnsi::iTempBuffer;
	TInt nTotalLen = 0;

	while (true)
		{
		ReadBuffer(pBuffer, CHUNK_SIZE);
		TInt len = (int)strnlen(pBuffer, CHUNK_SIZE);

		if (len < CHUNK_SIZE)
			{ // se encontro el fin de la cadena
			nTotalLen = (int)(pBuffer - STAnsi::iTempBuffer + len + 1);

			// acomodar la posicion en el fichero
			Seek(startPos + nTotalLen, CFile::begin);
			break;
			}

		pBuffer += CHUNK_SIZE;
		ASSERT(pBuffer - STAnsi::iTempBuffer + CHUNK_SIZE < TEMP_BUFFER_SIZE);
		}

	aStr.Copy(STAnsi::iTempBuffer);//, nTotalLen);
	}

// Igual que el anterior pero devuelve un LPCSTR alojado en el FastHeap
LPCSTR CFile::ReadFastAsSZString()
	{
	TInt startPos = GetPosition();
	char* pBuffer = STAnsi::iTempBuffer;
	TInt nTotalLen = 0;

	while (true)
		{
		ReadBuffer(pBuffer, CHUNK_SIZE);
		TInt len = (int)strnlen(pBuffer, CHUNK_SIZE);

		if (len < CHUNK_SIZE)
			{ // se encontro el fin de la cadena
			nTotalLen = (int)(pBuffer - STAnsi::iTempBuffer + len + 1);

			// acomodar la posicion en el fichero
			Seek(startPos + nTotalLen, CFile::begin);
			break;
			}

		pBuffer += CHUNK_SIZE;
		ASSERT(pBuffer - STAnsi::iTempBuffer + CHUNK_SIZE < TEMP_BUFFER_SIZE);
		}

	LPSTR szString = (LPSTR)CFastHeap::Get(nTotalLen);
	strcpy_s(szString, nTotalLen, STAnsi::iTempBuffer);
	return szString;
	}

TInt CArchive::ReadClass()
	{
	DWORD obTag;
	WORD wTag;
	TInt idxClassReal = -1;
	
	*this >> wTag;
	if (wTag == wBigObjectTag)
		*this >> obTag;
	else
		obTag = ((wTag & wClassTag) << 16) | (wTag & ~wClassTag);
	
	if (wTag == wNewClassTag)
		{
		WORD schemaNum;
		*this >> schemaNum;
		CStringA sClassName;
		ReadClassName(sClassName);
		
		//TRACE("ClassName: %s", CS2SZ(sClassName));
		idxClassReal = GetClassIndexByName(sClassName);
		
		if (idxClassReal == -1)
			ASSERT(FALSE);
		
		iCachedClasses.Add(idxClassReal);
		iCachedIndex.Add(iIndexAdjust);
		iIndexAdjust++;
		}
	else
		{
		TInt idxClass = (obTag & ~dwBigClassTag);
		if (idxClass == iLastIdxClass)
			idxClassReal = iLastRealIdxClass;
		else
			{
			iLastIdxClass = idxClass;
			TInt index = iCachedIndex.Find(idxClass);
			if (index != -1)
				iLastRealIdxClass = idxClassReal = iCachedClasses[index];
			}
		
		if (idxClassReal == -1)
			ASSERT(FALSE);
		}
	
	return idxClassReal;
	}

// Solo para casos en que ya se sabe de antemano cual es la clase
void CArchive::FastReadClass()
	{
	WORD wTag;
	*this >> wTag;
	
	if (wTag == wNewClassTag)
		{
		WORD schemaNum;
		*this >> schemaNum;
		
		FastReadClassName();
		
		// simular el comportamiento de la mfc
		iCachedClasses.Add(-1);
		iCachedIndex.Add(iIndexAdjust);
		iIndexAdjust++;
		}
	}

void CArchive::FastReadObjects(BYTE* aData, TInt aCount, TInt aObjectSize)
	{
	FastReadClass();
	
	ReadBuffer(aData + 2, aObjectSize*aCount - 2);
	iIndexAdjust += aCount;
	}

CMFCObject* CArchive::ReadObject()
	{
	CMFCObject* pOb = NULL;
	
	TInt idxClass = ReadClass();
	
	//TRACE1("ClassName: %s", CS2SZ(sClassName));
#ifdef _DEBUG
#ifdef _SYMBIAN_
	RThread thread;
	TThreadPriority priority = thread.Priority();
	thread.SetPriority(EPriorityAbsoluteHigh);
#endif
	//HEAP_SIZE_START;
#endif
	
	pOb = GetObjectByClassIndex(idxClass);
	
	if (pOb)
		{
		iIndexAdjust++;
		pOb->Serialize(*this);
		}
	else
		ASSERT(FALSE);

#ifdef _DEBUG
	//HEAP_SIZE_DUMP1(GetClassNameByIndex(idxClass));
#ifdef _SYMBIAN_
	thread.SetPriority(priority);
#endif
#endif
	
	return pOb;
	}

//-----------------------------------------------------------------------------

// Solo para casos en que ya se sabe de antemano cual es la clase
void CArchiveMem::FastReadClass()
	{
	WORD wTag;
	*this >> wTag;
	
	if (wTag == wNewClassTag)
		{
		WORD schemaNum;
		*this >> schemaNum;
		
		FastReadClassName();
		
		// simular el comportamiento de la mfc
		iCachedClasses.Add(-1);
		iCachedIndex.Add(iIndexAdjust);
		iIndexAdjust++;
		}
	}

void CArchiveMem::FastReadObjects(BYTE* aData, TInt aCount, TInt aObjectSize)
	{
	FastReadClass();
	
	ReadBuffer(aData + 2, aObjectSize*aCount - 2);
	iIndexAdjust += aCount;
	}

TInt CArchiveMem::ReadClass()
	{
	DWORD obTag;
	WORD wTag;
	TInt idxClassReal = -1;
	
	*this >> wTag;
	if (wTag == wBigObjectTag)
		*this >> obTag;
	else
		obTag = ((wTag & wClassTag) << 16) | (wTag & ~wClassTag);
	
	if (wTag == wNewClassTag)
		{
		WORD schemaNum;
		*this >> schemaNum;
		CStringA sClassName;
		ReadClassName(sClassName);
		
		//TRACE("ClassName: %s", CS2SZ(sClassName));
		idxClassReal = GetClassIndexByName(sClassName);
		
		if (idxClassReal == -1)
			ASSERT(FALSE);
		
		iCachedClasses.Add(idxClassReal);
		iCachedIndex.Add(iIndexAdjust);
		iIndexAdjust++;
		}
	else
		{
		TInt idxClass = (obTag & ~dwBigClassTag);
		if (idxClass == iLastIdxClass)
			idxClassReal = iLastRealIdxClass;
		else
			{
			iLastIdxClass = idxClass;
			TInt index = iCachedIndex.Find(idxClass);
			if (index != -1)
				iLastRealIdxClass = idxClassReal = iCachedClasses[index];
			}
		
		if (idxClassReal == -1)
			ASSERT(FALSE);
		}
	
	return idxClassReal;
	}

/*CMFCObject* CArchiveMem::ReadObject()
	{
	CMFCObject* pOb = NULL;
	TInt idxClass = ReadClass();
	
	pOb = GetObjectByClassIndex(idxClass);
	
	if (pOb)
		{
		iIndexAdjust++;
		pOb->SerializeMem(*this);
		}
	else
		ASSERT(FALSE);

	return pOb;
	}*/

