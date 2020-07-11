
#include "WinUtil.h"


TInt _CArray_Expand = 10;

CArrayValue<CListNode> CNodeAllocator::arrNodes;
TInt CNodeAllocator::iNodeCount = 0;

CFastHeap* gFastHeap = NULL;

void CWordArray::Serialize(CArchive &ar)
	{
	TInt nCount = ar.ReadCount();
	SetSize(nCount);
	
	if (nCount > 0)
		ar.ReadBuffer(pData, nCount*2);
	}

void CWordArray::SerializeMem(CArchiveMem &ar)
	{
	TInt nCount = ar.ReadCount();
	SetSize(nCount);
	
	if (nCount > 0)
		ar.ReadBuffer(pData, nCount*2);
	}

void CFastWordArray::Serialize(CArchive &ar)
	{
	TInt nCount = ar.ReadCount();
	SetSize(nCount);
	
	if (nCount > 0)
		ar.ReadBuffer(pData, nCount*2);
	}

void CFastWordArray::SerializeMem(CArchiveMem &ar)
	{
	Load(ar);
	}

#ifdef ACTIVE_SAVE

void CFastWordArray::Save(CArchive &ar) const
	{
	ar.WriteCount(GetCount());
	ar.WriteBuffer((LPVOID)GetBuffer(), GetCount()*sizeof(GetAt(0)));
	}

void CSmartStrArray::Save(CArchive &ar)
	{
	// escribir primero el numero de strings
	ar << (WORD)iOffset.GetCount();
	if (iOffset.GetCount() > 0)
		{
		int len = iBuffer.length() + 1; // incluir el null terminator del final
		ar << len;
		ar.WriteBuffer((LPVOID)iBuffer.c_str(), len);

		//TRACEARRSTR((*this));
		}
	}

#endif

void CFastStrArray::Load(CArchive &ar)
	{
	WORD strCount;
	ar >> strCount;

	if (strCount > 0)
		{
		int len;
		ar >> len;

		m_pBuffer = (LPCSTR)CFastHeap::Get(len);
		ar.ReadBuffer((LPVOID)m_pBuffer, len);

		m_Offset.SetSize(strCount, FALSE);

		m_Offset[0] = m_pBuffer;
		if (strCount > 1)
			{
			int idxStr = 1;
			for(LPCSTR pStr = m_pBuffer; ; ++pStr)
				if (*pStr == 0)
					{
					m_Offset[idxStr++] = pStr+1;
					//TRACE("%s",pStr+1);
					if (idxStr >= strCount)
						break;
					}
			}

		//for(int i=0; i<GetCount(); i++)
		//	TRACE("%s", (*this)[i]); 
		}
	}

void CStrArray::Serialize(CArchive &ar)
	{
	if (ar.IsLoading())
		{
		TInt nCount = ar.ReadCount();
		SetSize(nCount);
	
		for(TInt i=0; i<nCount; i++)
			ar >> (*this)[i];
		}
	else
		{
#ifdef ACTIVE_SAVE
		ar.WriteCount(GetCount());
	
		for(TInt i=0; i<GetCount(); i++)
			ar << (*this)[i];
#endif
		}
	}

void CObArray::Serialize(CArchive &ar)
	{
	if (ar.IsLoading())
		{
		TInt nNewSize = ar.ReadCount();
		SetSize(nNewSize);
		
		for(TInt i=0; i<nNewSize; i++)
			{
			CMFCObject* pOb = ar.ReadObject();
			SetAt(i, pOb);
			}
		}
	}
