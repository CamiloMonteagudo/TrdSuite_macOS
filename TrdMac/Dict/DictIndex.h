/*
 ============================================================================
 Name		: DictIndex.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CDictIndex declaration
 ============================================================================
 */

#ifndef DICTINDEX_H
#define DICTINDEX_H

// INCLUDES
#include "DictTypes.h"
#include "DictError.h"


#define DICC_FILE				1
#define DICC_READONLY		2
#define DICC_MEM        4
#define DICC_LOADKEYS   32
#define DICC_CACHEKEYS  64

#define USERDICT_MASK   0x80000000

// Header del indice
typedef struct TagIDX_HDR
  {
  char  	ID[6];       // Identificador de los datos
  TUint32 Ver;         // Versión de la implementación de los índices
  TUint32 Size;        // Tamaño de la estructura que mantiene los datos                  
  TUint32 Flgs;        // Banderas que definen los atributos
  TUint32 MaxSize;     // Tamaño máximo del fichero mapeado
  TUint32 DatSize;     // Tamaño de los datos
  TUint32 MemSize;     // Tamaño de la memoria reservada
  TUint32 NumRec;      // Número de record totales                        
  TUint32 IdxKey1;     // Clave 1 para la sincronización                  
  TUint32 IdxKey2;     // Clave 2 para la sincronización
  char  	IdObj[20];   // Nombre del objeto (Para identificar si esta cargado)
  TUint32 sIdx[256];   // SubIndice donde comienza cada letra          
  TUint32 Reserv1;     // Reservado para uso futuro
  TUint32 Reserv2;     // Reservado para uso futuro
  } TIDX_HDR;
  

// CLASS DECLARATION

/**
 *  CDictIndex
 * 
 */
typedef enum {CACHE_NO = 0, CACHE_YES = 1 } TUseCache;

// CDictIndex siempre se usa con CACHE_YES, excepto en los CDictLoad, las funciones de modificacion
// solo se pueden usar con CACHE_YES
template<TUseCache CACHE_KEYS>
class CDictIndex
	{
private:
	TIDX_HDR mHeader;
	CUIntArray mIndex;
	CArrayValue<LPCSTR> mKeys;		// array de las claves para cada indice, funciona como una cache, 
    // se llena en demanda y solo se usa si CACHE_KEYS == CACHE_YES, las claves se guardan como 
    // LPCSTRs que se asume vienen del FastHeap o de un CStringA (en el caso que venga de un UserDict)
    // y que ambos casos viven el mismo o mas tiempo que el DictIndex

public:
	// Constructors and destructor

	CDictIndex():mIndex()
		{
		}

	~CDictIndex()
		{
		}

	TUint Open(CFile* aIndexFile, BOOL aLoadFromStream)
		{
		if (aIndexFile == NULL || !aIndexFile->isOpen())
			{
			return CAN_NOT_OPENFILE_IDX;
			}
	
		TInt nFileSize = aIndexFile->GetLength();
		if (nFileSize < (TInt)sizeof(TIDX_HDR))
			{
			TRACE("BAD_OPENFILE_IDX_1");
			return BAD_OPENFILE_IDX;
			}

		// Datos que contiene el header
		aIndexFile->ReadBuffer(&mHeader, sizeof(TIDX_HDR));
	
		TUint result = CheckHeader(nFileSize);
		if (result)
			return result;

		if (aLoadFromStream)
			{
			TInt pos = aIndexFile->GetPosition();
			if (pos + (TInt)mHeader.DatSize - (TInt)sizeof(mHeader) > nFileSize)
				{
				TRACE("BAD_OPENFILE_IDX_2");
				return BAD_OPENFILE_IDX;
				}		
			}
		else if ((TInt)mHeader.DatSize != nFileSize)
			{
			TRACE("BAD_OPENFILE_IDX_3");
			return BAD_OPENFILE_IDX;
			}
	
		// Leer todo el indice a memoria
		TUint32 indexSize = mHeader.DatSize - sizeof(TIDX_HDR);
		if (indexSize > 0)
			{
			//COLLECT_INTERVAL("READ_INDEX"); //0.27 al desconectar unidad
			TInt numItems = indexSize/4;
			mIndex.SetSize(numItems, FALSE);
			aIndexFile->ReadBuffer(mIndex.GetBuffer(), indexSize);

			if (CACHE_KEYS == CACHE_YES)
				mKeys.SetSize(numItems, TRUE);
			}

		return result;
		}

	void Close()
		{
		mKeys.Close();
		mIndex.Close();
		}

  void ClearCache()
    {
    if (CACHE_KEYS == CACHE_YES)
      {
      for(int i=0; i<mKeys.GetCount(); ++i)
        mKeys[i] = NULL;
      }
    }
    
	DWORD GetOffset(TInt aIdx)
		{
		ASSERT(aIdx >= 0 && aIdx < (TInt)mHeader.NumRec);
		
		return mIndex[aIdx];
		}
    
  inline void SetOffset(int aIdx, TUint32 aOffset)
    {
		ASSERT(aIdx >= 0 && aIdx < (TInt)mHeader.NumRec);
    mIndex[aIdx] = aOffset;
    }

  inline TInt GetNumRec()       // Obtiene el número de records
    { 
    return mHeader.NumRec;
    }

	// inserta una clave y su offset en el indice
	void InsertAt(int aIndex, const CStringA& aKey, DWORD aOffset)
		{
		mHeader.NumRec++;
		mIndex.InsertAt(aIndex, aOffset);
		mKeys.InsertAt(aIndex, aKey.c_str());
		}

	// borra una clave del indice
	void DeleteAt(int aIndex, int aUserDictIndex)
    {
    mHeader.NumRec--;
    mIndex.RemoveAt(aIndex);
    mKeys.RemoveAt(aIndex);
    
    // ajustar los indices de las palabras del dict de usuario
    OnUserKeyDeleted(aUserDictIndex);
    }
    
  // hacer los ajustes necesarios en el indice despues de borrar una palabra de usuario
  void OnUserKeyDeleted(int aUserDictIndex)
    {
    for(int i=0; i<mKeys.GetCount(); ++i)
      {
      if (mIndex[i] & USERDICT_MASK)
        {
        int userIndex = mIndex[i] & ~USERDICT_MASK;
        if (userIndex > aUserDictIndex)
          {
          mIndex[i] = USERDICT_MASK | (userIndex-1);
          }
        }
      }
    }

#ifdef ACTIVE_SAVE

	BOOL Save(const CStringA& sFileName, int* arrIndexs)
		{
		CFile file(sFileName, CFile::modeWrite);
		CArchive ar(&file, CArchive::store);

		return Save(ar, arrIndexs);
		}

	BOOL Save(CArchive& ar, int* arrIndexs)
		{
		ar.WriteBuffer(&iHeader, sizeof(iHeader));

		ar.WriteBuffer(arrIndexs, mHeader.NumRec*sizeof(arrIndexs[0]));

		return TRUE;
		}

#endif

	// retorna la llave de indice aIndex, primero la busca en la cache y si no esta la
	// busca en el diccionario, asume que la clave esta guardada en el fastheap
	template<class DICT>
	inline LPCSTR GetFastKeyAt(DICT* aDict, int aIndex)
		{
		ASSERT(CFastHeap::isEnabled());

		if (CACHE_KEYS == CACHE_NO)
			return aDict->GetFastKeyAtOffset(mIndex[aIndex]);
		else
			{
			LPCSTR szKey = mKeys[aIndex];
			if (szKey == NULL)
				szKey = mKeys[aIndex] = aDict->GetFastKeyAtOffset(mIndex[aIndex]);
			ASSERT(szKey != NULL);
			//TRACE("  key=%s", szKey);
			return szKey;
			}
		}

	template<class DICT>
	inline BOOL Find(DICT* aDict, const TDictKey& aKey, int* aIdx)
		{
		return Find(aDict, aKey, aIdx, 0, mHeader.NumRec - 1);
		}

	template<class DICT>
	BOOL Find(DICT* aDict, const TDictKey& aKey, int* aIdx, int aLowerBound, int aUpperBound)
		{
		//COLLECT_INTERVAL("Index::Find");
		//TRACE("Findkey=%s", aKey.c_str());

		if (aUpperBound >= (int)mHeader.NumRec)
			aUpperBound = (int)mHeader.NumRec - 1;

		if (aLowerBound > aUpperBound)
			{ // rango vacio
			*aIdx = -1;
			return FALSE;
			}

		CStringA csKey;  // solo se usa cuando no esta activo el FastHeap
		LPCSTR szKey;
		int ret = 0;
		//*aIdx    = -1;

	#if 1  
		// metodo tradicional de busqueda binaria
		int lo = aLowerBound;
		int hi = aUpperBound;
		int mid = 0;
		//int aLen = aKey.GetLength();
		LPCSTR szaKey = aKey.c_str();

		while (lo <= hi)
			{
			mid = (lo + hi) >> 1;

			if (CFastHeap::isEnabled())
				szKey = GetFastKeyAt(aDict, mid);
			else
				{
				//aDict->GetKeyAt(mid, csKey);
				aDict->GetKeyAtOffset(csKey, mIndex[mid]);
				
				szKey = csKey.c_str();
				}

			//ret = strFastCompare(szaKey, szKey, aLen);
			ret = strcmp(szaKey, szKey);
      //TRACE("compare (%s) (%s) = %d", szaKey, szKey, ret);
			//ret = memcmp(szaKey, szKey, aLen);
			//ret = aKey.Compare(szKey);

			if (ret < 0) // la llave buscada es menor que la central
				hi = mid - 1;
			else if (ret > 0) // la llave buscada es mayor que la central
				lo = mid + 1;
			else
				{ // se encontro la llave
				*aIdx = mid;
				//TRACE("found");

				return TRUE;
				}
			}

		// si llega aqui es que la clave no existe, en este caso la llave retornada debe ser 
		// la inmediata mayor que la buscada, a menos que la buscada sea mayor que la ultima
		if (ret > 0 && mid < (int)mHeader.NumRec - 1)
			mid++; 

		*aIdx = mid;
			
		//TRACE("not found");
		return FALSE;

	#endif

		/*
		//int num  = hi - lo + 1;
		//int half; 
		while (lo <= hi)
			{
			half = num/2;
			if (half != 0)
				{
				mid = lo + (num & 1 ? half : (half - 1));

				iDict->GetKeyAtOffset(key, iIndex[mid]);
				ret = aKey.Compare(key);

				if (ret < 0) // la llave buscada es menor que la actual
					{
					hi = mid - 1;
					num = num & 1 ? half : half-1;
					}
				else if (ret > 0)  // la llave buscada es mayor que la actual
					{
					lo = mid + 1;
					num = half;
					}
				else
					{ // se encontro la llave
					*aIdx = mid;
					if (iDict->iFlags & DICC_CACHEKEYS)
						iCachedKeys.Add(aKey, *aIdx | CACHE_KEY_FOUND);

					return TRUE;
					}
				}
			else if (num != 0)
				{ // solo queda una clave en el indice para chequear
				iDict->GetKeyAtOffset(key, iIndex[lo]);

				ret = aKey.Compare(key);
			
				*aIdx = ( ret<=0 )? lo : lo+1;

				if (iDict->iFlags & DICC_CACHEKEYS)
					{
					TInt idx = *aIdx;
					iCachedKeys.Add(aKey, (ret == 0) ? idx | CACHE_KEY_FOUND : idx);
					}
			
				return (ret == 0);
				}
			else
				break;
			}

		// devolver el indice mas cercano
		*aIdx = mid;
  
		if (iDict->iFlags & DICC_CACHEKEYS)
  		iCachedKeys.Add(aKey, *aIdx);
  
		return FALSE;*/
		}	

private:

	TUint CheckHeader(int nb)
		{
		// Verifica que el formato es correcto
		if (
			mHeader.ID[0] != 'I' || mHeader.ID[1] != 'D' || mHeader.ID[2] != 'X' || 
			mHeader.ID[3] != '_' || mHeader.ID[4] != 'D' || mHeader.ID[5] != 0   ||
			nb < (TInt)sizeof(TIDX_HDR) || 
			mHeader.DatSize < sizeof(TIDX_HDR) ||  
			mHeader.MemSize < mHeader.DatSize) 
			{
			TRACE("BAD_IDX_TYPE: nb=%d, mHeader.DatSize=%d, mHeader.MemSize=%d, sizeof(TIDX_HDR)=%d", nb, mHeader.DatSize, mHeader.MemSize, sizeof(TIDX_HDR));
			return BAD_IDX_TYPE;
			}

		// Verifica que la versión sea la adecuada
		if (mHeader.Ver != 1000 || mHeader.Size != sizeof(TIDX_HDR)) 
			{
			TRACE("BAD_IDX_VERSION");
			return BAD_IDX_VERSION;
			}

		return DICT_NO_ERROR;
		}

	};

#endif // DICTINDEX_H
