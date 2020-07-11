/*
 ============================================================================
 Name		: Dict.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CDict declaration
 ============================================================================
 */

#ifndef DICT_H
#define DICT_H

// INCLUDES
#include "WinUtil.h"
#include "DictTypes.h"
#include "DictIndex.h"
#include "DictError.h"
#include "DictData.h"
#include "DictCache.h"

// Chequea si una cadena es valida como clave de diccionario
inline BOOL CheckKey(LPCSTR aKey)
	{
	for(; *aKey != 0; aKey++)
		{
		if (((BYTE)*aKey) < 32)
			return FALSE;
		}

	return TRUE;
	}

typedef struct TagDICC_HEADER
  {
  char  	ID[6];       // Identificador de los datos
  TUint32	Ver;         // Versión de la implentación del diccionario
  TUint32	Size;        // Tamaño de la estructura que mantiene los datos                  
  TInt32 	Src;         // Código (Postal) del idioma fuente                   
  TInt32	Dest;        // Código (Postal) del idioma destino                  
  TInt32 	Flgs;        // Banderas que definen los atributos
  TUint32	MaxSize;     // Tamaño máximo del fichero mapeado
  TUint32	DatSize;     // Tamaño de los datos
  TUint32	MemSize;     // Tamaño de la memoria reservada
  TUint32	NumRec;      // Número de record totales
  TUint32	DelRec;      // Número de record borrados
  TUint32	IdxKey1;     // Ultima modificación del índice clave1
  TUint32	IdxKey2;     // Ultima modificación del índice clave2
  char  	IdObj[20];   // Nombre del objeto (Para identificar si esta cargado)
  TUint32	Reserv1;     // Reservado para uso futuro
  TUint32	Reserv2;     // Reservado para uso futuro
  } TDICC_HEADER;
  

  //--------------------------------------------------------------------------
  // Valores de retorno para la función 'FindFrase'
  //--------------------------------------------------------------------------
enum EFraseFind {FRS_NOFOUND = 0, FRS_SEMIFOUND, FRS_FULLFOUND};

  //--------------------------------------------------------------------------
  // indices para GetHeaderField y SetHeaderField
  //--------------------------------------------------------------------------
  #define HDR_DIC_ID      1
  #define HDR_DIC_VER     2
  #define HDR_SIZE        3
  #define HDR_SRC_ID      4
  #define HDR_DEST_ID     5
  #define HDR_DIC_FLGS    6
  #define HDR_MAX_REC     7
  #define HDR_DAT_SIZE    8
  #define HDR_MEM_SIZE    9
  #define HDR_MUM_REC     10
  #define HDR_DEL_REC     11
  #define HDR_MOD_KEY1    12
  #define HDR_MOD_KEY2    13
  #define HDR_OBJ_ID      14
  #define HDR_ESP_ID      15
  #define HDR_TYPE_ID     16
  #define HDR_MODE        17

  #define HDR_MAX_FLD     17
  
#define FLAG_NOHIDE   0x00000010
/*
#define DICC_FILE				1
#define DICC_READONLY		2
#define DICC_MEM        4
#define DICC_LOADKEYS   32
#define DICC_CACHEKEYS  64
  */

#ifdef ACTIVE_SAVE

class CActionPhrase;

#endif


class CDictAbs
	{
	public:

		// Devuelve clave y datos de un indice dado
		virtual BOOL GetAt(TInt aIdx, TDictKey* aName, TDictData* aData) = 0;

		// Estos dos solo se usan desde el indice
		// Lee una clave en una posicion
		virtual void GetKeyAtOffset(TDictKey& key, TUint32 aOffset) = 0;

		// Lee una clave en una posicion y la almacena en el FastHeap
		virtual LPCSTR GetFastKeyAtOffset(TUint32 aOffset) = 0;
	};


class CDictBase : public CMFCObject
	{
	protected:
		CFile* mFile;
		TInt   mBaseOffset;				// desplazamiento dentro del stream donde comienza este diccionario
		BOOL	 mHideData;
		CDictIndex<CACHE_YES>* mIndex;

		// Lee la clave y/o los datos de un indice dado
		void GetRef(int aIdx, TDictKey* aName, TDictData* aData);

		// Devuelve el desplazamiento de un indice
		inline DWORD GetOffset(TInt aIdx) 
			{
			return mIndex->GetOffset(aIdx);
			}

    inline void SetOffset(int aIndex, TUint32 aOffset)
      {
      mIndex->SetOffset(aIndex, aOffset);
      }

		// Pone la posicion del stream en un desplazamiento dado
		void SetStreamOffset(TInt aOffset)
			{
			aOffset += mBaseOffset;
			TInt streamPos = 0;
			streamPos = mFile->Seek(aOffset, CFile::begin);
			ASSERT(streamPos == aOffset);
			}

	private:

		// Lee los datos en la posicion actual
		void GetStreamData(TDictData& aData, int aDataLen)
			{
			LPCSTR sData = aData.GetBuffer(aDataLen + 1);
			mFile->ReadBuffer((LPVOID)sData, aDataLen);

			if (sData[aDataLen-1] == '\x0')
				aDataLen--; // para no incluir el null terminator

			aData.ReleaseBuffer(aDataLen);
			}

		void HideData(TDictData& aDatos)
			{
			if (mHideData)
				{
				TUint8* sBuffer = (TUint8*)aDatos.c_str();
				for(; *sBuffer; sBuffer++)
					{
					*sBuffer = 255 - *sBuffer;
					}
				}
			}

	public:

	};


/**
 *  CDict
 * 
 */
// DATA representa el tipo de dato de este diccionario, en caso de que se especifique 
// debe ser una clase que tenga un constructor que acepte un CStringA
// solo se usa en el metodo GetKeyT y en la cache, los demas siempre usan TDictData
//template<class CACHE = CIndexedCache<TDictData> >
template<class CACHE = CIndexedCache<const char> >
class CDictT : public CDictBase, public CDictAbs
	{

public:	
	CStringF mName;
	
protected:
	TInt   mCount;
	BOOL	 mIsOpen;
	BOOL   mLoadFromStream;	// indica si el diccionario y el indice deben cargarse desde un
													// stream o desde ficheros independientes
	TInt   mError;
	TDICC_HEADER mHeader;
	CStringA mMasterFileName;

	CACHE   mCache;

public:
	CStringA mDebugName;
	
	// Constructors and destructor
	CDictT(LPCSTR aName)
		{
		mDebugName = aName;
		mCount = 0;
		mIsOpen = FALSE;
		mHideData = TRUE;
		mError = DICT_NO_ERROR;
		mIndex = NULL;
		mFile = NULL;
		mLoadFromStream = FALSE;
		mBaseOffset = 0;
		}

	/**
	 * Destructor.
	 */
	~CDictT()
		{
		Close();
		}

	BOOL Load(const CStringA& aName) 
		{ 
		return Open(aName);
		};

	CDictIndex<CACHE_YES>* getIndex()
		{
		return mIndex;
		}

protected:
	virtual BOOL Open(const CStringA& aName = CFNull);
	BOOL OpenFromFile(CArchive &ar);

public:
	void Serialize(CArchive &ar, const CStringA& aDumpTxtFile)	
		{
		Serialize(ar, aDumpTxtFile.c_str());
		}

	void Serialize(CArchive &ar, LPCSTR aDumpTxtFile = NULL);

	virtual void Close();

#ifdef ACTIVE_SAVE

	BOOL Save(const CStringA& sFileName);
	BOOL Save(CArchive& ar, LPCSTR aIndexName = NULL, LPCSTR aDumpTxtFile = NULL);
	BOOL Save_(CArchive& ar, LPCSTR aIndexName = NULL, LPCSTR aDumpTxtFile = NULL);

	BOOL SaveOpt(const CStringA& sFileName);
	BOOL SaveOpt(CArchive& ar);

	BOOL SaveDCX(CArray<CActionPhrase>& arrWords);

#endif

  BOOL IsOpen() { return mIsOpen; }

	static LPCSTR sss;
	/*static void CheckCache(CACHE_ITEM* item)
		{
		if (item->found())
			{
			CStringA s;//(sss);
			s += "_CacheItemFound";
			COUNTER(s.c_str());
			}
		else
			{
			CStringA s;//(sss);
			s += "_CacheItemNoFound";
			COUNTER(s.c_str());
			}
		}*/

	/*class CCheckCache
		{
		public:
		void Action(CACHE_ITEM* item)
			{
			if (item->found())
				{
				CStringA s;//(sss);
				s += "_CacheItemFound";
				COUNTER(s.c_str());
				}
			else
				{
				CStringA s;//(sss);
				s += "_CacheItemNoFound";
				COUNTER(s.c_str());
				}
			}
		};*/

	void ClearCache(BOOL aClearIndex)
		{
		//TRACE("%s, count = %d, size = %d", iDebugName.c_str(), iCount, iHeader.DatSize);
		//sss = iDebugName.c_str();
		/*CCheckCache checkCache;
		if (iCount < 70000)
			//iCache.ForEach<CheckCache>();
			iCache.ForEach2(checkCache);*/

		//mCache.DeleteAll();
		mCache.RemoveAll();
    if (aClearIndex)
      mIndex->ClearCache();
		}

	TUint32 GetHdrField(TInt aIdxField) const;
	CStringA GetHdrFieldS(TInt aIdxField) const;
  
  //void SetHideData(BOOL aHideData) { iHideData = aHideData; }

	BOOL FindSuffix(const TDictKey& aKey, TDictData& aData);
	BOOL FindPreffix(const TDictKey& aKey, TDictData& aData);

	// Indica si una llave existe, llama a FindIndex por tanto no necesita ser virtual
  inline BOOL IsKey(const TDictKey& aKey)
  	{
		return mCache.IsKey(this, aKey);
  	};
	
	// Devuelve los datos de una llave
	BOOL GetKey(const TDictKey& aKey, TDictData& aData)
		{
		return mCache.GetKey(this, aKey, aData);
		}

	// Devuelve el indice mas cercano de una llave y si existe o no,
	BOOL FindIndex(const TDictKey& aName, TInt &aIndex)
		{
		return FindIndex(aName, aIndex, 0, mCount-1);
		}

  void GetKeyAt(TInt aIdx, TDictKey& aKey)
  	{
    GetAt(aIdx, &aKey, NULL); 										// Busca la llave
  	}

  void GetDataAt(TInt aIdx, TDictData& aData)
  	{
    GetAt(aIdx, NULL, &aData);   		  						// Obtiene los datos
  	}
    
  inline TInt GetCount() const { return mCount; }

protected:

	// estos se usan solo internamente dentro de esta clase (son protegidos)

	BOOL FindIndex(const TDictKey& aName, TInt &aIndex, TInt aStart, TInt aEnd)
		{
    if (CFastHeap::isEnabled())
      return mCache.FindIndex(this, aName, aIndex, aStart, aEnd);
    else
      return mIndex->Find(this, aName, &aIndex, aStart, aEnd);
		}

	// de momento no hace falta
	//BOOL GetKeyT(const TDictKey& aKey, DATA*& aData);
	
	TInt GetLastError() { return mError; };
	void ResetError() { mError = DICT_NO_ERROR; };

	BOOL CheckHeader(TUint aFileSize);
	};


//typedef CDictT<> CDict;

class CDict: public CDictT<>
	{
	public:
		CDict(LPCSTR aName):CDictT<>(aName)
			{
			}

	// Devuelve la clave y datos de un indice
	virtual BOOL GetAt(TInt aIdx, TDictKey* aName, TDictData* aData)
		{
		if (mCount == 0 || aIdx < 0)
  		return FALSE;

		if (aIdx >= mCount)  									// Si el indice es demasiado grande
			aIdx = mCount - 1;  								// Busca el último record
  
		GetRef(aIdx, aName, aData);    							// Copia llave y datos

		return TRUE;
		}

		// Estos dos solo se usan desde el indice
		// Lee una clave en una posicion
		virtual void GetKeyAtOffset(TDictKey& key, TUint32 aOffset)
			{
			SetStreamOffset(aOffset + 2);

			mFile->ReadAsSZString(key);
			}

		// Lee una clave en una posicion y la almacena en el FastHeap
		virtual LPCSTR GetFastKeyAtOffset(TUint32 aOffset)
			{
			SetStreamOffset(aOffset + 2);

			return mFile->ReadFastAsSZString();
			}
	};



// CSimpleDict, clase usada para casos donde no haga falta acceder al diccionario por indice
// ni que este tenga el concepto de palabra actual, ademas las claves se cargan completamente
// en memoria, todo esto unido hace que el diccionario sea muy eficiente.
// Los accesos a las claves y datos deben ser unicamente a traves de los metodos IsKey y GetKey
// el uso de otros metodos debe dar error de compilacion
/*template<class DATA>
class CSimpleDict : public CDictT<DATA,CSimpleCache<DATA>> 
	{ 
	protected:
	CIntArray iArrDataOff;

	public:
	// Obtiene los datos de una clave
	inline BOOL GetKey(const TDictKey& aKey, DATA* &aData)
		{
		return CDictT<DATA,CSimpleCache<DATA>>::iCache.GetKey(this, aKey, aData);
		}

  virtual void GetDataAt(TInt aIdx, TDictData& aData)
  	{
		if (CDictT<DATA,CSimpleCache<DATA>>::iIndex == NULL)
			{
			int start = iArrDataOff[aIdx];
			int size = iArrDataOff[aIdx+1] - start;
			CDictT<DATA,CSimpleCache<DATA>>::iFile->Seek(start);
			CDictT<DATA,CSimpleCache<DATA>>::iFile->ReadBufferAsCString(aData, size);
			}
		else
			CDictT<DATA,CSimpleCache<DATA>>::GetAt(aIdx, NULL, &aData);   		  						// Obtiene los datos
  	}

	//BOOL LoadOpt(const CStringA& aFileName);

#ifdef ACTIVE_SAVE
	BOOL SaveOpt();
#endif

	};*/


template<class CACHE>
LPCSTR CDictT<CACHE>::sss;


#include "DictInl.h"

#endif // DICT_H
