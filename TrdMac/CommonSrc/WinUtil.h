#ifndef WINUTIL_H_
#define WINUTIL_H_

#include <stddef.h>			// only needed for the definition of wchar_t
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

// Estos tres deben estar desactivados a menos que se quiera debuguear en release.
// Activa los TRACEs siempre
#define xTRACE_ALWAYS
// Activa siempre la coleccion de datos
#define xCOLLECT_ALWAYS
// Activa siempre las mediciones del uso del heap
#define xHEAP_ALWAYS

// Desactivar los checked iterators en windows
#define _SECURE_SCL 0


#ifdef DEBUG
	#define _DEBUG
#endif

#ifdef ACTIVE_SAVE

// En el convertidor los traces siempre estan activos
	#define TRACE_ALWAYS
	#define COLLECT_ALWAYS

#endif

// para cuando ACTIVE_SAVE se defina en las opciones del proyecto
#ifndef ACTIVE_SAVE

	#define LOAD_FROM_DCX

#endif

#ifdef LOAD_FROM_DCX
	#define EXT_DAT_LOAD ".dax"
	#define EXT_RED_LOAD ".dat"
	#define EXT_CNJ_LOAD ".dat"
#else
	#define EXT_DAT_LOAD ".dat"
	#define EXT_RED_LOAD ".dat"
	#define EXT_CNJ_LOAD ".dat"
#endif

#define EXT_DCC      ".dcx"
#define EXT_DAT_SAVE ".dax"


#if defined(COLLECT_ALWAYS) || defined(HEAP_ALWAYS)
	#define TRACE_ALWAYS
#endif

#ifdef __SYMBIAN32__
	#define _SYMBIAN_
#endif

#ifdef ANDROID
	#define _ANDROID_
#endif

#ifdef _MSC_VER
	#define _WINDOWS_
#endif

#ifdef __QNX__
	#define _BLACKBERRY_
#endif

#ifdef __APPLE__
  #define _APPLE_
  #define _IPHONE_
#endif


#define _MOBILE_

#define MERCADO


// definir _DEBUG para versiones debug, de lo contrario se asume release


#if defined(TRACE_ALWAYS)
	#define TRACE_DEFINED
#endif
#if defined(COLLECT_ALWAYS)
	#define COLLECT_DEFINED
#endif
#if defined(HEAP_ALWAYS)
	#define HEAP_DEFINED
#endif

#if defined(_DEBUG)

	#define TRACE_DEFINED
	#define COLLECT_DEFINED
	#define HEAP_DEFINED

#endif

#define Max(x,y) ((x) >= (y)) ? (x) : (y)
#define Min(x,y) ((x) < (y)) ? (x) : (y)
#define clamp(x, min, max) \
	{ \
	if ((x) < (min)) x = min; \
	if ((x) > (max)) x = max; \
	}


#define COUNT_OF(x) (sizeof(x)/sizeof((x)[0])) 

// tipos utiles

// simulando Symbian
typedef int32_t TInt;
typedef uint32_t TUint;
typedef int32_t TInt32;
typedef uint32_t  TUint32;
typedef int16_t TInt16;
typedef uint16_t TUint16;
typedef char TInt8;
typedef unsigned char TUint8;
typedef char TText;


#define LF 
#define _L8(x)	x
#define _LA(x)	x
#define _LT(x)	x
#define _LW(x)	x

#define DEBUG_NEW new

#define BASED_CODE

#define DLLENTRY(x) x

#define CA2CT(x) (x)
#define CT2CA(x) (x)

#define CRASH { int* p=NULL; *p=0;}

// simulando Windows
#ifdef _T
	#undef _T
	#define _T
#else
	#define _T
#endif

#ifndef _WINDOWS_

	typedef unsigned char       BYTE;
	typedef uint16_t            WORD;
	typedef uint32_t            DWORD;
	typedef uint32_t            UINT;

#ifdef _APPLE_
	#if !defined (OBJC_BOOL_DEFINED)
		typedef signed char		BOOL; 
		#define OBJC_BOOL_DEFINED
	#endif
#else
	typedef int                 BOOL;
#endif

	typedef WORD* LPWORD;
	typedef BYTE* LPBYTE;

	#define FAR

#endif

#ifndef FALSE

	#define FALSE		0
	#define TRUE		1

#endif

#ifndef LPSTR

	typedef void* LPVOID;
	typedef char* LPSTR;
	typedef const char* LPCSTR;
	//typedef const char* LPCWSTR;
	typedef const char* LPCFSTR;

#endif

#ifdef _DEBUG

	void DebugASSERT(BOOL bCondition, LPCSTR sFile, int aLine, LPCSTR sCondition);
	#define ASSERT(e) DebugASSERT(e, __FILE__, __LINE__ , #e)
	#define VERIFY(x) { BOOL b=x; ASSERT(b); }

#else

	#define ASSERT(e) ((void)0)
	#define VERIFY(x) { x; }

#endif

#ifdef _WINDOWS_

	#define FOLDER_SEP "\\"
	#define FOLDER_SEP_CH '\\'

#else

	#define FOLDER_SEP "/"
	#define FOLDER_SEP_CH '/'

#endif

// Devuelve el espacio reservado en el heap
TInt GetAllocSize();

// Devuelve el espacio libre en el heap (tener en cuenta que el heap se expande
// automaticamente por lo que esta funcion no da la cantidad de memoria real disponible)
TInt GetFreeSize();

// Para controlar el espacio usado en el heap
#ifdef HEAP_DEFINED
#define CHECK_HEAP CheckHeap()
#else
#define CHECK_HEAP
#endif

void CheckHeap();

#ifdef HEAP_DEFINED

#define HEAP_SIZE_START TInt __lastAlloc = GetAllocSize(); TInt __lastFree = GetFreeSize();
#define HEAP_SIZE_DUMP \
	{ \
	TInt sizeAlloc = GetAllocSize() - __lastAlloc; \
	TInt sizeFree = __lastFree - GetFreeSize(); \
	TRACE2("memory use by Alloc: %d, by Free: %d", sizeAlloc, sizeFree); \
	__lastAlloc = GetAllocSize(); \
	__lastFree = GetFreeSize(); \
	}
	
#define HEAP_SIZE_DUMPS(sText) \
	{ \
	TInt sizeAlloc = GetAllocSize() - __lastAlloc; \
	TInt sizeFree = __lastFree - GetFreeSize(); \
	TRACE3("%s   Alloc: %d, by Free: %d", sText, sizeAlloc, sizeFree); \
	__lastAlloc = GetAllocSize(); \
	__lastFree = GetFreeSize(); \
	}
	
#define HEAP_SIZE_DUMPX(sCode)  		\
	{ \
	TInt lastAlloc = GetAllocSize(); \
	TInt lastFree = GetFreeSize(); \
	sCode; \
	TInt sizeAlloc = GetAllocSize() - lastAlloc; \
	TInt sizeFree = lastFree - GetFreeSize(); \
	TRACE3("%-55s : %10d %10d", #sCode, sizeAlloc, sizeFree); \
	}

#define HEAP_SIZE_DUMP1(sClassName) \
	{ \
	TInt size = GetAllocSize() - __lastAlloc;	\
	AddClassMemData(sClassName, size);	\
	/*TRACE1("block size: %d", size);*/	\
	__lastAlloc = GetAllocSize();	\
	}

#define LOG_INDENT_UP 			CLogger::iLogIndent++
#define LOG_INDENT_DOWN 		CLogger::iLogIndent--

#define DUMP_CLASS_MEM_USAGE DumpClassMemUsage()

#else

#define HEAP_SIZE_START 
#define HEAP_SIZE_DUMP  
#define HEAP_SIZE_DUMPS(sText) 
#define HEAP_SIZE_DUMPX(sCode) sCode
#define HEAP_SIZE_DUMP1(sClassName)

#define LOG_INDENT_UP
#define LOG_INDENT_DOWN

#define DUMP_CLASS_MEM_USAGE

#endif

#if defined(COLLECT_DEFINED)

	#define COUNTER(NAME) CountData(NAME)
	#define DUMP_COUNTERS DumpCounters()

	#define COLLECT(NAME, VALUE) CollectData(NAME, VALUE)
	#define COLLECT_INTERVAL(NAME) _CTimeDelta __timeDelta(NAME, true)
	#define DUMP_COLLECTED_DATA DumpCollectedData()

	#define DUMP_INTERVAL _CTimeDelta __timeDelta("")
	#define DUMP_INTERVALS(NAME) _CTimeDelta __timeDelta(NAME)

#else

	#define COUNTER(NAME)
	#define DUMP_COUNTERS

	#define COLLECT(NAME, VALUE)
	#define COLLECT_INTERVAL(NAME)
	#define DUMP_COLLECTED_DATA

	#define DUMP_INTERVAL
	#define DUMP_INTERVALS(NAME)

#endif

void CountData(const char* aName);
void DumpCounters();

void CollectData(const char* aName, double aValue);
void DumpCollectedData();

void InitMFCSystem();
void FinishMFCSystem(BOOL aDumpData = TRUE);

void OptimizeForMemory();
void OptimizeForSpeed();

class CFile;
class CArchive;
class CArchiveMem;

// Clase que emula al CObject de la mfc (no se llama CObject porque esa clase ya existe en Symbian)
class CMFCObject
	{
public:
	
	CMFCObject() {};
	virtual ~CMFCObject() {};
	
	//CMFCObject(const CMFCObject& c) {};
	
	virtual void Serialize(CArchive &/*ar*/) {}; 
	};

void Log0(LPCSTR aText);
void Log(LPCSTR aText, ...);

#if defined(TRACE_DEFINED)

	#define TRACE(...) { Log(__VA_ARGS__); }
	#define TRACE0(s) { Log0(s); }
	#define TRACE1(s, p1) { Log(s, p1); }
	#define TRACE2(s, p1, p2) { Log(s, p1, p2); }
	#define TRACE3(s, p1, p2, p3) { Log(s, p1, p2, p3); }
	#define TRACE4(s, p1, p2, p3, p4) { Log(s, p1, p2, p3, p4); }

	#define LOG_ERROR(...) { Log(__VA_ARGS__); }

	#define TRACEARRSTR(arr) /*TRACE(#arr)*/ \
		for(int i=0; i<arr.GetCount(); i++) \
			TRACE("%s", arr[i].c_str()); 


#else

	#define TRACE(...) { }
	#define TRACE0(s) {  }
	#define TRACE1(s, p1) {  }
	#define TRACE2(s, p1, p2) {  }
	#define TRACE3(s, p1, p2, p3) {  }
	#define TRACE4(s, p1, p2, p3, p4) {  }

	#define LOG_ERROR(...) { }

	#define TRACEARRSTR(arr) { }

#endif

#define PEEKMESSAGE() {}


#ifdef __GNUC__

	#include "CStringNoRef.h"

#else

	#include "CString.h"

#endif

#ifdef _DEBUG

	void AddClassMemData(const CStringA& aName, TInt aMemSize);
	void DumpClassMemUsage();

#endif

// macros y funciones para la serializacion

typedef CMFCObject* (*TObjCreator)();

TInt AddRunTimeClass(const CStringA& sClassName, TObjCreator aCreatorFunc);
TInt GetClassIndexByName(const CStringA& sClassName);
CStringA GetClassNameByIndex(TInt idxClass);
CMFCObject* GetObjectByClassName(const CStringA& sClassName);
CMFCObject* GetObjectByClassIndex(TInt aidxClass);

#define DECLARE_SERIAL(class_name) \
	public: \
	static CMFCObject* CreateObject()


#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CMFCObject* class_name::CreateObject() \
	{ return new class_name; } \
	static TInt class_name##value = AddRunTimeClass(CStringA(#class_name), class_name::CreateObject)


#ifndef MB_OK

#define MB_OK         1
#define MB_ICONSTOP   2
#define MB_ERROR			16
#define MB_INFO				32
#define MB_YESNO      64
#define MB_LONGWAIT   4096

#define IDYES         1
#define IDNO          2
#define IDCANCEL      3
#define IDOK          4

#endif

#define AfxMessageBox MessageBox

// clase para imprimir intervalos de tiempo, solo se usa con objetivos de optimizacion

#if defined(TRACE_DEFINED)

	#define CTIMEDELTA _CTimeDelta

#else

#define CTIMEDELTA _CTimeDelta_Fake

// Clase equivalente a _CTimeDelta pero que no hace nada, se usa en la version Release
class _CTimeDelta_Fake
	{
public:
	_CTimeDelta_Fake(const char* aName = NULL, BOOL aCollect = FALSE)
		{
		}
	
	~_CTimeDelta_Fake()
		{
		}
	
	double GetSeconds()
		{
		return 0.0;
		}
	};

#endif

class _CTimeDelta
	{
private:
	const char* m_sName;
	static double m_ResCounter_1;	// 1/frecuencia del contador
#ifdef _WINDOWS_
	// contadores de windows de alta resolucion
	LARGE_INTEGER m_StartTime;
#else
	clock_t m_StartTime;
#endif
	BOOL m_bCollect;
public:

	static void Init()
		{
#ifdef _WINDOWS_
		LARGE_INTEGER PerfFrec;
		BOOL res = QueryPerformanceFrequency(&PerfFrec);

		if (res && PerfFrec.QuadPart != 0)
			m_ResCounter_1 = 1.0 / (double)PerfFrec.QuadPart;
		else
			{
			LOG_ERROR("Error getting Timer Resolution");
			m_ResCounter_1 = 0;
			}
#else
		m_ResCounter_1 = 1.0 / (double)CLOCKS_PER_SEC;
#endif

		//TRACE("Timer Resolution = %.15f", m_ResCounter_1);
		}

	_CTimeDelta(const char* aName = NULL, BOOL aCollect = FALSE)
		{
		m_sName = aName;
		m_bCollect = aCollect;

#ifdef _WINDOWS_
		QueryPerformanceCounter(&m_StartTime);
#else
		m_StartTime = clock();
#endif
		}
	
	~_CTimeDelta()
		{
#ifdef TRACE_DEFINED	    
		double seconds = GetSeconds();
		if (m_bCollect && m_sName)
			{
			COLLECT(m_sName, seconds);
			}
		else if (m_sName)
			TRACE2("%s : %.3f", m_sName, seconds);
#endif		
		}
	
	double GetSeconds()
		{
#ifdef _WINDOWS_
		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);
		double seconds = ((double)(endTime.QuadPart - m_StartTime.QuadPart)) * m_ResCounter_1;
#else
		clock_t endTime = clock();
		double seconds = ((double)(endTime - m_StartTime)) * m_ResCounter_1;
#endif

		return seconds;
		}
	};

#include "Logger.h"
#include "StrUtil.h"
#include "ArrayUtil.h"
#include "FastArray.h"
#include "FileUtil.h"

#include "ArrayUtil.inl"


#endif
