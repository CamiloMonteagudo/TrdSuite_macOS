
#include "DictMain.h"
#include "LanguageName.h"

//*******************************************************************************************************************************
// CDictGen
//*******************************************************************************************************************************
static CDictMain* CacheDicts[NLANGS][NLANGS] = {0};
static CDictMain* LastDict;

CStringA CDictGen::DictsPath;
CStringA CDictGen::DictsUserPath;

//------------------------------------------------------------------------------------------------------------------------------
// Obtiene un diccionario general con la direcci—n dada, si esta en el cache lo retorna, si no lo carga
 CDictMain* CDictGen::Get( int src, int des )
  {
  CDictMain* dic = CacheDicts[src][des];
  if( dic != NULL )
    {
    if( dic != LastDict )
      {
      dic->ClearCache(TRUE);
      LastDict = dic;
      }
      
    return dic;
    }
  
  CStringF DicPath = DictsPath + CLangIso::Name(src) + LF"2" + CLangIso::Name(des) + LF".dic";
  
  dic = new CDictMain();
  if( !dic->Open( DicPath ) ) return NULL;
  
  dic->Src = src;
  dic->Des = des;
  
  CacheDicts[src][des] = dic;
  
  LastDict = dic;
  return dic;
  }

//static CDict* ConjDicts[5] = {0};
////------------------------------------------------------------------------------------------------------------------------------
//// Obtiene un diccionario general con la direcci—n fuente src
//CDict* CDictGen::GetLang( int src )
//  {
//  CDict* dic = ConjDicts[src];
//  if( dic != NULL )
//    {
//    dic->ClearCache(TRUE);
//    return dic;
//    }
//  
//  dic = new CDict("");
//  
//  int des = (src==1)? 0 : 1;
//  CStringF DicPath = DictsPath + CLangIso::Name(src) + LF"2" + CLangIso::Name(des) + LF".dic";
//  
// 	if( !dic->Load(DicPath) )
//    {
//    delete dic;
//    return FALSE;
//    }
//  
//  ConjDicts[src] = dic;
//  return dic;
//  }

//------------------------------------------------------------------------------------------------------------------------------
// Obtiene un diccionario general con la direcci—n fuente src
CDictMain* CDictGen::Get( int src )
  {
  if( LastDict!=NULL && LastDict->Src==src )
    {
    LastDict->ClearCache(TRUE);
    return LastDict;
    }
  
  for( int i=0; i<CLangIso::Count(); ++i )  // Busca por todos los diccionarios fuentes
    {
    if( i != src )
      {
      CDictMain* dic = CDictGen::Get( src, i );
      if( dic != NULL ) return dic;
      }
    }
  
  return NULL;
  }

//*******************************************************************************************************************************
//CDictMain
//*******************************************************************************************************************************

BOOL CDictMain::Open( const CStringF& aName )
	{
	mModified = false;
  if( !CDictT<>::Open(aName) ) return FALSE;
  
	// Cargar el dicc de usuario
	CStringA sUserDict(aName);
	sUserDict.MakeLower();
  
  if( !CDictGen::DictsUserPath.IsEmpty() )
    {
    int p = sUserDict.ReverseFind(FOLDER_SEP_CH);
    sUserDict = CDictGen::DictsUserPath + sUserDict.Mid(p+1);
    }
  
  sUserDict.Replace(".dic", "_user.dic");
	mUserDict.load(sUserDict);

  updateIndex();
  checkIntegrity();

  setupFindFrase();
	return TRUE;
	}

// inicializa algunos valores necesarios para la busqueda de frases
void CDictMain::setupFindFrase()
  {
  FindIndex("xxxx a", mIdxXXXX_a);
  FindIndex("xxxxA", mIdxXXXXA);
  // cuantas claves hay por caracter como promedio en el rango 'a'...'z'
  mKeysPerChar = ((mIdxXXXXA - mIdxXXXX_a) /*<< 16*/) / ('z'-'a');
  if (mKeysPerChar <= 0)
    mKeysPerChar = 1;
  }

BOOL CDictMain::Save()
	{
	mModified = false;
	return mUserDict.save();
	}

//---------------------------------------------------------------------------------------
// Esta función se utiliza para comenzar el proceso de busqueda de una frase. Debe de 
// utilizarse para buscar la primera palabra de la frase y preparar el mecanismo para 
// buscar el resto de la frase, si existe.
// Devuelve en aIndex la posicion mas cercana a la frase
//---------------------------------------------------------------------------------------
BOOL CDictMain::FindInitFrase(const TDictKey& aFrase, int& aIndex)
  {
	//TRACE("FindInitFrase: \"%s\"", aFrase.c_str());
	BOOL res = FindIndex(aFrase, aIndex);
  if (res == FALSE)
		{ // no se encontro la frase exacta, ver si al menos hay un macheo parcial
		LPCSTR szKey = mIndex->GetFastKeyAt(this, aIndex);
		int len = aFrase.GetLength();
		res = strncmp(szKey, aFrase.c_str(), len) == 0 && szKey[len] == ' ';
		}

  return res;
  }

//---------------------------------------------------------------------------------------
// Esta función se utiliza para continuar el proceso de busqueda de una frase,
// de forma tal que la frase encontrada sea la más grande posible.
// Esta función puede retornar 3 valores diferentes:
//
// FRS_NOFOUND - Este valor se retorna para terminar la busqueda de la frase
//               ya que no se encuentra en el diccionario ninguna llave que
//               comience con la frase buscada.
//
// FRS_FULLFOUND - Este valor indica que la frase fue encontrada en el
//                 diccionario, y se debe comenzar a probar con una frase
//                 más larga.
//
// FRS_SEMIFOUND - Este valor indica que la frase no fue encontrada pero se
//                encontro una llave que comienza con la frase buscada, por
//                tanto se debe probar con una frase más larga.
//---------------------------------------------------------------------------------------
// La busqueda de frases solo se usa en diccionarios principales
EFraseFind CDictMain::FindNextFrase(const TDictKey& aFrase, int& aIndex)
  {
  ASSERT( mIsOpen );        			// Existe diccionario principal
  ASSERT( mIdxXXXX_a > -1 );
	//TRACE("FindNextFrase: \"%s\"", aFrase.c_str());

	LPCSTR szFrase = aFrase.c_str();
  int lenFrase = aFrase.GetLength();                      // Longitud a comparar

	// hacer una iteracion simple a ver si encuentra el resultado
	// pedir la clave de la posicion actual
	LPCSTR szKey = mIndex->GetFastKeyAt(this, aIndex);
	int ret = strncmp(szKey, szFrase, lenFrase);
	if (ret == 0)
		{	// la parte izquierda de esta clave coincide con la frase a buscar
		if (szKey[lenFrase] == '\x0')
			return FRS_FULLFOUND;    // Es la llave

		if (szKey[lenFrase] == ' ')
			return FRS_SEMIFOUND;    // Le sigue otra palabra

		// nos pasamos de la frase a buscar y no se encontro nada
		return FRS_NOFOUND;
		}
	else if (ret > 0) // nos pasamos de la frase a buscar y no se encontro nada
		return FRS_NOFOUND;

	// aIndex esta antes de la frase a buscar

	int nIter = 0;
	int idxEnd = -1;
	int delta = 4;
	
	if (*((DWORD*)szFrase) == 0x78787878 && szFrase[4] == ' ')
		{ // la frase empieza con "xxxx "
		BYTE ch5 = (BYTE)szFrase[5];
		if (ch5 < (BYTE)'a' || ch5 > (BYTE)'z')
			{
			delta = 512;//(idxXXXX_a - idxXXXX) / 4;
			}
		else // szFrase[5] esta en el intervalo 'a'...'z'
			{ // la frase empieza con "xxxx a" o superior
			if (aIndex < mIdxXXXX_a)
				aIndex = mIdxXXXX_a;

			// si la frase tiene dos espacios al menos, usar un delta pequeño, ya que se supone
			// que estemos mas cerca del objetivo
			for(int i=6; ; i++)
				{
				char ch = szFrase[i];
				if (ch == '\x0')
					break;
				else if (ch == ' ')
					{
					delta = 64;
					break;
					}
				}

			if (delta != 64)
				{
				// tratar de adivinar la posicion de la frase asumiendo una distribucion uniforme
				delta = mKeysPerChar << 0;
				int hintPos = mIdxXXXX_a + mKeysPerChar * (ch5 - (BYTE)'a');
				hintPos = Min(hintPos, mIdxXXXXA);

				szKey = mIndex->GetFastKeyAt(this, hintPos);
				ret = strcmp(szKey, szFrase);
				if (ret < 0) // la clave esta antes de la frase
					{ // avanzar hasta encontrar o pasarse de la frase
					aIndex = hintPos;
					idxEnd = hintPos + delta;

					for(;  idxEnd < mIdxXXXXA; nIter++)
						{
						szKey = mIndex->GetFastKeyAt(this, idxEnd);
						ret = strcmp(szKey, szFrase);

						if (ret < 0)
							idxEnd += delta;
						else if (ret > 0)
							break;
						else
							{ // se encontro la frase
							aIndex = idxEnd;
							return FRS_FULLFOUND;
							}
						}
					}
				else if (ret > 0) // la clave esta despues de la frase
					{ // retroceder hasta encontrar o pasarse de la frase
					idxEnd = hintPos;
					aIndex = idxEnd - delta;

					for(; aIndex >= mIdxXXXX_a; nIter++)
						{
						szKey = mIndex->GetFastKeyAt(this, aIndex);
						ret = strcmp(szKey, szFrase);

						if (ret > 0)
							aIndex -= delta;
						else if (ret < 0)
							break;
						else
							return FRS_FULLFOUND; // se encontro la frase
						}

					}
				else
					{
					aIndex = hintPos;
					return FRS_FULLFOUND; // se encontro la frase
					}

				clamp(aIndex, mIdxXXXX_a, mIdxXXXXA);
				clamp(idxEnd, aIndex, mIdxXXXXA);
				}
			}
		}
		
	if (idxEnd == -1)
		{
		idxEnd = aIndex + delta;
		if (idxEnd >= mCount)
			idxEnd = mCount-1;

		for(;;)
			{
			nIter++;
			szKey = mIndex->GetFastKeyAt(this, idxEnd);
			ret = strcmp(szKey, szFrase);
			//ret = memcmp(szKey, szFrase, lenFrase);
			//ret = strFastCompare(szKey, szFrase, lenFrase);
			if (ret < 0)
				{
				//estamos antes de la frase, ajustar el rango y seguir buscando
				delta <<= 1;
				aIndex = idxEnd;
				if (aIndex >= mCount)
					return FRS_NOFOUND;

				idxEnd = aIndex + delta;
				if (idxEnd >= mCount)
					idxEnd = mCount-1;
				}
			else if (ret == 0 && szKey[lenFrase] == '\x0')
				{	// se encontro la frase exacta
				aIndex = idxEnd;
				return FRS_FULLFOUND;    // Es la llave
				}
			else
				// nos pasamos de la frase, el intervalo a buscar es [aIndex, idxEnd]
				break;
			}
		}

	// buscar la clave usando los indices dentro de un intervalo

	BOOL res = FALSE;
	if (aIndex != idxEnd)
		res = FindIndex(aFrase, aIndex, aIndex, idxEnd);

	if (res)
		{
		//TRACE("Phrase found: %s", szFrase);
    return FRS_FULLFOUND;    // Es la llave
		}

	// la clave buscada no existe (ya no se puede devolver FRS_FULLFOUND)
	szKey = mIndex->GetFastKeyAt(this, aIndex);
	if (strncmp(szKey, szFrase, lenFrase) == 0 && szKey[lenFrase] == ' ')
		// la parte izquierda de esta clave coincide con la clave a buscar
		return FRS_SEMIFOUND;    // Le sigue otra palabra

  // No se encuentra
	return FRS_NOFOUND;
  }
