

template<class ARRAY, class ARCHIVE>
inline void FastLoadValue(ARRAY& aArray, ARCHIVE& aArchive) 
	{
	ASSERT(aArchive.IsLoading());
	if (aArchive.IsLoading())
		{
		TInt nNewSize = aArchive.ReadCount();
		aArray.SetSize(nNewSize);
		
		if (nNewSize == 0)
			return;
		
		aArchive.FastReadObjects((LPBYTE)aArray.GetBuffer(), nNewSize, sizeof(*aArray.GetBuffer()));
		}
	}

template<class ARRAY, class ARCHIVE>
inline void FastLoadClass(ARRAY& aArray, ARCHIVE& aArchive, BOOL aCallConstructor = FALSE)
	{
	ASSERT(aArchive.IsLoading());
	if (aArchive.IsLoading())
		{
		TInt nNewSize = aArchive.ReadCount();
		aArray.SetSize(nNewSize, aCallConstructor);
		
		if (nNewSize == 0)
			return;
		
		for(TInt i=0; i<nNewSize; i++)
			aArchive.FastReadObject(aArray.GetBuffer()+i);
		}
	}



template<class TYPE>
void CArrayValue<TYPE>::SerializeAsObArray(CArchive &ar) 
	{
	FastLoadValue(*this, ar);
	}

template<class TYPE>
void CArrayValue<TYPE>::Load(CArchiveMem &ar) 
	{
	SetSize(ar.ReadCount());
	for(int i=0; i<GetCount(); i++)
		GetAtPtr(i)->Load(ar);
	}

template<class TYPE>
void CArray<TYPE>::Load(CArchive &ar) 
	{
	SetSize(ar.ReadCount());
	for(int i=0; i<GetCount(); i++)
		GetAtPtr(i)->Load(ar);
	}
	
template<class TYPE>
void CArray<TYPE>::SerializeAsObArray(CArchive &ar) 
	{
	FastLoadClass(*this, ar, TRUE);
	}

template<class TYPE>
void CArray<TYPE>::SerializeAsObArrayMem(CArchiveMem &ar) 
	{
	FastLoadClass(*this, ar, TRUE);
	}

template<class TYPE>
void CFastArrayValue<TYPE>::SerializeAsObArray(CArchive &ar) 
	{
	FastLoadValue(*this, ar);
	}

template<class TYPE>
void CFastArrayValue<TYPE>::SerializeAsObArrayMem(CArchiveMem &ar) 
	{
	FastLoadValue(*this, ar);
	}

template<class TYPE>
void CFastArray<TYPE>::Load(CArchiveMem &ar) 
	{
	SetSize(ar.ReadCount());
	for(int i=0; i<GetCount(); i++)
		GetAtPtr(i)->Load(ar);
	}
	
template<class TYPE>
void CFastArray<TYPE>::SerializeAsObArray(CArchive &ar) 
	{
	FastLoadClass(*this, ar, TRUE);
	}

template<class TYPE>
void CFastArray<TYPE>::SerializeAsObArrayMem(CArchiveMem &ar)
	{
	FastLoadClass(*this, ar, TRUE);
	}
