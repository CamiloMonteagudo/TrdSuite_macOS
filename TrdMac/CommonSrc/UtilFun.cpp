////////////////////////////////////////////////////////////////////////////
//   Funciones utiles                                                     //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez        29/12/97                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "UtilFun.h"
                        

/////////////////////////////////////////////////////////////////////////////
/*
void DelQuotationMarks(LPSTR buffer)
{
    LPSTR ptrini, ptrfin;

    for (ptrini=ptrfin=buffer; *ptrfin != '\0'; ++ptrfin)
        if (*ptrfin != '\"') { *ptrini = *ptrfin;  ++ptrini; }

    *ptrini = '\0';
}

/////////////////////////////////////////////////////////////////////////////

void DelArguments(LPSTR buffer)
{
    LPSTR ptr;

    ptr = strstr(buffer, ".exe");
    if (ptr==NULL) ptr = strstr(buffer, ".EXE");

    if (ptr) *(ptr+4) = '\0';
}
*/
/////////////////////////////////////////////////////////////////////////////

void PutSufijo(CStringA &left, CStringA &corchete, const CStringA &right)
    {                     
    CStrArray keyarray;
    CStrArray dataarray;
    for(;;)
        {
        int fin = corchete.Find(',');
        if( fin == -1)
            break;
        CStringA key(corchete,fin);   
        CStringA data;   
        
        corchete = corchete.Right(corchete.GetLength()-(fin+1));
        fin = corchete.Find(';');
        if(fin == -1)
            {
            data = corchete;
            corchete.Empty();
            }
        else
            {
            data = corchete.Left(fin);
            corchete = corchete.Right(corchete.GetLength()-(fin+1));
            }                                              
        keyarray.Add(key);             
        dataarray.Add(data);             
        }
    for(int i = 0; i < keyarray.GetSize(); i++)    
        {
        CStringA tmp = left.Right(keyarray[i].GetLength());
        if(tmp == keyarray[i])
            {     
            left = left.Left(left.GetLength()-keyarray[i].GetLength());
            left += dataarray[i];
            break;
            }
        }
    left = left + right;    
    }

/////////////////////////////////////////////////////////////////////////////
/*
int StrIndex(const char *s,const char c)                                              
    {       
    const char *tmp = strchr(s,c);
    ASSERT(tmp != NULL);
    return tmp - s;
    }                  

/////////////////////////////////////////////////////////////////////////////

CStringA &IntToString(CStringA &str,int i)                                              
    {
    char tmp[100];
    _itoa_s(i,tmp,100,10);
	str = (LPCSTR)tmp;
    return str;
    }

/////////////////////////////////////////////////////////////////////////////

void StringOfIntToArray(CStringA s,CWordArray *array)
    {                     
    for(;;)
        {
        int fin = s.Find(',');
        if( fin == -1)
            {           
            int i = atoi(s);
            array->Add(i);             
            break;
            }
        else
            {    
            CStringA key(s,fin);   
            s = s.Right(s.GetLength()-(fin+1));
            int i = atoi(key);
            array->Add(i);             
            }
        }
    }            */
/////////////////////////////////////////////////////////////////
// Funcion general para extraer un subcampo dentro de una cadena que 
// se encuentre dividida en campos
/////////////////////////////////////////////////////////////////

BOOL GetSubCampo(CString &subcampo,	// Cadena donde se retorna el contenido del subcampo
				 const CStringA& str,		// Cadena que contiene todos los campos
				 char name,		// Nombre del campo a seleccionar
				 char divisor)	// elemento que se emplea como divisor #@, etc
//				 const CStringA& name,		// Nombre del campo a seleccionar
//				 LPCSTR divisor)	// elemento que se emplea como divisor #@, etc
	{
	BOOL ret = FALSE;

	subcampo.Empty();

	char inter[3];
	inter[0] = divisor;
	inter[1] = name;
	inter[2] = 0;
	//CString inter(divisor);
	//inter += name;
	int i = str.Find(inter);
	if(i != -1)
		{
		i += 2;//inter.Length();
		//CString tmp(str.Right(str.GetLength()-i-inter.Length()));
		int j = str.Find(divisor, i);
		//int j = tmp.Find(divisor);
		if(j == -1)
			subcampo = str.Mid(i); //tmp;
		else
			subcampo = str.Mid(i, j - i);//tmp.Left(j);

		return ret;
		}

	return ret;
	}

/////////////////////////////////////////////////////////////////
// Funcion general para introducir un subcampo dentro de una cadena que 
// se encuentre dividida en campos
/////////////////////////////////////////////////////////////////

BOOL SetSubCampo(CStringA &str,		// Cadena que contiene todos los campos
				 const CStringA& subcampo,	// Cadena con el contenido del subcampo
				 LPCSTR name,		// Nombre del campo a seleccionar
				 LPCSTR divisor)	// elemento que se emplea como divisor #@, etc
	{
	BOOL ret = FALSE;

	CStringA buff(str);
	CStringA inter(divisor);
	inter += name;
	int i = buff.Find(inter);
	if(i != -1)
		{
		CStringA inicio(str,i);
		CStringA resto(buff.Right(buff.GetLength()-i-inter.GetLength()));
		int j = resto.Find(divisor);
		
		if(j != -1)
			{
			resto = resto.Right(resto.GetLength()-j);
			}
		else
			resto.Empty();

		str = inicio + resto + inter + subcampo; 
		ret = TRUE;
		}
	else
		{
		str += inter;
		str += subcampo;
		}

	return ret;
	}

////////////////////////////////////////////////////////////////////////////

void StringToArray(const CStringA& s, CSmartStrArray *array, const char* separador)
	{
	StringToArray(s.c_str(), array, separador);
	}

////////////////////////////////////////////////////////////////////////////

void StringToArray(LPCSTR s, CSmartStrArray *array, const char* separador)
  {
	//COLLECT_INTERVAL("StringToArray"); // 0.01 20968
  if(*s == '\x0')
  	return;

  array->Preallocate(1, (int)strlen(s));  // la cantidad de elementos es desconocida
    
  for(;;)
  	{
		int fin = StrFindOneOf(s, separador);
    if( fin == -1)
    	{
			array->Add(s);
      break;
      }
    else
      {    
      array->Add(s, fin);
      s += fin+1;
      }
    }
  
  //xxx array->Compress();
  }            
    
////////////////////////////////////////////////////////////////////////////

void StringToArrayWithSeparator(const CStringA& s, CStrArray *array, LPCSTR separador)
  {
  //array->Preallocate(1, s.GetLength());  // la cantidad de elementos es desconocida

	LPCSTR pStr = s.c_str();
  for(; *pStr != 0;)
    {
    int posSep = StrFindOneOf(pStr, separador);
    if( posSep == -1)
      {
      array->Add(CStringA(pStr));
      break;
      }

		// la clave
		if (posSep > 0)
      array->Add(CStringA(pStr, posSep));

		pStr += posSep;
		char chSeparator = *pStr;
    int i;
    for(i = 1; pStr[i] == chSeparator; i++);

		// el salto
    array->Add(CStringA(pStr, i));

		pStr += i;
    }
  
  //xxx array->Compress();
  }

////////////////////////////////////////////////////////////////////////////

void ArrayToString(CStringA &s,CStrArray &array,LPCSTR separador)
  {                     
  s.Empty();
  TInt nCount = array.GetSize();
	if (nCount == 0)
		return;

  TInt sLen = 0;
  TInt sepLen = (int)strlen(separador);
  
  for(int i = 0;i < nCount;i++)
  	sLen += array[i].GetLength() + sepLen;
  
  s.Preallocate(sLen);
  
  for(int i = 0;i < nCount;i++)
    {                                
    s += array[i];
    if( i < array.GetUpperBound())
      s+= separador;
    }
  }

////////////////////////////////////////////////////////////////////////////

CStringA StrGetDir(const CStringA& aPath)	
	{
	CStringA res(aPath);
	int i = res.ReverseFind(FOLDER_SEP_CH);
	if (i >= 0)
		res = res.Left(i+1);

	return res;
	}

//---------------------------------------------------------------------------
//      Pone la extensión a un nombre de File y le quita la que tenga
//----------------------------------------------------------------------------

void StrPutFileExt(CStringF &name, const CStringF& ext)
{
    SetFileExtension(name, ext);
}

//---------------------------------------------------------------------------

void SetFileExtension(CStringF& aFilename, CStringF aExt)
{
    //CStringW Ext(aExt);

    // Remove the beginning and trailing points

    aFilename.TrimRight('.');
    aExt     .TrimLeft ('.');

    // Locate the extension point

    int pto = aFilename.ReverseFind('.');
    int bs  = aFilename.ReverseFind(FOLDER_SEP_CH);

    // Validate the point after the backslash

    if (pto!=-1 && bs<pto) aFilename = aFilename.Left(pto);

    // Adiciona el punto y la extensión

    aFilename += '.';
    aFilename += aExt;
}

//-------------------------------------------------------------------------------------------
// Borra todos los caracteres de 's2' que se encuentran en 's1'
//-------------------------------------------------------------------------------------------
void DelPbrk(CStringA &s,const CStringA& s2)
  { 
  ASSERT( /*s2 != NULL && */s2.GetLength() > 0);  
  while (1)
    {
    int i = s.FindOneOf(s2);

    if (i == -1)
      break;
    else        
      s.Delete(i);
    }
  }

//-------------------------------------------------------------------------------------------
void DelAccent(CStringA &s)
	{
	s.Replace('á','a');
	s.Replace('é','e');
	s.Replace('í','i');
	s.Replace('ó','o');
	s.Replace('ú','u');
	s.Replace('Á','A');
	s.Replace('É','E');
	s.Replace('Í','I');
	s.Replace('Ó','O');
	s.Replace('Ú','U');
	s.Replace('â','a');
	s.Replace('ê','e');
	s.Replace('î','i');
	s.Replace('ô','o');
	s.Replace('û','u');
	s.Replace('Â','A');
	s.Replace('Ê','E');
	s.Replace('Ê','I');
	s.Replace('Î','O');
	s.Replace('Ô','U');
	s.Replace('ã','a');
	s.Replace('õ','i');
	s.Replace('Ã','A');
	s.Replace('Õ','E');
	s.Replace('à','a');
	s.Replace('è','e');
	s.Replace('ì','i');
	s.Replace('ò','o');
	s.Replace('ù','u');
	s.Replace('À','A');
	s.Replace('È','E');
	s.Replace('Ì','I');
	s.Replace('Ò','O');
	s.Replace('Ù','U');
	}

//-------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////

#if defined _MOBILE_
/******************************************************************************************************************
                   FUNCIONES ESPECIFICAS PARA WINDOWS CE
******************************************************************************************************************/

//------------------------------------------------------------------------------------------------------------------
// Sustituye a CStdioFile::ReadString(LPSTR lpsz, UINT nMax) ya que en CE no existe la función ReadStringA
// (Solo existe ReadStringW)
//------------------------------------------------------------------------------------------------------------------
LPSTR ReadLine(LPSTR lpsz, UINT nMax, CFile& f )
{
	ASSERT(lpsz != NULL);
	//ASSERT(AfxIsValidAddress(lpsz, nMax));

	TInt iniPos = f.GetPosition();

	int nRead = f.ReadBuffer((LPVOID)lpsz, nMax );
	
	if( nRead == 0 )
    {
		//AfxThrowFileException(CFileException::endOfFile, -1, f.GetFileName());
		CRASH;
		//User::Leave(KErrEof);
    return NULL;
    }

  int EndStr = nRead;
	for(int i=0; i< nRead; ++i)             // Busca fin de linea
		if(lpsz[i]=='\n')                     // Lo encontro
		  {
			EndStr = i;                         // Cambia final
			break;
		  }

	lpsz[EndStr] = '\0';                    // Le pone el caracter NULL

	if( EndStr == nRead )                   // Esta al final o no encontro fin de linea
	  return lpsz;                          // No hay que mover el puntero
	
	if( EndStr != nRead )                   // Se encontro el fin de linea, lo salta 
		++EndStr;

	f.Seek(iniPos + EndStr, CFile::begin ); // Mueve posicion del fichero al final de la linea
	
	return lpsz;
}

#endif

//------------------------------------------------------------------------------------------------------------------
// Sustituye a CStringA::TrimRight(LPSTR lpsz, UINT nMax) ya que esta llama a StringTraits::IsSpace() la cual a su 
// vez llama a GetStringTypeExA de Win32 API la cual no existe en CE. (Solo GetStringTypeExW)
//------------------------------------------------------------------------------------------------------------------
/*void TrimRight( CStringA& s )
  {
  int i   = s.GetLength()-1;
  int end = i;

  while( i>0 && isspace(s[i]) ) --i;

  if( i != end )
    s.Truncate(i+1);
  }

#endif*/
