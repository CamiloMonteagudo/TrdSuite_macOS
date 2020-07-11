#ifndef COMPLEXTYPES_H_
#define COMPLEXTYPES_H_

#include <string.h>
#include "WinUtil.h"
#include "DictTypes.h"

//---------------------------------------------------------------------------------------
// Esta clase encapsula el trabajo con los tipos gramaticales, simples y compuestos
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Estructura para definir las caracteristicas de cada tipo soportado  
//---------------------------------------------------------------------------------------
typedef struct tagTypeInfo { 
  LPCSTR FullType;
  int    nTypes;
  LPCSTR sTypes;
  } TYPEINFO;

//---------------------------------------------------------------------------------------
// Lista de tipos soprtados
//---------------------------------------------------------------------------------------
static TYPEINFO InfoTypes[]= 
  { 
    {_T"SS",1,_T"SS"}, // - Sustantivo
    {_T"AA",1,_T"AA"}, // - Adjetivo
    {_T"DD",1,_T"DD"}, // - Adverbio
    {_T"VT",1,_T"VT"}, // - Verbo Transitivo
    {_T"NP",1,_T"NP"}, // - Nombre Propio
    {_T"PP",1,_T"PP"}, // - Preposición
    {_T"PT",1,_T"PT"}, // - Participio Transitivo
    {_T"GT",1,_T"GT"}, // - Gerundio Transitivo
    {_T"CC",1,_T"CC"}, // - Conjunción
    {_T"JJ",1,_T"JJ"}, // - Interjección

    {_T"AI",1,_T"AI"}, // - Adjetivo Estático

    {_T"VI",1,_T"VI"}, // - Verbo Intransitivo
    {_T"VR",1,_T"VR"}, // - Verbo Reflexivo
    {_T"VA",1,_T"VA"}, // - Verbo Auxiliar
    {_T"HT",1,_T"HT"}, // - Verbo Tercera Persona Transitivo
    {_T"HI",1,_T"HI"}, // - Verbo Tercera Persona Intransitivo

    {_T"PI",1,_T"PI"}, // - Participio Intransitivo

    {_T"GI",1,_T"GI"}, // - Gerundio Intransitivo

    {_T"KK",1,_T"KK"}, // - keyword (no usar, es un tipo ficticio para indicar keyword)
    
    {_T"XA",4,_T"SS,AA,DD,VT"       },
    {_T"XB",4,_T"SS,AA,VT,VI"       }, 
    {_T"XH",3,_T"SS,AA,DD"          }, 
    {_T"XJ",2,_T"SS,AA"             }, 
    {_T"YF",2,_T"SS,VT"             }, 
    {_T"SV",3,_T"SS,VT,VI"          }, 
    {_T"XG",2,_T"SS,VI"             }, 
    {_T"TA",4,_T"SS,AA,VT,VR"       },             
    {_T"ZB",3,_T"SS,AA,VT"          },            
    {_T"ZF",4,_T"SS,VT,VI,VR"       }, 
    {_T"YG",2,_T"SS,VR"             }, 
    {_T"XK",2,_T"SS,DD"             }, 
    {_T"XD",3,_T"AA,VT,DD"          }, 
    {_T"XM",2,_T"AA,DD"             }, 
    {_T"ZV",3,_T"VT,VI,VR"          }, 
    {_T"XP",4,_T"VT,VI,VR,DD"       },           
    {_T"XL",2,_T"VR,DD"             },           
    {_T"VV",2,_T"VT,VI"             }, 
    {_T"XY",2,_T"AI,DD"             }, 
    {_T"XO",2,_T"NP,AA"             },                     
    {_T"ZG",3,_T"NP,VT,VI"          }, 

    {_T"ZJ",3,_T"SS,VR,VT"          }, 
    {_T"XQ",5,_T"SS,AA,VR,DD,VT"    },            
    {_T"XC",3,_T"AA,DD,VI"          }, 
    {_T"XI",3,_T"AA,VT,VI"          }, 
    {_T"ZI",2,_T"AA,VT"             }, 
    {_T"XE",2,_T"AA,VI"             }, 
    {_T"ZH",2,_T"VR,VT"             }, 
    {_T"XF",2,_T"DD,VT"             }, 

    {_T"UR",5,_T"SS,AA,VT,VI,DD"    },
    {_T"QN",5,_T"SS,AI,VT,VI,DD"    },
    {_T"QF",5,_T"SS,AA,VT,VI,VR"    },
    {_T"UO",3,_T"SS,AA,VI"          },
    {_T"QO",3,_T"SS,AI,DD"          },
    {_T"QA",4,_T"SS,VT,VI,DD"       },
    {_T"QE",4,_T"AA,VT,VI,DD"       },
    {_T"QG",3,_T"VT,VI,DD"          },
    {_T"UA",2,_T"SS,NP"             },
    //  {"PB",2,_T"AA,Adjetivo preposicional" }	
    {_T"UH",3,_T"AA,PT,PI"          },
    {_T"UB",2,_T"AA,PT"             },
    {_T"UW",2,_T"AA,PI"             },
    {_T"QB",4,_T"SS,AA,PT,PI"       },
    {_T"QC",3,_T"SS,AA,PT"          },
    {_T"QD",3,_T"SS,AA,PI"          },
    {_T"UZ",3,_T"SS,PT,PI"          },
    {_T"UC",2,_T"SS,PT"             },
    {_T"UM",4,_T"SS,AA,GT,GI"       },
    {_T"UT",3,_T"SS,AA,GT"          },
    {_T"UX",3,_T"SS,AA,GI"          },
    {_T"UI",3,_T"SS,GT,GI"          },
    {_T"UE",2,_T"SS,GT"             },
    {_T"UJ",2,_T"SS,GI"             },
    {_T"UK",3,_T"AA,GT,GI"          },
    {_T"UD",2,_T"AA,GT"             },
    {_T"UN",2,_T"AA,GI"             },
    {_T"UL",3,_T"SS,HT,HI"          },
    {_T"UF",2,_T"SS,HT"             },
    {_T"UG",2,_T"SS,HI"             },
    {_T"UV",3,_T"SS,NP,AA"          },
    {_T"UY",4,_T"SS,NP,VT,VI"       },
    {_T"QH",2,_T"DD,HT"             },
    {_T"QI",2,_T"DD,PP"             },
    {_T"QJ",2,_T"SS,VA"             },
    {_T"QK",2,_T"SS,VT,VA"          },
    {_T"QL",4,_T"SS,AA,DD,PP"       },
    {_T"QM",6,_T"SS,AA,VT,VI,DD,PP" },
    {_T"ZZ",3,_T"SS,VR,VI"          },
    {_T"ZZ",3,_T"SS,VR,VI"          }
  };

class CComplexTypes
{
	CStrArray iTypes;


public:
	CComplexTypes() {};
	~CComplexTypes() { iTypes.Close(); };


  //---------------------------------------------------------------------------
  // int AddSingleType( LPCSTR sType, int idx ) 
  //
  // sType - Tipo simple que se quiere añadir para formar un tipo compuesto
  // idx   - Número de tipo que se quiera sustituir o -1 si se desea añadir
  //         un subtipo.
  //
  // Esta funcion adiciona un subtipo simple, en el analisis de la 
  // determinacion de un tipo complejo. el argumento 'idx' se utiliza
  // para sustituir un tipo de los que ya se encuentra en el analisis
  // por otro diferente, si se quiere agregar uno nuevo, ponga idx = -1;
  //
  // Retorna -1 si no se puede adicionar, sino el indice del tipo en la lista,
  // si ya el tipo se encontraba retorna su indice (no lo agrega ni lo sutitulle)
  // si se dio idx retorna idx, sino retorna el indice al ultimo elemento.
  //---------------------------------------------------------------------------
  int AddSingleType( const CStringA& aType, int aIdx=-1 ) 
    { 
    for( int j=0; j<iTypes.GetSize(); ++j )   		// Para todos los subtipos existente
      if( iTypes[j] == aType )                	// Si es el tipo que se quiere añadir
        return j;                               // Retorna su indice

    if( aIdx==-1 )                              // No se especifico indice
      return (int)iTypes.Add( aType );       // Lo adiciona el final

    if( aIdx<0 || aIdx >= iTypes.GetCount() )    	// Indice incorrecto
      return -1;                                // Retorna error

    iTypes.SetAt(aIdx, aType);                     	// 

    return aIdx;
    }

  //---------------------------------------------------------------------------
  // bool GetComplexType( LPSTR Type )
  // 
  // Type - Buffer de al menos 3 caracteres para retornar la designación del
  //        tipo complejo resultante, si la función trabaja satisfactoriamente.
  //
  // Esta funcion obtine cual es el tipo complejo que contiene a todos los
  // tipos simple añadidos por 'AddSingleType'.
  //
  // Retorno: Esta funcion retorna true, si encuentra un tipo que este
  //          compuesto por todos los subtipos dados y FALSE si este tipo
  //          no existe. 
  //---------------------------------------------------------------------------
  bool GetComplexType( CStringA& aType )
    {  
    int n = sizeof(InfoTypes)/sizeof(InfoTypes[0]);     // Cantidad de tipos soprtados

    for( int i=0; i<n; ++i )                            // Busca en la lista de tipos
      if( InfoTypes[i].nTypes == iTypes.GetCount() )   // Tiene el mismo # de subtipos
        if( CmpTypes(i)== 0 )                           // Existen todos los subtipos
          {                                       
          //lstrcpy( aType, InfoTypes[i].FullType );
          aType = InfoTypes[i].FullType;
          return true;                   
          }

    return false;                               // No lo encontro
    }

  //-----------------------------------------------------------------------------------
  // Esta función determina, si los tipos que se ha adicionado son correctos, ya que
  // coinciden parcial o toltalmente con uno de los tipos sortados, para garantizar
  // coincidencia total, usar GetComplexType.
  //-----------------------------------------------------------------------------------
  bool IsGoodTypes()
    {  
    int n = sizeof(InfoTypes)/sizeof(InfoTypes[0]);     // Cantidad de tipos soprtados

    for( int i=0; i<n; ++i )                            // Busca en la lista de tipos
      if( CmpTypes(i)!= -1 )                            // Existen todos los subtipos
        return true;

    return false;                                       // No lo encontro
    }

  //------------------------------------------------------------------------------------
  // Compara el tipo 'idx' de la lista de tipos, con todos los tipos adicionados por
  // 'AddSingleType' si alguno de los tipos no se encuentra retorna -1, si falta algun
  // tipo simple por adicionar retorna la cantidad que faltan, sino retorna 0.
  //------------------------------------------------------------------------------------
  int CmpTypes( int idx )
    { 
    CStringA sType(InfoTypes[idx].sTypes);   			// Copia subtipos del tipo a comparar

    int j;
    for( j=0; j<iTypes.GetCount(); ++j )       		// Para todos los subtipos
      if( sType.Find(iTypes[j]) == -1 )       	// Busca si esta en el tipo
        return -1;                              // No encontro ese subtipo 

    return InfoTypes[idx].nTypes - j;           // Rertorna tipos que faltan
    }

  //------------------------------------------------------------------------------------
  // Obtiene el indice del tipo 'sType' en la lista de tipos soportados, si el tipo no
  // se encuentra en la lista retorna -1.
  //------------------------------------------------------------------------------------
  int TypeNameToIdx( const CStringA& sType )
    {
    int n = sizeof(InfoTypes)/sizeof(InfoTypes[0]);     // Numero de tipos soportados

    // Busca por todos los tipos soportados
    int i;
    //for( i=0; i<n && strcmp( (char*)InfoTypes[i].FullType, (char*)sType ); ++i );
    for( i=0; i<n; ++i )
    	{
      //CStringA sFullType(InfoTypes[i].FullType);
      if (sType == InfoTypes[i].FullType)
      	break;
    	}

    if( i >= n ) return -1;                            // No lo encontro

    return i;                                           // Retorna el indice
    }

  //------------------------------------------------------------------------------------
  // Dado un el tipo complejo 'aType' y un conjunto de tipos simples adicionados con
  // 'AddSingleType' determina cual es el tipo simple que falta, si no se puede 
  // garantizar esas condiciones, se retorna cadena vacia.
  //------------------------------------------------------------------------------------
  void GetDeafultType(CStringA& dType, const CStringA& aType ) 
    {
    dType.Empty();
    //CString dType;

    int idx = TypeNameToIdx(aType);             // Obtiene indice del tipo complejo
    if( idx==-1 ) return;				                // No existe...

    if( InfoTypes[idx].nTypes != iTypes.GetCount()+1 )   // Numero de tipos incorrecto
      return;         				                   // Retorna error

    CStringA sType( InfoTypes[idx].sTypes );     // Copia subtipos del tipo a comparar

    int j=0;
    for(;j<iTypes.GetCount(); ++j )          	  	// Para todos los subtipos
      {
      int i = sType.Find( iTypes[j] );        	// Busca si esta el subtipo
      if( i == -1 )                             // No encontro ese subtipo 
        return;                           			// Retorna error

      sType.SetAt(i, ',');	  	                // Lo borra de la busqueda
      sType.SetAt(i+1, ',');
      }

    int n = sType.Length();                  		// Numero de caracteres
    for( j=0; j<n && sType[j]==','; ++j ) {};   // Salta caracteres borrados

    if( j>n-2 ) return;                   			// Llego hasta el final

    dType = sType.Mid(j,2);                      // Retorna el tipo...
    }
};


#endif /*COMPLEXTYPES_H_*/
