
#ifndef DICTUSER_H
#define DICTUSER_H

#include "WinUtil.h"
//#include "DictMain.h"


class CDictItem
	{
	DWORD mIndex;   // posicion en el indice del dicc principal, el bit 0 indica si es una
									// adicion o sobreescritura del dicc principal
                  // NOTA: este indice solo tiene un valor correcto cuando se llama desde
                  // CDictMain::updateIndex(), no usar en ningun otro caso
	public:
	CStringA mKey, mData; // clave y datos
  DWORD mOrgOffset;  // desplazamiento de la palabra original en el dicc principal

	CDictItem() {}

	CDictItem(const CStringA& aKey, const CStringA& aData, int aIndex, DWORD aOrgOffset, bool aIsAdd):mKey(aKey),
    mData(aData), mOrgOffset(aOrgOffset), mIndex((aIndex << 1) + (int)aIsAdd)
		{
		}

	void update(const CStringA& aData, int aIndex, bool aIsAdd)
    {
    mData = aData;
    mIndex = (aIndex << 1) + (int)aIsAdd;
    }

	bool isAdd() const
		{
		return (mIndex & 1);
		}

private:
  friend class CDictMain;
  //friend void CDictMain::updateIndex();
  
  // indice de la palabra en el indice del dicc principal
  // NOTA: este indice solo tiene un valor correcto cuando se llama desde
  // CDictMain::updateIndex(), no usar en ningun otro caso
	int getIndex() const
		{
		return mIndex >> 1;
		}

public:
	void incIndex()
		{
		mIndex += 2;
		}
    
	void decIndex()
    {
    mIndex -= 2;
    }

	void load(CFile& file)
		{
		file.ReadCStringA(mKey);
		file.ReadCStringA(mData);
		mIndex = file.ReadUint32();
		mOrgOffset = file.ReadUint32();
		}

	void save(CFile& file)
		{
		file.WriteCString(mKey);
		file.WriteCString(mData);
		file.WriteUint32(mIndex);
		file.WriteUint32(mOrgOffset);
		}

	};

class CDictUser
	{
	CStringA mFileName;
	CArray<CDictItem> mItems;

	public:
		BOOL load(const CStringA& aName)
			{
      //TRACE("Cargando DictUser");
			mItems.RemoveAll();

			mFileName = aName;

			CFile file;
			if (!file.Open(mFileName, CFile::modeRead))
				return FALSE;

			int count = file.ReadInt32();
			mItems.SetSize(count);

			for(int i=0; i<count; ++i)
				{
				mItems[i].load(file);
        //TRACE(" %skey=%s off=%d", mItems[i].isAdd() ? "add": "", mItems[i].mKey.c_str(), mItems[i].mOrgOffset);
				}

			file.Close();
			return TRUE;
			}

		BOOL reLoad()
			{
			return load(mFileName);
			}

		BOOL save()
			{
			CFile file;
			if (!file.Open(mFileName, CFile::modeWrite))
				return FALSE;

			int count = mItems.GetCount();
			file.WriteInt32(count);
			for(int i=0; i<count; ++i)
				{
				mItems[i].save(file);
				}

			file.Close();
			return TRUE;
			}

		void removeAll()
			{
			mItems.RemoveAll();
			}

    void removeKey(const TDictKey& aKey)
      {
			int index = FindKey(aKey);
			if (index >= 0)
        mItems.RemoveAt(index);
      }
  
    void removeAt(int aIndex, bool aAdjustIndexs)
      {
      CStringA deletedKey = mItems[aIndex].mKey;
      mItems.RemoveAt(aIndex);
      
      // ajustar los indices de los items subsiguientes
      if (aAdjustIndexs)
        for(int i=aIndex,count=mItems.GetCount(); i<count; ++i)
          {
          if (mItems[i].mKey > deletedKey)
            mItems[i].decIndex();
          }
      }
      
		BOOL GetKey(const TDictKey& aKey, TDictData& aData)
			{
			aData.Empty();

			int aIndex = FindKey(aKey);
			if (aIndex < 0)
				return FALSE;

			aData = GetDataAt(aIndex);

			return TRUE;
			}

		int GetCount()
			{
			return mItems.GetCount();
			}

		int FindKey(const CStringA& aKey)
			{
			for(int i=0,count=mItems.GetCount(); i<count; ++i)
				{
				if (mItems[i].mKey == aKey)
					return i;
				}

			return -1;
			}

		CDictItem* GetAt(int aIdx) const
			{
			return &mItems[aIdx];
			}

		CStringA GetKeyAt(int aIdx) const
			{
			if (aIdx < 0)
				return CANull;

			return mItems[aIdx].mKey;
			}

		CStringA GetDataAt(int aIdx) const
			{
			if (aIdx < 0)
				return CANull;

			return mItems[aIdx].mData;
			}

    void SetDataAt(int aIdx, const CStringA& aData)
      {
      mItems[aIdx].mData = aData;
      }
      
		// Adiciona una clave con su dato (o la reemplaza si la clave ya existe)
		// aDictIndex es el indice en el diccionario principal
		// Devuelve el indice donde se añadio
		int Add(const CStringA& aKey, const CStringA& aData, int aDictIndex, DWORD aOrgOffset, bool aIsAdd)
			{
			int index = FindKey(aKey);
			if (index >= 0)
				{ // si ya existe en el UserDict solo hay que hacer un update sin cambiar el offset
        GetAt(index)->update(aData, aDictIndex, aIsAdd);
				}
			else
				{
				index = mItems.GetCount();
				mItems.Add(CDictItem(aKey, aData, aDictIndex, aOrgOffset, aIsAdd));
				/*if (aIndex == -1) // ocurre cuando el dict esta vacio
					aIndex = mItems.Add(CDictItem(aKey, aData));
				else
					mItems.InsertAt(aIndex, CDictItem(aKey, aData));*/
				}

			return index;
			}
	};

#endif
