///////////////////////////////////////////////////////////////////////////////
//	Se implementa la Clase CSGrepArray
//	Esta clase permite hacer arreglos de expresiones regulares donde buscar
//	estos arreglos pueden ser cargados de file, de recursos  o de un char array
//
//	primera versión				F.Suárez			980104
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include "RegExp.h"
#include <string.h>
//#include "PlataformDef.h"
#include "UtilFun.h"

IMPLEMENT_SERIAL(CSGrepArray,CObject,1);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// Cambia caracteres de control
///////////////////////////////////////////////////////////////////////////////

/*static void ControlChar(char *s)
  {   
  static const char *control[] = {
    "\\n",       // \n
    "\n",
    "\\N",       // \n
    "\n",
    "\\r",       // \r
    "\r",
    "\\R",       // \r
    "\r",
    "\\t",       // \t
    "\t",             
    "\\T",       // \t
    "\t",             
    NULL
    };

#if _DEBUG
  int j = strlen(s);
#endif                           

  for(int i = 0;control[i] != NULL; i+=2)
    {                     
    for(;;)
      {
      char *tmp = strstr(s,control[i]);
      if(tmp)
        { 
        ASSERT(control[i+1] != NULL);
        *tmp = *control[i+1];
        memcpy(tmp+1,tmp+2,strlen(tmp+2)+1);
        }
      else
        break;  
      }   
    }
#if _DEBUG
  j = strlen(s);
#endif                           

  }*/

/////////////////////////////////////////////////////////////////////////////
//   Carga una cadena de un puntero a recurso
/////////////////////////////////////////////////////////////////////////////

/*static void LlenaCadena(			    // llena una CStringA hasta /n/r de los recursos
                        LPCSTR res,	// puntero a los recursos
                        int k,		// posicion de trabajo en los recursos
                        int lres,	// longitud del puntero a los recursos res
                        int *fin,	// posicion en la que se terminó de leer la cadena de los recursos
                        CStringA &ret)	// Cadena donde retornar los datos
  {
  char s[500];

  for(int j = 0; k < lres && res[k] != '\0' && res[k+1] != '\0' && j < 500; k++,j++)
    {
    if(res[k] == '\r' && res[k+1] == '\n')
      {     
      k+=2;
      s[j] = '\0';
      ControlChar(s);
      break;
      }
    else
      s[j] = res[k];
    }

  ret = s;
  *fin = k;
  }*/

/////////////////////////////////////////////////////////////////////////////

CSGrepArray::CSGrepArray(const char **fn,BOOL findreplace,BOOL id)
  { 
  m_Error = FALSE;
  m_Id = id;
  m_FindReplace = findreplace;

  if(fn == NULL)         
    return;

  CSGrepArrayL(fn, findreplace, id);
  }

void CSGrepArray::CSGrepArrayL(const char **fn,BOOL /*findreplace*/,BOOL /*id*/)
	{
  CStringA index,re,replace;
  for(int i = 0;fn[i] != NULL;)
    {
    index.Empty();
		re.Empty();
		replace.Empty();

    if(fn[i] != NULL)
      {
      if(m_Id == TRUE)
        {
        index = (const char *)fn[i];
        i++;
        }
      }

    if(fn[i] != NULL)
      {
      re = fn[i];
      i++;
      }
    if(fn[i] != NULL)
      {
      if(m_FindReplace == TRUE)
        {
        replace = (const char *)fn[i];
        i++;
        }
      }
    if(re.IsEmpty() == FALSE)
      {
      int idx = -1;
      if(m_Id == TRUE)
        idx = atoics(index);
      m_ptrArray.Add(re,replace,idx);
      //m_ptrArray.Add(new CSGrep(re,replace,idx));
      }
    }
	}

/////////////////////////////////////////////////////////////////////////////

CSGrepArray::~CSGrepArray()
  {            
  /*for(int i = 0; i <= m_ptrArray.GetUpperBound(); i++)
    {                    
    delete m_ptrArray.GetAt(i);
    }*/
  m_ptrArray.DeleteAll();
  }

/////////////////////////////////////////////////////////////////////////////

int CSGrepArray::Match(const CStringA& s/*, int *lenMatch*/)
  {       
  //ASSERT( s != NULL);                  
  for( int i= 0; i < m_ptrArray.GetSize();i++)
    { 
    CSGrep *g = m_ptrArray.GetAtPtr(i);
    ASSERT( g != NULL);
    if(g->Match(s))
      {
			//if (lenMatch)
			//	*lenMatch = g->LenghtMatch();
      return (m_Id == TRUE) ? g->GetIndex() : i;
      }
    }   
	//if (lenMatch)
	//	*lenMatch = 0; 
  return -1;  
  }

/////////////////////////////////////////////////////////////////////////////

void CSGrepArray::Replace(CStringA &r, BOOL continuo)
  {
  if(m_FindReplace == FALSE)
    return;

  for( int i= 0; i < m_ptrArray.GetSize();i++)
    { 
    CSGrep *g = m_ptrArray.GetAtPtr(i);
    ASSERT( g != NULL);

    BOOL ret = g->Replace(r);
    if(ret == TRUE && continuo == FALSE)
      return;
    }   
  }

///////////////////////////////////////////////////////////////////////////////
// Fin SGrepArray
/////////////////////////////////////////////////////////////////////////////

