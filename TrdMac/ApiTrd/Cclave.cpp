////////////////////////////////////////////////////////////////////////////
//   Funciones de búsqueda de claves                                      //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementan funciones para el trabajo con claves                   // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CWord.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
//static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////////////////////////////////////////////////////////////////
int CompareClave(const void *_key, const void *_s );


int CompareClave(const void *_key, const void *_s )
  {                   /* compare */
#ifdef _SYMBIAN_
  const char *s = (const char *)_s;
#else
  PalClave *s = (PalClave *) _s;
#endif

  ASSERT(_key != NULL);
  ASSERT(_s != NULL);

#ifdef _SYMBIAN_
  const char* key = *(const char**)_key;
  return strcmp((const char *)key, (const char *)s);
#else
  return strcmp((const char *)_key, (const char *)s->w);
#endif
  }                   /* compare */

////////////////////////////////////////////////////////////////////////////

int FindHotWord(   // Retorna subindice de palabra clave -1 si no      
		const CStringA& s ,// string que se pregunta si verbo irreg o derivado 
    PalClave *p,   // ptr a arreglo de palabra structuras de claves    
    int total)     // total de palabras claves en el arreglo           
    {                   // FindIrrgVerb                                 
    const int width = sizeof(PalClave); // width de la struct PalClave  

    //ASSERT( s != NULL);
    ASSERT( p != NULL);
    PalClave *c = (PalClave *) bsearch(s.c_str(), p, total, width, CompareClave);
    return ((c) ? c->i : -1);
    }                   // FindIrrgVerb 

////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
int  DumpClave(    // comprueba si el arreglo esta en orden alfabetico
    PalClave *p,   // arreglo  a comprobar 
    int total)     // cantidad de elementos del arreglo 
    {
    //const int width = sizeof(PalClave); // width de la struct PalClave  

    ASSERT( p != NULL);
    
    for (int i = 1;  i < total - 1;  i++)
        {
        if (!((strcmp(p[i - 1].w, p[i].w) < 0)
             && (strcmp(p[i].w, p[i + 1].w) < 0)))
            {
            //TRACE("*********Clave[%s]\n", p[i].w);
            return 0;
            }
        }
    return 1;   
    }
#endif

////////////////////////////////////////////////////////////////////////////
/**********
int CompareTipoPatron(const void *key, const void *_s )
    {                   // compare 
    PATRON_TIPO *s = (PATRON_TIPO *) _s;
#ifdef _DEBUG
    const char *k = (const char *) key;
#endif

    ASSERT(key != NULL);
    ASSERT(_s != NULL);
    return strcmp((const char *)key, (const char *)s->tipo);
    }                   // compare 

*********/
////////////////////////////////////////////////////////////////////////////
#if 0
char FindPatronTipo(   // Retorna subindice de palabra clave -1 si no      
    const char *s ,// string que se pregunta si verbo irreg o derivado 
    PATRON_TIPO *p,// ptr a arreglo de palabra structuras de claves    
    int total)     // total de palabras claves en el arreglo           
    {                   // FindIrrgVerb                                 
    const int width = sizeof(PATRON_TIPO);  // width de la struct PalClave  

    ASSERT( s != NULL);
    ASSERT( p != NULL);
    PATRON_TIPO *c = (PATRON_TIPO *) bsearch((const char *)s, p, total, width, CompareTipoPatron);
    return ((c)?(c->patron):' ');
    }                   // FindIrrgVerb 
#endif  
////////////////////////////////////////////////////////////////////////////
