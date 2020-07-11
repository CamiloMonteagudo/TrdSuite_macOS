#ifndef FASTARRAY_H_
#define FASTARRAY_H_

#include "FastHeap.h"


// Contiene Arrays que usan el FastHeap para pedir memoria

// Array que nunca llama a constructores ni destructores de TYPE, usa el FastHeap para pedir memoria
// usa memset para inicializar en 0, solo se puede llamar al SetSize una vez.
template<class TYPE>
class CFastArrayValue : public CMFCObject
	{
protected:
	TYPE* pData;
	TInt iCount;
	
public:
	CFastArrayValue()
		{
		pData = NULL;
		iCount = 0;
		}
	
	virtual ~CFastArrayValue()
		{
		RemoveAll();
		}
	
	inline TYPE* GetBuffer() { return pData; }
	
	inline TInt GetUpperBound() { return iCount - 1; }
	inline TInt GetCount() const { return iCount; }
	inline TInt GetSize() const { return iCount; }
	
	inline BOOL IsEmpty() const { return iCount == 0; }
	
	void SetSize(TInt aNewSize, BOOL aFillZero = TRUE) 
		{
		ASSERT(iCount == 0);
		if (aNewSize == 0)
			return;
		
		pData = (TYPE*)CFastHeap::Get(sizeof(TYPE)*aNewSize);
		iCount = aNewSize;
				
		// inicializar en cero (hace falta para que los casos de CArray<DWORD> y 
		// similares se inicializen en 0) 
		if (aFillZero)
			memset(pData, 0, sizeof(TYPE)*aNewSize);
		}
	
	//inline TYPE operator[](TInt aIndex) const { return pData[aIndex]; };
	inline TYPE& operator[](TInt aIndex) const { return pData[aIndex]; };
	
	//inline TYPE GetAt(TInt aIdx) const { return (*this)[aIdx]; }
	inline const TYPE& GetAt(TInt aIdx) const { return (*this)[aIdx]; }
	inline TYPE* GetAtPtr(TInt aIndex) const { return pData + aIndex; }
	inline void SetAt(TInt aIdx, const TYPE& aValue) { pData[aIdx] = aValue; }
	
	TInt Find(const TYPE& aValue)
		{
		for(TInt i=0; i<iCount; i++)
			if (pData[i] == aValue)
				return i;
		
		return -1;
		}
	
	void RemoveAll()
		{
		iCount = 0;
		pData = NULL;
		}
	
	virtual void Serialize(CArchive &/*ar*/) 
		{
		ASSERT(FALSE);
		}
		
	void SerializeAsObArray(CArchive &ar);
		
	void SerializeAsObArrayMem(CArchiveMem &ar);

	template<class ARCHIVE>
	void Load(ARCHIVE &ar) 
		{
		SetSize(ar.ReadCount());
		for(int i=0; i<GetCount(); i++)
			GetAtPtr(i)->Load(ar);
		}

	template<class ARCHIVE>
	void LoadBurst(ARCHIVE &ar) 
		{
		SetSize(ar.ReadCount(), FALSE);
		ar.ReadBuffer(GetBuffer(), GetCount()*sizeof(GetAt(0)));
		}

#ifdef ACTIVE_SAVE
	void Save(CArchive &ar) const
		{
		ar.WriteCount(GetCount());
		for(int i=0; i<GetCount(); i++)
			GetAt(i).Save(ar);
		}
#endif
	};


// Array de clases usando el FastHeap, parecido al anterior pero puede opcionalmente llamar al 
// constructor y destructor de las clases
template <class TYPE>
class CFastArray: public CMFCObject
	{
	private:
		TYPE* pData;
		TInt iCount;
		
	public:
	CFastArray()
		{
		pData = NULL;
		iCount = 0;
		}
		
	virtual ~CFastArray()
		{
		RemoveAll(TRUE);
		}
		
	inline TYPE* GetBuffer() { return pData; }
		
	inline TInt GetUpperBound() { return iCount - 1; }
	inline TInt GetCount() const { return iCount; }
	inline TInt GetSize() const { return iCount; }
		
	inline BOOL IsEmpty() const { return iCount == 0; }
		
	// en los FastArray el SetSize solo se puede llamar una vez		
	void SetSize(TInt aNewSize, BOOL aCallConstructor = FALSE) 
		{
		ASSERT(iCount == 0);
		if (aNewSize == 0)
			return;
			
		pData = (TYPE*)CFastHeap::Get(sizeof(TYPE)*aNewSize);
		iCount = aNewSize;
					
		// llamar al constructor
		if (aCallConstructor)
			{
			for(TInt i=0; i<aNewSize; i++)
				::new( (void*)(pData + i) ) TYPE;
			}
		else
			// inicializar en cero 
			memset((void*)pData, 0, sizeof(TYPE)*aNewSize);
		}
		
	//inline TYPE operator[](TInt aIndex) const { return pData[aIndex]; };
	inline const TYPE& operator[](TInt aIndex) const { return pData[aIndex]; };
		
	inline const TYPE& GetAt(TInt aIdx) const { return (*this)[aIdx]; }
	//inline TYPE& GetAt(TInt aIdx) { return (*this)[aIdx]; }
	inline TYPE* GetAtPtr(TInt aIndex) const { return pData + aIndex; }

	/*void SetAt(TInt aIdx, const TYPE& aValue)
		{
		TYPE* pType = pData + aIdx;
		pType->~TYPE();		// destruir el elemento viejo
			
		// llamar al copy constructor
		::new( (void*)( pType ) ) TYPE(aValue);
		}*/
		
	TInt Find(const TYPE& aValue)
		{
		for(TInt i=0; i<iCount; i++)
			if (pData[i] == aValue)
				return i;
			
		return -1;
		}
		
	void RemoveAll(BOOL aCallDestructor = TRUE)
		{
		if (aCallDestructor)
			for(TInt i=0; i<iCount; i++)
				pData[i].~TYPE();				// llamar al destructor
			
		iCount = 0;
		pData = NULL;
		}
		
	virtual void Serialize(CArchive &/*ar*/) 
		{
		ASSERT(FALSE);
		}
	
	void SerializeAsObArray(CArchive &ar);
		
	void SerializeAsObArrayMem(CArchiveMem &ar);

	void Load(CArchiveMem &ar);

#ifdef ACTIVE_SAVE
	void Save(CArchive &ar) const
		{
		ar.WriteCount(GetCount());
		for(int i=0; i<GetCount(); i++)
			GetAt(i).Save(ar);
		}
#endif
	};


class CFastWordArray
	{
protected:
	WORD* pData;
	TInt iCount;
	
public:
	CFastWordArray()
		{
		pData = NULL;
		iCount = 0;
		}
	
	virtual ~CFastWordArray()
		{
		RemoveAll();
		}
	
	inline WORD* GetBuffer() const { return pData; }
	
	inline TInt GetUpperBound() { return iCount - 1; }
	inline TInt GetCount() const { return iCount; }
	inline TInt GetSize() const { return iCount; }
	
	inline BOOL IsEmpty() const { return iCount == 0; }
	
	void SetSize(TInt aNewSize) 
		{
		ASSERT(iCount == 0);
		
		if (aNewSize == 0)
			return;
		
		pData = CFastHeap::GetWords(aNewSize);
		iCount = aNewSize;
				
		// inicializar en cero (hace falta para que los casos de CArray<DWORD> y 
		// similares se inicializen en 0) 
		//memset(pData, 0, sizeof(WORD)*aNewSize);
		}
	
	//inline WORD operator[](TInt aIndex) const { return pData[aIndex]; };
	inline WORD& operator[](TInt aIndex) const { return pData[aIndex]; };
	
	//inline WORD GetAt(TInt aIdx) const { return (*this)[aIdx]; }
	inline const WORD& GetAt(TInt aIndex) const { return pData[aIndex]; }
	inline void SetAt(TInt aIdx, WORD aValue) { pData[aIdx] = aValue; }
	
	TInt Find(WORD aValue)
		{
		for(TInt i=0; i<iCount; i++)
			if (pData[i] == aValue)
				return i;
		
		return -1;
		}
	
  void RemoveAll()
		{
		pData = NULL;
		iCount = 0;
		}
	
  virtual void Serialize(CArchive &ar);
  virtual void SerializeMem(CArchiveMem &ar);

	template<class ARCHIVE>
	void Load(ARCHIVE &ar)
		{
		TInt nCount = ar.ReadCount();
		SetSize(nCount);
	
		if (nCount > 0)
			ar.ReadBuffer(pData, nCount*2);
		}

#ifdef ACTIVE_SAVE
	void Save(CArchive &ar) const;
#endif
	};


class CFastStrArray
	{
	LPCSTR m_pBuffer;		// buffer donde se guardan todas la cadenas de manera consecutiva
	CFastArrayValue<LPCSTR> m_Offset;		// array de cada una de la cadenas
	public:

	CFastStrArray()
		{
		m_pBuffer = NULL;
		}

	void Load(CArchive &ar);

	inline TInt GetCount() { return m_Offset.GetCount(); }

	inline LPCSTR GetAt(TInt aIndex) const
		{
		return m_Offset[aIndex];
		}

	inline LPCSTR operator[](TInt aIndex) const
		{ return GetAt(aIndex); }
	};


#endif /*FASTARRAY_H_*/
