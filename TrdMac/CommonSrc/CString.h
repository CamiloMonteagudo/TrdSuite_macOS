#ifndef CSTRING_H_
#define CSTRING_H_

#include <string.h>
#include <string>
#include <ctype.h>
#include "FastHeap.h"

#ifdef __STDC_WANT_SECURE_LIB__

#define sprintf_ sprintf_s
#define vsprintf_ vsprintf_s

#else

#define sprintf_ sprintf
#define vsprintf_ vsprintf

#endif


template<class _ST>
class TStringDataT;

// Inicializa los buffers globales usados internamente por la CString
void InitCStringBuffers();

// Resetea el conteo de referencia de m_pNullStrData, debe llamarse al traducir cada oracion
void ResetStringData();

// Libera los buffers globales usados internamente por la CString
void FreeCStringBuffers();

//-------------------------------------------------------------------------------------------
// Convierte el caracter 'c' a mayusculas
//-------------------------------------------------------------------------------------------
inline char CharAnsiToUpper(BYTE ch)
  {
	if (ch >= 'a' && ch <= 'z')
		return ch - ('a' - 'A');
	else if (ch >= 0xE0 && ch <= 0xFE && ch != 0xF7)
		return ch - ('a' - 'A');

	return ch;
  }

//-------------------------------------------------------------------------------------------
// Convierte el caracter 'c' a minusculas
//-------------------------------------------------------------------------------------------
inline char CharAnsiToLower(BYTE ch)
	{
	if (ch >= 'A' && ch <= 'Z')
		return ch + ('a' - 'A');
	else if (ch >= 0xC0 && ch <= 0xDE && ch != 0xD7)
		return ch + ('a' - 'A');

	return ch;
	}

#define AFTER_MODIFY

//#define TEMP_BUFFER_SIZE 3000
#define TEMP_BUFFER_SIZE 5000
#define MIN_STR_BUFF_SZIE 4

// clases que contienen los tipos de datos de los strings para ansi y unicode
class STAnsi
	{
public:
	typedef std::string	_string;
	typedef char 				_char;
	
	//static void ToUnicode(std::wstring& aUnicode, const std::string& aAnsi);

	// buffer para usos temporales
	static char iTempBuffer[TEMP_BUFFER_SIZE];
	static TStringDataT<STAnsi>* m_pNullStrData;
	};

/*class STUnicode
	{
public:
	typedef std::wstring	_string;
	typedef wchar_t				_char;
	
	static void ToAnsi(std::string& aAnsi, const std::wstring& aUnicode);
	
	// buffer para usos temporales
	//static TBuf16<TEMP_BUFFER_SIZE> iTempBuffer;
	static _string iTempBuffer;
	
	static TStringDataT<STUnicode>* NullStrData;
	};*/

// forwards declarations

template<class _ST>
class CMFCString;

typedef CMFCString<STAnsi> CMFCStringA;

extern CMFCStringA CANull;


typedef enum {ENullStrData} TNullStrData;

// Clase que contiene un std::string e implementa un conteo de referencia
template<class _ST>
class TStringDataT 
	{
	private:

	typedef typename _ST::_string _string;
	typedef typename _ST::_char	_char;
	
	TInt iRefCount;					// contador de referencia
	_string iOrgBuffer;			// puntero a la cadena, nunca debe ser NULL
	
	enum { ENullRefCount = 1000000000 };
	public:
	
//#define TRACE_CONST TRACE("TStringDataT() this=%d", this)
//#define TRACE_DEST TRACE("~TStringDataT() this=%d", this)
#define TRACE_CONST 
#define TRACE_DEST

	// constructor por defecto
	TStringDataT():iRefCount(0),iOrgBuffer() { TRACE_CONST; }

	// constructor especial para ser usado solo por el objeto NullStringDataT
	TStringDataT(TNullStrData):iRefCount(ENullRefCount),iOrgBuffer()
		{TRACE_CONST; 
		}
	
	// constructor que crea un string vacio con un largo determinado
	TStringDataT(TInt aLen)
		{
		TRACE_CONST;
		iRefCount = 1;
		iOrgBuffer.reserve(aLen);
		}

	// Copia los valores del TStringDataT pasado, duplicando la cadena 
	TStringDataT(TStringDataT* aStringData):iRefCount(1),iOrgBuffer(aStringData->iOrgBuffer.c_str(), aStringData->iOrgBuffer.length())
		{
		TRACE_CONST; 
		}
	
	TStringDataT(const _char* aStr):iRefCount(1),iOrgBuffer(aStr) {TRACE_CONST; }
	
	TStringDataT(const _char* aStr, TInt aLen):iRefCount(1),iOrgBuffer(aStr, aLen) {TRACE_CONST; }

	TStringDataT(const _string& aDes):iRefCount(1),iOrgBuffer(aDes.c_str(), aDes.length())
		{
		TRACE_CONST; 
		}
	
	~TStringDataT()
		{
		//CHECK_HEAP;
		TRACE_DEST;
		}
	
	void resetNullRefCount()
		{
		ASSERT(this == _ST::m_pNullStrData);
		iRefCount = ENullRefCount;
		}

	inline void AddRef()
		{
		iRefCount++;
		}
	
	inline void Release()
		{
		//if (this != _ST::m_pNullStrData)
			if (--iRefCount == 0)
				{
				if (this != _ST::m_pNullStrData)
					delete this;
				}
		}

	inline TInt GetRefCount() const  { return iRefCount; }

	inline BOOL IsExclusive() const	{ return (iRefCount == 1); }
	
	inline _string& GetString()	{ return iOrgBuffer; }
	
	inline const _string* GetStringC() const	{ return &iOrgBuffer; }
	
	inline const _char* c_str() const
		{
		return iOrgBuffer.c_str();
		}

	inline const _char* Data() const
		{
		return iOrgBuffer.data();
		}

	inline void Preallocate(TInt aNewLen)
		{
		iOrgBuffer.reserve(aNewLen);
		}
	
	};

#define STR_NULL ""

template<class _ST>
class CMFCString
	{
	typedef typename _ST::_string _string;
	typedef typename _ST::_char	_char;
	typedef TStringDataT<_ST>	_TStringData;
	
	_TStringData* iData;		 // nunca puede ser NULL	

public:
	
	// Constructor por defecto
	CMFCString():iData(_ST::m_pNullStrData)
		{
		ASSERT(_ST::m_pNullStrData != NULL);
		//iData->AddRef();
		}
	
	explicit CMFCString(TInt aSize):iData(NULL)
		{
		iData = new _TStringData(aSize);
		}
	
	CMFCString(const CMFCString& aStr):iData(NULL)
		{
		CopyC(aStr);
		}

	CMFCString(const CMFCString& aStr, TInt aLen):iData(NULL)
		{
		CopyC(aStr.Left(aLen));
		}
	
	// el explicit es para evitar conversiones automaticas a CMFCString
	// que usarian mas memoria de la necesaria
	explicit CMFCString(const _string& aDes):iData(NULL)
		{
		CopyC(aDes);
		}
	
	// el explicit es para evitar conversiones automaticas a CMFCString
	// que usarian mas memoria de la necesaria
	// puede usarse sin explicit si se usa solo la cantidad minima imprescindible de 
	// CMFCString como parametros en los metodos de esta clase, en su lugar mejor es usar 
	// _string y char*, asi no se hacen conversiones automaticas de char* a CMFCString
	/*explicit*/ CMFCString(const _char* aStr):iData(NULL)
		{
		if (aStr == NULL)
			{
			iData = _ST::m_pNullStrData;
			//iData->AddRef();
			}
		else
			iData = new _TStringData(aStr);
		
		AFTER_MODIFY;
		}

	explicit CMFCString(const _char* aStr, TInt aLen):iData(NULL)
		{
		if (aStr == NULL || aLen <= 0)
			{
			iData = _ST::m_pNullStrData;
			//iData->AddRef();
			}
		else
			iData = new _TStringData(aStr, aLen);
		
		AFTER_MODIFY;
		}

	explicit CMFCString(const _char* aStr1, const _char* aStr2):iData(NULL)
		{
		int len1 = strlen(aStr1);
		int len2 = strlen(aStr2);
		iData = new _TStringData(len1 + len2);
		iData->GetString().assign(aStr1, len1);
		iData->GetString().append(aStr2, len2);
		}

	explicit CMFCString(_char aCh, const _char* aStr2):iData(NULL)
		{
		iData = new _TStringData(1 + strlen(aStr2));
		iData->GetString().assign(1, aCh);
		iData->GetString().append(aStr2);
		}
	
	~CMFCString()
		{
		iData->Release();
		}
	
	enum TCopyMode { ECopy, ENoCopy};
	
private:
	
	inline BOOL IsExclusive() { return iData->IsExclusive(); }
	

	// Hacer que esta instancia tenga acceso exclusivo al TStringData,
	// lo que implica que se debe cumplir que iData->iRefCount == 1
	// aDesiredSize es opcional e indica el tamaño con que se debe crear el buffer
	// si se crea uno
	inline void PrepareForWriting(TCopyMode aCopyContent = ECopy, TInt aDesiredSize = 0)
		{
		if (!IsExclusive())
			PrepareForWriting_(aCopyContent, aDesiredSize);
		}
	
	void PrepareForWriting_(TCopyMode aCopyContent = ECopy, TInt aDesiredSize = 0)
		{
		if (iData == _ST::m_pNullStrData)
			{
			//iData->Release();
			iData = new _TStringData(aDesiredSize);
			}
		else if (aCopyContent == ENoCopy)
			{ // liberar el TStringData y crear uno nuevo sin copiar la cadena
			iData->Release();
			iData = new _TStringData(aDesiredSize);
			}
		else
			{  // hacer una copia de la que tengamos acceso exclusivo
			_TStringData* newData = new _TStringData(iData);
			
			// liberar los datos viejos y asignar el nuevo
			iData->Release();
			iData = newData;
			}
		}

	// Este metodo debe usarse con cuidado, hay que asegurarse que el buffer tenga espacio
	// suficiente para las modificaciones
protected:
	inline _string& GetString()
		{
		PrepareForWriting();

		return iData->GetString();
		}

	// Igual al GetDes pero que asume que el CString ya es exclusivo
	inline _string& GetStringEx()
		{
		return iData->GetString();
		}

public:

	// los operadores deben estar todos con CMFCString, const char * y _string 
	//inline BOOL operator==(const CMFCString& aStr) { return Compare(aStr) == 0; };
	inline BOOL operator==(const _string& aDes) const		{ return Compare(aDes) == 0; };
	inline BOOL operator==(const _char* aStr) 	const		{ return Compare(aStr) == 0; };
	inline BOOL operator==(const _char aChar) 	const		{ return IsChar(aChar); };
	//inline BOOL operator!=(const CMFCString& aStr) { return Compare(aStr) != 0; };
	inline BOOL operator<(const _string& aDes) 	const	  { return Compare(aDes) < 0; };
	inline BOOL operator>(const _string& aDes) 	const	  { return Compare(aDes) > 0; };
	inline BOOL operator!=(const _string& aDes) const		{ return Compare(aDes) != 0; };
	inline BOOL operator!=(const _char* aStr) 	const		{ return Compare(aStr) != 0; };
	inline CMFCString& operator=(const CMFCString& aStr) { Copy(aStr); return *this; };
	inline CMFCString& operator=(const _string& aDes) 	 { Copy(aDes.c_str()); return *this; };
	inline CMFCString& operator=(const _char* aStr) 		 { Copy(aStr); return *this; };
	inline CMFCString& operator=(const _char aChar) 		 { Copy(aChar); return *this; };
	//inline CMFCString& operator+=(const CMFCString& aStr) { Append(aStr); return *this; };
	inline CMFCString& operator+=(const _string& aDes) 		{ Append(aDes); return *this; };
	inline CMFCString& operator+=(const _char* aStr) 			{ Append(aStr); return *this; };
	inline CMFCString& operator+=(_char aChar) 						{ AppendChar(aChar); return *this; };
	friend inline CMFCString operator+(const CMFCString& aStr1, const CMFCString& aStr2)
		{
		TInt len = aStr1.GetLength() + aStr2.GetLength();
		CMFCString res(len);
		res.GetStringEx().assign(aStr1);
		res.GetStringEx().append(aStr2);
		return res;
		}
	
	friend inline CMFCString operator+(const _char* aStr1, const CMFCString& aStr2)
		{
		TInt len1 = strlen(aStr1);
		CMFCString res(len1 + aStr2.GetLength());
		res.GetStringEx().assign(aStr1, len1);
		res.GetStringEx().append(aStr2);
		return res;
		}
	friend inline CMFCString operator+(const CMFCString& aStr1, const _char* aStr2)
		{
		TInt len2 = strlen(aStr2);
		CMFCString res(aStr1.GetLength() + len2);
		res.GetStringEx().assign(aStr1);
		res.GetStringEx().append(aStr2, len2);
		return res;
		}
	
	/*friend inline CMFCString operator+(const CMFCString& aStr1, const _string& aStr2)
		{
		TInt len = aStr1.GetLength() + aStr2.Length();
		CMFCString res(len);
		res = aStr1;
		res.Append(aStr2);
		return res;
		}*/
	
	friend inline CMFCString operator+(const CMFCString& aStr1, _char aChar)
		{
		TInt len = aStr1.GetLength() + 1;
		CMFCString res(len);
		res.GetStringEx().assign(aStr1);
		res.GetStringEx().append(1, aChar);
		return res;
		}
	friend inline CMFCString operator+(_char aChar, const CMFCString& aStr2)
		{
		TInt len = 1 + aStr2.GetLength();
		CMFCString res(len);
		res.GetStringEx().assign(1, aChar);
		res.GetStringEx().append(aStr2);
		return res;
		}
	
	inline const _char operator[](TInt aIndex) const { return GetAt(aIndex); };
	//inline _char& operator[](TInt aIndex) { return GetDes()[aIndex]; };

	inline operator const _string&() const { return *GetDesC(); };
  inline const _string& toString() const { return *this; }

	// provoca error por redundancia
	//inline operator const _char*() const { return c_str(); };

	inline _char GetAt(TInt aIndex) const { return (*GetDesC())[aIndex]; };
	
	// Hace una copia del string pasado
	void Copy(const CMFCString& aStr)
		{
		if (this == &aStr) return;
		
		iData->Release();		
		iData = aStr.iData;
		iData->AddRef();
		
		AFTER_MODIFY;
		}

	void Copy(const _char* aStr)
		{
		if (IsExclusive())
			{
			GetStringEx() = aStr;
			}
		else
			{
			iData->Release();
			iData = new _TStringData(aStr);
			}
		
		AFTER_MODIFY;
		}

	void Copy(_char aChar)
		{
		if (!IsExclusive())
			{
			iData->Release();
			iData = new _TStringData(16);
			}
		
		GetStringEx().assign(1, aChar);
		AFTER_MODIFY;
		}

	// Los CopyC son para ser usados desde el constructor, donde iData es NULL
	void CopyC(const CMFCString& aStr)
		{
		if (this == &aStr) return;
		
		iData = aStr.iData;
		iData->AddRef();
		
		AFTER_MODIFY;
		}
	
	void CopyC(const _string& aDes)
		{
		iData = new _TStringData(aDes);
		
		AFTER_MODIFY;
		}
	
public:	
// ************************************************	
// *         Funciones de solo lectura            *
// ************************************************
	
	inline TInt GetRefCount() const
		{
		return iData->GetRefCount();
		}
	
	// Devuelve un std::string* que puede usarse para acceder a la cadena directamente 
	// para solo lectura
	inline const _string* GetDesC() const
		{
		return iData->GetStringC();
		}
	
	inline TInt GetLength() const
		{
		return GetDesC()->length();
		}
	
	inline TInt Length() const
		{
		return GetDesC()->length();
		}
	
	inline TInt GetAllocLength() const
		{
		return GetDesC()->capacity();
		}

	inline BOOL IsEmpty() const 
		{ 
		return (GetLength() == 0); 
		};

	inline BOOL IsNull() const
		{
		return (GetLength() == 0);
		}
	
	const _char* toFastHeap() const
		{
		int len = GetLength() + 1;
		_char* pStr = (_char*)CFastHeap::Get(len);
		const _char* pSource = c_str();
		for(int i=0; (pStr[i] = pSource[i]) != 0; i++);
		return pStr;
		}

	inline TInt Compare(int aStart, int aCount, const _string& aDes) const
		{
		return GetDesC()->compare(aStart, aCount, aDes);
		}

	inline TInt Compare(const _string& aDes) const
		{
		return GetDesC()->compare(aDes);
		}
	
	inline TInt Compare(const _char* aStr) const
		{
		return GetDesC()->compare(aStr);
		}
	
	inline bool StartWith(const _string& aDes) const
		{
		return GetDesC()->compare(0, aDes.length(), aDes) == 0;
		}

	inline bool StartWith(const _char* aStr) const
		{
		for(const _char* sThis = c_str(); *aStr != 0; aStr++, sThis++)
			if (*aStr != *sThis)
				return false;

		return true;
		}

	/*inline bool EndWith(const _string& aDes) const
		{
		return GetDesC()->compare(0, aDes.length(), aDes) == 0;
		}*/

	inline bool EndWith(const _char* aStr) const
		{
		int aLen = strlen(aStr);
		if (aLen > GetLength())
			return false;

		const _char* sThis = c_str() + GetLength() - aLen;
		aLen--;
		for(; aLen >= 0; aLen--)
			if (aStr[aLen] != sThis[aLen])
				return false;

		return true;
		}

	inline TInt IsChar(const _char aChar) const
		{
		const _string* des = GetDesC();
		return des->length() == 1 && (*des)[0] == aChar;
		}
	
	inline TInt Find(const _char* aStr, TInt aStart = 0) const
		{
		TInt pos = GetDesC()->find(aStr, aStart);
		return pos;
		};

	inline TInt Find(const _string& s, TInt aStart = 0) const
		{
		TInt pos = GetDesC()->find(s, aStart);
		return pos;
		};
	
	inline TInt Find(const _char& aChar, TInt aStart = 0) const
		{
		TInt pos = GetDesC()->find(aChar, aStart);
		return pos;
		};
	
	inline TInt ReverseFind(_char aChar) const	
		{ 
		TInt pos = GetDesC()->rfind(aChar);
		return pos;
		};
	
	inline TInt FindOneOf(const _string& aCharSet, int pStart = 0) const
		{
		TInt pos = GetDesC()->find_first_of(aCharSet, pStart);
		return pos;
		};
	
	inline TInt FindOneOf(const _char* aCharSet, int pStart = 0) const
		{
		TInt pos = GetDesC()->find_first_of(aCharSet, pStart);
		return pos;
		};
	
	// asumiendo que esta cadena tiene el camino de un fichero, esta funcion devuelve 
	// el nombre del fichero
	CMFCString GetFileName() const
		{
		int i = ReverseFind(FOLDER_SEP_CH);
		if (i >= 0)
			return Mid(i+1);

		return CANull;
		}

	// asumiendo que esta cadena tiene el camino de un fichero, esta funcion devuelve 
	// el camino del directorio (incluyendo el \ final)
	CMFCString GetFileDir() const
		{
		int i = ReverseFind(FOLDER_SEP_CH);
		if (i >= 0)
			return Left(i+1);

		return CANull;
		}

	// devuelve un CString con el segmento especificado
	CMFCString Mid(TInt aPos, TInt aLen) const
		{
		if (aPos < 0)
			aPos = 0;
		if (aLen < 0)
			aLen = 0;

		TInt thisLen = GetLength();
		if (aPos + aLen > thisLen)
			aLen = thisLen - aPos; 
			
		if (aPos > thisLen || aLen == 0)
			{ // devolver cadena vacia
			return CANull;
			}
		
		return CMFCString(GetDesC()->substr(aPos, aLen));
		}
	
	CMFCString Mid(TInt aPos) const
		{
		if (aPos < 0)
			aPos = 0;
		
		if (aPos > GetLength())
			{ // devolver cadena vacia
			return CANull;
			}
		
		return CMFCString(GetDesC()->substr(aPos));
		}

	CMFCString Left(TInt aLen) const
		{
		if (aLen <= 0)
			return CANull;
		
		const _string* str = GetDesC();
		int thisLen = str->length();
		if (aLen >= thisLen)
			{
			return CMFCString(*this);
			}
		else
			{
			return CMFCString(str->substr(0, aLen));
			}
		}

	CMFCString Right(TInt aLen) const
		{
		if (aLen <= 0)
			return CANull;
		
		const _string* str = GetDesC();
		TInt thisLen = str->length();
		if (aLen >= thisLen)
			{
			return CMFCString(*this);
			}
		else
			{
			return CMFCString(str->substr(thisLen - aLen, aLen));
			}
		}
	
// ************************************************	
// *         Funciones de modificacion            *
// ************************************************	
	
	// Reserva el buffer de la cadena con la longitud especificada
	void Preallocate(TInt aNewLen, TCopyMode aCopyContent = ECopy)
		{
		PrepareForWriting(aCopyContent, aNewLen);
		
		// tomar en cuenta caso en que iData == NULL
		if (iData)
			iData->Preallocate(aNewLen);
		else
			iData = new _TStringData(aNewLen);
		}

	inline void Empty()
		{
		if (iData != _ST::m_pNullStrData)
			{
			iData->Release();
			iData = _ST::m_pNullStrData;
			//iData->AddRef();
			}
		}
	
	inline const _char* c_str() const
		{
		return iData->c_str();
		}

	inline void FreeExtra()
		{
		GetDesC()->reserve(0);
		}
	
	// Equivalente del GetBuffer de la CString, despues de llamar a esta funcion el CString 
	// no se puede modificar hasta que se llame a ReleaseBuffer, pero si no se modifica el
	// largo de la cadena el llamado a ReleaseBuffer es opcional
	_char* GetBuffer(TInt aLen)
	  {
	  Preallocate(aLen);
		GetStringEx().resize(aLen, 0); //TODO: mejorar y averiguar porque hace falta esto, el Preallocate debia bastar
	  return (_char*)iData->Data(); //c_str();
	  }
	
	// Equivalente del ReleaseBuffer de la CString
	// No se puede llamar sin haber llamado antes a GetBuffer()
	void ReleaseBuffer(int nNewLength = -1)
		{
		ASSERT(IsExclusive());
		
		if (nNewLength == -1)
			{
			TInt maxLen = GetAllocLength();
			const _char* sText = iData->Data();
			/*TInt len = strnlen(sText, maxLen);
			if (len < maxLen)
				{
				Truncate(len);
				return;
				}*/
			// necesita buscar el null terminator para setear el nuevo tamaño de la cadena.		
			for(TInt i=0; i<maxLen; i++)
				if (sText[i] == 0)
					{ // se encontro el fin de la cadena
					Truncate(i);
					return;
					}
			
			TRACE("Null terminator not found");
			ASSERT(FALSE);     // no se encontro el null terminator
			}
		else
			{
			SetLength(nNewLength);
			}
		}
	
	// Trunca la cadena a una nueva longitud.
	// No se puede usar para hacer la cadena mas grande del tamaño reservado, para ese caso
	// usar PreAllocate()
	inline void Truncate(TInt aLen)
		{
		SetLength(aLen);
		}
	
	// Trunca la cadena a una nueva longitud.
	// No se puede usar para hacer la cadena mas grande del tamaño reservado, para ese caso
	// usar PreAllocate()
	inline void SetLength(TInt aLen)
		{
		GetString().resize(aLen);
		
		AFTER_MODIFY;
		}

	// Lee una cadena de un stream en el formato de C (null terminator)
	//void ReadFromFileAsNullTerm(CFile& aFile);

	// Lee una linea de un fichero texto, no incluye los cambios de linea
	void ReadLine(FILE* aFile)
		{
		_char* pBuffer = STAnsi::iTempBuffer;
		if (fgets(pBuffer, sizeof(STAnsi::iTempBuffer), aFile) == NULL)
			{
			Empty();
			return;
			}

		int len = strlen(STAnsi::iTempBuffer);

		// eliminar los cambios de linea
		if (len > 0 && (pBuffer[len-1] == 10 || pBuffer[len-1] == 13))
			len--;
		if (len > 0 && (pBuffer[len-1] == 10 || pBuffer[len-1] == 13))
			len--;
		pBuffer[len] = 0;

		Copy(STAnsi::iTempBuffer);//, nTotalLen);
	
		AFTER_MODIFY;
		}

	// Simula el format de la CString
	void FormatList(const _char* aFormat, va_list list)
		{
		vsprintf_(_ST::iTempBuffer, aFormat, list);
		Copy(_ST::iTempBuffer);
		}
	
	void Format(const _char* aFormat, ...)
		{
		va_list list;
		va_start(list, aFormat);
		
		FormatList(aFormat, list);
		}
	
	void SetAt(TInt aIndex, _char aChar) 
		{ 
		(GetString())[aIndex] = aChar;
		}
	
	void MakeLower()
		{
		PrepareForWriting();

		_char* pText = (_char*)c_str();
		TInt len = GetLength();

		for(TInt i=0; i<len; i++)
			pText[i] = CharAnsiToLower(pText[i]);
		}

	void MakeUpper()
		{
		PrepareForWriting();

		_char* pText = (_char*)c_str();
		TInt len = GetLength();

		for(TInt i=0; i<len; i++)
			pText[i] = CharAnsiToUpper(pText[i]);
		}
	
	void Append(const _string& aDes)
		{
		// calcular la longitud necesaria
		TInt newLen = GetLength() + aDes.length();
		Preallocate(newLen, ECopy);
		
		GetStringEx().append(aDes);
		
		AFTER_MODIFY;
		}
	
	/*void Append(const CMFCString& aStr)
		{
		Append(*(aStr.GetDesC()));
		}*/
	
	void Append(const _char* aStr)
		{
		// calcular la longitud necesaria
		TInt len = strlen(aStr);
		Preallocate(GetLength() + len, ECopy);
		
		GetStringEx().append(aStr, len);
		
		AFTER_MODIFY;
		}
	
	void Append(const _char* aStr, TInt aLen)
		{
		// calcular la longitud necesaria
		TInt newLen = GetLength() + aLen;
		Preallocate(newLen, ECopy);
		
		GetStringEx().append(aStr, aLen);
		
		AFTER_MODIFY;
		}

	void AppendChar(const _char aChar)
		{
		// calcular la longitud necesaria
		TInt len = GetLength() + 1;
		Preallocate(len, ECopy);
		
		GetStringEx().append(1, aChar);
		
		AFTER_MODIFY;
		}

  typedef typename _string::size_type size_type;
	
	void Enclose(const _char aChar)
		{
		TInt len = GetLength() + 2;
		Preallocate(len, ECopy);

		GetStringEx().insert((size_type)0, (size_type)1, aChar);
		GetStringEx().append((size_type)1, aChar);
		}

	void Insert(TInt aPos, const _string& aDes)
		{
		// calcular la longitud necesaria
		TInt newLen = GetLength() + aDes.length();
		Preallocate(newLen, ECopy);
		
		GetStringEx().insert(aPos, aDes);
		
		AFTER_MODIFY;
		}

	/*void Insert(TInt aPos, const _char* aStr)
		{
		TInt newLen = GetLength() + strlen(aStr);
		Preallocate(newLen, ECopy);
		
		GetDesEx().insert(aPos, aStr);
		
		AFTER_MODIFY;
		}*/

	void Delete(TInt aPos, TInt aLen = 1)
		{
		GetString().erase(aPos, aLen);
		
		AFTER_MODIFY;
		}

	TInt Remove(_char aChar)
		{
		ASSERT(aChar != 0);

		PrepareForWriting();
		
		_char *sStart, *sFill, *sLook;
		sStart = sFill = sLook = (_char*)c_str();

		// buscar el primer caracter a eliminar
		for(; *sLook != 0; sLook++)
			if (*sLook == aChar)
				{
				sFill = sLook; // a partir de aqui se empieza a eliminar
				break;
				}

		if (*sLook == 0)
			return 0; // ningun caracter a eliminar

		sLook++;

		for(; *sLook != 0; sLook++)
			{
			if (*sLook != aChar)
				*(sFill++) = *sLook;
			}

		/*if ((sFill - sStart) & 1)
			int k=0;
		if (sFill - sStart != GetLength() - (GetLength()-2)/3)
			COUNTER(sStart);*/
		GetStringEx().resize(sFill - sStart);
		AFTER_MODIFY;
		
		return sLook - sFill;
		}

	TInt Replace(_char aOld, _char aNew)
		{
		ASSERT(aNew != 0);
		
		PrepareForWriting();
		
		TInt len = Length();
		TInt nReplaceCount = 0;
		_char* sBuffer = (_char*)c_str();
		
		for(TInt i = 0; i < len; i++)
			if (sBuffer[i] == aOld)
				{
				sBuffer[i] = aNew;
				nReplaceCount++;
				}

		AFTER_MODIFY;
		
		return nReplaceCount;
		}
	
	/*TInt Replace(const _string& aOld, const _char* aNew)
		{
		_TPtrC ptrNew((_TText*)aNew);
		
		return Replace(aOld, ptrNew);
		}
	
	TInt Replace(const _char* aOld, const _string& aNew)
		{
		_TPtrC ptrOld((_TText*)aOld);
		
		return Replace(ptrOld, aNew);
		}*/
	
	TInt Replace(const _char* aOld, const _char* aNew)
		{
		PrepareForWriting();
		
		TInt count = 0;
		TInt pos = 0;
		TInt len = strlen(aOld);
		TInt newLen = strlen(aNew);
		
		while (true)
			{
			pos = Find(aOld, pos);
			if (pos >= 0)
				{
				Delete(pos, len);
				Insert(pos, aNew);
				pos += newLen;
				count++;
				}
			else
				break;
			
			}
		
		return count;
		}

	
	TInt Replace(const _string& aOld, const _string& aNew)
		{
		PrepareForWriting();
		
		TInt count = 0;
		TInt pos = 0;
		TInt len = aOld.length();
		TInt newLen = aNew.length();
		
		while (true)
			{
			pos = Find(aOld, pos);
			if (pos >= 0)
				{
				Delete(pos, len);
				Insert(pos, aNew);
				pos += newLen;
				count++;
				}
			else
				break;
			
			}
		
		return count;
		}

	void TrimLeft(_char aChar = ' ') 
		{
		PrepareForWriting();
		
		const _char* str = c_str();
		TInt len = Length();
		TInt i = 0;
		while(i < len && str[i] == aChar) i++;
		
		if (i > 0)
			Delete(0, i);
		}
	
	void TrimLeft(const _char* aChars) 
		{
		PrepareForWriting();
		
		const _char* str = c_str();
		TInt len = Length();
		TInt i = 0;
		
		while(i < len && strchr(aChars, str[i]) != NULL) i++;
		
		if (i > 0)
			Delete(0, i);
		}

	void TrimRight(_char aChar = ' ') 
		{
		PrepareForWriting();
		
		const _char* str = c_str();
		TInt len = Length();
		TInt i = len - 1;
		while(i >= 0 && str[i] == aChar) i--;
		
		if (i != len-1)
			Truncate(i + 1);
		}	

	void TrimRight(const _char* aChars)
		{
		PrepareForWriting();
		
		const _char* str = c_str();
		TInt len = Length();
		TInt i = len - 1;
		while(i >= 0 && strchr(aChars, str[i]) != NULL) i--;
		
		if (i != len-1)
			Truncate(i + 1);
		}
	
	void Trim(_char aChar = ' ')
		{
		TrimRight(aChar);
		TrimLeft(aChar);
		}

	void Trim(const _char* aChars) 
		{
		TrimRight(aChars);
		TrimLeft(aChars);
		}

	void FromInt(int aValue)
		{
		if (aValue < 10)
			GetDesEx().assign(1, '0' + aValue);
		else
			{
			int rest = aValue % 10;
			aValue /= 10;
			GetDesEx().assign(1, '0' + rest);

			while (aValue > 0)
				{
				rest = aValue % 10;
				aValue /= 10;
				GetDesEx().insert((size_t)0, 1, (_char)('0' + rest));
				}
			}
		}
	
	static _char* FromInt_(int aValue)
		{
		#define BUFF_LEN 15
		static _char sbuffer[15];
		sbuffer[BUFF_LEN - 1] = '\x0';

		if (aValue < 10)
			{
			sbuffer[BUFF_LEN - 2] = '0' + aValue;
			return sbuffer + BUFF_LEN - 2;
			}
		else
			{
			int rest = aValue % 10;
			aValue /= 10;
			int len = BUFF_LEN - 2;
			sbuffer[len] = '0' + rest;

			while (aValue > 0)
				{
				rest = aValue % 10;
				aValue /= 10;
				sbuffer[--len] = '0' + rest;
				}

			return sbuffer + len;
			}
		}

	TInt ToInt() const
		{
		TInt res = 0;
		TInt i = GetLength() - 1;
		TInt pow = 1;
		_string* ptr = GetDesC();
		for(; i >= 0; i--)
			{
			char ch = (*ptr)[i];
			if (ch < '0' || ch > '9')
				break;

			ch -= '0';
			res += ch*pow;
			pow *= 10;
			}
		
		return res;
		}
	
	/*CMFCString<STAnsi> ToUnicode() const
		{
		return *this;
		}
	
	CMFCString<STAnsi> ToAnsi() const
		{
		return *this;
		}*/
	};


//typedef CMFCString<STAnsi> CMFCStringA;
//typedef CMFCString<STUnicode> CMFCStringW;

typedef CMFCStringA CStringA;
//typedef CMFCStringW CStringW;

#define CString CStringA
#define CStringW CStringA
#define CStringF CStringA

#define _CA (CStringA)
//#define _CW (CStringW)

//extern CStringA CANull;
//static CStringW CWNull;

#define CWNull CANull
#define CFNull CANull

#endif /*CSTRING_H_*/
