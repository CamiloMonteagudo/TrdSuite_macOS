#ifndef __REFTABLA
#define __REFTABLA

#include "WinUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CDataConj
// clase que almacena los datos para una conjugación verbal

class CDataConj//: public CObject
{	
//DECLARE_SERIAL(CDataConj);
public:
	CDataConj(){};

	CDataConj(CDataConj &dc)
		{
		m_Nombre = dc.m_Nombre;
		m_Modo = dc.m_Modo;		
		m_Tiempo = dc.m_Tiempo;	
		m_Compuesto = dc.m_Compuesto;
		}

	CDataConj& operator=(CDataConj &dc)
		{
		m_Nombre = dc.m_Nombre;
		m_Modo = dc.m_Modo;		
		m_Tiempo = dc.m_Tiempo;	
		m_Compuesto = dc.m_Compuesto;
		return *this;
		};

  virtual void Serialize(CArchive &ar);

	CString m_Nombre;	//Nombre de la conjugación
	WORD m_Modo;		//Modo
	WORD m_Tiempo;		//Tiempo
	BOOL m_Compuesto;	//Si es un tiempo compuesto
};


/////////////////////////////////////////////////////////////////////////////
// CRefTabla

class CRefTabla//: public CObject
  {	
  //DECLARE_SERIAL(CRefTabla);
  public:
    CRefTabla(const CStringA* name = NULL)
    	{
      if( name != NULL )
        m_Name = *name;
      }

    void SetName( const CStringA* name  = NULL)
      {
      if( name != NULL )
        m_Name = *name;
      }

    CString m_Name;
    CSmartStrArray m_TabColName;
    CWordArray m_TabColIndex;
    CSmartStrArray m_PronomPerson; //Pronombres personales del idioma
    CSmartStrArray m_PronomReflex; //Pronombres reflexivos del idioma
    CSmartStrArray m_TerminacReflex; //Terminaciones de verbos reflexivos
    CSmartStrArray m_TerminacVerbal; //Terminaciones de verbos que se sustituyen por los reflexivos
    CPointerArray<CDataConj> m_DataConjArray;//Lista con los datos de las conjugaciones válidas para un idioma
    //CObArray m_DataConjArray;//Lista con los datos de las conjugaciones válidas para un idioma

    //BOOL Load();
    BOOL Save();
    virtual void Serialize(CArchive &ar);

    ~CRefTabla()
      {
      for(int j =0; j < m_DataConjArray.GetSize(); j++)
        {
        if( m_DataConjArray[j] !=NULL ) 
          {
          delete m_DataConjArray[j];
          m_DataConjArray[j] = NULL;
          }
        }
      m_DataConjArray.RemoveAll();
      }

  };


#endif
