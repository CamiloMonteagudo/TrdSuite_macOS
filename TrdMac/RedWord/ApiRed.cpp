

#include <stdlib.h>
#include "ApiRed.h"
#include "RedWord.h"

//-----------------------------------------------------------------------------------------------------------------
// Crea un objeto de conjugación y retorna un handle para usar con las diferentes funciones del API
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(HREDW) RWNew()
  {
  return (HREDW)(new CRedWord());
  }

//-----------------------------------------------------------------------------------------------------------------
// Libera todos los recursos relacionados con el objeto de conjugación  'hRedW'
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) RWFree( HREDW hRedW )
  {
  delete ((CRedWord *)hRedW);
  CFastHeap::Disable();
  }

//-----------------------------------------------------------------------------------------------------------------
// Establece el directorio donde estan los diccionarios
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) RWSetDataPath( HREDW hRedW, const CStringF& Path )
  {
  ((CRedWord *)hRedW)->SetDataPath(Path);
  }

//-----------------------------------------------------------------------------------------------------------------
// Obtiene el directorio donde estan los diccionarios
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(CStringF) RWGetDataPath( HREDW hRedW )
  {
  return ((CRedWord *)hRedW)->GetDataPath();
  }

//-----------------------------------------------------------------------------------------------------------------
// Establece el handle de conjugación que se debe usar para la conjugación
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) RWSetConjHandle( HREDW hRedW, HCONJ hConj, bool free )
  {
  ((CRedWord *)hRedW)->SetConjHandle( hConj, free );
  }

//-----------------------------------------------------------------------------------------------------------------
// Establece el puntero al diccionario general de palabras
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(void) RWSetGenDict( HREDW hRedW, LPVOID hDic, bool free )
  {
  ((CRedWord *)hRedW)->SetGenDict( (CDictMain*)hDic, free );
  }

//-----------------------------------------------------------------------------------------------------------------
// Abre un idioma, identificado por una cadena de caracteres (Codigo del idioma)
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(BOOL) RWOpen( HREDW hRedW, LPCSTR sLang )
  {
  CFastHeap::CreateOrReset();
  
  return ((CRedWord *)hRedW)->Open(atoi(sLang));
  }

//-----------------------------------------------------------------------------------------------------------------
// Abre un idioma, identificado por un entero (Codigo del idioma)
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(BOOL) RWOpen( HREDW hRedW, int iLang )
  {
  CFastHeap::CreateOrReset();
  
  return ((CRedWord *)hRedW)->Open(iLang);
  }

//-----------------------------------------------------------------------------------------------------------------
// Cierra la reducción, si ya se encontraba algún idioma cargado
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(BOOL) RWClose( HREDW hRedW )
  {
  return ((CRedWord *)hRedW)->Close();
  }

//-----------------------------------------------------------------------------------------------------------------
// Cierra la reducción, si ya se encontraba algún idioma cargado
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(HREDDATA) RWReduce( HREDW hRedW, const CStringA& Word )
  {
	//COLLECT_INTERVAL("RWReduce"); // 2.23 9534
  return ((CRedWord *)hRedW)->Reduce( Word );
  }

//-----------------------------------------------------------------------------------------------------------------
// Obtiene el manipulador de uno de los diccionarios que se usan en la reducción
//-----------------------------------------------------------------------------------------------------------------
DLLENTRY(LPVOID) RWGetPtrDicRef( HREDW hRedW, int name )
  {
  return ((CRedWord *)hRedW)->GetPtrDicRef( (RWDicName)name);
  }

DLLENTRY(void) RWDump( HREDDATA hRedData )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if (hData != NULL)
		hData->Dump();
	}
	
//-------------------------------------------------------------------------------------------------------------------
// Obtiene la cantidad de tipos de reducción de una palabra
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetCount( HREDDATA hRedData )
	{
	CRWhData *hData = (CRWhData *)hRedData;

	if( hData != NULL && hData->m_Idioma >= 0)
		return hData->m_Types.GetSize();

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene la raiz de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(CString) RWGetRoot( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_PalReduc);

	return CString();
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el tipo de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(CString) RWGetType( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Tipo);

	return CString();
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el modo de la reducci—n con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
int RWGetClassType( HREDDATA hRedData, int idx )
{
  CRWhData *hData = (CRWhData *)hRedData;
  if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
    return( hData->m_Types[idx].GetType());
  
  return -1;
}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el modo de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetMood( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Modo);

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el tiempo de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetTime( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Tiempo);

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene la persona de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetPerson( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Persona);

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el numero de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetNumber( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Numero);

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el genero de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetGender( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Genero);

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el grado de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(int) RWGetDegree( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Grado);

	return -1;
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el prefijo de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(CString) RWGetPreffix( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_Prefijo);

	return CString();
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el complemento directo de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(CString) RWGetCDirect( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_CDirecto);

	return CString();
	}

//-------------------------------------------------------------------------------------------------------------------
// Obtiene el complemento indirecto de la reducción con indice 'idx'
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(CString) RWGetCIndirect( HREDDATA hRedData, int idx )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if( hData != NULL && hData->m_Idioma >= 0 && idx>=0 && idx<hData->m_Types.GetSize() )
		return( hData->m_Types[idx].m_CIndirecto);

	return CString();
	}

//-------------------------------------------------------------------------------------------------------------------
// Libera un hData a los datos de la reducción
//-------------------------------------------------------------------------------------------------------------------
DLLENTRY(void) RWFreehData( HREDDATA hRedData )
	{
	CRWhData *hData = (CRWhData *)hRedData;
	if(hData != NULL)
		{
		hData->m_Types.RemoveAll();
		delete hData;
		}
	}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

