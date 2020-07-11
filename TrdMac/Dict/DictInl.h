


//***************************************************************************
//*                                CDict                                    *
//***************************************************************************

template<class CACHE>
BOOL CDictT<CACHE>::Open(const CStringF& aName)
	{
	//DUMP_INTERVALS("CDict::Open");
	mCount = 0;
	mName = aName;
	
	if (!mLoadFromStream)
		{
		if (mFile == NULL)
			mFile = new CFile(aName, CFile::modeRead);

		if (!mFile->isOpen())
			{
	    mError = CAN_NOT_OPENFILE;
	    LOG_ERROR("Dictionary not found: %s", CS2SZ(mName));
	    //ASSERT(FALSE);
	    return FALSE;
			}
		}
	
	if (mFile == NULL || !mFile->isOpen())
		{
	  LOG_ERROR("CDict: can't open file");
    mError = CAN_NOT_OPENFILE;
    ASSERT(FALSE);
    return FALSE;
		}
	
	TInt nFileSize = mFile->GetLength();
	if (nFileSize < (TInt)sizeof(TDICC_HEADER))
		{
	  LOG_ERROR("CDict: header too small");
    mError = NO_CANT_READFILE;
    ASSERT(FALSE);
    return FALSE;
		}

	// Datos que contiene el header
  mFile->ReadBuffer(&mHeader, sizeof(TDICC_HEADER));
  
  if (!CheckHeader(nFileSize))
  	{
	  LOG_ERROR("CDict: Bad Header");
    ASSERT(FALSE);
    return FALSE;
  	}
	
  mHideData = (GetHdrField(HDR_DIC_FLGS) & FLAG_NOHIDE) == 0;
  
  // abrir el indice
  
  // crearlo si no existe
  if (mIndex == NULL)
  	mIndex = new CDictIndex<CACHE_YES>();
  
  if (mLoadFromStream)
  	{	// ir hasta el final del diccionario
  	mFile->Seek(mHeader.DatSize - sizeof(TDICC_HEADER), CFile::current);

  	// cargar el indice que esta a continuacion
		mError = mIndex->Open(mFile, mLoadFromStream);
		if (mError != DICT_NO_ERROR)
		  LOG_ERROR("CDict: Can't open index (1)");
  	}
  else
  	{ // cargar el indice que esta como un fichero aparte con extension .idx
    CFile* indexStream = new CFile();

	  CStringA indexName(aName);
		int iDot = indexName.ReverseFind('.');
		if (iDot > 0)
			indexName = indexName.Left(iDot) + ".idx";
	  
		if (!indexStream->Open(indexName))
			{
		  LOG_ERROR("CDict: Index do not exist: %s", indexName.c_str());
			mError = CAN_NOT_OPENFILE_IDX;
	    ASSERT(FALSE);
	    return FALSE;
			}
		
		mError = mIndex->Open(indexStream, mLoadFromStream);
		if (mError != DICT_NO_ERROR)
		  LOG_ERROR("CDict: Can't open index: %s", CS2SZ(indexName));
		
		indexStream->Close();
		delete indexStream;
  	}

  mIsOpen = (mError == DICT_NO_ERROR);
	if (mIsOpen)
		{
		mCount  = mIndex->GetNumRec();// mHeader.NumRec;
		}
  
  if (mIsOpen)
  	{
		mCache.AfterLoad();

#ifdef ACTIVE_SAVE
		if (!mName.IsEmpty())
			{
			int p = mName.ReverseFind('\\');
			if (p != -1)
				TRACE("%s", mName.c_str() + p + 1)
			else
				TRACE("%s", mName.c_str());
			}
		//TRACE("count=%d", GetCount());
#endif

  	/*if (mName.Length() == 0)
  		TRACE1("SubDictionary loaded: %s", CS2SZ(mMasterFileName))
  	else
  		TRACE1("Dictionary loaded: %s", CS2SZ(mName));*/
  	}
  else
    ASSERT(FALSE);

	return mIsOpen;
	}

template<class CACHE>
void CDictT<CACHE>::Serialize(CArchive &ar, LPCSTR
#ifdef ACTIVE_SAVE
		aDumpTxtFile
#endif
		)
  {
  if( ar.IsLoading() )                      // Guardar diccionario
    {
#ifdef ACTIVE_SAVE
		if (aDumpTxtFile)
			TRACE("%s", aDumpTxtFile);
#endif
    Close();                                 // Libera todos los diccionarios
    OpenFromFile(ar);                  // Carga uno desde la posición del fichero
    }
	else
		{
#ifdef ACTIVE_SAVE
		Save(ar, NULL, aDumpTxtFile);
#endif
		}
  }

template<class CACHE>
BOOL CDictT<CACHE>::OpenFromFile(CArchive &ar)
	{
	ar.UpdateStreamPosition();
	mFile = ar.GetFile();     					// Obtiene manipualdor del fichero
	mMasterFileName = mFile->iFileName;
	mBaseOffset = mFile->GetPosition();
	mLoadFromStream = TRUE;
	
	BOOL res = Open();
	if (res)
		{
		ar.UpdatePosition();
		}
	
	return res;
	}

//--------------------------------------------------------------------------
// Esta función chequea si el argumento 'Hdr' apunta a un encabezamiento de
// un fichero diccionario, en ese caso retorna TRUE, en caso contrario retorna
// FALSE y activa el error correspondiente.
//--------------------------------------------------------------------------

template<class CACHE>
BOOL CDictT<CACHE>::CheckHeader(TUint aFileSize)
  {
  // Verifica que el formato es correcto
  if (mHeader.ID[0] != 'D' || mHeader.ID[1] != 'I' || mHeader.ID[2] != 'C' || 
  		mHeader.ID[3] != 'C' || mHeader.ID[4] != 0 ||
  		mHeader.DatSize < sizeof(TDICC_HEADER) ||  
      mHeader.MemSize < mHeader.DatSize) 
    {
    mError = BAD_DICC_TYPE;
    return FALSE;
    }

  // Verifica que la versión sea la adecuada
  if( mHeader.Ver != 1000 || mHeader.Size != sizeof(TDICC_HEADER) ) 
    {
    mError = BAD_DICC_VERSION;
    return FALSE;
    }

  // Verifica que el fichero contiene todos los datos
  if( aFileSize != 0 && aFileSize < mHeader.DatSize )
    {
    mError = BAD_DICC_TYPE;
    return FALSE;
    }
  
  return TRUE;
  }

template<class CACHE>
void CDictT<CACHE>::Close()
	{
	if (!mIsOpen)
		return;

	if (mIndex)
		{
		mIndex->Close();
		delete mIndex;
		mIndex = NULL;
		}
	
	if (mFile && mLoadFromStream == FALSE)
		{ // si se abre de un stream, el stream no nos pertenece y no podemos cerrarlo
		mFile->Close();
		delete mFile;
		mFile = NULL;
		}
	
	ClearCache(FALSE);
		
	if (mIsOpen)
		{
		/*if (mName.Length() == 0)
			TRACE1("SubDictionary unloaded: %s", CS2SZ(mMasterFileName))
		else
			TRACE1("Dictionary unloaded: %s", CS2SZ(mName));*/
		}
	
	mIsOpen = FALSE;
	}

template<class CACHE>
TUint32 CDictT<CACHE>::GetHdrField(TInt aIdxField) const
	{
  switch(aIdxField)
    {
    //case HDR_DIC_ID  : lstrcpy( sValor, m_Header->ID ); break;
    case HDR_DIC_VER : return mHeader.Ver;
    //case HDR_SIZE    : _itoa_s( m_Header->Size, sValor, 30, 10 ); break;
    case HDR_SRC_ID  : return mHeader.Src;
    case HDR_DEST_ID : return mHeader.Dest;
    case HDR_DIC_FLGS: return mHeader.Flgs;
    case HDR_MAX_REC : return mHeader.MaxSize;
    case HDR_DAT_SIZE: return mHeader.DatSize;
    case HDR_MEM_SIZE: return mHeader.MemSize;
    case HDR_MUM_REC : return mHeader.NumRec;
    case HDR_DEL_REC : return mHeader.DelRec;
    case HDR_MOD_KEY1: return mHeader.IdxKey1;
    case HDR_MOD_KEY2: return mHeader.IdxKey2;
    //case HDR_MODE    : _itoa_s( m_Mode, sValor, 30, 10 ); break;

    default:
      return 0;
    }
	}

template<class CACHE>
CStringA CDictT<CACHE>::GetHdrFieldS(TInt aIdxField) const
	{
	CStringA res;
	
  switch(aIdxField)
    {
    case HDR_OBJ_ID  : res = mHeader.IdObj; break;
    case HDR_ESP_ID  : res.Append((LPCSTR)&mHeader.Reserv1, 2); break;
    case HDR_TYPE_ID : res.Append((LPCSTR)&mHeader.Reserv2, 2); break;
    }
  
  return res;
	}

//---------------------------------------------------------------------------------------
// Esta función es solo para compativilidad. se usa solo para diccionarios de sufijos
//---------------------------------------------------------------------------------------
template<class CACHE>
BOOL CDictT<CACHE>::FindSuffix(const TDictKey& aKey, TDictData& aData)
  {
	BOOL ret = FALSE;

	//for(LPCSTR szKey = sKey.c_str(); *szKey != 0 && ret==FALSE; ++szKey)
  //	ret = GetKey(szKey, aData);
  for( int i=0; i<(int)aKey.Length() && ret==FALSE; ++i )
  	ret = GetKey(aKey.Mid(i), aData);

	return ret;

  }

//---------------------------------------------------------------------------------------
// Esta función es solo para compativilidad. se usa solo para diccionarios de prefijos
//---------------------------------------------------------------------------------------
template<class CACHE>
BOOL CDictT<CACHE>::FindPreffix(const TDictKey& s, TDictData& data)
  {
  ASSERT( mIsOpen );                           // Existe diccionario principal

  CString tmp(s);

  BOOL ret = FALSE;

  for( int i=s.GetLength(); i>0 && ret==FALSE; --i )
    {
    tmp.Truncate(i);

    ret = GetKey( tmp, data );
    }

  return ret;
  }

//--------------------------------------------------------------------------
// Esta función según el ultimo record referenciado, obtiene el contenido de
// la llave y/o el contenido de los datos, esto valores los coloca en los
// buffers 'Name' y 'Datos' repectivamente.
// Si se quiere ignorar alguno de estos datos se puede poner la longitud
// correspondiente 'Len1' o 'Len2' igual a 0, solo en ese caso se puede
// poner el puntero al buffer igual a NULL.
// Si el tamaño del buffer no es suficiente el dato correspondiente será
// truncado y se colocará en la varible global _Error_ la constante
// TRUNCATE_KEY o TRUNCATE_DATA para si se quiere consultarla.
// Si la ultima referencia es incorrecta los resultados de esta función son
// impredesibles
//--------------------------------------------------------------------------

inline void CDictBase::GetRef(int aIdx, TDictKey* aKey, TDictData* aData)
  {
  //COLLECT_INTERVAL("CDict::GetRef");
  
  SetStreamOffset(GetOffset(aIdx));
  
  TInt recordLen = mFile->ReadInt16(); 			// Longitud del record
  TInt keyLen = 0;

  if (aKey)
  	{
		mFile->ReadAsSZString(*aKey);
  	keyLen = aKey->Length() + 1;
  	}
  else
  	{
  	TDictKey key;
		mFile->ReadAsSZString(key);
  	keyLen = key.Length() + 1;
  	}
  
  int dataLen = recordLen - sizeof(TInt16) - keyLen;	// Actualiza longitud del ultimo dato
  if (aData && dataLen > 0)
  	{
  	GetStreamData(*aData, dataLen);
  	HideData(*aData);
  	}
  }

#ifdef ACTIVE_SAVE

// Salva el diccionario con el formato viejo
template<class CACHE>
BOOL CDictT<CACHE>::Save(const CStringA& sFileName)
	{
	CFile file(sFileName, CFile::modeWrite);
	CArchive ar(&file, CArchive::store);

	TRACE("Saving Dict: %s", sFileName.GetFileName().c_str());

	CStringA sIndexName(sFileName);
	StrPutFileExt(sIndexName, ".idx");

	CStringA txtDumpFile(sFileName);
	StrPutFileExt(txtDumpFile, ".txt");
	LPCSTR szDumpFile = txtDumpFile.GetFileName.c_str();

	BOOL res = Save_(ar, sIndexName.c_str(), szDumpFile);

	return res;
	}

template<class CACHE>
BOOL CDictT<CACHE>::Save(CArchive& ar, LPCSTR aIndexName, LPCSTR aDumpTxtFile)
	{
	//TRACE("Adding Dict to: %s", ar.m_strFileName.GetFileName().c_str());
	TRACE("Saving %s into %s", iDebugName.c_str(), ar.m_strFileName.GetFileName().c_str());
	return Save_(ar, aIndexName, aDumpTxtFile);
	}

template<class CACHE>
BOOL CDictT<CACHE>::Save_(CArchive& ar, LPCSTR aIndexName, LPCSTR aDumpTxtFile)
	{
	BOOL res = TRUE;
	CFile* file = ar.GetFile();
	int offStart = file->GetPosition();

	ar.WriteBuffer(&iHeader, sizeof(iHeader));

	CFile txtFile;
	
	aDumpTxtFile = NULL; // desactivar la creacion de los txt
	if (aDumpTxtFile != NULL)
		txtFile.Open(StrGetDir(file->iFileName) + aDumpTxtFile, CFile::modeWrite);

	int datSize = 0; // tamaño de los datos
	int keysLen = 0;
	int* arrIndexs = new int[iCount];
	for(int i=0; i<iCount; i++)
		{
		CStringA sKey, sData;
		GetAt(i, &sKey, &sData);

		if (!CheckKey(sKey.c_str()))
			{
			CStringA s(sKey);
			s.Replace('\x0A', ' ');
			s.Replace('\x0D', ' ');
			TRACE("Bad key: %s", s.c_str());
			}

		if (aDumpTxtFile != NULL)
			{
			CStringA sTxt;
			sTxt.Format("%s: %s", sKey.c_str(), sData.c_str());
			sTxt.Replace('\x0A', ' ');
			sTxt.Replace('\x0D', ' ');
			sTxt += '\n';
			txtFile.WriteStringSZ(sTxt.c_str(), FALSE);
			}

		HideData(sData);

		// sData tiene un 0x0 como parte de la cadena, seguida del null terminator
		WORD recordLen = sizeof(WORD) + sKey.GetLength() + 1 + sData.GetLength();
		keysLen += sKey.GetLength() + 1;
		arrIndexs[i] = file->GetPosition() - offStart;
		ar << recordLen;
		ar << sKey.c_str();
		ar.WriteBuffer((LPVOID)sData.c_str(), sData.GetLength());

		datSize += recordLen;
		}

	// arreglar el header (debido a los possibles registros borrados que fueron eliminados)
	iHeader.DelRec = 0;
	iHeader.DatSize = sizeof(iHeader) + datSize;
	iHeader.MemSize = iHeader.DatSize;

	file->Seek(offStart);
	ar.WriteBuffer(&iHeader, sizeof(iHeader));

	file->Seek(0, CFile::end);

	//TRACE("keysLen = %d", keysLen);
	if (aIndexName != NULL)
		res = iIndex->Save(aIndexName, arrIndexs);
	else
		res = iIndex->Save(ar, arrIndexs);

	delete[] arrIndexs;
	return res;
	}

#endif

struct TDictKeyWordOptHeader
	{
	int count;  			// cantidad de claves
	int numKeyWords;  // cantidad de keywords unicos
	int keySize;			// tamaño total de todas las claves
	int offIndex;     // offset donde empieza el indice
	};

#ifdef ACTIVE_SAVE

template<class CACHE>
BOOL CDictT<CACHE>::SaveDCX(CArray<CActionPhrase>& arrWords)
	{
	CStringA name(iName);
	name.Replace(".dcc", EXT_DCC);
	CFile file(name, CFile::modeWrite);

	TRACE("Saving DCX: %s", name.c_str());
	//TRACE("%s : %d %d", name.c_str(), header.count, header.numKeyWords);

	CArchive ar(&file, CArchive::store);

	int offStart = file.GetPosition();

	// escribir el header (despues tendra que ser reescrito)
	TDictKeyWordOptHeader header;
	header.count = GetCount();
	file.WriteBuffer(&header, sizeof(header));

	// escribir las claves
	for(int i=0; i<GetCount(); i++)
		{
		CStringA key;
		GetKeyAt(i, key);

		ar << key.c_str();
		}

	header.keySize = file.GetPosition() - sizeof(header);

	// escribir los datos y guadar el offset de cada uno, este offset se usa tambien
	// para saber el tamaño de cada dato por lo cual hay uno extra para saber el
	// tamaño del ultimo dato
	CWordArray arrKeyIndexs; // indice del KeyWord asociado a una clave segun su indice
	CArray<CKeyWordEntry*> arrKeyWord;

	CIntArray arrKeyOff;
	for(int i=0; i<GetCount(); i++)
		{
		CStringA sData;
		GetDataAt(i, sData);

		CKeyWordEntry* keyWord = new CKeyWordEntry(sData);

		keyWord->RemoveDuplicates();

		CStringA key;
		GetKeyAt(i, key);

		int idxYaExiste = -1;

		for(int j=0; j<arrKeyWord.GetCount(); j++)
			{
			if (keyWord->IsEqual(*arrKeyWord[j]))
				{
				idxYaExiste = j;
				break;
				}
			}

		if (idxYaExiste == -1)
			{
			//TRACE("------------------------");
			//TRACE("key = %s", key.c_str());
			idxYaExiste = arrKeyWord.Add(keyWord);

			// guardar el offset de cada keyword
			int posa = file.GetPosition();
			arrKeyOff.Add(posa);

			// escribir el keyword al fichero
			keyWord->Save(ar, arrWords);
			}
		//else
		//	TRACE("key = %s", key.c_str());

		// guardar indice de cada clave
		arrKeyIndexs.Add(idxYaExiste);//m_arrKeyOff.GetCount() - 1);

		//file.WriteBuffer((LPVOID)data.c_str(), data.GetLength() + 1);
		//sum += data.GetLength() + 1;
		}

	// escribir todos los KeyWords y sus datos en el log
	/*for(int i=0; i<arrKeyWord.GetCount(); i++)
		{
		TRACE("--------------");
		for(int j=0; j<arrKeyIndexs.GetCount(); j++)
			{
			if (arrKeyIndexs[j] == i)
				{
				CStringA key;
				GetKeyAt(j, key);
				TRACE(key.c_str());
				}
			}

		arrKeyWord[i]->Dump();
		}*/

	// cantidad de keywords
	header.numKeyWords = arrKeyOff.GetCount();

	// guardar el final del ultimo keyword y el inicio de los indices
	int pos = file.GetPosition();
	arrKeyOff.Add(pos);
	header.offIndex = pos;

	// escribir el array de las posiciones de cada dato
	for(int i=0; i<arrKeyOff.GetCount(); i++)
		{
		ar << arrKeyOff[i];
		}

	// escribir el array de los indices de cada clave en el array de las posiciones de los datos
	for(int i=0; i<arrKeyIndexs.GetCount(); i++)
		{
		ar << arrKeyIndexs[i];
		}

	// reescribir el header
	file.Seek(offStart, CFile::begin);

	file.WriteBuffer(&header, sizeof(header));

	file.Close();

	return TRUE;
	}

#endif
//***************************************************************************
//*                                CSimpleDict                              *
//***************************************************************************

struct TDictOptHeader
	{
	int count;  			// cantidad de claves
	int keySize;			// tamaño total de todas las claves
	int offIndex;     // offset donde empieza el indice
	};

/*template<class DATA>
BOOL CSimpleDict<DATA>::LoadOpt(const CStringA& aFileName)
	{
	//COLLECT_INTERVAL("LoadOpt");
	
	CStringA name(aFileName);
	name.Replace(".dcc", EXT_DCC);
	CDictT<DATA,CSimpleCache<DATA>>::iFile = new CFile(name, CFile::modeRead);

	int offStart = CDictT<DATA,CSimpleCache<DATA>>::iFile->GetPosition();

	// leer el header
	TDictOptHeader header;
	CDictT<DATA,CSimpleCache<DATA>>::iFile->ReadBuffer(&header, sizeof(header));

	CDictT<DATA,CSimpleCache<DATA>>::iCount = header.count;

	// leer las claves
	int sum = 0;
	CStringA sKeys(header.keySize);

	{
	//COLLECT_INTERVAL("read_comodin");
	CDictT<DATA,CSimpleCache<DATA>>::iFile->ReadBuffer((LPVOID)sKeys.c_str(), header.keySize);

	// leer las posiciones de cada dato
	iArrDataOff = new int[CDictT<DATA,CSimpleCache<DATA>>::iCount + 1];
	CDictT<DATA,CSimpleCache<DATA>>::iFile->Seek(header.offIndex, CFile::begin);
	CDictT<DATA,CSimpleCache<DATA>>::iFile->ReadBuffer((LPVOID)iArrDataOff, (CDictT<DATA,CSimpleCache<DATA>>::iCount + 1)*sizeof(iArrDataOff[0]));
  }

	// guardar las claves en la cache
	{
	COLLECT_INTERVAL("create_cache"); // cuesta mas este que leer las claves
	CDictT<DATA,CSimpleCache<DATA>>::iCache.Reserve(CDictT<DATA,CSimpleCache<DATA>>::iCount);
	LPCSTR pKeys = sKeys.c_str();
	for(int i=0; i<CDictT<DATA,CSimpleCache<DATA>>::iCount; i++)
		{	
		CDictT<DATA,CSimpleCache<DATA>>::iCache.Add(pKeys, i);
		for(pKeys++ ; *pKeys != 0; pKeys++);
		pKeys++;
		//int len = strlen(pKeys);
		//pKeys += len + 1;
		}
	}

	iIsOpen = TRUE;

	return iIsOpen;
	}*/

#ifdef ACTIVE_SAVE

/*template<class CACHE>
BOOL CDictT<CACHE>::SaveOpt(const CStringA& aFileName)
	{
	CStringA name(aFileName);
	name.Replace(".dic",".dix");
	CFile file(name, CFile::modeWrite);

	CArchive ar(&file, 0);

	return SaveOpt(ar);
	}*/
	
template<class CACHE>
BOOL CDictT<CACHE>::SaveOpt(CArchive& ar)
	{
	CFile* file = ar.GetFile();

	int offStart = file->GetPosition();

	// escribir el header (despues tendra que ser reescrito)
	TDictOptHeader header;
	header.count = GetCount();
	file->WriteBuffer(&header, sizeof(header));

	int offKeys = file->GetPosition();

	// escribir las claves
	for(int i=0; i<GetCount(); i++)
		{
		CStringA key;
		GetKeyAt(i, key);

		ar << key.c_str();
		}

	header.keySize = file->GetPosition() - offKeys;

	// escribir los datos y guadar el offset de cada uno, este offset se usa tambien
	// para saber el tamaño de cada dato por lo cual hay uno extra para saber el
	// tamaño del ultimo dato
	int* arrDataOff = new int[GetCount() + 1];
	for(int i=0; i<GetCount(); i++)
		{
		CStringA sData;
		GetDataAt(i, sData);

		int posa = file->GetPosition();
		arrDataOff[i] = posa;

		//file.WriteBuffer((LPVOID)data.c_str(), data.GetLength() + 1);
		file->WriteCString(sData);
		//DATA* data = new DATA(sData);
		//data->SaveOpt(ar);
		//delete data;

		int posb = file->GetPosition();
		}
		
	header.offIndex = file->GetPosition();
	arrDataOff[GetCount()] = header.offIndex;

	// escribir las posiciones de cada dato
	for(int i=0; i<GetCount()+1; i++)
		{
		ar << arrDataOff[i];
		}

	delete[] arrDataOff;
	arrDataOff = NULL;

	// reescribir el header
	file->Seek(offStart, CFile::begin);

	file->WriteBuffer(&header, sizeof(header));

	return TRUE;
	}
	
#endif	

