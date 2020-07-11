#ifndef FILEUTIL_H_
#define FILEUTIL_H_

#include <stdio.h>

BOOL FileExists(const CStringW& aFileName);

FILE* fopen_(const char * aFileName, const char * mode);
FILE* tmpfile_();

// Similar al CFile de la MFC pero solo admite lectura, soporta buffering
class CFile
	{
protected:
	FILE *iFile;
	TInt iLength;
	
public:	
	
	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	enum EOpenFlags { // nota: no todos estan implementados
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		/*modeReadWrite =    (int) 0x00002,
		shareCompat =      (int) 0x00000,
		shareExclusive =   (int) 0x00010,
		shareDenyWrite =   (int) 0x00020,
		shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		modeNoInherit =    (int) 0x00080,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000,
		typeBinary =       (int) 0x08000,
		osNoBuffer =       (int) 0x10000,
		osWriteThrough =   (int) 0x20000,
		osRandomAccess =   (int) 0x40000,
		osSequentialScan = (int) 0x80000,*/
		};

	CStringA iFileName;
	
	CFile()
		{
		iFile = NULL;
		iLength = 0;
		}

	CFile(const CStringA& aFileName, EOpenFlags aMode)
		{
		iFile = NULL;
		iLength = 0;

		Open(aFileName, aMode);
		}
	
	~CFile()
		{
		Close();
		}
	
	void Close()
		{
		if (iFile)
			{
			fclose(iFile);
			iFile = NULL;
			}
		}
	
	BOOL isOpen() { return iFile != NULL; }

	TInt GetLength() { return iLength; }
	
	inline BOOL Open(const CStringA& aFileName, EOpenFlags aMode = modeRead)
		{
		return Open(aFileName.c_str(), aMode);
		}
	
	BOOL Open(const char* aFileName, EOpenFlags aMode = modeRead)
		{
		//TRACE("CFile::Open: %s", aFileName);
		iFileName = aFileName;
		iLength = 0;
		
		ASSERT(iFile == NULL);

		if ((aMode & modeWrite) != 0)
			iFile = fopen_(aFileName, "wb");
		else
			iFile = fopen_(aFileName, "rb");

		if (iFile) // obtener el tamaño del fichero
			{
			fseek(iFile, 0, SEEK_END);
			iLength = (int)ftell(iFile);
			fseek(iFile, 0, SEEK_SET);
			}
		
		return iFile != NULL;
		}
	
	inline TInt GetPosition()
		{
		TInt pos = (int)ftell(iFile);
		return pos;
		}
	
	TInt Seek(TInt aOffset, TInt aFrom = begin)
		{
		int mode;
		
		if (aFrom == begin)
			mode = SEEK_SET;
		else if (aFrom == current)
			mode = SEEK_CUR;
		else if (aFrom == end)
			mode = SEEK_END;
		else
			return 0;

		int res = 0;
    res = fseek(iFile, aOffset, mode);
		ASSERT(res == 0);

		return aOffset;
		}
	
	inline TInt ReadBuffer(LPVOID lpsz, UINT nMax)
		{
		//TInt posBefore = GetPosition();
		
		int nReads = (int)fread(lpsz, 1, (TInt)nMax, iFile);
		return nReads;
		
		//TInt posAfter = GetPosition();
		//return posAfter - posBefore;
		}
	
	inline void ReadBufferAsCString(CStringA& aBuffer, int size)
		{
		LPSTR buffer = aBuffer.GetBuffer(size);
		
		fread(buffer, 1, size, iFile);

		aBuffer.ReleaseBuffer(size);
		}

	void ReadCStringA(CStringA& aString)
		{
		TInt len = ReadUint8();
		
		if (len == 0)
			{
			aString.Empty();
			}
		else
			{
			if (len == 255) // para leer cadenas de hasta 65535 caracteres
				len = ReadUint16();
			
			TUint8* buffer = (TUint8*)aString.GetBuffer(len);
			
			ReadBuffer(buffer, len);
			}
		}

	void ReadAsSZString(CStringA& aStr);
	LPCSTR ReadFastAsSZString();

	inline CStringA ReadLine()
		{
		CStringA res;
		res.ReadLine(iFile);
		return res;
		}

#define DEFINE_WRITE(TYPE) \
	void Write##TYPE(T##TYPE aValue) \
		{	\
		fwrite((LPVOID)&aValue, sizeof(T##TYPE), 1, iFile);	\
		}

  DEFINE_WRITE(Int32);
  DEFINE_WRITE(Uint32);
  DEFINE_WRITE(Int16);
  DEFINE_WRITE(Uint16);
  DEFINE_WRITE(Int8);
  DEFINE_WRITE(Uint8);

	/*void WriteUint8(TUint8 aValue)
	void WriteInt8(TInt8 aValue)
	void WriteUint16(TUint16 aValue)
	void WriteUint32(TUint32 aValue)
	void WriteInt32(TInt32 aValue)
		{
		fwrite((LPVOID)&aValue, sizeof(aValue), 1, iFile);
		}*/

	void WriteBuffer(LPVOID aBuffer, UINT nSize)
		{
		fwrite(aBuffer, 1, (TInt)nSize, iFile);
		}

	void WriteStringSZ(LPCSTR aStr, BOOL aIncludeNull = TRUE)
		{
		int len = (int)strlen(aStr);
		WriteBuffer((LPVOID)aStr, len + (int)aIncludeNull);
		}

	// Nota: este CString no se salva en el mismo formato de la MFC
	void WriteCString(const CStringA& aStr)
		{
		TInt len = aStr.GetLength();
		/*int bytesWritten = 0;
		WriteUint16(len);
		bytesWritten += 2;*/
		if (len >= 255)
			{
			WriteUint8(255);
			WriteUint16(len);
			//bytesWritten += 3;
			}
		else
			{
			WriteUint8(len);
			//bytesWritten += 1;
			}
		
		if (len == 0)
			return;// bytesWritten;

		WriteBuffer((LPVOID)aStr.c_str(), len);
		//bytesWritten += len;

		return;// bytesWritten;
		}

#define DEFINE_READ(TYPE) \
	T##TYPE	Read##TYPE() \
		{	\
		T##TYPE value; \
		fread((LPVOID)&value, sizeof(T##TYPE), 1, iFile);	\
		return value;	\
		}

  DEFINE_READ(Int32);
  DEFINE_READ(Uint32);
  DEFINE_READ(Int16);
  DEFINE_READ(Uint16);
  DEFINE_READ(Int8);
  DEFINE_READ(Uint8);
	
	};

/*class CStdioFile : public CFile
	{
	};*/
	
/*class CMemFile : public CFile
	{
	public:
		
	CMemFile(CStringA& aBuffer)
		{
		TRACE("CMemFile");
		iFile = tmpfile_();
		if (iFile == NULL)
			{
			TRACE("Crash: iFile=null");
			}
		ASSERT(iFile != NULL);

		int len = aBuffer.GetLength() + 1;
		int res = setvbuf(iFile, NULL, _IOFBF, len + 1); // el +1 es porque el buffer siempre se crea con el multiplo de 2 mas pequeño
		ASSERT(res == 0);

		fwrite(CS2SZ(aBuffer), len, 1, iFile);
		TRACE("CMemFile-end");
		}
	
	};*/

// Igual al CArchive de la MFC pero de solo lectura
class CArchive
	{
public:
	// Flag values
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };
private:
	CFile* iFile;
	CIntArray iCachedClasses;
	CIntArray iCachedIndex;
	TInt iLastIdxClass;
	TInt iLastRealIdxClass;
	TInt iIndexAdjust;
	Mode iMode;
	BOOL iError;
	
public:
	CStringA m_strFileName;
	
		
	CArchive(CFile* aFile, Mode aMode = load)
		{
		iFile = aFile;
		iError = !iFile->isOpen();
		m_strFileName = aFile->iFileName;
		iMode = aMode;
		
		iCachedClasses.Add(-1);
		iCachedIndex.Add(0);
		iIndexAdjust = 1;
		iLastIdxClass = iLastRealIdxClass = -1;
		}
	
	void Close()
		{
		}
	
	void SetError() { iError = TRUE; }
	BOOL hasError()	{ return iError; }

	CFile* GetFile() { return iFile; }
	
	BOOL IsStoring() { return iMode == store; }
	BOOL IsLoading() { return iMode == load; }
	
	void UpdateStreamPosition() {};
	void UpdatePosition() {}
	
	inline CArchive& operator>>(TUint16& aValue) { aValue = iFile->ReadUint16(); return *this; };
	inline CArchive& operator>>(DWORD& aValue) { aValue = iFile->ReadUint32(); return *this; };
	inline CArchive& operator>>(TInt& aValue) { aValue = iFile->ReadInt32(); return *this; };
	inline CArchive& operator>>(TUint8& aValue) { aValue = iFile->ReadUint8(); return *this; };
	inline CArchive& operator>>(TInt8& aValue) { aValue = iFile->ReadInt8(); return *this; };
	inline CArchive& operator>>(CStringA& aValue) { iFile->ReadCStringA(aValue); return *this; };


#ifdef ACTIVE_SAVE

	inline CArchive& operator<<(const TInt8& aValue) { iFile->WriteInt8(aValue); return *this; };
	inline CArchive& operator<<(const TUint8& aValue) { iFile->WriteUint8(aValue); return *this; };
	inline CArchive& operator<<(const TUint16& aValue) { iFile->WriteUint16(aValue); return *this; };
	inline CArchive& operator<<(const TInt& aValue) { iFile->WriteInt32(aValue); return *this; };
	inline CArchive& operator<<(LPCSTR aStr) { iFile->WriteStringSZ(aStr); return *this; };
	inline CArchive& operator<<(const CString& aStr) { iFile->WriteCString(aStr); return *this; };

	inline void WriteCount(int aCount)
		{
		if (aCount < 0xFFFF)
			*this << (WORD)aCount;
		else
			{
			*this << (WORD)0xFFFF;
			*this << aCount;
			}
		}

	void WriteBuffer(LPVOID aBuffer, TInt aSize)
		{
		iFile->WriteBuffer(aBuffer, aSize);
		}

#endif

	inline void ReadBuffer(LPVOID aData, TInt aSize)
		{
		iFile->ReadBuffer(aData, aSize);
		}
	
	inline void SkipBytes(TInt aSize)
		{
		iFile->Seek(aSize, CFile::current);
		}
	
	inline TUint32 ReadCount()
		{
		WORD wCount;
		*this >> wCount;
		if (wCount != 0xFFFF)
			return wCount;

		DWORD dwCount;
		*this >> dwCount;
		return dwCount;
		}
	
	inline void ReadClassName(CStringA& aString)
		{
		WORD len;
		*this >> len;
		
		//aString.Preallocate(len, CStringA::ENoCopy);
		//aString.Truncate(len);
		TUint8* buffer = (TUint8*)aString.GetBuffer(len);
		aString.Truncate(len);
		
		ReadBuffer(buffer, len);
		}
	
	inline void FastReadClassName()
		{
		WORD len;
		*this >> len;
		
		SkipBytes(len);
		}
	
	
	void FastReadClass();
	TInt ReadClass();

	void FastReadObjects(BYTE* aData, TInt aCount, TInt aObjectSize);
	CMFCObject* ReadObject();
	
	template<class TYPE>
	void FastReadObject(TYPE* pOb)
		{
		FastReadClass();
		iIndexAdjust++;

		pOb->Serialize(*this);
		}
	
	void Flush() { };
	};


// Igual al CArchive de la MFC pero de solo lectura
class CArchiveMem
	{
private:
	CIntArray iCachedClasses;
	CIntArray iCachedIndex;
	TInt iLastIdxClass;
	TInt iLastRealIdxClass;
	TInt iIndexAdjust;

	BYTE* iOrgBuffer;
	BYTE* iBuffer;
	
public:
	CStringA m_strFileName;
	
	// Flag values
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };
		
	/*CArchiveMem(CFile* aFile, int aLen = -1)
		{
		m_strFileName = aFile->iFileName;

		if (aLen == -1)
			aLen = aFile->GetLength();

		iOrgBuffer = iBuffer = new BYTE[aLen];
		aFile->Read(iBuffer, aLen);

		iCachedClasses.Add(-1);
		iCachedIndex.Add(0);
		iIndexAdjust = 1;
		iLastIdxClass = iLastRealIdxClass = -1;
		}*/
	
	CArchiveMem(CStringA aBuffer)
		{
		iOrgBuffer = NULL;
		iBuffer = (LPBYTE)aBuffer.c_str();

		iCachedClasses.Add(-1);
		iCachedIndex.Add(0);
		iIndexAdjust = 1;
		iLastIdxClass = iLastRealIdxClass = -1;
		}

	~CArchiveMem()
		{
		Close();
		}

	void Close()
		{
		delete iOrgBuffer;
		iOrgBuffer = NULL;
		}
	
	//CFile* GetFile() { return iFile; }
	
	inline BOOL IsStoring() { return FALSE; }
	inline BOOL IsLoading() { return TRUE; }
	
	void UpdateStreamPosition() {};
	void UpdatePosition() {}
	
#define BUFFERREAD1(TYPE) \
	inline T##TYPE Read##TYPE##L() \
		{	\
		return *(T##TYPE*)iBuffer++;	\
		}

#define BUFFERREAD(TYPE) \
	inline T##TYPE Read##TYPE##L() \
		{	\
		T##TYPE res = *(T##TYPE*)iBuffer; \
		iBuffer += sizeof(T##TYPE); \
		return res;	\
		}
	
	BUFFERREAD1(Uint8);
	BUFFERREAD(Uint16);
	BUFFERREAD(Uint32);
	BUFFERREAD1(Int8);
	BUFFERREAD(Int32);

	inline CArchiveMem& operator>>(TUint16& aValue) { aValue = ReadUint16L(); return *this; };
	inline CArchiveMem& operator>>(DWORD& aValue) { aValue = ReadUint32L(); return *this; };
	inline CArchiveMem& operator>>(TInt& aValue) { aValue = ReadInt32L(); return *this; };
	inline CArchiveMem& operator>>(TUint8& aValue) { aValue = ReadUint8L(); return *this; };
	inline CArchiveMem& operator>>(TInt8& aValue) { aValue = ReadInt8L(); return *this; };

	inline void ReadBuffer(LPVOID aBuffer, TInt aSize)
		{
		memcpy(aBuffer, iBuffer, aSize);
		iBuffer += aSize;
		}
	
	inline void ReadBuffer2(LPBYTE aBuffer, TInt aSize)
		{
		for(; aSize > 0; aSize--)
			*(aBuffer++) = *(iBuffer++);
		}

	inline void SkipBytes(TInt aSize)
		{
		iBuffer += aSize;
		}
	
	inline CArchiveMem& operator>>(CStringA& aValue) { ReadCStringA(aValue); return *this; };

	inline TUint32 ReadCount()
		{
		WORD wCount;
		*this >> wCount;
		if (wCount != 0xFFFF)
			return wCount;

		DWORD dwCount;
		*this >> dwCount;
		return dwCount;
		}
	
	void ReadCStringA(CStringA& aString)
		{
		TUint8 len8;
		*this >> len8;
		TInt len = len8;
		
		if (len == 0)
			{
			aString.Empty();
			}
		else
			{
			if (len == 255) // para leer cadenas de hasta 65535 caracteres
				{
				TUint16 len16;
				*this >> len16;
				len = len16;
				}
			
			TUint8* buffer = (TUint8*)aString.GetBuffer(len);
			aString.Truncate(len);
			
			ReadBuffer(buffer, len);
			}
		}
	
	inline void ReadClassName(CStringA& aString)
		{
		WORD len;
		*this >> len;
		
		TUint8* buffer = (TUint8*)aString.GetBuffer(len);
		aString.Truncate(len);
		
		ReadBuffer(buffer, len);
		}
	
	inline void FastReadClassName()
		{
		WORD len;
		*this >> len;
		
		SkipBytes(len);
		}
	
	void FastReadClass();

	TInt ReadClass();

	void FastReadObjects(BYTE* aData, TInt aCount, TInt aObjectSize);

	CMFCObject* ReadObject();
	
	template<class TYPE>
	void FastReadObject(TYPE* pOb)
		{
		FastReadClass();
		iIndexAdjust++;

		pOb->SerializeMem(*this);
		}
	
	void Flush() { };
	};

#endif /*FILEUTIL_H_*/
