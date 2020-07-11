
#ifndef ARRAYUTIL_H_
#define ARRAYUTIL_H_

#include <stdlib.h>

#if defined(_BLACKBERRY_)
	#define MAP_TYPE_HASH_MAP
	#include <hash_map>
	#define map_class stlport::hash_map
#elif defined(_APPLE_)
	#define MAP_TYPE_UNORDERED_MAP
	#include <tr1/unordered_map>
	#define map_class std::tr1::unordered_map
#else
	#include <unordered_map>
	#define map_class std::tr1::unordered_map
#endif

extern TInt _CArray_Expand;

// se garantiza que aNewSize > aCapacity
inline int ExpandArray(int aCapacity, int aNewSize)
	{
  if (aCapacity == 0)
    return Max(aNewSize, 4);
  
  while ((aCapacity *= 2) < aNewSize);
  return aCapacity;
	//return aNewSize + _CArray_Expand;
	}

// Clase que emula el CArray de la MFC

template<class TYPE>
class CArrayValue : public CMFCObject
	{
protected:
	TYPE* pData;
	TInt iCount;
	TInt iCapacity;
	
public:
	CArrayValue()
		{
		pData = NULL;
		iCount = 0;
		iCapacity = 0;
		}
	
	virtual ~CArrayValue()
		{
		Close();
		}
	
	void Close()
		{
		if (pData)
			{
			std::free(pData);
			pData = NULL;
			}
		iCount = 0;
		iCapacity = 0;
		}
	
	inline TYPE* GetBuffer() { return pData; }
	
	inline TInt GetUpperBound() { return iCount - 1; }
	inline TInt GetSize() const { return iCount; }
	inline TInt GetCount() const { return iCount; }
	
	inline BOOL IsEmpty() const { return iCount == 0; }
	
	void Compress()
		{
		}
	
	void PreAllocate(int aNewSize)
		{
		if (aNewSize > iCapacity)
			{
			//COUNTER("CArray::realloc");
			//COLLECT("CArray::realloc presize", GetSize());
			//COLLECT("CArray::realloc postsize", aNewSize);
			iCapacity = ExpandArray(iCapacity, aNewSize);
			pData = (TYPE*)std::realloc(pData, iCapacity*sizeof(TYPE));
			}
		}
	
	void SetSize(TInt aNewSize, BOOL aFillZero = TRUE) 
		{
		if (aNewSize == iCount)
			return;
		
		TInt oldCount = iCount;
		if (aNewSize > oldCount)
			{ // expandir el array
			PreAllocate(aNewSize);

			// inicializar en cero (hace falta para que los casos de CArray<DWORD> y 
			// similares se inicializen en 0) 
			if (aFillZero)
				memset(pData + oldCount, 0, (aNewSize - oldCount)*sizeof(TYPE));
			iCount = aNewSize;
			}
		else
			{ // encoger el array
			if (aNewSize == 0)
				RemoveAll();
			else
				RemoveAt(aNewSize, oldCount - aNewSize);
			}
		}
	
	inline TYPE& operator[](TInt aIndex) const { return pData[aIndex]; };
	//inline TYPE operator[](TInt aIndex) const { return pData[aIndex]; };
	//inline TYPE& operator[](TInt aIndex) { return pData[aIndex]; };
	
	inline TYPE* GetAtPtr(TInt aIdx) const { return pData + aIdx; }
	//inline TYPE GetAt(TInt aIdx) const { return (*this)[aIdx]; }
	inline const TYPE& GetAt(TInt aIdx) const { return (*this)[aIdx]; }
	inline void SetAt(TInt aIdx, const TYPE& aValue) { pData[aIdx] = aValue; }
	
	TInt Find(const TYPE& aValue)
		{
		for(TInt i=0; i<iCount; i++)
			if (pData[i] == aValue)
				return i;
		
		return -1;
		}
	
	void Copy(const CArrayValue& aArray)
		{
		RemoveAll();
		InsertAt(0, &aArray);
		}
		
	CArrayValue& operator=(const CArrayValue& aArray)
		{ Copy(aArray); return *this; };
	
	void RemoveAll()
		{
		iCount = 0;
		}
	
	void RemoveAt(TInt aIdx, TInt aCount = 1) 
		{
		if (aIdx == 0 && aCount == iCount)
			RemoveAll();
		else
			{
			// desplazar los items que siguen
			if (aIdx + aCount < iCount)
				{
				TInt moveCount = iCount - (aIdx + aCount);
				memmove(pData + aIdx, pData + aIdx + aCount, sizeof(TYPE)*moveCount);
				}
			
			iCount -= aCount;
			}
		}
	
	void InsertAt(TInt aIdx, const TYPE& aValue) 
		{ 
		SetSize(iCount + 1, FALSE);
		
		// desplazar los elementos
		memmove(pData + aIdx + 1, pData + aIdx, sizeof(TYPE)*(iCount - aIdx - 1));
		
		pData[aIdx] = aValue;
		}
	
	void InsertAt(TInt aIdx, const CArrayValue<TYPE>* aArray) 
		{
		TInt count = aArray->GetSize();
		
		PreAllocate(iCount + count);
		
		// desplazar los elementos
		memmove(pData + aIdx + count, pData + aIdx, sizeof(TYPE)*(iCount - aIdx - 1));

		// copiar los elementos
		memcpy(pData + aIdx, aArray->pData, sizeof(TYPE)*count);
		//for(TInt i=0; i<count; i++)
		//	pData[aIdx++] = aArray->pData[i]);
		
		//for(TInt i=0; i<count; i++)
		//	InsertAt(aIdx++, aArray->GetAt(i));
		}
	
	TInt Add(const TYPE& aValue) 
		{ 
		SetSize(iCount + 1, FALSE);
		
		pData[iCount - 1] = aValue;

		return iCount - 1;
		}
	
	virtual void Serialize(CArchive &/*ar*/) 
		{
		ASSERT(FALSE);
		}
		
	void SerializeAsObArray(CArchive &ar);

	void Load(CArchiveMem &ar);

#ifdef ACTIVE_SAVE		
  void SaveBurst(CArchive &ar)
		{
		ar.WriteCount(GetCount());
		ar.WriteBuffer(GetBuffer(), GetCount()*sizeof(GetAt(0)));
		}
#endif
	};


template <class TYPE>
class CArray: public CMFCObject
	{
private:
	TYPE* pData;
	TInt iCount;
	TInt iCapacity;
		
public:
	CArray()
		{
		pData = NULL;
		iCount = 0;
		iCapacity = 0;
		}
		
	/*virtual*/ ~CArray()
		{
		Close();
		}
		
	inline void Close()
		{
		RemoveAll();
			
		if (pData)
			{
			std::free(pData);
			pData = NULL;
			iCapacity = 0;
			}
		}
		
	inline TYPE* GetBuffer() { return pData; }
		
	inline TInt GetUpperBound() const { return iCount - 1; }
	inline TInt GetSize() const  { return iCount; }
	inline TInt GetCount() const { return iCount; }
		
	inline BOOL IsEmpty() const { return iCount == 0; }
		
	void Compress()
		{
		}
		
	void PreAllocate(TInt aNewSize)
		{
		if (aNewSize > iCapacity)
			{
			//COUNTER("CArrayClass::realloc");
			iCapacity = ExpandArray(iCapacity, aNewSize);
			pData = (TYPE*)std::realloc(pData, iCapacity*sizeof(TYPE));
			}
		}
	
	void SetSize(TInt aNewSize, BOOL aCallConstructor = TRUE) 
		{
		if (aNewSize == iCount)
			return;
		
		TInt oldCount = iCount;
		if (aNewSize > oldCount)
			{ // expandir el array
			PreAllocate(aNewSize);
			
			if (aCallConstructor)
				for(TInt i=oldCount; i<aNewSize; i++)
					::new( (void*)(pData + i) ) TYPE;

			iCount = aNewSize;
			}
		else
			{ // encoger el array
			if (aNewSize == 0)
				RemoveAll();
			else
				RemoveAt(aNewSize, oldCount - aNewSize);
			}
		}
	
	//inline TYPE operator[](TInt aIndex) const { return pData[aIndex]; };
	inline TYPE& operator[](TInt aIndex) const 
    {
    ASSERT(aIndex >= 0 && aIndex < iCount);
    return pData[aIndex];
    };
		
	//inline TYPE GetAt(TInt aIndex) const { return pData[aIndex]; }
	inline const TYPE& GetAt(TInt aIndex) const 
    {
    ASSERT(aIndex >= 0 && aIndex < iCount);
    return pData[aIndex];
    }
    
	inline TYPE* GetAtPtr(TInt aIndex) const 
    {
    ASSERT(aIndex >= 0 && aIndex < iCount);
    return pData + aIndex;
    }

	void SetAt(TInt aIndex, const TYPE& aValue)
		{
    ASSERT(aIndex >= 0 && aIndex < iCount);
    
		// TODO: MFC solo hace: pData[aIdx] = aValue
		TYPE* pType = pData + aIndex;
		pType->~TYPE();		// destruir el elemento viejo
			
		// llamar al copy constructor
		::new( (void*)( pType ) ) TYPE(aValue);
		}
		
	TInt Find(const TYPE& aValue)
		{
		for(TInt i=0; i<iCount; i++)
			if (pData[i] == aValue)
				return i;
			
		return -1;
		}
		
	void Copy(const CArray& aArray)
		{
		RemoveAll();
		InsertAt(0, &aArray);
		}
			
	CArray& operator=(const CArray& aArray)
		{ Copy(aArray); return *this; };
		
	// el DeleteAll es para hacer esta clase mas compatible con la CObArray
	void DeleteAll()
		{
		RemoveAll();
		}

	void RemoveAll()
		{
		for(TInt i=0; i<iCount; i++)
			pData[i].~TYPE();				// llamar al destructor
			
		iCount = 0;
		}
		
	void RemoveAt(TInt aIdx, TInt aCount = 1) 
		{
    ASSERT(aIdx >= 0 && aIdx < iCount);
    
		if (aIdx == 0 && aCount == iCount)
			RemoveAll();
		else
			{
			for(TInt i=0; i<aCount; i++)
				{
				pData[aIdx + i].~TYPE();				// llamar al destructor
				}
				
			// desplazar los items que siguen
			if (aIdx + aCount < iCount)
				{
				TInt moveCount = iCount - (aIdx + aCount);
				memmove((void*)(pData + aIdx), (void*)(pData + aIdx + aCount), sizeof(TYPE)*moveCount);
				}
				
			iCount -= aCount;
			}
		}
		
	void InsertAt(TInt aIdx, const TYPE& aValue) 
		{ 
    ASSERT(aIdx >= 0 && aIdx <= iCount);
		SetSize(iCount + 1, FALSE);
			
		// desplazar los elementos
		memmove(pData + aIdx + 1, pData + aIdx, sizeof(TYPE)*(iCount - aIdx - 1));
			
		// llamar al copy constructor
		::new( (void*)(pData + aIdx) ) TYPE(aValue);
		}
		
	void InsertAt(TInt aIdx, const CArray<TYPE>* aArray) 
		{
    ASSERT(aIdx >= 0 && aIdx <= iCount);
		TInt count = aArray->GetSize();
			
		PreAllocate(iCount + count);
			
		for(TInt i=0; i<count; i++)
			InsertAt(aIdx++, aArray->GetAt(i));
		}
		
	TInt Add(const TYPE& aValue) 
		{ 
		SetSize(iCount + 1, FALSE);
			
		// llamar al copy constructor
		::new( (void*)( pData + iCount - 1 ) ) TYPE(aValue);

		return iCount - 1;
		}

	template<class ARG1>
	TInt Add(ARG1& aArg1)
		{ 
		SetSize(iCount + 1, FALSE);
		// llamar al constructor
		::new( (void*)( pData + iCount - 1 ) ) TYPE(aArg1);
		return iCount - 1;
		}

	template<class ARG1, class ARG2>
	TInt Add(ARG1& aArg1, ARG2& aArg2)
		{ 
		SetSize(iCount + 1, FALSE);
		// llamar al constructor
		::new( (void*)( pData + iCount - 1 ) ) TYPE(aArg1, aArg2);
		return iCount - 1;
		}

	template<class ARG1, class ARG2, class ARG3>
	TInt Add(ARG1& aArg1, ARG2& aArg2, ARG3& aArg3)
		{ 
		SetSize(iCount + 1, FALSE);
		// llamar al constructor
		::new( (void*)( pData + iCount - 1 ) ) TYPE(aArg1, aArg2, aArg3);
		return iCount - 1;
		}

	TInt AddUnique(const TYPE& aValue)
		{
		int idx = Find(aValue);
		if (idx == -1)
			idx = Add(aValue);

		return idx;
		}

	virtual void Serialize(CArchive &/*ar*/)
		{
		ASSERT(FALSE);
		}
	
	void SerializeAsObArray(CArchive &ar);

	void SerializeAsObArrayMem(CArchiveMem &ar);

	void Load(CArchive &ar);

#ifdef ACTIVE_SAVE
	void Save(CArchive &ar) const
		{
		ar.WriteCount(GetCount());
		for(int i=0; i<GetCount(); i++)
			GetAt(i).Save(ar);
		}

	// usado en los CActionPhrase
	template<class TYPE1>
	void Save(CArchive &ar, CArray<TYPE1>& arrWords) const
		{
		ar.WriteCount(GetCount());
		for(int i=0; i<GetCount(); i++)
			GetAt(i).Save(ar, arrWords);
		}

#endif
		
	};


class CWordArray: public CArrayValue<WORD>
	{
	public:

	virtual void Serialize(CArchive &ar);
  virtual void SerializeMem(CArchiveMem &ar);
	};

typedef CArrayValue<TInt> CIntArray;
typedef CArrayValue<DWORD> CUIntArray;

template<class TYPE>
class CPointerArray :  public CArrayValue<TYPE*>	
	{
	public:

	// vacia la lista y destruye todos los elementos
	void DeleteAll()
		{
		for(int i=0; i<CArrayValue<TYPE*>::iCount; i++)
			{
			if (CArrayValue<TYPE*>::pData[i])
				delete CArrayValue<TYPE*>::pData[i];
			}

		CArrayValue<TYPE*>::RemoveAll();
		}
	};

// Especializacion de la clase anterior para los void	(no tiene el DeleteAll)
template<>
class CPointerArray<void> :  public CArrayValue<void*>	
	{
	};
	
typedef CPointerArray<void> CPtrArray;

//class CObArray :  public CPointerArray<CMFCObject>	
class CObArray :  public CArrayValue<CMFCObject*>	
	{
	public:

	inline CMFCObject* GetAtPtr(TInt aIndex) { return pData[aIndex]; }

	inline void DeleteAll()
		{
		for(TInt i=0; i<iCount; i++)
			{
			if (pData[i])
				delete pData[i]; 
			}
		
		CArrayValue<CMFCObject*>::RemoveAll();
		}

	inline void InsertAt(TInt aIdx, const CObArray* aArray)
		{
		CArrayValue<CMFCObject*>::InsertAt(aIdx, aArray);
		}
		
	inline void InsertAt(TInt aIdx, const CMFCObject* aObject)
		{
		CArrayValue<CMFCObject*>::InsertAt(aIdx, (CMFCObject* const&)aObject);
		}
	
  virtual void Serialize(CArchive &ar);

	};

class CStrArray: public CArray<CStringA>
	{
	public:

	~CStrArray() {};

	virtual void Serialize(CArchive &ar);
		
	TInt Find(const CStringA& aItem)
		{
		TInt nCount = GetSize();
		for(TInt i=0; i<nCount; i++)
			{
			if ((*this)[i] == aItem)
				return i;
			}
		
		return -1;
		}

	void Sort()
		{
		for(int i=0; i<GetCount()-1; i++)
			{
			CString si = GetAt(i);
			for(int j=i+1; j<GetCount(); j++)
				{
				CStringA sj = GetAt(j);
				if (si > sj)
					{
					SetAt(i, sj);
					SetAt(j, si);
					si = sj;
					}
				}
			}

		}
	
	CStringA Join(char ch)
		{
		CStringA res;
		for(int i=0; i<GetCount(); i++)
			{
			res += GetAt(i) + ch;
			}

		return res;
		}

	void Split(const CStringA& aStr, char separator)
		{
		int pos = 0;
		for(;;)
  		{
			int fin = aStr.Find(separator, pos);
			if( fin == -1)
    		{
				if (pos < aStr.GetLength())
					Add(aStr.Mid(pos));
				break;
				}
			else
				{
				if (fin > pos)
					Add(aStr.Mid(pos, fin - pos));
				pos = fin + 1;
				}
			}

		}
	};

// Clase que guarda una lista de strings de manera eficiente en cuanto al uso de memoria.
// las cadenas son de solo lectura, no se pueden modificar o borrar una vez añadidas
class CSmartStrArray
	{
	std::string iBuffer;		// bufer donde se guardan todas la cadenas de manera consecutiva
	CIntArray   iOffset;		// desplazamiento del inicio de cada una de la cadenas
public:

	CSmartStrArray(TInt aLen = 512)
		{
		iBuffer.reserve(aLen);
		}

	~CSmartStrArray()
		{
		Close();
		}
	
	void Preallocate(int aNumItems, int sItemLen)
		{
		int aStrTotalLen = aNumItems * (sItemLen + 1);
		if (aStrTotalLen > iBuffer.capacity())
			iBuffer.reserve(aStrTotalLen);
		
		iOffset.PreAllocate(aNumItems);
		}
	
	/*void Preallocate2(TInt aNumItems, TInt aTotalLen)
		{
		if (aTotalLen > iBuffer.capacity())
			iBuffer.reserve(aTotalLen);
		
		iOffset.PreAllocate(aNumItems);
		}*/

	void Close()
		{
		RemoveAll();
		}
	
	void RemoveAll()
		{
		iBuffer.clear();
		iOffset.Close();
		}
	
	inline TInt GetCount() { return iOffset.GetCount(); }
	
	inline TInt GetUpperBound() { return iOffset.GetCount() - 1; }
	
	void Add(const CStringA& aDesc)
		{
		size_t newLen = iBuffer.length() + 1 + aDesc.GetLength();
		if (newLen > iBuffer.capacity())
			{
			size_t expandSize = Max(1000, newLen/2);
			iBuffer.reserve(newLen + expandSize);
			}
		
		if (iOffset.GetCount() > 0)
			iBuffer.append(1, '\x0');	// añadir el null terminator entre las cadenas, para convertir facilmente a LPCSTR
		iOffset.Add((int)iBuffer.length());
		iBuffer.append(aDesc.toString());
		}
	
	void Add(LPCSTR aStr, int aLen)
		{
		size_t newLen = iBuffer.length() + 1 + aLen;
		if (newLen > iBuffer.capacity())
			{
			size_t expandSize = Max(1000, newLen/2);
			iBuffer.reserve(newLen + expandSize);
			}
		
		if (iOffset.GetCount() > 0)
			iBuffer.append(1, '\x0');	// añadir el null terminator entre las cadenas, para convertir facilmente a LPCSTR
		iOffset.Add((int)iBuffer.length());
		iBuffer.append(aStr, aLen);
		}

	// Reduce la memoria usada por esta clase al minimo imprescindible, solo debe llamarse
	// despues de añadir todos los elementos
	void Compress()
		{
		iBuffer.reserve(0);
		iOffset.Compress();
		}
	
	void Copy(const CSmartStrArray& aArrStr)
		{
		RemoveAll();
		iBuffer = aArrStr.iBuffer;
		iOffset = aArrStr.iOffset;
		}

#ifdef ACTIVE_SAVE
	// nota: este es ineficiente, solo se usa para salvar
	void Copy(const CStrArray& aStrArray)
		{
		RemoveAll();

		for(int i=0; i<aStrArray.GetCount(); i++)
			Add(aStrArray[i]);
		}
	
  void Save(CArchive &ar);
#endif

	int Find(const CStringA& name)
		{
		int nSize = GetCount();

		for(int i=0; i<nSize; i++)
			if (name.Compare(GetAtSZ(i)) == 0)
				return i;

		return -1;
		}
	
	inline CSmartStrArray& operator=(const CSmartStrArray& aArrStr)
		{ Copy(aArrStr); return *this; };

	inline const CStringA operator[](TInt aIndex) const
		{ return CStringA(GetAtSZ(aIndex)); }

	inline CStringA GetAt(TInt aIndex) const
		{ return (*this)[aIndex]; }
	
	/*inline std::string GetAtPtr(TInt aIndex) const
		{
		TInt startNext;
		TInt start = iOffset[aIndex];
		
		if (aIndex == iOffset.GetSize()-1)
			startNext = iBuffer.length();
		else
			startNext = iOffset[aIndex+1] - 1;
		
		return iBuffer.substr(start, startNext - start);
		}*/

	inline LPCSTR GetAtSZ(TInt aIndex) const
		{
		return iBuffer.c_str() + iOffset[aIndex];
		}

	};


#if defined(_BLACKBERRY_)

class myHashStdString
	{
public:
	size_t operator()(const std::string& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
		LPCSTR __s = _Keyval.c_str();
		unsigned long __h = 0;
		for ( ; *__s; ++__s)
			__h = 5*__h + *__s;

		return __h;
		}
	};

#endif

template <class TYPE>
class CMap
	{
	typedef void (*TMapItemDeleter)(TYPE aItem);
	typedef void (*TForEachFunc)(TYPE aItem);
	
	typedef std::string string_;
#if defined(_BLACKBERRY_)
	typedef	map_class<string_, TYPE, myHashStdString> my_map;
#else	
	typedef	map_class<string_, TYPE> my_map;
#endif	
		

public:
	typedef typename my_map::const_iterator Const_Iterator;
	typedef typename my_map::iterator Iterator;
	typedef typename my_map::value_type Item_Pair;

	protected:

		my_map iMap;
		
	public:
		CMap() 
#ifdef MAP_TYPE_UNORDERED_MAP
			//:iMap(32)
#endif
			{};

		~CMap()
		 { };
		
		inline Const_Iterator begin()
			{
			return iMap.begin();
			}

		inline Const_Iterator end()
			{
			return iMap.end();
			}

		template<TForEachFunc FOREACH_FUNC>
		void ForEach()
			{
			for(Const_Iterator it = iMap.begin(); it != iMap.end(); it++)
				(*FOREACH_FUNC)(it->second);
			}

		template<class TACTION>
		void ForEach2(TACTION& aAction)
			{
			for(Const_Iterator it = iMap.begin(); it != iMap.end(); it++)
				aAction.Action(it->second);
			}
			
		template<TMapItemDeleter DELETER>
		void DeleteAllT()
			{
			for(Const_Iterator it = iMap.begin(); it != iMap.end(); it++)
				(*DELETER)(it->second);

			RemoveAll();
			}
		
		void DeleteAll()
			{
			for(Const_Iterator it = iMap.begin(); it != iMap.end(); it++)
				if (it->second)
					delete it->second;

			RemoveAll();
			}

		inline void RemoveAll()
			{
			iMap.clear();
			}

		inline void Reserve(int nElems)
			{
			iMap.rehash(nElems);
			}

		inline void Add(const CStringA& aKey, const TYPE& aValue)
			{
			iMap.insert(Item_Pair(aKey, aValue));
			}
		
		// Busca si una llave existe, si existe devuelve TRUE y retValue apunta al valor
		// sino existe la inserta con valor vacio, devuelve FALSE y retValue apunta al
		// valor vacio insertado
		// retValue es una referencia al puntero, por lo que el valor puede ser modificado
		inline BOOL FindAdd(const CStringA& aKey, TYPE* &retValue)
			{
			std::pair<Iterator, bool> pr;
			pr = iMap.insert(Item_Pair(aKey.toString(), TYPE()));
			// pr.second == TRUE significa que se inserto exitosamente, sino es que ya existia
			// y pr.first->second apunta al elemento en cuestion
			retValue = &pr.first->second;
			return !pr.second;
			}

		inline TYPE* FindPtr(const CStringA& aKey)
			{
			//Const_Iterator iter = iMap.find(aKey);
			Iterator iter = iMap.find(aKey);
			if (iter != iMap.end())
				return &iter->second;

			return NULL;
			}

		inline BOOL Exists(const CStringA& aKey) const
			{
			return iMap.find(aKey) != iMap.end();
			}

		inline BOOL Find(const CStringA& aKey, TYPE& aValue) const
			{
			Const_Iterator iter = iMap.find(aKey);
			if (iter != iMap.end())
				{
				aValue = iter->second;
				return TRUE;
				}
			else
				return FALSE;
			}
	};
	
class myHash : public std::unary_function<LPCSTR, size_t>
	{	// hash functor
public:
	typedef LPCSTR _Kty;

	size_t operator()(const _Kty& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
		LPCSTR __s = _Keyval;
		unsigned long __h = 0;
		for ( ; *__s; ++__s)
			__h = 5*__h + *__s;

		return __h;
		}
	};

struct myEqualTo : public std::binary_function<LPCSTR, LPCSTR, bool>
	{	// functor for operator==
	bool operator()(const LPCSTR& _Left, const LPCSTR& _Right) const
		{	// apply operator== to operands
		LPCSTR left = _Left;
		LPCSTR right = _Right;
		for(; *left != 0; ++left,++right)
			if (*left != *right)
				return false;

		return (*left == 0) && (*right == 0);
		//return strcmp(_Left, _Right) == 0;
		}
	};

template <class TYPE>
class CMapSZ
	{
	typedef LPCSTR string_;
	typedef std::pair<string_, TYPE> Item_Pair;
	typedef	map_class<string_, TYPE, myHash, myEqualTo> my_map;

public:
	typedef typename my_map::const_iterator Const_Iterator;
	typedef typename my_map::iterator Iterator;

	protected:

		my_map iMap;
		
	public:
		CMapSZ() 
#ifdef MAP_TYPE_UNORDERED_MAP
			//:iMap(32)
#endif
			{};

		~CMapSZ()
		 { };
		 
		inline int GetCount()
			{
			return (int)iMap.size();
			}
			
		inline Const_Iterator begin()
			{
			return iMap.begin();
			}

		inline Const_Iterator end()
			{
			return iMap.end();
			}

		inline void DeleteAll()
			{
			for(Const_Iterator it = iMap.begin(); it != iMap.end(); it++)
				delete it->second;

			RemoveAll();
			}
		
		inline void RemoveAll()
			{
			iMap.clear();
			}

#ifdef MAP_TYPE_UNORDERED_MAP
		inline void Reserve(int nElems)
			{
			iMap.rehash(nElems);
			}
#else
		inline void Reserve(int /*nElems*/) {}
#endif

		template<class TACTION>
		void ForEach(TACTION& aAction)
			{
			for(Const_Iterator it = iMap.begin(); it != iMap.end(); it++)
				aAction.Action(it->first, it->second);
			}
			
		inline void Add(LPCSTR aKey, const TYPE& aValue)
			{
			iMap.insert(Item_Pair(aKey, aValue));
			/*TRACE("inserted : %d", pr.second);
			if (pr.second == false)
				TRACE("valor = %s", pr.first->second);*/
			}
		
		/*inline void Add(LPCSTR aKey, TYPE&& aValue)
			{
			iMap.insert(Item_Pair(aKey, std::forward<TYPE>(aValue)));
			}*/

		// Busca si una llave existe, si existe devuelve TRUE y retValue apunta al valor
		// sino existe la inserta con valor vacio, devuelve FALSE y retValue apunta al
		// valor vacio insertado
		// retValue es una referencia al puntero, por lo que el valor puede ser modificado
		inline BOOL FindAdd(LPCSTR aKey, TYPE* &retValue)
			{
			std::pair<Iterator, bool> pr;
			pr = iMap.insert(Item_Pair(aKey, TYPE()));
			// pr.second == TRUE significa que se inserto exitosamente, sino es que ya existia
			// y pr.first->second apunta al elemento en cuestion
			retValue = &pr.first->second;
			return !pr.second;
			}

		inline TYPE* FindPtr(LPCSTR aKey)
			{
			//Const_Iterator iter = iMap.find(aKey);
			Iterator iter = iMap.find(aKey);
			if (iter != iMap.end())
				return &iter->second;

			return NULL;
			}

		inline BOOL Exists(LPCSTR aKey) const
			{
			return iMap.find(aKey) != iMap.end();
			}

		inline BOOL Find(LPCSTR aKey, TYPE& aValue) const
			{
			Const_Iterator iter = iMap.find(aKey);
			if (iter != iMap.end())
				{
				aValue = iter->second;
				return TRUE;
				}
			else
				return FALSE;
			}
	};

// Implementa la CObList de la mfc como una lista doble enlazada circular

class CListNode;
typedef CListNode* POSITION;

class CListNode 
	{
	friend class CObList;
	friend class CNodeAllocator;
	
	CListNode* iNext;
	CListNode* iPrev;
	CMFCObject* iElem;
	
	void Set(CMFCObject* aElem, POSITION aPos = NULL, BOOL aInsertAfter = TRUE)
		{
		//COUNTER("CListNode");
		iElem = aElem;
		
		if (aPos == NULL)
			{ // caso cuando la lista esta vacia
			iNext = this;
			iPrev = this;
			}
		else if (aInsertAfter)
			{ // insertar el nodo despues de aPos
			iNext = aPos->iNext;
			iPrev = aPos;
			aPos->iNext = this;
			iNext->iPrev = this;
			}
		else
			{ // insertar el nodo antes de aPos
			iNext = aPos;
			iPrev = aPos->iPrev;
			aPos->iPrev = this;
			iPrev->iNext = this;
			}
		}
	};

#define MAX_NODES 3000

class CNodeAllocator
	{
protected:
	static CArrayValue<CListNode> arrNodes;
	static TInt iNodeCount;
	
public:
	static void InitNodes()
		{
		arrNodes.SetSize(MAX_NODES);
		iNodeCount = 0;
		}
	
	static void FreeNodes()
		{
		iNodeCount = 0;
		arrNodes.Close();
		}

	static void Restart()
		{
		iNodeCount = 0;
		}
	
	static CListNode* GetNewNode(CMFCObject* aElem, POSITION aPos = NULL, BOOL aInsertAfter = TRUE)
		{
		ASSERT(arrNodes.GetBuffer() != NULL);
		ASSERT(iNodeCount < MAX_NODES);

		CListNode* pNode = arrNodes.GetAtPtr(iNodeCount++);
		pNode->Set(aElem, aPos, aInsertAfter);
		
		return pNode;
		}
			
	};

class CObList : public CMFCObject
	{
	CListNode* iHead;

public:
	CObList()
		{
		iHead = NULL;
		};
	
	virtual ~CObList()
		{
		RemoveAll();
		}

	/*void CheckList()
		{
		int count1 = 0;
		POSITION pos = GetHeadPosition();
		
		while (pos)
			{
			GetNext(pos);
			count1++;
			}
		
		pos = GetTailPosition();
		int count2 = 0;
		while (pos)
			{
			GetPrev(pos);
			count2++;
			}
		
		if (count1 != count2)
			int k=0;
		}*/
	
	inline BOOL IsEmpty() { return iHead == NULL; }
	
	inline POSITION GetHeadPosition() { return iHead; };
	
	inline POSITION GetTailPosition() { return iHead ? iHead->iPrev : NULL; } ;
	
	inline CMFCObject* GetHead() { return iHead ? iHead->iElem : NULL; };
	inline CMFCObject* GetTail() { return iHead ? iHead->iPrev->iElem : NULL; };

	inline CMFCObject*& GetNext(POSITION& aPos)
		{
		POSITION pos = aPos;
		aPos = aPos->iNext;
		if (aPos == iHead)
			aPos = NULL;
		
		return pos->iElem;
		};
	
	inline CMFCObject*& GetPrev(POSITION& aPos)
		{
		POSITION pos = aPos;
		
		aPos = (aPos == iHead) ? NULL : aPos->iPrev;
		
		return pos->iElem;
		};
	
	inline CMFCObject* GetAt(POSITION aPos) { return aPos->iElem; };
	
	inline void SetAt(POSITION aPos, CMFCObject* aElem)
		{
		aPos->iElem = aElem;
		}
	
	POSITION InsertAfter(POSITION aPos, CMFCObject* aElem)
		{
		//CheckList();
		
		if (aElem == NULL)
			ASSERT(FALSE);
		else if (aPos == NULL)
			return AddTail(aElem);
		else
			return CNodeAllocator::GetNewNode(aElem, aPos);
		
		ASSERT(FALSE);
		return aPos;
		}
	
	POSITION InsertBefore(POSITION aPos, CMFCObject* aElem)
		{
		//CheckList();

		if (aElem == NULL)
			ASSERT(FALSE);
		else if (aPos == NULL)
			return AddHead(aElem);
		else
			{
			CListNode* node = CNodeAllocator::GetNewNode(aElem, aPos, FALSE);
			// actualizar la cabeza
			if (iHead == aPos)
				iHead = node;
			
			return node;
			}
		
		ASSERT(FALSE);
		return aPos;
		}
	
	inline POSITION AddHead(CMFCObject* aElem)
		{
		if (iHead == NULL)
			iHead = CNodeAllocator::GetNewNode(aElem);
		else
			iHead = CNodeAllocator::GetNewNode(aElem, iHead, FALSE);
			
		//CheckList();
		return iHead;
		}
	
	inline POSITION AddTail(CMFCObject* aElem)
		{
		if (iHead == NULL)
			iHead = CNodeAllocator::GetNewNode(aElem);
		else
			CNodeAllocator::GetNewNode(aElem, iHead->iPrev);
			
		//CheckList();
		return iHead->iPrev;
		}
	
	void RemoveAt(POSITION aPos)
		{
		if (!aPos) return;
		
		if (aPos->iNext != aPos) // no hay un solo elemento
			{
			aPos->iNext->iPrev = aPos->iPrev;
			aPos->iPrev->iNext = aPos->iNext;
			
			if (aPos == iHead)
				iHead = aPos->iNext;
			}
		else // hay un solo elemento
			{
			ASSERT(iHead == aPos);
			iHead = NULL;
			}
		
		//delete aPos;
		//CheckList();
		};
	
	void RemoveAll()
		{
		/*CListNode* pItem = iHead;
		while (pItem != NULL)
			{
			CListNode* pNext = pItem->iNext;
			//delete pItem;
			pItem = pNext;
			if (pItem == iHead)
				break;
			}*/
		
		iHead = NULL;
		};
	
	inline CMFCObject* RemoveHead()
		{
		if (iHead == NULL)
			return NULL;
		
		CMFCObject* pElem = iHead->iElem;
		
		RemoveAt(iHead);
		
		return pElem;
		};

	inline CMFCObject* RemoveTail()
		{
		if (iHead == NULL)
			return NULL;
		
		CMFCObject* pElem = iHead->iPrev->iElem;
		
		RemoveAt(iHead->iPrev);
		
		return pElem;
		};
	
	};

#endif
