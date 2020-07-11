////////////////////////////////////////////////////////////////////////////
//   Class CGenero                                                        //
//   enum  Genero                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Su�rez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del g�nero de // 
//  las palabras en la oraci�n                                            //
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
	
	CGenero()                  // Inicia G�nero con MASCULINO como impl�cito
		{
		m_Gen = MASCULINO;
		}
		
	CGenero(Genero g)        
		{
	 	ASSERT( g >= MASCULINO && g <= NEUTRO);
		m_Gen = g;
		}
		
	void Set(Genero g)         // Pone G�nero
	 	{
	 	ASSERT( g >= MASCULINO && g <= NEUTRO);
	 	m_Gen = g;
	 	}
	 	
	void Set(CGenero  g)         // Pone G�nero
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
                    
                    
	Genero Get()               // Retorna el G�nero
		{
	 	ASSERT( m_Gen >= MASCULINO && m_Gen <= NEUTRO);
		return m_Gen;
		}
	};
#endif
