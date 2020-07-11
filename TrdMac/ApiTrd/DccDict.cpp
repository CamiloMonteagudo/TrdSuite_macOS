
#include "CompiledCmdArray.h"
#include "DccDict.h"

//CArray<CActionPhrase> CCompiledRuleTable::arrWords;

BOOL CSimpleCache<CKeyWordEntry>::GetKey(CDccDict* aDict, LPCSTR aKey, CKeyWordEntry* &aData)
	{
	CKeyWordEntry** pData = CMapSZ<CKeyWordEntry*>::FindPtr(aKey);
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

	intptr_t index = (intptr_t)*pData;
	if (index < CACHE_INDEX_LIMIT)
		{ // es un indice, aun no se han cargado los datos
		// crear el item y actualizar la cache
		*pData = aDict->GetDataAt((int32_t)(index-1));
		}

	// devolver los datos
	aData = *pData;
				
	return TRUE;
	}

CKeyWordEntry* CDccDict::GetDataAt(TInt aIdx)
  {
	CKeyWordEntry* keyWord = m_arrKeyWord[aIdx];
	if (keyWord != NULL)
		return keyWord;

	int start = m_arrKeyOff[aIdx];
	int size = m_arrKeyOff[aIdx+1] - start;
	//TRACE("GetDataAt - %d", size);
	CStringA sData;
	iFile->Seek(start);
	iFile->ReadBufferAsCString(sData, size);

	keyWord = CFastHeap::New<CKeyWordEntry>(sData);
	m_arrKeyWord[aIdx] = keyWord;
	return keyWord;
  }

BOOL CDccDict::Load(const CStringA& aFileName)
	{
	CStringA name(aFileName);
	name.Replace(".dcc", EXT_DCC);
	iFile = new CFile(name, CFile::modeRead);

	// leer el header
	TDictKeyWordOptHeader header;
	iFile->ReadBuffer(&header, sizeof(header));

	iCount = header.count;

	//TRACE("%s : %d %d", name.c_str(), header.count, header.numKeyWords);
	// leer las claves
	LPSTR pKeys = (LPSTR)CFastHeap::Get(header.keySize);
	
	iFile->ReadBuffer((LPVOID)pKeys, header.keySize);

	// leer las posiciones de cada keyword, hay una de mas para saber el tamaño de la ultima
	m_arrKeyOff.SetSize(header.numKeyWords + 1);
	iFile->Seek(header.offIndex, CFile::begin);
	iFile->ReadBuffer((LPVOID)m_arrKeyOff.GetBuffer(), (header.numKeyWords + 1)*sizeof(m_arrKeyOff[0]));

	// leer el array de los indices a keywords para cada clave
	CWordArray arrKeyWordIndex;
	arrKeyWordIndex.SetSize(iCount, FALSE);
	iFile->ReadBuffer((LPVOID)arrKeyWordIndex.GetBuffer(), iCount*sizeof(arrKeyWordIndex[0]));

	m_arrKeyWord.SetSize(header.numKeyWords);

	// guardar las claves en la cache
	{
	// COLLECT_INTERVAL("create_cache"); // es despreciable
	iCache.Reserve(iCount);
	for(int i=0; i<iCount; i++)
		{	
		// cada clave se guarda con su indice al array de keywords
		iCache.Add(pKeys, arrKeyWordIndex[i]);

		// avanzar a la proxima cadena
		for(pKeys++ ; *pKeys != 0; pKeys++);
		pKeys++;
		}
	}

	iIsOpen = TRUE;

	return iIsOpen;
	}

