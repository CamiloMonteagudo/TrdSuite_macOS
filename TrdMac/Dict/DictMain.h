#ifndef MAINDICT_H
#define MAINDICT_H

#include "Dict.h"
#include "DictUser.h"
#include "Observer.h"

class CDictMain;
//-------------------------------------------------------------------------------------------------------------------
// Clase para manejar los diccionarios de palabras de forma centralizada, para que no se carguen m‡s de una ves
// y para mantenerlo en un cache y hacer mas efeciente su uso
class CDictGen
  {
  public:
  static CStringA DictsPath;
  static CStringA DictsUserPath;
  
  static CDictMain* Get( int src, int des );
//  static CDict* GetLang( int src );
  static CDictMain* Get( int src );
  };

//-------------------------------------------------------------------------------------------------------------------
// Clase para los diccionarios principales, soporta edicion a traves de un UserDict
class CDictMain: public CDict
	{
  friend CDictGen;
  
	protected:
		CDictUser mUserDict;      // dict con las palabras de usuario
		bool		mModified;				// indica si el dicc ha sido modificado
		int			mIdxXXXX_a;				// indice de la palabra "xxxx a"
		int			mIdxXXXXA;				// indice de la palabra "xxxxA"
		DWORD		mKeysPerChar;			// cantidad de claves por caracter en el rango "xxxx a"-"xxxxA"
    int     Src;              // Idioma fuente del diccionario
    int     Des;              // Idioma destino del diccionario
  
		CDictMain():CDict("DictMain"),mUserDict() 
			{
			mModified = false;
			mIdxXXXX_a = mIdxXXXXA = -1;
			mKeysPerChar = 0;
			}

		~CDictMain() {}

		BOOL Open(const CStringF& aName = CFNull);

//		void AddRef()
//			{
//			mRefCount++;
//			}
//
//		virtual void Close()
//			{
//			if (mModified)
//				Save();
//
//			CDictT<>::Close();
//			mUserDict.removeAll();
//			}
  
    // tipo que indica el tipo de modificacion realizada al diccionario
    enum EModifyType 
      {
      EITEM_MODIFIED,       // se usa lo mismo para items modificados o insertados
      EITEM_DELETED         // cuando se borra una clave del diccionario
      };
  
    // Se llama cuando se modifica el diccionario
    void afterModify(EModifyType aModify)
      {
      //limpiar la cache
      ClearCache(aModify == EITEM_DELETED);
  
      setupFindFrase();
  
      mModified = true;
      }
  
    // inicializa algunos valores necesarios para la busqueda de frases
    void setupFindFrase();

	public:

		// Solo salva el dicc de usuario
		BOOL Save();

    int getUserWordCount()
      {
      return mUserDict.GetCount();
      }
      
		// para la busqueda de frases
		BOOL FindInitFrase(const TDictKey& aName, int& aIndex);
		EFraseFind FindNextFrase(const TDictKey& aName, int& aIndex);

		// actualizar el indice con los datos del diccionario de usuario
		void updateIndex()
			{
			for(int i=0; i<mUserDict.GetCount(); ++i)
				{
				const CDictItem* pItem = mUserDict.GetAt(i);
				if (pItem->isAdd())
					{ // es una palabra adicionada
          mIndex->InsertAt(pItem->getIndex(), pItem->mKey, USERDICT_MASK | i);
          mCount++;
					}
				else
					{ // es una palabra modificada
          mIndex->SetOffset(pItem->getIndex(), USERDICT_MASK | i);
					}
				}
			}

    void checkIntegrity()
      {
#ifdef _DEBUG
      int numUserKeys = 0;
			for(int i=0; i<GetCount(); ++i)
        {
        if (!isUserKey(i))
          continue;
       
        numUserKeys++;
        DWORD offset = GetOffset(i);
        int userIndex = offset & ~USERDICT_MASK;
        ASSERT(userIndex >= 0 && userIndex < mUserDict.GetCount());
        const CDictItem* pItem = mUserDict.GetAt(userIndex);
        if (pItem->isAdd())
          { // es una palabra adicionada
          ASSERT(pItem->mOrgOffset == 0);
          }
        else
          { // es una palabra modificada
          ASSERT(pItem->mOrgOffset > 0);
       
          // chequear igualdad con la clave real
          CStringA sKey;
          CDict::GetKeyAtOffset(sKey, pItem->mOrgOffset);
          ASSERT(sKey == pItem->mKey);
          }
       
        // chequear orden con las claves adyacentes
        CStringA sKey1,sKey2;
        GetKeyAt(i-1, sKey1);
        GetKeyAt(i+1, sKey2);
        ASSERT(pItem->mKey > sKey1);
        ASSERT(pItem->mKey < sKey2);
        }
       
      ASSERT(numUserKeys == mUserDict.GetCount());  
      
#endif      
      }
      
		virtual void GetKeyAtOffset(TDictKey& aKey, TUint32 aOffset)
			{
			if (aOffset & USERDICT_MASK)
				{
				int index = aOffset & ~USERDICT_MASK;
				aKey = mUserDict.GetKeyAt(index);
				}
			else
				CDict::GetKeyAtOffset(aKey, aOffset);
			}

		virtual LPCSTR GetFastKeyAtOffset(TUint32 aOffset)
			{
			if (aOffset & USERDICT_MASK)
				{
				int index = aOffset & ~USERDICT_MASK;
        //LPCSTR s1, s2;
        //s1 = mUserDict.GetKeyAt(index).c_str();
        //s2 = mUserDict.GetAt(index)->mKey.c_str();
        //if (s1 != s2)
        //  TRACE("*********************************");
				//return s1;//mUserDict.GetKeyAt(index).c_str();
				return mUserDict.GetKeyAt(index).c_str();
				}
			else
				return CDict::GetFastKeyAtOffset(aOffset);
			}

		virtual BOOL GetAt(TInt aIdx, TDictKey* aName, TDictData* aData)
			{
			if (mCount == 0 || aIdx < 0)
  			return FALSE;

			if (aIdx >= mCount)  									// Si el indice es demasiado grande
				aIdx = mCount - 1;  								// Busca el último record
  
			TUint32 offset = GetOffset(aIdx);
			if (offset & USERDICT_MASK)
				{ // la palabra esta en el dicc de usuario
				aIdx = offset & ~USERDICT_MASK;
				if (aName)
					*aName = mUserDict.GetKeyAt(aIdx);
				if (aData)
					*aData = mUserDict.GetDataAt(aIdx);
				}
			else
				GetRef(aIdx, aName, aData);    							// Copia llave y datos

			return TRUE;
			}
  
    // Indica si una palabra, dado su indice, esta en el dicc de usuario
    BOOL isUserKey(int aIndex)
      {
      return (GetOffset(aIndex) & USERDICT_MASK) != 0;
      }
  
    // Indica si el diccionario fue modificado o no
    BOOL isDictModified()
      {
      return mModified;
      }
  
		// Adiciona una palabra al diccionario, actualiza el dicc de usuario y el indice del
		// dicc principal
		// Esto invalida la cache del dicc.
    // Retorna TRUE si la clave se adiciono al dic y false si solo fue una modificacion de una clave existente
		BOOL AddKey(const CStringA& aKey, const CStringA& aData)
			{
      BOOL res;
			int index;
			BOOL bFound = FindIndex(aKey, index);

			if (bFound)
				{ // es una palabra modificada
        DWORD orgOffset = GetOffset(index);
        if ((orgOffset & USERDICT_MASK) != 0)
          { // la palabra ya esta en el dicc de usuario, solo hay que actualizar el dato
          int userIndex = orgOffset & ~USERDICT_MASK;
          mUserDict.SetDataAt(userIndex, aData);
          }
        else
          { // adicionar la palabra al dicc de usuario
          mIndex->SetOffset(index, USERDICT_MASK | mUserDict.Add(aKey, aData, index, orgOffset, false));
          }
          
        res = FALSE;
				}
      else
				{ // es una palabra adicionada
				DWORD offset = USERDICT_MASK | mUserDict.Add(aKey, aData, index, 0, true);
				mIndex->InsertAt(index, aKey, offset);
				mCount++;

        res = TRUE;
				}

      afterModify(EITEM_MODIFIED);
      checkIntegrity();
      
      return res;
			}
      
		// se supone que aKey exista en el dicc principal
		BOOL DeleteKey(const CStringA& aKey)
			{
      int userIndex = mUserDict.FindKey(aKey);
      if (userIndex < 0)
        return FALSE;

      bool bAdjustIndexs = false;
      CDictItem* item = mUserDict.GetAt(userIndex);
      int index;
      mIndex->Find(this, aKey, &index);
      if (item->isAdd())
        { // es una palabra adicionada, se borra completa
        mIndex->DeleteAt(index, userIndex);
        mCount--;
          
        // hay que ajustar los indices de las palabras en el userdicc
        bAdjustIndexs = true;
        }
      else
        { // es una palabra modificada, restaurar el offset original de la palabra
        SetOffset(index, item->mOrgOffset);
      		  
        mIndex->OnUserKeyDeleted(userIndex);
        }
          
      mUserDict.removeAt(userIndex, bAdjustIndexs);
      
      afterModify(EITEM_DELETED);
      checkIntegrity();
      return TRUE;
      }
      
    // borra todas las palabras del dicc de usuario, funciona borrando palabra a palabra lo cual es
    // muy ineficiente pero no parece haber una solucion mejor
    void DeleteAllUserKeys()
      {
      for(int userIndex=mUserDict.GetCount()-1; userIndex>=0; userIndex--)
        {
        bool bAdjustIndexs = false;
        CDictItem* item = mUserDict.GetAt(userIndex);
        int index;
        mIndex->Find(this, item->mKey, &index);
        if (item->isAdd())
          { // es una palabra adicionada, se borra completa
          mIndex->DeleteAt(index, userIndex);
          mCount--;
          
          // hay que ajustar los indices de las palabras en el userdicc
          bAdjustIndexs = true;
          }
        else
          { // es una palabra modificada, restaurar el offset original de la palabra
          SetOffset(index, item->mOrgOffset);
          
          mIndex->OnUserKeyDeleted(userIndex);
          }
      
        mUserDict.removeAt(userIndex, bAdjustIndexs);
        }
      
      //afterModify(EITEM_DELETED);
      //checkIntegrity();
      }
  
      
	};


#endif