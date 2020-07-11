

#include "WinUtil.h"
#include "GramType.h"

void OptimizeForMemory()
	{
	_CArray_Expand = 0;
	}

void OptimizeForSpeed()
	{
	//_CArray_Expand = 64;
	_CArray_Expand = 4;
	}

#ifdef _DEBUG
void DebugASSERT(BOOL bCondition, LPCSTR sFile, int aLine, LPCSTR sCondition)
	{
	if (!bCondition) 
		{
		char sBuffer[4000];
		sprintf_(sBuffer, "ASSERT failed: %s, line %d, ASSERT(%s)", sFile, aLine, sCondition);
		LOG_ERROR(sBuffer);
		CRASH;
		}
	}
#endif

CLogger* pLogger = NULL;
void Log0(LPCSTR aText)
	{
	if (pLogger == NULL)
		{
		pLogger = new CLogger();
		}

	pLogger->Log(aText);
	}

void Log(LPCSTR aText, ...)
	{
	if (pLogger == NULL)
		{
		pLogger = new CLogger();
		}
	
	char buffer[20000];
	va_list list;
	va_start(list, aText);
	vsprintf_(buffer, aText, list);
  va_end(list);

	pLogger->Log(buffer);
	}

CStrArray arrCounters;
CIntArray arrCountersValues;

void CountData(const char* aName)
	{
	static BOOL bRecursive = FALSE;
	if (bRecursive) return;
	bRecursive = TRUE;
	
	CStringA sName(aName);
	TInt idx = arrCounters.Find(sName);
	if (idx == -1)
		{
		arrCounters.Add(sName);
		arrCountersValues.Add(1);
		}
	else
		arrCountersValues.SetAt(idx, arrCountersValues[idx] + 1);
	
	bRecursive = FALSE;
	}

void DumpCounters()
	{
	TRACE("---------------------------     COUNTERS     -----------------------------");
	for(TInt i=0; i<arrCounters.GetSize(); i++)
		{
		TRACE2("%-20s : %8d", CS2SZ(arrCounters[i]), arrCountersValues[i]);
		}
	TRACE("--------------------------------------------------------------------------");
	
	arrCounters.Close();
	arrCountersValues.Close();
	}

struct TCollectData
	{
	double iMin;
	double iMax;
	TInt iCount;
	double iSum;
	
	TCollectData()
		{
		iMin = -1;
		iMax = -1;
		iCount = 0;
		iSum = 0;
		}
	
	TCollectData(double aValue)
		{
		iMin = aValue;
		iMax = aValue;
		iCount = 1;
		iSum = aValue;
		}
	
	void Add(double aValue)
		{
		if (iMin == -1 || iMin > aValue)
			iMin = aValue;
		
		if (iMax == -1 || iMax < aValue)
			iMax = aValue;
		
		iCount++;
		iSum += aValue;
		}
	
	};// TCollectData;

CStrArray arrCollectedName;
CArrayValue<TCollectData> arrCollectedValues;

void CollectData(const char* aName, double aValue)
	{
	static BOOL bRecursive = FALSE;
	if (bRecursive) return;
	bRecursive = TRUE;
	
	CStringA sName(aName);
	TInt idx = arrCollectedName.Find(sName);
	if (idx == -1)
		{
		TCollectData data(aValue);
		
		arrCollectedName.Add(sName);
		arrCollectedValues.Add(data);
		}
	else
		{
		arrCollectedValues[idx].Add(aValue);
		}
	
	bRecursive = FALSE;
	}

void DumpCollectedData()
	{
	TRACE("---------------------------  COLLECTED DATA  -----------------------------");
	CStringA s;
	s.Format("%-25s %10s %10s %10s %10s %8s", "Nombre", "Minimo", "Maximo", "Promedio", "Total", "Cantidad");
	TRACE0(CS2SZ(s));
	
	for(TInt i=0; i<arrCollectedName.GetSize(); i++)
		{
		TCollectData value = arrCollectedValues[i];
		float fAverage = (value.iCount == 0) ? 0 : (float)value.iSum / value.iCount;
		
		s.Format("%-25s %10.2f %10.2f %10.2f %10.2f %8d", CS2SZ(arrCollectedName[i]), value.iMin, value.iMax, fAverage, value.iSum, value.iCount);
		TRACE0(CS2SZ(s));
		}

	TRACE("--------------------------------------------------------------------------");
	
	arrCollectedName.Close();
	arrCollectedValues.Close();
	}


#ifdef _DEBUG
class CClassData
	{
	public:
	CStringA iName;
	TInt iMemMin;
	TInt iMemMax;
	TInt iMemTotal;
	TInt iCount;
	
	CClassData(const CStringA& aName)
		{
		iName = aName;
		iMemMin = -1;
		iMemMax = -1;
		iMemTotal = 0;
		iCount = 0;
		}
	
	void Update(TInt aMemSize)
		{
		if (iMemMin == -1 || iMemMin > aMemSize)
			iMemMin = aMemSize;
		
		if (iMemMax == -1 || iMemMax < aMemSize)
			iMemMax = aMemSize;
		
		iMemTotal += aMemSize;
		iCount++;
		}
	

	static void DumpHeader()
		{
		CStringA s;
		s.Format("%-25s %8s %8s %10s %8s %8s", "Clase", "Minimo", "Maximo", "Promedio", "Total", "Cantidad");
		TRACE(CS2SZ(s));
		}
	
	void Dump()
		{
		CStringA s;
		float fAverage = (iCount == 0) ? 0 : (float)iMemTotal / iCount;
		s.Format("%-25s %8d %8d %10.2f %8d %8d", CS2SZ(iName), iMemMin, iMemMax, fAverage, iMemTotal, iCount);
		TRACE(CS2SZ(s));
		}
	};
#endif

CStrArray* arrClassName = NULL;
CPtrArray* arrClassCreator = NULL;

#ifdef _DEBUG 	
CPointerArray<CClassData>* arrClassData = NULL;

void AddClassMemData(const CStringA& aName, TInt aMemSize)
	{
	//if (aMemSize < 20)
	//	int k=0;
	TInt idx = arrClassName->Find(aName);
	if (idx >= 0)
		{
		arrClassData->GetAt(idx)->Update(aMemSize);
		}
	}

void DumpClassMemUsage()
	{
	CClassData::DumpHeader();
	for(TInt i=0; i<arrClassData->GetSize(); i++)
		{
		arrClassData->GetAt(i)->Dump();
		}
	}
#endif

TInt AddRunTimeClass(const CStringA& sClassName, TObjCreator aCreatorFunc)
	{
	if (arrClassName == NULL)
		arrClassName = new CStrArray();
	if (arrClassCreator == NULL)
		arrClassCreator = new CPtrArray();

	arrClassName->Add(sClassName);
	arrClassCreator->Add((void*)aCreatorFunc);

#ifdef _DEBUG 	
	if (arrClassData == NULL)
		arrClassData = new CPointerArray<CClassData>();
	CClassData* pClassData = new CClassData(sClassName);
	arrClassData->Add(pClassData);
#endif	
	return 0;
	}

TInt GetClassIndexByName(const CStringA& sClassName)
	{
	return arrClassName->Find(sClassName);
	}

CStringA GetClassNameByIndex(TInt idxClass)
	{
	if (idxClass >= 0 && idxClass < arrClassName->GetSize())
		return (*arrClassName)[idxClass];
	else
		return CStringA();
	}

CMFCObject* GetObjectByClassName(const CStringA& sClassName)
	{
	return GetObjectByClassIndex(arrClassName->Find(sClassName));
	}

CMFCObject* GetObjectByClassIndex(TInt aidxClass)
	{
	if (aidxClass >= 0)
		{
		TObjCreator pCreatorFunc = (TObjCreator)(*arrClassCreator)[aidxClass];
		return pCreatorFunc();
		}
	
	ASSERT(FALSE);
	return NULL;
	}

double _CTimeDelta::m_ResCounter_1 = 0.0;

#ifdef _WINDOWS_

struct TMemUse
	{
	int memUsed;
	int nBlocksUsed;
	int nBlocksFree;
	};

void windowsHeapDump(TMemUse& aMemUse)
	{
	int memSize = 0;
  _HEAPINFO hinfo;
  int heapstatus;
  hinfo._pentry = NULL;
  int nUsed = 0;
	int nFree = 0;

  while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK /*&& numLoops < 100*/)
		{
		if (hinfo._useflag == _USEDENTRY)
			{
			memSize += hinfo._size;
			nUsed++;
			}
		else
			nFree++;
    //printf("%6s block at %Fp of size %4.4X\n",
    //          (hinfo._useflag == _USEDENTRY ? "USED" : "FREE"),
    //          hinfo._pentry, hinfo._size);
		}

  switch(heapstatus)
	  {
		/*case _HEAPEMPTY:
				TRACE("OK - empty heap\n");
				break;
		case _HEAPEND:
				TRACE("OK - end of heap\n");
				break;*/
		case _HEAPBADPTR:
				TRACE("ERROR - bad pointer to heap\n");
				break;
		case _HEAPBADBEGIN:
				TRACE("ERROR - bad start of heap\n");
				break;
		case _HEAPBADNODE:
				TRACE("ERROR - bad node in heap\n");
				break;
		}

	aMemUse.memUsed = memSize;
	aMemUse.nBlocksUsed = nUsed;
	aMemUse.nBlocksFree = nFree;
	}

#endif

//TInt MaxHeapUsage = 871000; // max. cantidad usada por los modulos de dict y conj
TInt MaxHeapUsage = 0;
TInt MaxFreeSize = 0;
TInt MaxFreeCells = 0;
TInt MaxAllocCells = 0;

TInt GetAllocSize()
	{
	TInt totalSize = 0;
#ifdef _ANDROID_
	totalSize = mallinfo().uordblks;
#elif defined _WINDOWS_
	/*TMemUse aMemUse;
	windowsHeapDump(aMemUse);
	totalSize = aMemUse.memUsed;
	MaxAllocCells += aMemUse.nBlocksUsed;
	MaxFreeCells += aMemUse.nBlocksFree;*/
	_CrtMemState state;
	state.lHighWaterCount = 0;
	_CrtMemCheckpoint(&state);
	totalSize = state.lHighWaterCount;
#endif
	return totalSize;
	}

TInt GetFreeSize()
	{
	TInt totalSize = 0;
#ifdef _ANDROID_
	totalSize = mallinfo().fordblks;
#endif
	return totalSize;
	}

void CheckHeap()
	{
	TInt k = GetAllocSize();
	if (k > MaxHeapUsage)
		{
		MaxHeapUsage = k;
		MaxFreeSize = GetFreeSize();
		//if (MaxHeapUsage >= 12406000 && MaxAllocCells == 0)
#ifdef _ANDROID_
			MaxFreeCells = mallinfo().ordblks;
#endif
		}
	}

void InitMFCSystem()
	{
	_CTimeDelta::Init();

	OptimizeForMemory();
	
	InitCStringBuffers();

	CGramType::fillTypeArray();

	CNodeAllocator::InitNodes();
	}

void FinishMFCSystem(BOOL aDumpData)
	{
	CFastHeap::DeleteAll();

	if (aDumpData)
		{
		//DUMP_CLASS_MEM_USAGE;
		DUMP_COUNTERS;
		DUMP_COLLECTED_DATA;
	
		TRACE1("Max Heap Usage: %d", MaxHeapUsage);
		TRACE1("   Free Memory: %d", MaxFreeSize);
		TRACE1("    Alloc Cells: %d", MaxAllocCells);
		TRACE1("    Free Cells: %d", MaxFreeCells);
		}
	
	CGramType::clearTypes();

	CNodeAllocator::FreeNodes();
	
	FreeCStringBuffers();

	//delete pLogger;
	//pLogger = NULL;
	}

//TUint8* CSimpleHeap::iBuffer = NULL;
//TInt CSimpleHeap::iCount = 0;

