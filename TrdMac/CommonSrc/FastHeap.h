#ifndef FASTHEAP_H_
#define FASTHEAP_H_


class CFastHeap
	{
protected:
	static CFastHeap* gFirstHeap;
	static CFastHeap* gCurrHeap;
	static BOOL gEnabled;

	CFastHeap* m_pNext;
	LPBYTE m_pData;
	TInt m_iOffset;
	TInt m_iSize;

	// Para los word se usa un buffer especifico pues el buffer general tiene que estar 
	// alineado en 4 bytes y si se usa para words se desperdiciaria memoria
	WORD* m_pWordData;
	TInt m_iWordOffset;
	TInt m_iWordSize;

protected:
	
	CFastHeap(TInt aSize, TInt aWordSize)
		{
		//COUNTER("CFastHeap_Count");
		Init(aSize, aWordSize);
		}
	
	~CFastHeap()
		{
		Free();
		}

	void Init(TInt aSize, TInt aWordSize)
		{
		m_pNext = NULL;

		m_iSize = aSize;
		m_pData = new BYTE[aSize];
		m_iOffset = 0;
		
		m_pWordData = new WORD[aWordSize];
		m_iWordSize = aWordSize;
		m_iWordOffset = 0;

		// inicializar en cero el buffer de words
		//memset(pWordData, 0, sizeof(WORD)*aWordSize);
		TRACE("FastHeap created with %.1f Mbytes (%d bytes, %d words)", 
			(aSize + aWordSize*2.0)/(1024.0*1024.0),
			aSize, aWordSize);
		}

	// Crea un nuevo Heap y lo convierte en el actual
	static CFastHeap* NewHeap();

	void Free()
		{
		delete m_pData;
		m_pData = NULL;
		m_iOffset = 0;
		m_iSize = 0;

		delete m_pWordData;
		m_pWordData = NULL;
		m_iWordOffset = 0;
		m_iWordSize = 0;

		if (m_pNext)
			delete m_pNext;
		m_pNext = NULL;
		}
	
	void Reset()
		{
		m_iOffset = 0;

		// inicializar en cero el buffer de words (la parte que se uso)
		//memset(pWordData, 0, sizeof(WORD)*iWordOffset);

		m_iWordOffset = 0;

		if (m_pNext)
			m_pNext->Reset();
		}

	inline LPVOID _Get(TInt aSize)
		{
		int newSize = (aSize + 3) & ~3; // alinear a 4 bytes

		if (m_iOffset + newSize > m_iSize)
			{ // no cabe aqui, pedir la memoria en el proximo heap
			if (m_pNext == NULL)
				m_pNext = NewHeap();

			return m_pNext->_Get(aSize);
			}
		else
			{
			LPBYTE pRet = m_pData + m_iOffset;
			m_iOffset += newSize; // alinear a 4 bytes
			ASSERT(m_iOffset <= m_iSize);
			return (LPVOID)pRet;
			}
		}

	inline WORD* _GetWords(TInt aNumWords)
		{
		if (m_iWordOffset + aNumWords > m_iWordSize)
			{ // no cabe aqui, pedir la memoria en el proximo heap
			if (m_pNext == NULL)
				m_pNext = NewHeap();  // crear un heap nuevo

			return m_pNext->_GetWords(aNumWords);
			}
		else
			{
			WORD* pRet = m_pWordData + m_iWordOffset;
			m_iWordOffset += aNumWords;
			ASSERT(m_iWordOffset <= m_iWordSize);
			return pRet;
			}
		}

	void _CheckUse()
		{
		COLLECT("CFastHeap::ByteSize", m_iOffset);
		COLLECT("CFastHeap::WordSize", m_iWordOffset);
		if (m_pNext)
			m_pNext->_CheckUse();
		}
	
public:

	static BOOL isEnabled()
		{
		return gEnabled;
		}

	static void Enable()
		{
		gEnabled = TRUE;
		}

	static void Disable()
		{
		gEnabled = FALSE;
		}

	// Crea el FastHeap o los reinicializa
	static void CreateOrReset()
		{
		if (gCurrHeap == NULL)
			{
			gFirstHeap = NewHeap();
			}
		else
			{
			gFirstHeap->Reset();
			gCurrHeap = gFirstHeap;
			}

		Enable();
		}
	
	static void DeleteAll()
		{
		delete gFirstHeap;
		gFirstHeap = gCurrHeap = NULL;

		Disable();
		}

	static void CheckUse()
		{
		if (gFirstHeap)
			gFirstHeap->_CheckUse();
		}
	
	// Ejemplo de uso de los new:
	// CStringA* p = CFastHeap::New<CStringA>("pepe");

	template<class TYPE>
	static inline TYPE* New()
		{
		TYPE* p = (TYPE*)Get(sizeof(TYPE));

		::new( (void*)(p) ) TYPE;

		return p;
		};

	template<class TYPE, class ATYPE1>
	static inline TYPE* New(ATYPE1 arg1)
		{
		TYPE* p = (TYPE*)Get(sizeof(TYPE));

		::new( (void*)(p) ) TYPE(arg1);

		return p;
		};

	template<class TYPE, class ATYPE1, class ATYPE2>
	static inline TYPE* New(ATYPE1 arg1, ATYPE2 arg2)
		{
		TYPE* p = (TYPE*)Get(sizeof(TYPE));

		::new( (void*)(p) ) TYPE(arg1, arg2);

		return p;
		};

	template<class TYPE, class ATYPE1, class ATYPE2, class ATYPE3>
	static inline TYPE* New(ATYPE1 arg1, ATYPE2 arg2, ATYPE3 arg3)
		{
		TYPE* p = (TYPE*)Get(sizeof(TYPE));

		::new( (void*)(p) ) TYPE(arg1, arg2, arg3);

		return p;
		};

	static inline LPVOID Get(TInt aSize)
		{
		ASSERT(gCurrHeap != NULL);
		return gCurrHeap->_Get(aSize);
		}

	static inline WORD* GetWords(TInt aNumWords)
		{
		ASSERT(gCurrHeap != NULL);
		return gCurrHeap->_GetWords(aNumWords);
		}
	};


#endif