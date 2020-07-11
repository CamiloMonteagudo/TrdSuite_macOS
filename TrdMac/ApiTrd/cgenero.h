////////////////////////////////////////////////////////////////////////////
//   Class CGenero                                                        //
//   enum  Genero                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del género de // 
//  las palabras en la oración                                            //
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__GENERO__)
#define __GENERO__
#include "ApiConj.h"

//enum Genero{MASCULINO,FEMENINO,NEUTRO};

class CGenero
	{   
	protected:
	
	Genero m_Gen;
	
	public:
	
	CGenero()                  // Inicia Género con MASCULINO como implícito
		{
		m_Gen = MASCULINO;
		}
		
	CGenero(Genero g)        
		{
	 	ASSERT( g >= MASCULINO && g <= NEUTRO);
		m_Gen = g;
		}
		
	void Set(Genero g)         // Pone Género
	 	{
	 	ASSERT( g >= MASCULINO && g <= NEUTRO);
	 	m_Gen = g;
	 	}
	 	
	void Set(CGenero  g)         // Pone Género
	 	{
	 	m_Gen = g.m_Gen;
	 	}
                    
	CGenero& operator=(Genero g)	
		{            
		Set(g);
		return *this;
		}
		
	CGenero& operator=(CGenero  g)	
		{            
		Set(g);
		return *this;
		}
		
	BOOL operator==(CGenero  g)	
		{            
		return (m_Gen == g.m_Gen);
		}
		
	BOOL operator==(Genero  g)	
		{            
	 	ASSERT( g >= MASCULINO && g <= NEUTRO);
		return (m_Gen == g);
		}
	BOOL operator!=(CGenero  g)	
		{            
		return (m_Gen != g.m_Gen);
		}
		
	BOOL operator!=(Genero  g)	
		{            
	 	ASSERT( g >= MASCULINO && g <= NEUTRO);
		return (m_Gen != g);
		}
                    
                    
	Genero Get()               // Retorna el Género
		{
	 	ASSERT( m_Gen >= MASCULINO && m_Gen <= NEUTRO);
		return m_Gen;
		}
	};
#endif
