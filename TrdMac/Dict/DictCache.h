#ifndef DICTCACHE_H
#define DICTCACHE_H



// PACK_CACHE hace que la cache ocupe menos memoria (4 bytes menos)
// pero es un poco mas lenta
#define PACK_CACHE

// Items guardados en la cache (esta no se usa, sino su especializacion mas abajo)
template<class DATA>
class CCachedItem
	{
	private:
		//CCachedItem(const CCachedItem&) {}
		CCachedItem& operator=(const CCachedItem& other) {}

	protected:
	DWORD mIndex;					// indice de la clave en el diccionario (o su posicion mas cercana)

#ifndef PACK_CACHE
	BOOL mFound;					// indica si la clave existe o no
#endif

	public:
	const DATA* mData;		// datos de la clave

	CCachedItem(CCachedItem& aItem)
		{
		mIndex = aItem.mIndex;
#ifndef PACK_CACHE
		mFound = aItem.mFound;
#endif
		mData = aItem.mData;
    aItem.mData = NULL;
		}

	CCachedItem():mIndex(0),mData(NULL)
		{
		}

#ifdef PACK_CACHE
	CCachedItem(const DATA* aData, int aIndex, BOOL aFound):mIndex((aIndex << 1) | aFound),mData(aData)
#else
	CCachedItem(const DATA* aData, int aIndex, BOOL aFound):mIndex(aIndex),mFound(aFound),mData(aData)
#endif
		{	}

#ifdef PACK_CACHE
	CCachedItem(int aIndex, BOOL aFound):mIndex((aIndex << 1) | aFound),mData(NULL)
#else
	CCachedItem(int aIndex, BOOL aFound):mIndex(aIndex),mFound(aFound),mData(NULL)
#endif
		{	}

	~CCachedItem()
		{
		if (mData)
			delete mData;
		}

	inline BOOL found()
		{
#ifdef PACK_CACHE
		return mIndex & 1;
#else
		return mFound;
#endif
		}

	inline DWORD index()
		{
#ifdef PACK_CACHE
		return mIndex >> 1;
#else
		return mIndex;
#endif
		}
	};



// Especializacion para los LPCSTR
template<>
class CCachedItem<const char>
	{
	private:
		//CCachedItem(const CCachedItem&) {}
		CCachedItem& operator=(const CCachedItem& other);// {}

	protected:
	DWORD mIndex;					// indice de la clave en el diccionario (o su posicion mas cercana)

#ifndef PACK_CACHE
	BOOL mFound;					// indica si la clave existe o no
#endif

	public:
	LPCSTR mData;		// datos de la clave, van en el FastHeap por lo que no hay que liberarlos

	CCachedItem(const CCachedItem& aItem)
		{
		mIndex = aItem.mIndex;
#ifndef PACK_CACHE
		mFound = aItem.mFound;
#endif
		mData = aItem.mData;
		//aItem.mData = NULL;
		}

	CCachedItem():mIndex(0),mData(NULL)
		{
		}

#ifdef PACK_CACHE
	CCachedItem(LPCSTR aData, int aIndex, BOOL aFound):mIndex((aIndex << 1) | aFound),mData(aData)
#else
	CCachedItem(LPCSTR aData, int aIndex, BOOL aFound):mIndex(aIndex),mFound(aFound),mData(aData)
#endif
		{	}

#ifdef PACK_CACHE
	CCachedItem(int aIndex, BOOL aFound):mIndex((aIndex << 1) | aFound),mData(NULL)
#else
	CCachedItem(int aIndex, BOOL aFound):mIndex(aIndex),mFound(aFound),mData(NULL)
#endif
		{	}

	inline BOOL found()
		{
#ifdef PACK_CACHE
		return mIndex & 1;
#else
		return mFound;
#endif
		}

	inline DWORD index()
		{
#ifdef PACK_CACHE
		return mIndex >> 1;
#else
		return mIndex;
#endif
		}
	};

// Cache que guarda el indice mas cercano a cada elemento, si existe y los datos de la clave.
// Esta hecha para diccionarios que necesiten los indices aun cuando la clave no exista
// (por ejemplo en la busqueda de frases) y que no tengan todas las claves cargadas.
// Se guardan tanto las claves que existen, como las que no existen (para no leer de fichero
// cuando se pide repetidas veces una clave que no existe)
// Es la cache que usan los diccionarios por defecto.
template<class DATA>
//class CIndexedCache: public CMap<CCachedItem<DATA>*>
//class CIndexedCache: public CMapSZ<CCachedItem<DATA>*>
class CIndexedCache: public CMapSZ<CCachedItem<DATA> >
	{
	public:
	typedef CCachedItem<DATA> CACHE_ITEM;
	typedef DATA CACHE_DATA;

	void AfterLoad()
		{
		}

	template<class DICT>
	inline BOOL IsKey(DICT* aDict, const CStringA& aKey)
		{
		int idx;
		return aDict->FindIndex(aKey, idx);
		}

	template<class DICT>
	inline BOOL GetKey(DICT* aDict, const TDictKey& aKey, TDictData& aData)
		{
		aData.Empty();

		CACHE_ITEM* cachedItem = this->FindPtr(aKey.c_str());
		if (cachedItem)
			{ // la clave existe en la cache
			BOOL bFound = cachedItem->found();

			if (cachedItem->mData != NULL)
				aData = cachedItem->mData;
			else if (bFound)
				{
				//COUNTER("ReadData2");
				// la clave existe en el dicc pero aun no se han cargado los datos
				aDict->GetDataAt(cachedItem->index(), aData);
				cachedItem->mData = aData.toFastHeap();
				}

			return bFound;
			}
		else
			{ // la clave no existe en la cache, buscarla y añadirla
      //TRACE("Cargando key=%s", aKey.c_str());
			CACHE_DATA* data = NULL;
			int index;
			BOOL bFound = aDict->getIndex()->Find(aDict, aKey, &index);

			if (bFound)
				{
        //TRACE("   tiene datos");
				aDict->GetDataAt(index, aData);
				data = aData.toFastHeap();
				}

			this->Add(aKey.toFastHeap(), CACHE_ITEM(data, index, bFound));

			return bFound;
			}
		}

	template<class DICT>
	BOOL FindIndex(DICT* aDict, const TDictKey& aName, TInt &aIndex, TInt aStart, TInt aEnd)
		{
		CACHE_ITEM* cachedItem = this->FindPtr(aName.c_str());
		if (cachedItem)
			{ // la clave existe en la cache
			//COUNTER("CacheHit");
			aIndex = cachedItem->index();
			return cachedItem->found();
			}
		else
			{ // la clave no existe en la cache, buscarla y añadirla
			//COUNTER("CacheMiss");
			BOOL bFound = aDict->getIndex()->Find(aDict, aName, &aIndex, aStart, aEnd);
			// los datos no se piden ahora porque podrian no hacer falta
			this->Add(aName.toFastHeap(), CACHE_ITEM(aIndex, bFound));

			return bFound;
			}
		}
	};


enum { CACHE_INDEX_LIMIT = 50000};

// Cache mas simple que no guarda los indices ni si el elemento existe o no.
// Esta hecha para caches que carguen todas las claves de una vez, asi lo que no
// exista en la cache, es que no existe en el diccionario tampoco, por tanto
// no se guardan las claves que no existen, lo que es mas rapido y consume menos memoria.
// Si los datos son NULL es que la clave no tiene datos.
// Si los datos son menores a un numero dado, es que aun no se ha cargado ese dato.
// y el numero es el indice del elemento del diccionario donde estan los datos.
template<class DATA>
class CSimpleCache: public CMap<DATA*>
	{
	public:

	typedef DATA ITEM;
	typedef typename CMap<DATA*>::Const_Iterator Const_Iterator;
	
	// añade una clave a la cache indicando su indice (para cargar los datos despues
	// cuando haga falta)
	inline void Add(const CStringA& aKey, int aIndex)
		{
		CMap<DATA*>::Add(aKey, (DATA*)(aIndex + 1)); // el +1 es para que 0 no se confunda con NULL
		}

	enum EFillWhat {KEYS_ONLY = 1, KEYS_AND_DATA};

	template<class DICT>
	void Fill(const DICT* aDict, EFillWhat aWhat)
		{
		for(int i=0; i<aDict->GetCount(); i++)
			{
			CStringA sKey;
			DATA* Data = NULL;

			if (aWhat == KEYS_ONLY)
				{
				aDict->GetKeyAt(i, sKey);

				// inicialmente lo que se guarda en la cache es el indice de la clave
				Add(sKey, i);
				}
			else
				{
				CStringA sData;
				aDict->GetAt(i, &sKey, &sData);

				Data = new DATA(sData);
				CMap<DATA*>::Add(sKey, Data);
				}
			}
		}

	template<class DICT>
	void AfterLoad(const DICT* aDict)
		{
		Fill(aDict, KEYS_ONLY);
		}

	inline BOOL IsKey(const CStringA& aKey) const
		{
		return CMap<DATA*>::Exists(aKey);
		}

	template<class DICT>
	BOOL GetKey(const DICT* aDict, const CStringA& aKey, DATA* &aData)
		{
		DATA** pData = CMap<DATA*>::FindPtr(aKey);
		if (pData == NULL)
			{
			aData = NULL;
			return FALSE; // la clave no existe en la cache ni en el diccionario
			}

		// la clave existe en la cache
		if (*pData == NULL)
			{
			// la clave existe pero no tiene datos (esto puede ocurrir al menos en un dict de reduccion)
			aData = NULL;
			return TRUE;
			}

		DWORD index = (DWORD)*pData;
		if (index < CACHE_INDEX_LIMIT)
			{ // es un indice, aun no se han cargado los datos
			TDictData sData;
			aDict->GetDataAt(index-1, sData);

			// crear el item y actualizar la cache
			*pData = new DATA(sData);
			}

		// devolver los datos
		aData = *pData;
				
		return TRUE;
		}

	void DeleteAll()
		{
		for(Const_Iterator it = CMap<DATA*>::begin(); it != CMap<DATA*>::end(); it++)
			{
			DWORD index = (DWORD)it->second;
			if (index > CACHE_INDEX_LIMIT)
				delete it->second;
			}

		CMap<DATA*>::RemoveAll();
		}

	};



#endif
