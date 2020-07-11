
#include "Dict.h"
#include "DictLoad.h"
#include "UtilFun.h"


//***************************************************************************
//*                                CDictLoad                                *
//***************************************************************************

BOOL CDictLoad::Open(CFile* aFile, TInt aFlags)
	{
	iCount = 0;
	iFlags = aFlags;
	
	if (!iLoadFromStream)
		{
		if (!aFile->isOpen())
			{
	    iError = CAN_NOT_OPENFILE;
	    LOG_ERROR("Dictionary not found: %s", CS2SZ(iName));
	    //ASSERT(FALSE);
	    return FALSE;
			}
		}
	
	if (aFile == NULL || !aFile->isOpen())
		{
	  LOG_ERROR("CDict: can't open file");
    iError = CAN_NOT_OPENFILE;
    ASSERT(FALSE);
    return FALSE;
		}
	
	TInt nFileSize = aFile->GetLength();
	if (nFileSize < (TInt)sizeof(TDICC_HEADER))
		{
	  LOG_ERROR("CDict: header too small");
    iError = NO_CANT_READFILE;
    ASSERT(FALSE);
    return FALSE;
		}

	// Datos que contiene el header
  aFile->ReadBuffer(&iHeader, sizeof(TDICC_HEADER));
  
  if (!CheckHeader(nFileSize))
  	{
	  LOG_ERROR("CDict: Bad Header");
    ASSERT(FALSE);
    return FALSE;
  	}
	
  iNoHide = (GetHdrField(HDR_DIC_FLGS) & FLAG_NOHIDE) != 0;
  
  // abrir el indice
  
  // crearlo si no existe
  if (iIndex == NULL)
  	iIndex = new CDictIndex<CACHE_NO>();

	// leer todo el contenido del diccionario

 	aFile->ReadBufferAsCString(m_Buffer, iHeader.DatSize - sizeof(TDICC_HEADER));
	m_szBuffer = m_Buffer.c_str();
  
  if (iLoadFromStream)
  	{	// ir hasta el final del diccionario
  	//aFile->Seek(iHeader.DatSize - sizeof(TDICC_HEADER), CFile::current);

  	// cargar el indice que esta a continuacion
		iError = iIndex->Open(aFile, iLoadFromStream);
		if (iError != DICT_NO_ERROR)
		  LOG_ERROR("CDict: Can't open index (1)");
  	}
  else
  	{ // cargar el indice que esta como un fichero aparte con extension .idx
    CFile* indexStream = new CFile();

	  CStringA indexName(iName);
		int iDot = indexName.ReverseFind('.');
		if (iDot > 0)
			indexName = indexName.Left(iDot) + ".idx";
	  
		if (!indexStream->Open(indexName))
			{
		  LOG_ERROR("CDict: Index do not exist: %s", indexName.c_str());
			iError = CAN_NOT_OPENFILE_IDX;
	    ASSERT(FALSE);
	    return FALSE;
			}
		
		iError = iIndex->Open(indexStream, iLoadFromStream);
		if (iError != DICT_NO_ERROR)
		  LOG_ERROR("CDict: Can't open index: %s", CS2SZ(indexName));
		
		indexStream->Close();
		delete indexStream;
  	}

  iIsOpen = (iError == DICT_NO_ERROR);
	if (iIsOpen)
		{
		iCount  = (iIndex) ? iIndex->GetNumRec() : iHeader.NumRec;

		//TRACE("DictCount = %d", iCount);
		// llenar la tabla hash
		if (iCount > 0)
			{
			iHash.Reserve(iCount);
			for(int i=0; i<iCount; i++)
				{
				LPCSTR szKey, szData;
				GetAt(i, szKey, szData);
				//TRACE("  %s = %s", szKey, szData);
				iHash.Add(szKey, szData);
				}

			//TRACE("ReDump");
			//Dump();
			}
		}

  if (iIsOpen)
  	{
  	/*if (iName.Length() == 0)
  		TRACE1("SubDictionary loaded: %s", CS2SZ(iMasterFileName))
  	else
  		TRACE1("Dictionary loaded: %s", CS2SZ(iName));*/
  	}
  else
    ASSERT(FALSE);

	return iIsOpen;
	}

void CDictLoad::Serialize(CArchive &ar, LPCSTR aDumpTxtFile)
  {
  if( ar.IsLoading() )                      // Guardar diccionario
    {
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

BOOL CDictLoad::OpenFromFile(CArchive &ar)
	{
	ar.UpdateStreamPosition();
	CFile* file = ar.GetFile();     					// Obtiene manipualdor del fichero
	iMasterFileName = file->iFileName;
	//iBaseOffset = file->GetPosition();
	iLoadFromStream = TRUE;
	
	BOOL res = Open(file);
	if (res)
		ar.UpdatePosition();
	
	return res;
	}

//--------------------------------------------------------------------------
// Esta función chequea si el argumento 'Hdr' apunta a un encabezamiento de
// un fichero diccionario, en ese caso retorna TRUE, en caso contrario retorna
// FALSE y activa el error correspondiente.
//--------------------------------------------------------------------------

BOOL CDictLoad::CheckHeader(TUint aFileSize)
  {
  // Verifica que el formato es correcto
  if (iHeader.ID[0] != 'D' || iHeader.ID[1] != 'I' || iHeader.ID[2] != 'C' || 
  		iHeader.ID[3] != 'C' || iHeader.ID[4] != 0 ||
  		iHeader.DatSize < sizeof(TDICC_HEADER) ||  
      iHeader.MemSize < iHeader.DatSize) 
    {
    iError = BAD_DICC_TYPE;
    return FALSE;
    }

  // Verifica que la versión sea la adecuada
  if( iHeader.Ver != 1000 || iHeader.Size != sizeof(TDICC_HEADER) ) 
    {
    iError = BAD_DICC_VERSION;
    return FALSE;
    }

  // Verifica que el fichero contiene todos los datos
  if( aFileSize != 0 && aFileSize < iHeader.DatSize )
    {
    iError = BAD_DICC_TYPE;
    return FALSE;
    }
  
  return TRUE;
  }

void CDictLoad::Close()
	{
	if (!iIsOpen)
		return;

	if (iIndex)
		{
		iIndex->Close();
		delete iIndex;
		iIndex = NULL;
		}
	
	if (iIsOpen)
		{
		/*if (iName.Length() == 0)
			TRACE1("SubDictionary unloaded: %s", CS2SZ(iMasterFileName))
		else
			TRACE1("Dictionary unloaded: %s", CS2SZ(iName));*/
		}
	
	iIsOpen = FALSE;
	}

TUint32 CDictLoad::GetHdrField(TInt aIdxField) const
	{
  switch(aIdxField)
    {
    //case HDR_DIC_ID  : lstrcpy( sValor, m_Header->ID ); break;
    case HDR_DIC_VER : return iHeader.Ver;
    //case HDR_SIZE    : _itoa_s( m_Header->Size, sValor, 30, 10 ); break;
    case HDR_SRC_ID  : return iHeader.Src;
    case HDR_DEST_ID : return iHeader.Dest;
    case HDR_DIC_FLGS: return iHeader.Flgs;
    case HDR_MAX_REC : return iHeader.MaxSize;
    case HDR_DAT_SIZE: return iHeader.DatSize;
    case HDR_MEM_SIZE: return iHeader.MemSize;
    case HDR_MUM_REC : return iHeader.NumRec;
    case HDR_DEL_REC : return iHeader.DelRec;
    case HDR_MOD_KEY1: return iHeader.IdxKey1;
    case HDR_MOD_KEY2: return iHeader.IdxKey2;
    //case HDR_MODE    : _itoa_s( m_Mode, sValor, 30, 10 ); break;

    default:
      return 0;
    }
	}

CStringA CDictLoad::GetHdrFieldS(TInt aIdxField) const
	{
	CStringA res;
	
  switch(aIdxField)
    {
    case HDR_OBJ_ID  : res = iHeader.IdObj; break;
    case HDR_ESP_ID  : res.Append((LPCSTR)&iHeader.Reserv1, 2); break;
    case HDR_TYPE_ID : res.Append((LPCSTR)&iHeader.Reserv2, 2); break;
    }
  
  return res;
	}

#ifdef ACTIVE_SAVE

// Salva el diccionario con el formato viejo
BOOL CDictLoad::Save(const CStringA& sFileName)
	{
	CFile file(sFileName, CFile::modeWrite);
	CArchive ar(&file, CArchive::store);

	TRACE("Saving Dict: %s", sFileName.GetFileName().c_str());

	CStringA sIndexName(sFileName);
	StrPutFileExt(sIndexName, ".idx");

	CStringA txtDumpFile(sFileName);
	StrPutFileExt(txtDumpFile, ".txt");
	LPCSTR szDumpFile = txtDumpFile.GetFileName().c_str();

	BOOL res = Save_(ar, sIndexName.c_str(), szDumpFile);

	return res;
	}

BOOL CDictLoad::Save(CArchive& ar, LPCSTR aIndexName, LPCSTR aDumpTxtFile)
	{
	//TRACE("Adding Dict to: %s", ar.m_strFileName.GetFileName().c_str());
	TRACE("Saving %s into %s", iDebugName.c_str(), ar.m_strFileName.GetFileName().c_str());
	return Save_(ar, aIndexName, aDumpTxtFile);
	}

BOOL CDictLoad::Save_(CArchive& ar, LPCSTR aIndexName, LPCSTR aDumpTxtFile)
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
		//CStringA sKey, sData;
		LPCSTR sKey, sData;
		GetAt(i, sKey, sData);

		if (aDumpTxtFile != NULL)
			{
			CStringA sTxt;
			sTxt.Format("%s: %s\n", sKey, sData);
			txtFile.WriteStringSZ(sTxt.c_str(), FALSE);
			}

		HideData(sData);

		// sData tiene un 0x0 como parte de la cadena, seguida del null terminator
		WORD recordLen = sizeof(WORD) + strlen(sKey) + 1 + strlen(sData) + 1;
		keysLen += strlen(sKey) + 1;
		arrIndexs[i] = file->GetPosition() - offStart;
		ar << recordLen;
		ar << sKey;
		ar.WriteBuffer((LPVOID)sData, strlen(sData) + 1);

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
